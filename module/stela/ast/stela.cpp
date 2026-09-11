#include "stela/ast/stela.hpp"
#include "stela/ast/nulStela.hpp"
#include "stela/visitor/stelaVisitor.hpp"

namespace by {

    namespace {
        static nulStela inner;
    }

    BY(DEF_ME(stela), DEF_VISIT())

    me::stela(std::initializer_list<me*> subs, const std::string& name): _name(name) { add(subs); }

    me::stela(const me& rhs, const std::string& name):
        super(), _subs(rhs._subs), _name(name), _prefix(rhs._prefix), _postfix(rhs._postfix) {}

    me::stela(const std::string& name): _name(name) {}

    me& me::operator[](const std::string& name) { return sub(name); }

    me& me::operator[](const nchar* name) { return sub(name); }

    me::operator nbool() const { return isExist(); }

    me& me::sub(const std::string& name) {
        auto found = _subs.find(name);
        WHEN(found.isEnd()) .ret(inner);

        me& ret = found->get() OR.ret(inner);
        return ret;
    }

    me& me::sub(const nchar* name) {
        WHEN_NUL(name).ret(inner);
        return sub(std::string(name));
    }

    me& me::sub(nidx n) {
        if(n < 0 || n >= _subs.size()) return inner;
        return *(begin() + n)->get();
    }

    nbool me::has(const std::string& name) const { return !_subs.find(name).isEnd(); }

    void me::add(const stela& new1) { _subs.insert(new1.getName(), tstr<me>(new1)); }

    void me::add(std::initializer_list<stela*> subs) {
        for(auto e: subs)
            add(*e);
    }

    void me::del(const stela& it) { del(it.getName()); }

    void me::del(const std::string& name) { _subs.erase(name); }

    void me::del(const nchar* name) {
        WHEN_NUL(name).ret();
        _subs.erase(std::string(name));
    }

    const std::string& me::getPrefix() const { return _prefix; }

    void me::setPrefix(const std::string& newPrefix) { _prefix = newPrefix; }

    void me::setPrefix(const nchar* newPrefix) {
        WHEN_NUL(newPrefix).ret();
        _prefix = newPrefix;
    }

    const std::string& me::getPostfix() const { return _postfix; }

    void me::setPostfix(const std::string& newPostfix) { _postfix = newPostfix; }

    void me::setPostfix(const nchar* newPostfix) {
        WHEN_NUL(newPostfix).ret();
        _postfix = newPostfix;
    }

    const std::string& me::getName() const { return _name; }

    void me::setName(const std::string& newName) { _name = newName; }

    void me::setName(const nchar* newName) {
        WHEN_NUL(newName).ret();
        _name = newName;
    }

    ncnt me::len() const { return _subs.size(); }

    me::iterator me::begin() { return _subs.begin(); }

    me::iterator me::end() { return _subs.end(); }

    nint me::asInt() const { return 0; }

    nchar me::asChar() const { return '\0'; }

    const std::string& me::asStr() const {
        static std::string inner;
        return inner;
    }

    nbool me::asBool() const { return false; }

    nbool me::isExist() const { return true; }
} // namespace by
