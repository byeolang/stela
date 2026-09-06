/// @file
#pragma once

#include "stela/ast/valStela.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Version stela node for semantic version handling
     *  @details Similar to @ref valStela, this @ref stela holds a `version` type value.
     *
     *  @section version type
     *  The stela language is a lightweight version of byeol language, a special-purpose
     *  language for manifests and options. The version type has three variables:
     *  `major.minor.fix`, and can also express ranges.
     *
     *  @remark Exception on a malformed version string
     *  The string constructors throw if the argument is not `major.minor.fix`. A short
     *  version is padded instead: `verStela("1.2")` stores `"1.2.0"`.
     */
    class _nout verStela: public valStela {
        BY(CLASS(verStela, valStela), VISIT())

    public:
        verStela(int major, int minor, int fix);
        explicit verStela(const std::string& verStr);
        explicit verStela(const nchar* verStr);

    public:
        nbool operator>(const me& rhs) const;
        nbool operator<(const me& rhs) const;
        nbool operator==(const me& rhs) const;
        nbool operator!=(const me& rhs) const;
        nbool operator<=(const me& rhs) const;
        nbool operator>=(const me& rhs) const;

    public:
        nint asMajor() const;
        nint asMinor() const;
        nint asFix() const;

    private:
        static nint _isFromBigger(nint from, nint to);
        static std::string _normalize(const std::string& verStr);
        nint _splitVers(nint n) const;

    private:
        static constexpr nint VER_LEN = 3;
        static constexpr const nchar* DELIMITER = ".";
    };
} // namespace by
