/// @file
#pragma once

#include "stela/ast/valStela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Boolean literal — the `BOOLVAL` form of stela source.
     *  @details A tag subtype over @ref valStela so a visitor can tell a boolean from a
     *  number. Storage and value conversion are inherited; nothing is added.
     */
    class _nout boolStela: public valStela {
        BY(CLASS(boolStela, valStela), VISIT())

    public:
        boolStela(nbool val, const std::string& name = "");
        boolStela(const std::string& rawVal, const std::string& name = "");
        boolStela(const nchar* rawVal, const std::string& name = "");
        // a bare literal like boolStela("true") must keep reaching the string ctor
        // above, so the numeric ones are refused rather than left to convert.
        boolStela(nint val, const std::string& name = "") = delete;
        boolStela(nflt val, const std::string& name = "") = delete;
    };
} // namespace by
