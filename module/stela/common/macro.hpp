/// @file
#pragma once

#include "stela/common/dep.hpp"

namespace by {

    class stelaVisitor;
    class stelaVisitInfo;
} // namespace by

// ACCEPT for stela AST.
//   Same macro names as byeol core (VISIT / DEF_VISIT), but bound to stelaVisitor
//   and stelaVisitInfo. byeol undefines these in its own metaExtension header and
//   redefines them to bind to its own visitor. Since include order for the two
//   AST hierarchies never crosses (stela cannot see byeol), the two definitions
//   never collide within one translation unit's AST body.
#define __BY__DECL_VISIT_0()      __BY__DECL_VISIT_1(super)
#define __BY__DECL_VISIT_1(SUPER)                                   \
public:                                                             \
    using SUPER::accept;                                            \
    void accept(const stelaVisitInfo& i, stelaVisitor& v) override; \
                                                                    \
private:
#define __BY__DECL_VISIT(...)     BY_OVERLOAD(__BY__DECL_VISIT, __VA_ARGS__)

#define __BY__DECL_DEF_VISIT_0()  __BY__DECL_DEF_VISIT_1(me)
#define __BY__DECL_DEF_VISIT_1(ME) \
    void ME::accept(const stelaVisitInfo& i, stelaVisitor& v) { v.visit(i, *this); }
#define __BY__DECL_DEF_VISIT(...) BY_OVERLOAD(__BY__DECL_DEF_VISIT, __VA_ARGS__)
