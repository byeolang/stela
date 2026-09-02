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
     *  The index is zero-padded so that the child map's lexicographic key order is
     *  also numeric order; bare "10" would sort ahead of "2". The padding width caps
     *  how many elements keep their order — see stelaParser::IDX_WIDTH.
     */
    class _nout arrStela: public stela {
        BY(CLASS(arrStela, stela), VISIT())

    public:
        using super::stela;
    };
} // namespace by
