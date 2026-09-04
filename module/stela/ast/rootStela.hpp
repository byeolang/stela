/// @file
#pragma once

#include "stela/ast/defStela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief The compilation unit — the one node with no source form of its own.
     *  @details stela source has an implicit top level: statements at column 0 belong
     *  to the file, not to any `def` block. rootStela is that top level. It is a
     *  @ref defStela in every respect but one: it is never written back out, because
     *  no `def` line produced it.
     *
     *  It exists so that "is this the root?" is a type, not a test. Anything taking a
     *  plain @ref stela used to answer that question by inspecting the traversal depth,
     *  which spread one document-shape rule across every such function and left the
     *  answer wrong for any subtree visited on its own.
     *
     *  @ref stelaParser produces exactly one per parse.
     */
    class _nout rootStela: public defStela {
        BY(CLASS(rootStela, defStela), VISIT())

    public:
        using super::defStela;
    };
} // namespace by
