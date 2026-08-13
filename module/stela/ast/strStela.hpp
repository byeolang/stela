/// @file
#pragma once

#include "stela/ast/valStela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief String-value marker over @ref valStela.
     *  @details A tag subtype so @ref stelaWriter can wrap the serialized form in
     *  double quotes to match stela source syntax. All storage and value
     *  conversion is inherited from @ref valStela; no methods are added or
     *  overridden.
     */
    class _nout strStela: public valStela {
        BY(CLASS(strStela, valStela), VISIT())

    public:
        using super::valStela;
    };
} // namespace by
