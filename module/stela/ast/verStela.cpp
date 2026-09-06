#include "stela/ast/verStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"
#include <stdexcept>

namespace by {

    BY(DEF_ME(verStela, valStela), DEF_VISIT())

    me::verStela(int major, int minor, int fix):
        super(std::to_string(major) + DELIMITER + std::to_string(minor) + DELIMITER + std::to_string(fix)) {}

    me::verStela(const std::string& verStr): super(_normalize(verStr)) {}

    me::verStela(const nchar* verStr): super(_normalize(verStr ? verStr : "")) {}

    nbool me::operator>(const me& rhs) const {
        nint res = _isFromBigger(asMajor(), rhs.asMajor());
        WHEN(res != 0) .ret(res == 1);

        res = _isFromBigger(asMinor(), rhs.asMinor());
        WHEN(res != 0) .ret(res == 1);

        return _isFromBigger(asFix(), rhs.asFix()) > 0;
    }

    nbool me::operator<(const me& rhs) const { return rhs.operator>(*this); }

    nbool me::operator==(const me& rhs) const { return asStr() == rhs.asStr(); }

    nbool me::operator!=(const me& rhs) const { return !operator==(rhs); }

    nbool me::operator<=(const me& rhs) const { return !operator>(rhs); }

    nbool me::operator>=(const me& rhs) const { return !operator<(rhs); }

    nint me::asMajor() const { return _splitVers(0); }

    nint me::asMinor() const { return _splitVers(1); }

    nint me::asFix() const { return _splitVers(2); }

    nint me::_isFromBigger(nint from, nint to) {
        if(from > to) return 1;
        if(from == to) return 0;
        return -1;
    }

    std::string me::_normalize(const std::string& verStr) {
        std::stringstream ss(verStr);
        std::string token;
        std::string them[VER_LEN] = {"0", "0", "0"};

        // rejecting here is what lets asMajor() and the operators below skip error
        // handling entirely: once stored, the string is three numeric segments.
        nint n = 0;
        for(; n < VER_LEN && std::getline(ss, token, DELIMITER[0]); n++) {
            if(token.empty() || token.find_first_not_of("0123456789") != std::string::npos)
                throw std::invalid_argument("verStela: '" + verStr + "' is not a major.minor.fix version");

            them[n] = token;
        }

        // a trailing segment means more than 3 parts; n == 0 means the string was empty.
        if(n <= 0 || std::getline(ss, token))
            throw std::invalid_argument("verStela: '" + verStr + "' is not a major.minor.fix version");

        return them[0] + DELIMITER + them[1] + DELIMITER + them[2];
    }

    nint me::_splitVers(nint n) const {
        std::stringstream ss(asStr());
        std::string token;

        for(nint step = 0; step <= n; step++)
            std::getline(ss, token, DELIMITER[0]);

        return std::stoi(token);
    }
} // namespace by
