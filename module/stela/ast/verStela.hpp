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
     *  language for manifests and options. The version type is `major.minor.patch` with an
     *  optional fourth segment, and can also express ranges.
     *
     *  @remark The stored string is normalized, not the one given
     *  A short version pads up to `major.minor.patch`, so `verStela("1.2")` stores
     *  `"1.2.0"`. Past that a segment is kept as written, and comparison reads every
     *  segment on both sides, so `1.2.3` and `1.2.3.0` are the same version.
     *
     *  @remark Exception on a malformed version string
     *  The string constructors throw if the argument is not `major.minor.patch` with an
     *  optional `.rev`.
     */
    class _nout verStela: public valStela {
        BY(CLASS(verStela, valStela), VISIT())

    public:
        verStela(nint major, nint minor, nint patch);
        verStela(nint major, nint minor, nint patch, nint rev);
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
        nint asPatch() const;
        nint asRev() const;

    private:
        static nint _isFromBigger(nint from, nint to);
        static std::string _makeValid(const std::string& verStr);
        nint _splitVers(nint n) const;

    private:
        static constexpr nint MIN_VER_LEN = 3;
        static constexpr nint MAX_VER_LEN = 4;
        static constexpr const nchar* DELIMITER = ".";
    };
} // namespace by
