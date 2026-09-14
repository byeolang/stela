#include "stela/parser/stelaState.hpp"
#include "stela/ast.hpp"

namespace by {

    BY_DEF_ME(stelaState)

    me::stelaState() { rel(); }

    void me::rel() {
        _states.assign(1, comment());
        _loc = PREFIX;
        _commentCol = 0;
    }

    comment& me::_getCurrentState() { return _states.front(); }

    void me::onStartComment(const nchar* marker, ncnt col) {
        // consecutive prefix lines join with a newline; a postfix is written back verbatim,
        // so only a prefix drops its absolute indentation.
        auto& state = _getCurrentState();
        if(_loc == PREFIX && !state.prefix.empty()) state.prefix += "\n";
        _commentCol = _loc == PREFIX ? col : 0;
        onComment(marker);
    }

    void me::onComment(const nchar* text) {
        WHEN_NUL(text).ret();

        auto& state = _getCurrentState();
        std::string& dst = _loc == PREFIX ? state.prefix : state.postfix;
        for(ncnt skip = 0; *text; ++text) {
            // a `##` block keeps each later line's offset from the marker, not its column:
            // the writer re-indents to the node's depth.
            if(skip && *text == ' ') {
                --skip;
                continue;
            }
            skip = *text == '\n' ? _commentCol : 0;
            dst += *text;
        }
        if(_loc == CODE) _loc = POSTFIX;
    }

    void me::onCode(nbool isSeparator) {
        _loc = CODE;
        WHEN(_loc != POSTFIX || isSeparator).ret();

        // a comment between two bits of code (`{1, ## c ## 3}`) belongs to what follows, not
        // to what came before -- except before a `,` or `}`, whose element takes it.
        auto& state = _getCurrentState();
        state.prefix += state.postfix;
        state.postfix.clear();
    }

    void me::onNewLine(nbool ignoreWhitespace) {
        // a swallowed newline (inside `{}`) does not end the statement.
        auto& state = _getCurrentState();
        if(ignoreWhitespace) state.prefix += state.postfix;
        else if(_states.size() > 1) _states.back().postfix += state.postfix;
        state.postfix.clear();
        _loc = PREFIX;
    }

    void me::onVisitInnerNode() {
        // move, not copy: the pending prefix is ejected into the construct's frame. its
        // postfix stays in the buffer -- that belongs to the line, not the construct.
        auto& state = _getCurrentState();
        _states.push_back(comment{std::move(state.prefix), std::string()});
        state.prefix.clear();
    }

    void me::born(stela& val) {
        auto& state = _getCurrentState();
        val.setComment(comment{state.prefix, std::string()});
        state.prefix.clear();
    }

    comment me::onLeaveInnerNode() {
        comment top = std::move(_states.back());
        _states.pop_back();
        // a deferred construct (a `def` header) already holds its trailing comment; an
        // assignment's is still live.
        auto& state = _getCurrentState();
        if(top.postfix.empty()) top.postfix = std::move(state.postfix), state.postfix.clear();
        return top;
    }

    std::string me::takePending() {
        auto& state = _getCurrentState();
        std::string ret = std::move(state.prefix);
        state.prefix.clear();
        return ret;
    }

    std::string me::takePostfix() {
        auto& state = _getCurrentState();
        std::string ret = std::move(state.postfix);
        state.postfix.clear();
        return ret;
    }
} // namespace by
