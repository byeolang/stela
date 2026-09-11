#include "stela/visitor/writer/stelaWriter.hpp"
#include "stela/ast.hpp"
#include <fstream>

namespace by {

    BY(DEF_ME(stelaWriter))

    std::string me::write(stela& root) {
        _ss.str("");
        _ss.clear();

        // a bare block never came from source, so writing a `def` line for it would
        // invent syntax no file ever contained.
        WHEN(!root.cast<rootStela>()) .ret("");

        work(root);
        if(!root.getPostfix().empty()) _ss << root.getPostfix() << "\n";
        return _ss.str();
    }

    nbool me::writeFile(stela& root, const std::string& path) {
        WHEN(!root.cast<rootStela>()) .ret(false);

        std::ofstream fout(path);
        if(!fout.is_open()) {
            BY_I("stelaWriter::writeFile: cannot open '%s' for writing", path.c_str());
            return false;
        }
        fout << write(root);
        return fout.good();
    }

    nbool me::onVisit(const stelaVisitInfo& i, rootStela& it) {
        // no `def` line produced the compilation unit, so none is written back.
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, defStela& it) {
        _writePrefix(i, it);
        _ss << _indent(i.depth) << "def " << it.getName();
        _writePostfix(it);
        _ss << "\n";
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, valStela& it) {
        _open(i, it) << it.getRepr();
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
            if(!it.getPostfix().empty()) _ss << " " << it.getPostfix();
            return;
        }

        _writePostfix(it);
        _ss << "\n";
    }

    void me::_writePrefix(const stelaVisitInfo& i, stela& it) {
        WHEN(it.getPrefix().empty()) .ret();

        // stored without indentation so that a node moving depth takes its comment with it.
        std::stringstream lines(it.getPrefix());
        for(std::string line; std::getline(lines, line);)
            _ss << (line.empty() ? "" : _indent(i.depth)) << line << "\n";
    }

    void me::_writePostfix(stela& it) {
        WHEN(it.getPostfix().empty()) .ret();
        _ss << "  " << it.getPostfix();
    }

    void me::_writeElemPrefix(stela& it) {
        WHEN(it.getPrefix().empty()) .ret();

        // a newline is only legal right after the comma, and a `#` comment runs to the
        // end of its line, so the array has to break there.
        _ss << it.getPrefix() << (it.getPrefix().rfind("##", 0) == 0 ? " " : "\n");
    }

    std::string me::_indent(nint depth) {
        // write() rejects a non-rootStela, so depth is always >= 1 here and the
        // subtraction cannot underflow.
        return std::string((depth - 1) * 4, ' ');
    }
} // namespace by
