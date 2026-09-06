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
        // spelled out rather than `using super::valStela`: that would inherit the
        // numeric ctors too, and strStela(42) would silently store "42".
        strStela(const std::string& rawVal, const std::string& name = "");
        strStela(const nchar* rawVal, const std::string& name = "");
    };
} // namespace by
