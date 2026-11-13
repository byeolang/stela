#include "stela/visitor/writer/stelaWriter.hpp"
#include "stela/ast.hpp"
#include <fstream>

namespace by {

    BY(DEF_ME(stelaWriter))

    std::string me::write(stela& root) {
        _ss.str("");
        _ss.clear();

        WHEN(!root.isRoot() || !root.cast<defStela>())
            .err("only a root block is available for code generation").ret("");

        work(root);
        _ss << root.getComment() << "\n";
        return _ss.str();
    }

    nbool me::writeFile(stela& root, const std::string& path) {
        WHEN(!root.isRoot() || !root.cast<defStela>())
            .err("only a root block is available for code generation").ret(false);

        std::ofstream fout(path);
        if(!fout.is_open()) {
            BY_I("cannot open '%s' for writing", path.c_str());
            return false;
        }
        fout << write(root);
        return fout.good();
    }

    nbool me::onVisit(const stelaVisitInfo& i, defStela& it) {
        WHEN(it.isRoot()) .ret(true); // writer need to omit `def` keyword for root element.

        _ss << _indent(i.depth) << "def " << it.getName() << it.getComment() << "\n";
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, valStela& it) {
        _open(i, it) << it.asStr();
        _close(i, it);
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, strStela& it) {
        _open(i, it) << "\"" << it.asStr() << "\"";
        _close(i, it);
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, arrStela& it) {
        _open(i, it) << "{";
        return true;
    }

    void me::onLeave(const stelaVisitInfo& i, arrStela& it) {
        _ss << "}";
        _close(i, it);
    }

    std::stringstream& me::_open(const stelaVisitInfo& i, stela& it) {
        // an element has no name of its own: a comma separates it from the previous one.
        if(i.parent TO(template cast<arrStela>())) {
            if(i.index) _ss << ", ";
            _writeElemPrefix(it);
            return _ss;
        }

        _writePrefix(i, it);
        _ss << _indent(i.depth) << it.getName() << " := ";
        return _ss;
    }

    void me::_close(const stelaVisitInfo& i, stela& it) {
        if(i.parent TO(template cast<arrStela>())) {
            // only a `##` comment can be the postfix of an element: a newline is illegal before `,` or `}`.
            _ss << " " << it.getComment();
            return;
        }

        _writePostfix(it);
        _ss << "\n";
    }

    std::string me::_indent(nint depth) {
        // write() rejects any stela instance which is not root. so depth is always >= 1 here and the
        // subtraction cannot underflow.
        return std::string((depth - 1) * 4, ' ');
    }
} // namespace by
