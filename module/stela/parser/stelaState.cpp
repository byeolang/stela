#include "stela/parser/stelaState.hpp"
#include "stela/ast/stela.hpp"

namespace by {

    BY_DEF_ME(stelaState)

    nbool me::state::isValid() const { return _dst && !_comment.empty(); }

    nbool me::state::apply() {
        WHEN(!isValid()) .ret(false);

        _dst->setComment(_comment);
        rel();
        return true;
    }

    void me::state::rel() {
        _comment = "";
        _dst.rel();
    }

    me::stelaState() { rel(); }

    void me::rel() { _states.assign(1, state{}); }

    void me::onComment(ncnt depth, const nchar* text) {
        WHEN_NUL(text).ret();

        _getLatest(depth).setComment(text);
    }

    stela& me::onLeave(ncnt depth, stela& it) {
        _getLatest(depth).setDst(it);
        return it;
    }

    void me::onTokenIndentOrDedent(ncnt depth) {
        _states.resize(depth); // root stmt started at depth 1.
    }

    me::state& me::_getLatest(ncnt depth) {
        onTokenIndentOrDedent(depth);
        return _states.back();
    }

} // namespace by
