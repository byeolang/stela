/// @file
#pragma once

#include "stela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief emptyStela node representing empty values.
     *  this class usually used for representing a comment.
     */
    class _nout emptyStela: public stela {
        BY(CLASS(emptyStela, stela), VISIT())

    public:
        emptyStela(const std::string& comment = ""): super(_makeName(), comment) {}

    public:
        const std::string& asStr() const override;
        nchar asChar() const override;
        nint asInt() const override;
        nbool asBool() const override;

    private:
        static std::string _makeName();
    };
}
