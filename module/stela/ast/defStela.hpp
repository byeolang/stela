/// @file
#pragma once

#include "stela/ast/stela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Named block of children — the `def` form of stela source.
     *  @details A tag subtype so @ref stelaWriter emits a `def <name>` block. It adds no
     *  storage and no methods: children are ordinary @ref stela children, so every
     *  access goes through the plain @ref stela interface.
     *
     *  This is what @ref stela itself used to be. @ref stela is abstract now, and the
     *  `def`-block behaviour that only a block should have lives here, so a sibling
     *  form like @ref arrStela no longer inherits it.
     *
     *  @section usage Usage
     *  @code
     *      defStela root("root");
     *      root.add(new strStela("byeol", "name"));
     *  @endcode
     */
    class _nout defStela: public stela {
        BY(CLASS(defStela, stela), VISIT())

    public:
        using super::stela;
    };
} // namespace by
