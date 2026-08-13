/// @file
#pragma once

#include "stela/common.hpp"

namespace by {

    class stela;

    /** @ingroup stela
     *  @brief Traversal context handed to every stelaVisitor callback.
     *  @details Deliberately smaller than byeol's visitInfo: stela's map-of-children
     *  layout has no ordering, so index/len are irrelevant. Only the child's name,
     *  its parent, and its depth (for indentation and structural queries) are kept.
     */
    class _nout stelaVisitInfo {
        BY(ME(stelaVisitInfo))

    public:
        stelaVisitInfo(const std::string& newName, stela* newParent, nint newDepth);

    public:
        std::string name;
        stela* parent;
        nint depth;
    };
} // namespace by
