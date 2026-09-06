#include "stela/ast/strStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"

namespace by {
    BY(DEF_ME(strStela, valStela), DEF_VISIT())

    me::strStela(const std::string& rawVal, const std::string& name): super(rawVal, name) {}

    me::strStela(const nchar* rawVal, const std::string& name): super(rawVal, name) {}
} // namespace by
