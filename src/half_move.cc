/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
 *
 *
 * Miniature is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Miniature is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Miniature. If not, see <http://www.gnu.org/licenses/>.
 */

#include "half_move.h"
#include "graphics_board_item.h"

namespace Miniature
{

mHalfMove::
mHalfMove(const MPosition& position)
: m_position(position),
  m_position_origin(position),
  m_origin(QPoint(-1, -1)),
  m_target(QPoint(-1, -1)),
  m_captured_piece(0),
  m_promotion(0),
  m_selected_piece(0),
  m_logic_analyzer(0)
{}

mHalfMove::
mHalfMove(const MPosition& position, const QPoint &origin, const QPoint &target)
: m_position(position),
  m_position_origin(position),
  //m_origin(origin),
  m_target(target),
  m_captured_piece(0),
  m_promotion(0),
  m_selected_piece(0),
  m_logic_analyzer(0)
{
    if(!select(origin))
    {
        qFatal("Invalid position or origin parameters in ctor call.");
    }
}

mHalfMove::
~mHalfMove()
{}

const MPosition& mHalfMove::
getPosition() const
{
    return m_position;
}

bool mHalfMove::
select(const QPoint &origin)
{
    MPiece *piece = m_position.pieceAt(origin);
    if(piece)
    {
        m_origin = origin;
        m_selected_piece = piece;
        m_selected_piece->select();

        return true;
    }

    return false;
}

void mHalfMove::
deSelect()
{
    if (m_promotion)
    {
        m_promotion->deSelect();
        m_promotion = 0;
    }

    if (m_selected_piece)
    {
        m_selected_piece->deSelect();
        m_selected_piece->hideGhost();
        m_selected_piece = 0;
    }
}

bool mHalfMove::
applyToTarget(const QPoint &target)
{
    m_target = target;
    return apply();
}

bool mHalfMove::
apply()
{
    // With m_selected_piece we also assume that select() was called on it.
    Q_ASSERT(m_selected_piece && isValidOrigin() && isValidTarget());

    undo();
    // Let's treat cancelling of a move as a special move that needs no logic check ...
    if (m_origin == m_target)
    {
        return false;
    }

    MLogicAnalyzer::MStateFlags result = m_logic_analyzer.verifyMove(&m_position, m_origin, m_target);
    if (MLogicAnalyzer::VALID & result)
    {

        // TODO: check for capture flag instead.
        MPiece *maybe_captured = m_position.pieceAt(m_target);
        if (maybe_captured)
        {
            m_captured_piece = maybe_captured;
            m_captured_piece->hide();
        }

        m_position.movePiece(m_origin, m_target);
        m_selected_piece->showGhostAt(m_origin);

        // logic analyzer forgets to run check checks after promotions!
        if (MLogicAnalyzer::PROMOTION & result)
        {
            m_promotion = new MQueen(m_selected_piece->getColour());
            m_promotion->select();
            m_position.addPieceAt(m_promotion, m_target);

            MGraphicsBoardItem *board_item = dynamic_cast<MGraphicsBoardItem *>(m_selected_piece->parentItem());
            if (!board_item)
            {
                qFatal("Corrupted piece ownership.");
            }

            board_item->addPiece(m_promotion);
        }

        m_position.setInCheck((MLogicAnalyzer::CHECK | MLogicAnalyzer::CHECKMATE) & result);

        return true;
    }

    return false;
}

void mHalfMove::
undo()
{
    Q_ASSERT(m_selected_piece && isValidOrigin() && isValidTarget());

    if (m_promotion)
    {
        delete m_promotion;
        m_promotion = 0;
    }

    if(m_selected_piece)
    {
        if (m_captured_piece)
        {
            m_captured_piece->show();
            m_captured_piece = 0;
        }

        // can be hidden due to promotion
        m_selected_piece->show();
        m_selected_piece->select();
        m_selected_piece->moveTo(m_selected_piece->mapFromCell(m_origin));

        m_position = m_position_origin;
    }
}

bool mHalfMove::
isValidOrigin() const
{
    return (QPoint(-1, -1) != m_origin);
}

bool mHalfMove::
isValidTarget() const
{
    return (QPoint(-1, -1) != m_target);
}

} // namespace Miniature