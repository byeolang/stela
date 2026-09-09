#include "stela/ast/verStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"
#include <stdexcept>

namespace by {

    BY(DEF_ME(verStela, valStela), DEF_VISIT())

    me::verStela(nint major, nint minor, nint patch):
        super(std::to_string(major) + DELIMITER + std::to_string(minor) + DELIMITER + std::to_string(patch)) {}

    me::verStela(nint major, nint minor, nint patch, nint rev):
        super(std::to_string(major) + DELIMITER + std::to_string(minor) + DELIMITER + std::to_string(patch) +
            DELIMITER + std::to_string(rev)) {}

    me::verStela(const std::string& verStr): super(_makeValid(verStr)) {}

    me::verStela(const nchar* verStr): super(_makeValid(verStr ? verStr : "")) {}

    nbool me::operator>(const me& rhs) const {
        nint res = _isFromBigger(asMajor(), rhs.asMajor());
        WHEN(res != 0) .ret(res == 1);

        res = _isFromBigger(asMinor(), rhs.asMinor());
        WHEN(res != 0) .ret(res == 1);

        res = _isFromBigger(asPatch(), rhs.asPatch());
        WHEN(res != 0) .ret(res == 1);

        return _isFromBigger(asRev(), rhs.asRev()) > 0;
    }

    nbool me::operator<(const me& rhs) const { return rhs.operator>(*this); }

    // not a string compare: "1.2.3" and "1.2.3.0" are the same version, and only one
    // of them keeps its fourth segment.
    nbool me::operator==(const me& rhs) const {
        return asMajor() == rhs.asMajor() && asMinor() == rhs.asMinor() && asPatch() == rhs.asPatch() &&
            asRev() == rhs.asRev();
    }

    nbool me::operator!=(const me& rhs) const { return !operator==(rhs); }

    nbool me::operator<=(const me& rhs) const { return !operator>(rhs); }

    nbool me::operator>=(const me& rhs) const { return !operator<(rhs); }

    nint me::asMajor() const { return _splitVers(0); }

    nint me::asMinor() const { return _splitVers(1); }

    nint me::asPatch() const { return _splitVers(2); }

    nint me::asRev() const { return _splitVers(3); }

    nint me::_isFromBigger(nint from, nint to) {
        if(from > to) return 1;
        if(from == to) return 0;
        return -1;
    }

    std::string me::_makeValid(const std::string& verStr) {
        std::stringstream ss(verStr);
        std::string token;
        std::string them[MAX_VER_LEN] = {"0", "0", "0", "0"};

        // rejecting here is what lets asMajor() and the operators below skip error
        // handling entirely: once stored, the string is numeric segments only.
        nint n = 0;
        for(; n < MAX_VER_LEN && std::getline(ss, token, DELIMITER[0]); n++) {
            if(token.empty() || token.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("verStela: '" + verStr + "' is not a major.minor.patch[.rev] version");

            them[n] = token;
        }

        // a trailing segment means more than 4 parts; n == 0 means the string was empty.
        if(n <= 0 || std::getline(ss, token))
            throw std::invalid_argument("verStela: '" + verStr + "' is not a major.minor.patch[.rev] version");

        // a short version pads up to major.minor.patch, but an absent rev stays
        // absent: three segments is the common case and is written back as three.
        nint len = n > MIN_VER_LEN ? n : MIN_VER_LEN;
        std::string ret = them[0];
        for(nint step = 1; step < len; step++)
            ret += DELIMITER + them[step];

        return ret;
    }

    nint me::_splitVers(nint n) const {
        std::stringstream ss(asStr());
        std::string token;

        for(nint step = 0; step <= n; step++)
            WHEN(!std::getline(ss, token, DELIMITER[0])) .ret(0);

        try {
            return std::stoi(token);
        } catch(const std::exception&) { return 0; }
    }
} // namespace by
