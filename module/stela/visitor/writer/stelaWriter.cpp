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
            BY_I("stelaWriter::emitToFile: cannot open '%s' for writing", path.c_str());
            return false;
        }
        fout << write(root);
        return fout.good();
    }

    nbool me::onVisit(const stelaVisitInfo& i, stela& it) {
        // depth 0 is the compilation-unit root — never emitted; only descended into.
        WHEN(i.depth == 0).ret(true);
        _ss << _indent(i.depth) << "def " << it.getName() << "\n";
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, valStela& it) {
        _ss << _indent(i.depth) << it.getName() << " := " << it.asStr() << "\n";
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, strStela& it) {
        _ss << _indent(i.depth) << it.getName() << " := \"" << it.asStr() << "\"\n";
        return true;
    }

    nbool me::onVisit(const stelaVisitInfo& i, verStela& it) {
        _ss << _indent(i.depth) << it.getName() << " := " << it.asStr() << "\n";
        return true;
    }

    std::string me::_indent(nint depth) {
        // root is skipped, so a top-level child (depth 1) sits at column 0.
        return std::string((depth - 1) * 4, ' ');
    }
} // namespace by
