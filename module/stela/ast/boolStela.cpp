#include "stela/ast/boolStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"

namespace by {
    BY(DEF_ME(boolStela, valStela), DEF_VISIT())

    me::boolStela(nbool val, const std::string& name): super(val, name) {}

    me::boolStela(const std::string& rawVal, const std::string& name): super(rawVal, name) {}

    me::boolStela(const nchar* rawVal, const std::string& name): super(rawVal, name) {}
} // namespace by
