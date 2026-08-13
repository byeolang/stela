#include "stela/visitor/stelaVisitor.hpp"
#include "stela/ast.hpp"

namespace by {

    BY_DEF_ME(stelaVisitor)

    me::stelaVisitor() {}

    me::~stelaVisitor() {}

    void me::work(stela& root) {
        _visited.clear();
        root.accept(stelaVisitInfo(root.getName(), nullptr, 0), *this);
    }

#define X(T)                                                                                 \
    void me::visit(const stelaVisitInfo& i, T& it) {                                         \
        WHEN(!_markVisited(it)) .ret();                                                      \
        if(onVisit(i, it)) onTraverse(i, it);                                                \
        onLeave(i, it);                                                                      \
    }                                                                                        \
                                                                                             \
    nbool me::onVisit(const stelaVisitInfo& i, T& it) { return onVisit(i, (T::super&) it); } \
                                                                                             \
    void me::onLeave(const stelaVisitInfo& i, T& it) { onLeave(i, (T::super&) it); }

#include "visitee.inl"
#undef X

    void me::visit(const stelaVisitInfo& i, stela& it) {
        WHEN(!_markVisited(it)) .ret();
        if(onVisit(i, it)) onTraverse(i, it);
        onLeave(i, it);
    }

    nbool me::onVisit(const stelaVisitInfo& i, stela& it) { return true; }

    void me::onLeave(const stelaVisitInfo& i, stela& it) {}

    void me::onTraverse(const stelaVisitInfo& i, stela& it) {
        for(auto e = it.begin(); e != it.end(); ++e) {
            stela& child = e->second.get() OR_CONTINUE;
            child.accept(stelaVisitInfo(child.getName(), &it, i.depth + 1), *this);
        }
    }

    nbool me::_markVisited(stela& it) {
        WHEN(_visited.find(&it) != _visited.end()) .ret(false);
        return _visited[&it] = true;
    }
} // namespace by
