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
        struct state {
            nbool isValid() const;
            nbool apply();

            std::string comment;
            tweak<stela> dst;
        };

    public:
        stelaState();

    public:
        // scanner events:
        void onComment(const nchar* text);

        /**
            @brief A line ended.
            @param ignoreWhitespace this will be true if parser met whitespaces when it's ignoreing whitespace.
         */
        void onNewLine();

        // reductions:
        /** @brief Enters a construct whose node reduces later, stashing its prefix. */
        stela& onLeave(stela& toApply);
        stela* onLeave(stela* it) BY_SIDE_FUNC(it, onLeave(it), it);

        void rel();

    private:
        state& _getLatest();

    private:
        std::vector<state> _states;
    };
} // namespace by
