/// @file
#pragma once

#include "stela/visitor/stelaVisitor.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Serializes a stela tree back to stela source text.
     *  @details Implemented as a @ref stelaVisitor. writes a canonical form.
     *
     *  What IS preserved: values, structure, child order, comments, and the spelling of
     *  number literals (`0xFF` stays `0xFF`). Round-trip
     *  `parse -> edit -> write -> parse` gives the same value tree, and
     *  `write -> parse -> write` produces the same string (canonical fixed point).
     *
     *  What is NOT preserved:
     *   - original indentation width and any hand alignment: indentation is generated
     *     from depth, so a node inserted into a hand-written block still lands right
     *   - blank lines
     *   - where a comment closing a block sits: it becomes the prefix of the node after the block
     *
     *  @section root_handling Root handling
     *  The writer serializes a compilation unit, so it accepts only a @ref rootStela:
     *  handing write() or writeFile() any other node is rejected (empty string / false).
     *  The @ref rootStela itself is never emitted as `def <name>` — only its children
     *  are — matching the shape of stela source, where the top level is implicit.
     *
     *  @section usage Usage
     *  @code
     *      std::string src = stelaWriter().write(*root);
     *      stelaWriter().writeFile(*root, "out.by");
     *  @endcode
     */
    class _nout stelaWriter: public stelaVisitor {
        BY(ADT(stelaWriter, stelaVisitor))

    public:
        /**
         * @brief Serializes @p root and its subtree to a string.
         */
        std::string write(stela& root);
        /**
         * @brief Serializes @p root and writes the result to @p path (LF, UTF-8, non-atomic).
         * @return true on success. false if the file could not be opened for writing.
         */
        nbool writeFile(stela& root, const std::string& path);
        nbool writeFile(stela& root, const nchar* path) BY_SIDE_FUNC(path, writeFile(root, std::string(path)), false);

    protected:
        nbool onVisit(const stelaVisitInfo& i, rootStela& it) override;
        nbool onVisit(const stelaVisitInfo& i, defStela& it) override;
        nbool onVisit(const stelaVisitInfo& i, valStela& it) override;
        nbool onVisit(const stelaVisitInfo& i, strStela& it) override;
        nbool onVisit(const stelaVisitInfo& i, arrStela& it) override;
        void onLeave(const stelaVisitInfo& i, arrStela& it) override;

    private:
        std::string _indent(nint depth);
        std::stringstream& _open(const stelaVisitInfo& i, stela& it);
        void _close(const stelaVisitInfo& i, stela& it);
        void _writePrefix(const stelaVisitInfo& i, stela& it);
        void _writePostfix(stela& it);
        void _writeElemPrefix(stela& it);

    private:
        std::stringstream _ss;
    };
} // namespace by
