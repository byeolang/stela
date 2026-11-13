#include "stela/parser/stelaState.hpp"
#include "stela/ast/stela.hpp"

namespace by {

    BY_DEF_ME(stelaState)

    nbool me::state::isValid() const { return dst || !comment.empty(); }

    nbool me::state::apply() {
        WHEN(isValid()).ret(false);

        dst->setComment(comment);
        return true;
    }

    me::stelaState() { rel(); }

    void me::rel() {
        _states.assign(1, state{});
    }

    void me::onComment(const nchar* text) {
        WHEN_NUL(text).ret();

        _getLatest().comment = text;
    }

    void me::onNewLine() {
        state& latest = _getLatest();
        WHEN(!latest.isValid()).ret();

        if(!latest.apply())
            _states.push_back(state{}); // keep this state and add new state for next code line to parse.
        else
            _states.pop_back();
    }

    stela& me::onLeave(stela& it) {
        _getLatest().dst.bind(it);
        return it;
    }

    me::state& me::_getLatest() {
        if(_states.empty()) _states.push_back(state{});

        return _states.back();
    }

} // namespace by
