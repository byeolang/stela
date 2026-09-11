/// @file
#pragma once

#include "stela/ast/stela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Scalar value — the abstract base of every leaf form.
     *  @details Holds a scalar value as a raw string in `_rawVal` and converts to
     *  primitive types on request via `asInt()`, `asBool()`, etc.
     *
     *  It is an ADT: the storage and the conversions live here and are reused, while
     *  each syntactic form is a concrete subtype (@ref numStela, @ref boolStela,
     *  @ref strStela, @ref verStela) that a visitor can discriminate. The ctors are
     *  protected so no caller can build a value node whose form is unknown.
     *
     *  @remark Exception on type conversion failure
     *  If type conversion fails, an exception is thrown, so be careful.
     */
    class _nout valStela: public stela {
        BY(ADT(valStela, stela), VISIT())
        // only the parser sees a spelling. a public setter would let the spelling disagree
        // with the value, and the writer trusts the spelling.
        friend class stelaParser;

    protected:
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

        /**
         * @brief The literal as it was written in source, e.g. `0xFF` for a value of 255.
         * @details Falls back to asStr() for a node built in code, which has no spelling
         *          of its own. Editing a value means adding a new node, so an edit never
         *          keeps a stale spelling.
         */
        const std::string& getRepr() const;

    private:
        static std::string toLower(std::string it);

    private:
        std::string _rawVal;
        std::string _repr;
    };
}
