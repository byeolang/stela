/// @file
#pragma once

#include "stela/common.hpp"

namespace by {

    class stela;

    /** @ingroup stela
     *  @brief Traversal context handed to every stelaVisitor callback.
     *  @details Mirrors byeol's visitInfo, minus `additionalLen`, which has no stela
     *  counterpart. `index` and `len` place the child among its siblings — for an
     *  @ref arrStela, its position in the array — so a visitor never has to rebuild
     *  what @ref stelaVisitor::onTraverse already knows.
     */
    class _nout stelaVisitInfo {
        BY(ME(stelaVisitInfo))

    public:
        stelaVisitInfo(const std::string& newName, stela* newParent, nidx newIndex, ncnt newLen, nint newDepth);

    public:
        std::string name;
        stela* parent;
        nidx index;
        ncnt len;
        nint depth;
    };
} // namespace by
