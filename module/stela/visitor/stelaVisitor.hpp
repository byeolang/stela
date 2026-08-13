/// @file
#pragma once

#include "stela/visitor/stelaVisitInfo.hpp"
#include <map>

namespace by {

#define X(T) class T;
#include "visitee.inl"
#undef X

    /** @ingroup stela
     *  @brief Preorder visitor for the stela AST.
     *  @details Mirrors byeol's visitor in miniature. Each visit goes through three
     *  phases:
     *   1. onVisit()   — inspect the node; return false to skip the subtree.
     *   2. onTraverse() — descend into children (default: iterate the child map).
     *   3. onLeave()   — post-visit hook.
     *
     *  Typed overloads (`onVisit(..., valStela&)`) are dispatched from each subclass's
     *  `accept()` (injected by the VISIT macro). A type-specific override that is not
     *  provided chains up to `onVisit(..., stela&)` via the `super` typedef, matching
     *  byeol's convention.
     *
     *  stela trees are pure (no shared subgraphs), so unlike byeol's visitor no
     *  visited-set is maintained.
     */
    class _nout stelaVisitor : public typeProvidable, public clonable {
        BY(CLASS(stelaVisitor))

    public:
        stelaVisitor();
        ~stelaVisitor() override;

    public:
        void work(stela& root);

#define X(T)                                               \
    virtual void visit(const stelaVisitInfo& i, T& it);    \
    virtual nbool onVisit(const stelaVisitInfo& i, T& it); \
    virtual void onLeave(const stelaVisitInfo& i, T& it);
#include "visitee.inl"
#undef X

        virtual void visit(const stelaVisitInfo& i, stela& it);
        virtual nbool onVisit(const stelaVisitInfo& i, stela& it);
        virtual void onLeave(const stelaVisitInfo& i, stela& it);

        // traverse:
        //  generalized-way:
        virtual void onTraverse(const stelaVisitInfo& i, stela& it);

    private:
        nbool _markVisited(stela& it);

    private:
        std::map<stela*, nbool> _visited;
    };
} // namespace by
