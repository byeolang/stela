/// @file
#pragma once

#include "stela/ast/stela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Value-holding stela node.
     *  @details Holds a scalar value as a raw string in `_rawVal` and converts to
     *  primitive types on request via `asInt()`, `asBool()`, etc.
     *
     *  It is concrete, and the parser builds one directly for every numeric and
     *  boolean literal. The subtypes (@ref strStela, @ref verStela) exist purely so
     *  visitors can discriminate between them; the value-access implementation lives
     *  here and is reused by every subclass.
     *
     *  @remark Exception on type conversion failure
     *  If type conversion fails, an exception is thrown, so be careful.
     */
    class _nout valStela: public stela {
        BY(CLASS(valStela, stela), VISIT())

    public:
        valStela(const std::string& rawVal, const std::string& name = "");
        valStela(const nchar* rawVal, const std::string& name = "");
        valStela(nbool val, const std::string& name = "");
        valStela(nint val, const std::string& name = "");
        valStela(nflt val, const std::string& name = "");
        valStela(const me& rhs, const std::string& name = "");

    public:
        const std::string& asStr() const override;
        nchar asChar() const override;
        nint asInt() const override;
        nbool asBool() const override;

    private:
        static std::string toLower(std::string it);

    private:
        std::string _rawVal;
    };
}
