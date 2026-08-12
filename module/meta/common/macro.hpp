/// @file
#pragma once

#include "meta/common/dep.hpp"

/**
 *  @brief Meta-programming macro definitions for byeol type system
 */

// byeolMeta macro's sub-commands:
//  CLONE:
//      defines normal clone func. it have to be defined if you
//      just have declared non abstract class.
#define __BY__DECL_CLONE(ME)                                   \
public:                                                        \
    clonable* clone() const override { return new ME(*this); } \
                                                               \
private:

/**
 *  @brief Define super meta type for class type introspection
 */

//  TYPE:
//      you may need to undef and redefine this define only in your repo
//      if you're trying to use your custom type class.
#define __BY__BASE_TYPE type

//      defines super meta type.
//      for instance, if you set TYPE(type), then when you call getType() of your class,
//      it will returns instance of type of type.
#define __BY__DECL_TYPE(METATYPE)                                      \
public:                                                                \
    typedef METATYPE metaType;                                         \
    const __BY__BASE_TYPE& getType() const override { return ttype<me>::get(); } \
                                                                       \
private:

// INTERFACE:
//      marks that this class is abstract.
#define __BY__DECL_ADT_1(ME) \
    __BY__DECL_ME_1(ME)      \
    __BY__DECL_TYPE(__BY__BASE_TYPE)   \
    __BY__DECL_INIT_META(ME)
#define __BY__DECL_ADT_2(ME, SUPER) __BY__DECL_ADT_3(ME, SUPER, __BY__BASE_TYPE)
#define __BY__DECL_ADT_3(ME, SUPER, SUPERTYPE) \
    __BY__DECL_ME_2(ME, SUPER)                 \
    __BY__DECL_TYPE(SUPERTYPE)                 \
    __BY__DECL_INIT_META(ME)
#define __BY__DECL_ADT(...) BY_OVERLOAD(__BY__DECL_ADT, __VA_ARGS__)

// CLASS:
//      marks that this class is concrete class.
#define __BY__DECL_CLASS_1(ME) \
    __BY__DECL_ADT_1(ME)       \
    __BY__DECL_CLONE(ME)
#define __BY__DECL_CLASS_2(ME, SUPER) \
    __BY__DECL_ADT_2(ME, SUPER)       \
    __BY__DECL_CLONE(ME)
#define __BY__DECL_CLASS_3(ME, SUPER, SUPERTYPE) \
    __BY__DECL_ADT_3(ME, SUPER, SUPERTYPE)       \
    __BY__DECL_CLONE(ME)
#define __BY__DECL_CLASS(...) BY_OVERLOAD(__BY__DECL_CLASS, __VA_ARGS__)
