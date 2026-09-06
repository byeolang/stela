/// @file
#pragma once

#include "stela/ast/valStela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Numeric literal — the `INTVAL` and `FLTVAL` forms of stela source.
     *  @details A tag subtype over @ref valStela so a visitor can tell a number from a
     *  string or a boolean. Storage and value conversion are inherited; nothing is
     *  added.
     *
     *  Integers and floats share this one type: stela is a manifest language, where a
     *  number is just a number. Whether a value was written with a fraction is
     *  recoverable from asStr() if a caller ever needs it.
     */
    class _nout numStela: public valStela {
        BY(CLASS(numStela, valStela), VISIT())

    public:
        numStela(nint val, const std::string& name = "");
        numStela(nflt val, const std::string& name = "");
        numStela(const std::string& rawVal, const std::string& name = "");
        numStela(const nchar* rawVal, const std::string& name = "");
        // without this, numStela(true) binds to the nint overload by promotion and
        // stores "1". a boolean literal has its own node; it must not land here.
        numStela(nbool val, const std::string& name = "") = delete;
    };
} // namespace by
