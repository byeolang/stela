#include "stela/ast/numStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"

namespace by {
    BY(DEF_ME(numStela, valStela), DEF_VISIT())

    me::numStela(nint val, const std::string& name): super(val, name) {}

    me::numStela(nflt val, const std::string& name): super(val, name) {}

    me::numStela(const std::string& rawVal, const std::string& name): super(rawVal, name) {}

    me::numStela(const nchar* rawVal, const std::string& name): super(rawVal, name) {}
} // namespace by
