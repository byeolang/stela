# decide compiler:
#   variables such as CXX_COMPILER_ID and COMPILER_IS_GNUCC remain unchanged.
#   to resolve this issue, you must determine which compiler to use before calling
#   the project() function.
IF(EMSCRIPTEN)
    set(CMAKE_C_COMPILER "emcc")
    set(CMAKE_CXX_COMPILER "emcc")
ELSE()
    find_program(CLANGPP_PATH clang++)
    find_program(CLANG_PATH clang)
    IF(CLANGPP_PATH)
        set(CMAKE_CXX_COMPILER "${CLANGPP_PATH}")
        set(CMAKE_C_COMPILER "${CLANG_PATH}")
    ENDIF(CLANGPP_PATH)
ENDIF(EMSCRIPTEN)

cmake_minimum_required (VERSION 3.14)
project(stela C CXX)

# include build-common if this is host project.
get_property(build_common_included GLOBAL PROPERTY BUILD_COMMON_INCLUDED)
if(NOT build_common_included)
    list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../external/build-common)
    include(buildCommon)

    # parse version
    by_parse_ver("${CMAKE_CURRENT_SOURCE_DIR}/../CHANGELOGS.md")
    set(VERSION_NAME "") # don't use version name in default.
endif()


# dependencies check:
by_check_dependency(
  NAMES bison
  MIN_VERSION 3.8.0
)
by_check_dependency(
  NAMES flex
  MIN_VERSION 2.6.0
)

#   build-date:
string(TIMESTAMP BUILD_DATE_YEAR "%Y")
string(TIMESTAMP BUILD_DATE_MONTH "%m")
string(TIMESTAMP BUILD_DATE_MONTH_TEXT "%b")
string(TIMESTAMP BUILD_DATE_DAY "%d")
set(BUILD_DATE "${BUILD_DATE_MONTH_TEXT} ${BUILD_DATE_DAY} ${BUILD_DATE_YEAR}")
string(TIMESTAMP BUILD_TIME_HOUR "%H")
string(TIMESTAMP BUILD_TIME_MINUTE "%M")
string(TIMESTAMP BUILD_TIME_SECOND "%S")
string(TIMESTAMP BUILD_TIME "${BUILD_TIME_HOUR}:${BUILD_TIME_MINUTE}:${BUILD_TIME_SECOND}")
set(BUILD_DATE_AND_TIME "${BUILD_DATE} ${BUILD_TIME}")


message("\ninitialize CMake variables...")

set(CXX_DEFINITIONS "${DEFINITIONS} -DUNICODE")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(CheckIncludeFile)
check_include_file(execinfo.h HAS_EXECINFO)
if(HAS_EXECINFO)
    set(CMAKE_HAS_EXECINFO 1)
else()
    set(CMAKE_HAS_EXECINFO 0)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(CMAKE_SYSTEM_NAME "Linux")
endif()

IF(CMAKE_SYSTEM_NAME STREQUAL Darwin OR CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_MACOSX_RPATH 1)

    # common flags:
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fdiagnostics-color=always")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-delete-null-pointer-checks -Wno-sign-compare -Wno-unused-parameter -Wno-address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs -Wno-unused-function")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments -Wno-undefined-bool-conversion -Wno-c++20-extensions")

    IF(NOT EMSCRIPTEN)
        IF(CLANGPP_PATH)
            set(CMAKE_CXX_COMPILER "${CLANGPP_PATH}")
            set(CMAKE_C_COMPILER "${CLANG_PATH}")

            # clang flags:
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments -Wno-undefined-bool-conversion -Wno-c++20-extensions")

            IF(COVERAGE_TOOL)
                add_compile_options(-fprofile-instr-generate -fcoverage-mapping)
                add_link_options(-fprofile-instr-generate -fcoverage-mapping)
            ENDIF(COVERAGE_TOOL)

        ELSE(CLANGPP_PATH)
            # gcc flags:
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dangling-reference") # False positive
            #   older gccs arbitrarily optimize away member variable initialization syntax in destructors that are supposed to handle
            #   allocation new/delete correctly. It's a sort of false positive, because it works fine in newer gccs.
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-lifetime-dse")

            IF(COVERAGE_TOOL)
                add_compile_options(-fprofile-arcs -ftest-coverage)
                add_link_options(-fprofile-arcs -ftest-coverage)
            ENDIF(COVERAGE_TOOL)

        ENDIF(CLANGPP_PATH)
    ENDIF(NOT EMSCRIPTEN)

    IF(EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wno-unused-const-variable -Wno-non-pod-varargs -Wno-unused-variable")
    ELSE(EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -DBY_DEBUG -D_GLIBCXX_DEBUG")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
    ENDIF(EMSCRIPTEN)

    IF(CMAKE_RELEASE_INCLUDE_DBG_INFO STREQUAL True)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
    ENDIF(CMAKE_RELEASE_INCLUDE_DBG_INFO STREQUAL True)
    if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        set(CMAKE_SHARED_LINKER_FLAGS "-rpath .")
        set(CMAKE_EXE_LINKER_FLAGS "-rpath .")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
        if(CLANGPP_PATH)
            # Ensure libc++ runtime matches the headers provided by the detected clang.
            get_filename_component(BY_CLANG_DIR "${CLANGPP_PATH}" DIRECTORY)
            get_filename_component(BY_LLVM_ROOT "${BY_CLANG_DIR}/.." ABSOLUTE)
            set(BY_LLVM_LIBCXX_DIR "${BY_LLVM_ROOT}/lib/c++")
            if(EXISTS "${BY_LLVM_LIBCXX_DIR}/libc++.dylib")
                set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${BY_LLVM_LIBCXX_DIR} -Wl,-rpath,${BY_LLVM_LIBCXX_DIR}")
                set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L${BY_LLVM_LIBCXX_DIR} -Wl,-rpath,${BY_LLVM_LIBCXX_DIR}")
            endif()
        endif()
        set(BY_LIBRARY_EXTENSION "dylib")
    ELSE(CMAKE_SYSTEM_NAME STREQUAL Darwin)
        IF(EMSCRIPTEN)
            set(BY_LIBRARY_EXTENSION "a")
        ELSE(EMSCRIPTEN)
            set(BY_LIBRARY_EXTENSION "so")
        ENDIF(EMSCRIPTEN)
    ENDIF(CMAKE_SYSTEM_NAME STREQUAL Darwin)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL Darwin OR CMAKE_SYSTEM_NAME STREQUAL Linux OR CMAKE_SYSTEM_NAME STREQUAL Emscripten)
IF(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /permissive- /Zc:__cplusplus /Zc:preprocessor /utf-8 /Zc:strictStrings /Zc:rvalueCast /Zc:auto /Zc:throwingNew")
    set(defines -DWIN32)
    set(BY_LIBRARY_EXTENSION "dll")
ENDIF(CMAKE_SYSTEM_NAME STREQUAL Windows)

cmake_policy(SET CMP0015 NEW)
cmake_policy(SET CMP0042 NEW)


set(BY_POD_EXTENSION "pod")
get_filename_component(BY_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
set(BY_DEFAULT_INC_DIR
    ${BY_DIR}/module
    /usr/local/include)
include_directories(${BY_DEFAULT_INC_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BY_DIR}/bin)
set(BY_POD_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pod)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} /usr/local/lib)
set(PROJECT_SOURCE_DIR ${BY_DIR}/module)
SET(PROJECT_BINARY_DIR ${BY_DIR}/bin)
SET(BY_TESTDATA_SRC_PATH ${PROJECT_SOURCE_DIR}/test/testdata)
SET(BY_TESTDATA_PATH ${PROJECT_BINARY_DIR}/testdata)

if ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    set(CMAKE_IS_DBG 1)
else ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
    set(CMAKE_IS_DBG 0)
endif ("${CMAKE_BUILD_TYPE}" STREQUAL Debug)

add_definitions(${CXX_DEFINITIONS})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../module ${CMAKE_CURRENT_BINARY_DIR}/module)
