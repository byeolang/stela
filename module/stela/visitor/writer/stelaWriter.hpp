/// @file
#pragma once

#include "stela/visitor/stelaVisitor.hpp"

namespace by {

    /** @ingroup stela
     *  @brief Serializes a stela tree back to stela source text.
     *  @details Implemented as a @ref stelaVisitor. writes a canonical, lossy form.
     *  What is NOT preserved:
     *   - comments in the original file
     *   - original whitespace, blank lines, and indentation width beyond the writer's
     *     configured indent
     *   - the original insertion order of children (`std::map` inside @ref stela sorts
     *     them alphabetically)
     *   - the exact literal spelling of numbers (e.g. `1_000` becomes `1000`)
     *
     *  What IS preserved: values and structure. Round-trip
     *  `parse -> edit -> write -> parse` gives the same value tree, and
     *  `write -> parse -> write` produces the same string (canonical fixed point).
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
        // numStela, boolStela and verStela all render bare, so one override on
        // their shared base covers the three of them.
        nbool onVisit(const stelaVisitInfo& i, valStela& it) override;
        nbool onVisit(const stelaVisitInfo& i, strStela& it) override;
        nbool onVisit(const stelaVisitInfo& i, arrStela& it) override;
        void onLeave(const stelaVisitInfo& i, arrStela& it) override;

    private:
        std::string _indent(nint depth);
        std::stringstream& _open(const stelaVisitInfo& i, stela& it);
        void _close(const stelaVisitInfo& i);

    private:
        std::stringstream _ss;
    };
} // namespace by
