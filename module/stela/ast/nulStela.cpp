#include "stela/ast/nulStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"

namespace by {

    BY(DEF_ME(nulStela), DEF_VISIT())

    me::nulStela(): super() {}

    const std::string& me::asStr() const {
        static std::string inner = "";
        return inner;
    }

    nchar me::asChar() const { return '\0'; }

    nint me::asInt() const { return 0; }

    nbool me::asBool() const { return false; }

    nbool me::isExist() const { return false; }

    me& me::get() {
        static me inner;
        return inner;
    }
} // namespace by
