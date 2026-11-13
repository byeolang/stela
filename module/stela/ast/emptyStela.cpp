#include "stela/ast/emptyStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"

namespace by {

    BY(DEF_ME(emptyStela), DEF_VISIT())

    const std::string& me::asStr() const {
        static std::string inner = "";
        return inner;
    }

    nchar me::asChar() const { return '\0'; }

    nint me::asInt() const { return 0; }

    nbool me::asBool() const { return false; }

} // namespace by
