#include "stela/visitor/writer/stelaWriter.hpp"
#include "stela/ast.hpp"
#include <fstream>

namespace by {

    BY(DEF_ME(stelaWriter))

    std::string me::write(stela& root) {
        _ss.str("");
        _ss.clear();

        work(root);
        return _ss.str();
    }

    nbool me::writeFile(stela& root, const std::string& path) {
        std::ofstream fout(path);
        if(!fout.is_open()) {
            BY_I("stelaWriter::writeFile: cannot open '%s' for writing", path.c_str());
            return false;
        }
        fout << write(root);
        return fout.good();
    }

    nbool me::onVisit(const stelaVisitInfo& i, defStela& it) {
        // depth 0 is the compilation-unit root — never emitted; only descended into.
        WHEN(i.depth == 0) .ret(true);
        _ss << _indent(i.depth) << "def " << it.getName() << "\n";
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, valStela& it) {
        _open(i, it) << it.asStr();
        _close(i);
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, strStela& it) {
        _open(i, it) << "\"" << it.asStr() << "\"";
        _close(i);
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, arrStela& it) {
        _open(i, it) << "{";
        return true;
    }

    void me::onLeave(const stelaVisitInfo& i, arrStela& it) {
        _ss << "}";
        _close(i);
    }

    std::stringstream& me::_open(const stelaVisitInfo& i, stela& it) {
        // an element has no name of its own: a comma separates it from the previous one.
        if(i.parent TO(template cast<arrStela>())) {
            if(i.index) _ss << ", ";
            return _ss;
        }

        _ss << _indent(i.depth) << it.getName() << " := ";
        return _ss;
    }

    void me::_close(const stelaVisitInfo& i) {
        WHEN(i.parent TO(template cast<arrStela>())) .ret();
        _ss << "\n";
    }

    std::string me::_indent(nint depth) {
        // root is skipped, so a top-level child (depth 1) sits at column 0.
        return std::string((depth - 1) * 4, ' ');
    }
} // namespace by
