#include "stelaVisitInfo.hpp"

namespace by {
    BY(DEF_ME(stelaVisitInfo))

    me::stelaVisitInfo(const std::string& newName, stela* newParent, nidx newIndex, ncnt newLen, nint newDepth):
        name(newName), parent(newParent), index(newIndex), len(newLen), depth(newDepth) {}
}
