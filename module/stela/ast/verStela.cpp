#include "stela/ast/verStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"
#include <stdexcept>
#include <regex>

namespace by {

    BY(DEF_ME(verStela, valStela), DEF_VISIT())

    namespace {
        constexpr const nchar* DELIMITER = ".";
        constexpr const nchar* EMPTY_VER = ".0";
        constexpr const nchar* REG_PATT = R"(([0-9]+)(?:\.([0-9]+))?(?:\.([0-9]+))?(?:\.([0-9]+))?)";
        const std::regex _verDelim(REG_PATT);
    }

    me::verStela(nint major, nint minor, nint patch):
        super(std::to_string(major) + DELIMITER + std::to_string(minor) + DELIMITER + std::to_string(patch)) {}

    me::verStela(nint major, nint minor, nint patch, nint rev):
        super(std::to_string(major) + DELIMITER + std::to_string(minor) + DELIMITER + std::to_string(patch) +
            DELIMITER + std::to_string(rev)) {}

    namespace {
        static nint _isBigger(nint from, nint to) {
            if(from > to) return 1;
            if(from == to) return 0;
            return -1;
        }

        static std::string _makeValid(const std::string& verStr) {
            std::smatch match;
            if(!std::regex_match(verStr, match, _verDelim))
                throw std::invalid_argument("given versing doesn\'t match to expected format 'a.b.c.d'.");

            std::vector<std::string> parsed(match.begin() + 1, match.end());

            // need to fill `.0` to omitted versions:
            std::string ret = parsed[0];
            for(auto e = parsed.begin() + 1; e != parsed.end() ;++e)
                ret += e->empty() ? EMPTY_VER : DELIMITER + *e;
            return ret;
        }
    }

    me::verStela(const std::string& verStr): super(_makeValid(verStr)) {}

    me::verStela(const nchar* verStr): super(_makeValid(verStr ? verStr : "")) {}

    nbool me::operator>(const me& rhs) const {
        nint res = _isBigger(asMajor(), rhs.asMajor());
        WHEN(res != 0) .ret(res == 1);

        res = _isBigger(asMinor(), rhs.asMinor());
        WHEN(res != 0) .ret(res == 1);

        res = _isBigger(asPatch(), rhs.asPatch());
        WHEN(res != 0) .ret(res == 1);

        return _isBigger(asRev(), rhs.asRev()) > 0;
    }

    nbool me::operator<(const me& rhs) const { return rhs.operator>(*this); }

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
