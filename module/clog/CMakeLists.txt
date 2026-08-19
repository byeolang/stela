CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

include(FetchContent)

# fetching indep:
#   FetchContext could overwrites parent context(e.g. cmake variables),
#   so I have to declare mine first.
FetchContent_Declare(
    indep_dep
    SOURCE_SUBDIR build
    GIT_REPOSITORY https://github.com/byeolang/indep/
    GIT_TAG 1938600)
FetchContent_MakeAvailable(indep_dep)

set(moduleName "clog")
message("\nbuilding ${moduleName}...")

set(defCandidates "__${moduleName}=1")
foreach(def ${defCandidates})
    message("\tadd define ${def}")
    set(defines ${defines} -D${def})
endforeach(def)
add_definitions(${defines})


#   defines:
#       define sources:
file(GLOB_RECURSE sources
    "*.c"
    "*.cpp"
)
#           logging sources:
foreach(e ${sources})
    message("\tembeding ${e}")
endforeach(e)
#       define libraries:
set(dependencies ${dependencies} indep)
foreach(e ${dependencies})
    message("\tlinking ${e}")
endforeach(e)


#   execute:
IF(EMSCRIPTEN)
    add_library(${moduleName} ${sources})
ELSE(EMSCRIPTEN)
    add_library(${moduleName} SHARED ${sources})
    install(TARGETS ${moduleName} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib PERMISSIONS
        OWNER_WRITE OWNER_READ OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE)
ENDIF(EMSCRIPTEN)
set_target_properties(${moduleName} PROPERTIES SUFFIX ".${BY_LIBRARY_EXTENSION}")
target_link_libraries(${moduleName} ${dependencies})

# expose module/ as the public include root so consumers (and this module's own
# sources) can resolve `module/`-prefixed headers such as #include "clog/logger.hpp".
# propagated up the link graph, e.g. clog -> stela -> byeol.
target_include_directories(${moduleName}
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

#       let MSVC follow macro expansion of standard:
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    target_compile_options(${moduleName} PUBLIC "/Zc:preprocessor")
endif()
