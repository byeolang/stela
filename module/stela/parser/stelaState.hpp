/// @file
#pragma once

#include "stela/common.hpp"

namespace by {
    class stela;

    /** @ingroup stela
     *  @brief The state a construct carries across the gap between the scanner and
     *         bison's reductions.
     *  @details The scanner runs ahead of the reductions, so when a comment is scanned
     *  the node it belongs to does not exist yet. That pending state lives here as one
     *  @ref comment stack plus a cursor, instead of a flag per construct spread across
     *  @ref stelaParser:
     *
     *   - @ref location tracks where the scanner sits on the line, so a comment routes
     *     to a prefix or a postfix.
     *   - the stack mirrors nesting: entering a `def` or an assignment pushes a frame,
     *     reducing it pops the frame onto the node -- both the same way.
     */
    class _nout stelaState {
        BY(ME(stelaState))

    public:
        class _nout state {
        public:
            nbool isValid() const;
            nbool apply();
            void rel();

            const std::string& getComment() const;
            void setComment(const std::string& new1);

            const stela* getDst() const;
            void setDst(const stela& new1);

        private:
            std::string _comment;
            tweak<stela> _dst;
        };

    public:
        stelaState();

    public:
        // scanner events:
        void onComment(ncnt depth, const nchar* text);

        void onTokenNewLine(ncnt depth) {
            state& latest = _getLatest(depth);
            if(latest.getComment().empty()) latest.rel();
        }

        // reductions:
        /** @brief Enters a construct whose node reduces later, stashing its prefix. */
        stela& onLeave(ncnt depth, stela& toApply);
        stela* onLeave(ncnt depth, stela* it) BY_SIDE_FUNC(it, &onLeave(depth, *it), it);

        void onTokenIndentOrDedent(ncnt depth);
        void rel();

    private:
        state& _getLatest(ncnt depth);

    private:
        std::vector<state> _states;
    };
} // namespace by
