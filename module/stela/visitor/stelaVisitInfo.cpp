#include "stelaVisitInfo.hpp"

namespace by {
    BY(DEF_ME(stelaVisitInfo))

    me::stelaVisitInfo(const std::string& newName, stela* newParent, nint newDepth):
        name(newName), parent(newParent), depth(newDepth) {}
}
