/// @file
#pragma once

#include "stela/common.hpp"
#include "stela/ast/comment.hpp"

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

        enum location {
            PREFIX,
            CODE,
            POSTFIX,
        };

        struct state {
            std::string prefix;
            std::string postfix;
        };

    public:
        stelaState();

    public:
        // scanner events:
        void onStartComment(const nchar* marker, ncnt col);
        void onComment(const nchar* text);
        /** @brief A content token passed. @p isSeparator marks a `,` or `}`. */
        void onCode(nbool isSeparator);
        /** 
            @brief A line ended. 
            @param ignoreWhitespace this will be true if parser met whitespaces when it's ignoreing whitespace.
         */
        void onNewLine(nbool ignoreWhitespace);

        // reductions:
        /** @brief Enters a construct whose node reduces later, stashing its prefix. */
        void onVisitInnerNode();
        /** @brief Stamps the pending prefix onto a value node the moment it is born. */
        void born(stela& val);
        /** @brief Leaves the innermost construct, handing back its comment. */
        comment onLeaveInnerNode();

        std::string takePending();
        std::string takePostfix();

        void rel();

    private:
        // _stack.front(): the base scope, doubling as the buffer for
        // comments not yet claimed by an entered construct
        state& _getCurrentState(); 

    private:
        std::vector<state> _states; // open scopes; front() is the base, entering pushes above it
        location _loc;
        ncnt _commentCol;
    };
} // namespace by
