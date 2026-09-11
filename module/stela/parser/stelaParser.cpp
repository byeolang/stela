#include "stela/parser/stelaParser.hpp"
#include "stela/ast.hpp"
#include "stela/parser/bison/stelaLowparser.hpp"
#include "stela/parser/bison/stelaLowscanner.hpp"
#include "stela/parser/bison/stelaTokenScan.hpp"
#include <algorithm>
#include <cctype>

namespace by {

    using std::string;

    BY_DEF_ME(stelaParser)

    nint me::_onScan(ZZSTYPE* val, ZZLTYPE* loc, zzscan_t scanner) {
        int tok = _mode->onScan(*this, val, loc, scanner);
        if(_isIgnoreWhitespace && tok == NEWLINE) return SCAN_AGAIN;
        _isIgnoreWhitespace = false;

        switch(tok) {
            case SCAN_MODE_NORMAL: setScan<stelaNormalScan>(); return SCAN_AGAIN;
            case SCAN_MODE_INDENT: setScan<stelaIndentScan>(); return SCAN_AGAIN;
            case SCAN_MODE_INDENT_IGNORE: _isIgnoreWhitespace = true; return SCAN_AGAIN;
            case SCAN_MODE_END: tok = 0; // == zzterminate();
        }

        if(_isLineContent(tok)) {
            // a comment with more of the same line after it is the prefix of what follows rather
            // than the postfix of what came before. before `,` or `}` it is the postfix of the element those
            // close instead, and _addElem() takes it.
            if(_isTokenInLine && tok != ',' && tok != '}') _prefix += _postfix, _postfix.clear();
            _isTokenInLine = true;
        }

        return tok;
    }

    nint me::onScan(stelaParser& ev, ZZSTYPE* val, ZZLTYPE* loc, zzscan_t scanner, nbool& isBypass) {
        int tok;
        do
            // why do you put redundant _onScan() func?:
            //  because of definately, clang++ bug. when I use continue at switch statement inside
            //  of do-while loop here, it doesn't work like usual 'continue' keyword does, but it
            //  does like 'break'.
            tok = _onScan(val, loc, scanner);
        while(tok == SCAN_AGAIN);

        return tok;
    }

    nint me::onTokenEndOfFile() {
        BY_DI("tokenEvent: onTokenEndOfFile() indents.size()=%d", _indents.size());
        if(_indents.size() <= 1) _dispatcher.add(SCAN_MODE_END);
        else _dispatcher.addFront(onDedent(_indents.front(), SCAN_MODE_END));

        BY_DI("tokenEvent: onEndOfFile: finalize by adding 'NEWLINE', then dispatch end-of-file.");
        return NEWLINE;
    }

    nint me::onTokenColon(nint tok) {
        _dedent.useLater();
        return tok;
    }

    nint me::onTokenNewLine(nint tok) {
        BY_DI("tokenEvent: onNewLine: _isIgnoreWhitespace=%s, _indents.size()=%d", _isIgnoreWhitespace,
            _indents.size());
        if(_isDefHeader && !_defComments.empty()) {
            _defComments.back().second = _postfix;
            _postfix.clear();
            _isDefHeader = false;
        }
        // a swallowed newline does not end the statement, so the comment before it is the prefix of
        // what follows.
        if(_isIgnoreWhitespace) _prefix += _postfix, _postfix.clear();
        _isTokenInLine = false;

        if(!_isIgnoreWhitespace && _indents.size() >= 1) _dispatcher.add(SCAN_MODE_INDENT);
        _dedent.rel();
        return tok;
    }

    nbool me::_isLineContent(nint tok) {
        switch(tok) {
            case SCAN_AGAIN:
            case SCAN_EXIT:
            case SCAN_MODE_NORMAL:
            case SCAN_MODE_INDENT:
            case SCAN_MODE_INDENT_IGNORE:
            case SCAN_MODE_END:
            case NEWLINE:
            case INDENT:
            case DEDENT:
            case ENDOFFILE: return false;
        }
        return tok > 0;
    }

    void me::onComment(const nchar* text) {
        WHEN_NUL(text).ret();

        std::string& com = _isTokenInLine ? _postfix : _prefix;
        for(ncnt skip = 0; *text; ++text) {
            if(skip && *text == ' ') {
                --skip;
                continue;
            }
            skip = *text == '\n' ? _commentCol : 0;
            com += *text;
        }
    }

    void me::onCommentBegin(const nchar* text, ncnt col) {
        if(!_isTokenInLine && !_prefix.empty()) _prefix += "\n";
        // a postfix is written back as is, so only a prefix drops the absolute indentation.
        _commentCol = _isTokenInLine ? 0 : col;
        onComment(text);
    }

    nint me::onTokenDef(nint tok) {
        _defComments.emplace_back(_prefix, "");
        _prefix.clear();
        _isDefHeader = true;
        return tok;
    }

    nint me::onTokenDefAssign(nint tok) {
        _assignComments.push_back(_prefix);
        _prefix.clear();
        return tok;
    }

    stela* me::_bornVal(stela* val) {
        WHEN_NUL(val).ret(val);

        val->setPrefix(_prefix);
        _prefix.clear();
        return val;
    }

    nint me::onTokenComma(nint tok) { return _onTokenEndOfInlineBlock(onIgnoreIndent(tok)); }

    stela* me::onDefAssign(const std::string& name, stela* rhs) {
        // popped before the null check, or a bad rhs would shift every later statement's comment.
        std::string prefix = std::move(_assignComments.back());
        _assignComments.pop_back();
        WHEN_NUL(rhs).err("rhs is nul").ret(nullptr);

        rhs->setName(name);
        // a comment between `:=` and the value already went to the value.
        const std::string& valPrefix = rhs->getPrefix();
        if(!valPrefix.empty()) prefix += (prefix.empty() ? "" : "\n") + valPrefix;
        rhs->setPrefix(prefix);
        rhs->setPostfix(_postfix);
        _postfix.clear();
        return rhs;
    }

    nint me::_onTokenEndOfInlineBlock(nint tok) {
        WHEN(!_dedent.canDedent()) .ret(tok);

        BY_DI("tokenEvent: onTokenEndOfInlineBlock: '%c' [%d] use smart dedent!", (char) tok, tok);
        _dispatcher.addFront(tok);
        return _dedent.dedent();
    }

    nint me::onIndent(ncnt col, nint tok) {
        BY_DI("tokenEvent: onIndent(col: %d, tok: %d) indents.size()=%d", col, tok, _indents.size());
        _indents.push_back(col);
        _dispatcher.add(tok);
        return INDENT;
    }

    nint me::onDedent(ncnt col, nint tok) {
        BY_DI("tokenEvent: onDedent(col: %d, tok: %d) indents.size()=%d", col, tok, _indents.size());

        _indents.pop_back();
        nint now = _indents.back();
        if(now < col) report("wrong dedent lv");

        while(_indents.back() > col) {
            BY_DI("tokenEvent: onDedent: indentlv become %d -> %d", _indents.back(),
                _indents.size() > 1 ? _indents[_indents.size() - 2] : -1);
            _dispatcher.add(DEDENT);
            _indents.pop_back();
            if(_indents.size() <= 0) break;
        }

        _dispatcher.add(tok);
        return DEDENT;
    }

    nchar me::onScanUnexpected(const nchar* token) {
        report("unexpected_tok");
        return token[0];
    }

    nint me::onIgnoreIndent(nint tok) {
        _dispatcher.add(SCAN_MODE_INDENT_IGNORE);
        return tok;
    }

    stela* me::onDefBlock() { return new defStela(); }

    stela* me::onDefBlock(stela* stmt) { return onDefBlock(onDefBlock(), stmt); }

    stela* me::onDefBlock(stela* s, stela* stmt) {
        WHEN_NUL(s).err("s is nul").ret(new defStela());
        WHEN_NUL(stmt).err("stmt is nul").ret(s);

        s->add(stmt);
        return s;
    }

    verStela* me::onVer(const std::string& version) { return _bornVal(new verStela(version))->cast<verStela>(); }

    stela* me::onInt(const std::string& repr) {
        nint val = 0;
        try {
            val = std::stoi(repr, nullptr, 0);
        } catch(const std::exception&) {
            // a literal wider than nint. report it rather than let it escape parse().
            report("int literal out of range");
        }
        return _bornNum(new numStela(val), repr);
    }

    stela* me::onFlt(const std::string& repr) { return _bornNum(new numStela((nflt) std::atof(repr.c_str())), repr); }

    stela* me::_bornNum(numStela* num, const std::string& repr) {
        num->_repr = repr;
        return _bornVal(num);
    }

    stela* me::onDefProp(const std::string& name, stela& rhs) {
        rhs.setName(name);
        return &rhs;
    }

    stela* me::onDefArray() { return _bornVal(new arrStela()); }

    stela* me::onDefArray(stela& elem) {
        arrStela* ret = new arrStela();
        _addElem(*ret, elem);
        return ret;
    }

    stela* me::onDefArray(stela& as, stela& elem) {
        _addElem(as, elem);
        return &as;
    }

    void me::_addElem(stela& arr, stela& elem) {
        // elements arrive nameless: add() would key them all on "" and each would
        // overwrite the last.
        elem.setName(std::to_string(arr.len()));
        elem.setPostfix(_postfix);
        _postfix.clear();
        arr.add(elem);
    }

    stela* me::onDefOrigin(const std::string& name, stela& blk) {
        blk.setName(name);
        if(!_defComments.empty()) {
            blk.setPrefix(_defComments.back().first);
            blk.setPostfix(_defComments.back().second);
            _defComments.pop_back();
        }
        return &blk;
    }

    stela* me::onCompilationUnit(stela* subpod) {
        WHEN_NUL(subpod).err("subpod is null").ret(nullptr);

        // the grammar cannot tell the outermost block from a nested one, so it hands
        // over an ordinary defStela. binding it first gives it an owner, and the copy
        // adopts its children before the rebind drops it.
        _root.bind(subpod);
        rootStela* ret = new rootStela(*subpod, "root");
        // whatever is still pending comes after every node, so it closes the file.
        ret->setPostfix(_prefix);
        _prefix.clear();
        _root.bind(ret);
        return ret;
    }

    void me::onParseErr(const std::string& msg, const nchar* symbolName) { report(msg + " -> " + symbolName); }

    me::stelaParser(): _mode(nullptr), _isIgnoreWhitespace(false), _isTokenInLine(false), _isDefHeader(false) { rel(); }

    stelaTokenDispatcher& me::getDispatcher() { return _dispatcher; }

    std::vector<ncnt>& me::getIndents() { return _indents; }

    tstr<stela> me::parseFromFile(const std::string& path) {
        std::ifstream fout(path);
        if(fout.fail()) {
            // there is no file.
            report(path + " not exist.");
            return nullptr;
        }

        std::stringstream buf;
        buf << fout.rdbuf();
        return parse(buf.str());
    }

    tstr<stela> me::parseFromFile(const nchar* path) {
        WHEN_NUL(path).ret(nullptr);
        return parseFromFile(std::string(path));
    }

    std::string me::_removeCRLF(const std::string& codes) {
        size_t from = 0, at = codes.find("\r\n");
        WHEN(at == std::string::npos) .ret(codes);

        std::string ret;
        ret.reserve(codes.size());
        for(; at != std::string::npos; at = codes.find("\r\n", from)) {
            ret.append(codes, from, at - from);
            from = at + 1; // keeps the LF.
        }
        ret.append(codes, from, std::string::npos);

        return ret;
    }

    tstr<stela> me::parse(const std::string& codes) {
        _prepare();

        zzscan_t scanner;
        zzlex_init_extra(this, &scanner);

        const std::string stripped = _removeCRLF(codes);
        yy_buffer_state& bufState = (YY_BUFFER_STATE) _scanString(stripped.c_str(), scanner) OR.ret(nullptr);

        // fix Flex Bug here:
        //  when zz_scan_string get called, it returns bufState after malloc it.
        //  but some variables wasn't initialized. zz_bs_lineno(used to calculate
        //  current cursor position) is one of them.
        bufState.yy_bs_lineno = bufState.yy_bs_column = 0;
        zz_switch_to_buffer(&bufState, scanner);

#if ZZDEBUG
        // zzset_debug(1, scanner); // For Flex (no longer a global, but rather a member of)
        // zzdebug = 1;             // For Bison (still global, even in a reentrant)
#endif

        int res = zzparse(scanner);
        if(res) report("parsing has error");

        zz_delete_buffer(&bufState, scanner);
        zzlex_destroy(scanner);

        return _finalize();
    }

    tstr<stela> me::parse(const nchar* codes) {
        WHEN_NUL(codes).ret(nullptr);
        return parse(std::string(codes));
    }

    nbool me::isInit() const { return _mode; }

    void me::rel() {
        _states.clear();
        _states.push_back(0); // 0 for default state
        _dedent.rel();
        _prepare();
    }

    void me::_prepare() {
        _mode = nullptr;
        _root.rel();
        _isIgnoreWhitespace = false;
        _dispatcher.rel();
        _indents.clear();
        _prefix.clear();
        _postfix.clear();
        _isTokenInLine = false;
        _isDefHeader = false;
        _defComments.clear();
        _assignComments.clear();
        _commentCol = 0;
    }

    int me::pushState(int newState) {
        BY_I("push state %d -> %d", _states.back(), newState);
        _states.push_back(newState);
        return _states.back();
    }

    int me::popState() {
        int previous = _states.back();
        _states.pop_back();
        BY_I("pop state %d -> %d", previous, _states.back());
        return _states.back();
    }

    void me::report(const std::string& msg) { _errs.push_back(msg); }

    void* me::_scanString(const nchar* src, void* scanner) {
        if(!src || src[0] == '\0') {
            report("src is empty");
            return nullptr;
        }

        return zz_scan_string((nchar*) src, (zzscan_t) scanner);
    }

    tstr<stela> me::_finalize() {
        ncnt size = _errs.size();
        WHEN(size <= 0) .ret(_root);

        BY_I("stela: total %d errors found.", size);
        for(const auto& e: _errs)
            BY_I("stela: ERR: %s", e);
        return nullptr;
    }
} // namespace by
