/// @file
#pragma once

#include "stela/ast/stela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Ordered list of elements.
     *  @details A tag subtype so @ref stelaWriter emits `{a, b, c}` inline instead of a
     *  `def` block. It adds no storage and no methods: elements are ordinary children
     *  named "0000", "0001", "0002", ... so every access goes through the plain
     *  @ref stela interface. Callers never need to downcast to reach an element.
     *
     *  @section usage Usage
     *  @code
     *      stela& deps = root->sub("deps");
     *      for(ncnt n = 0; n < deps.len(); ++n)
     *          deps.sub(n).asStr();
     *  @endcode
     *
     *  @remark Element order
     *  Elements are named "0", "1", "2", ... and come back in that order because
     *  @ref stela keeps its children in insertion order.
     */
    class _nout arrStela: public stela {
        BY(CLASS(arrStela, stela), VISIT())

    public:
        using super::stela;
    };
} // namespace by
