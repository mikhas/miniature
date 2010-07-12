/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2010 Michael Hasselmann <michael@taschenorakel.de>
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
 * @author Michael Hasselmann <michael@taschenorakel.de>
 * @author Alban Crequy <alban.crequy@collabora.co.uk>
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

#include <game_store.h>

using namespace Miniature;

MGameStore::
MGameStore(QObject *parent)
: QObject(parent),
  m_candidate_move(MPosition()),
  m_index(-1)
{}

MGameStore::
~MGameStore()
{}

void MGameStore::
reset(const MPosition &start_position)
{
    m_game.clear();
    m_game.append(start_position);

    m_candidate_move = mHalfMove(start_position);
    m_index = 0;

    Q_EMIT (hasWhiteToMove() ? whiteToMove(start_position)
                             : blackToMove(start_position));
}

int MGameStore::
getIndex() const
{
    return m_index;
}

void MGameStore::
setupStartPosition()
{
    m_game.clear();
    m_game = MPositionList(); // leaky?
    Q_ASSERT(m_game.empty());

    MPosition pos;

    pos.addPieceAt(new MRook(MPiece::BLACK), QPoint(0,0));
    pos.addPieceAt(new MRook(MPiece::BLACK), QPoint(7,0));
    pos.addPieceAt(new MRook(MPiece::WHITE), QPoint(0,7));
    pos.addPieceAt(new MRook(MPiece::WHITE), QPoint(7,7));

    pos.addPieceAt(new MKnight(MPiece::BLACK), QPoint(1,0));
    pos.addPieceAt(new MKnight(MPiece::BLACK), QPoint(6,0));
    pos.addPieceAt(new MKnight(MPiece::WHITE), QPoint(1,7));
    pos.addPieceAt(new MKnight(MPiece::WHITE), QPoint(6,7));

    pos.addPieceAt(new MBishop(MPiece::BLACK), QPoint(2,0));
    pos.addPieceAt(new MBishop(MPiece::BLACK), QPoint(5,0));
    pos.addPieceAt(new MBishop(MPiece::WHITE), QPoint(2,7));
    pos.addPieceAt(new MBishop(MPiece::WHITE), QPoint(5,7));

    pos.addPieceAt(new MQueen(MPiece::BLACK), QPoint(3,0));
    pos.addPieceAt(new MQueen(MPiece::WHITE), QPoint(3,7));

    pos.addPieceAt(new MKing(MPiece::BLACK), QPoint(4,0));
    pos.addPieceAt(new MKing(MPiece::WHITE), QPoint(4,7));

    for (int i = 0; i < 8; ++i)
    {
        pos.addPieceAt(new MPawn(MPiece::BLACK), QPoint(i,1));
        pos.addPieceAt(new MPawn(MPiece::WHITE), QPoint(i,6));
    }

    pos.resetCastling();

    m_game.append(pos);
    m_index = 0;
    m_candidate_move = mHalfMove(pos);

    Q_ASSERT(!m_game.empty());

    Q_EMIT whiteToMove(pos);
}

bool MGameStore::
hasWhiteToMove() const
{
    return (isValidPosition(m_index) && (MPiece::WHITE == (m_game[m_index]).getColourToMove()));
}

bool MGameStore::
hasBlackToMove() const
{
    return !hasWhiteToMove();
}

void MGameStore::
onPositionRequested(int index)
{
    if (!isValidPosition(index))
        return;

    m_index = index;
    MPosition pos = m_game[m_index];

    // Updating the pieces is enough, since in Miniature, pieces never get
    // lost during a game! They are merely hidden. New pieces get added during
    pos.updatePieces();
    m_candidate_move.deSelect();
    m_candidate_move = mHalfMove(pos);
    Q_EMIT (hasWhiteToMove() ? whiteToMove(pos)
                             : blackToMove(pos));
}

void MGameStore::
onCandidateMoveConfirmed(MPosition &pos)
{
    qDebug() << "MGameStore::onCandidateMoveConfirmed";
    m_candidate_move.deSelect();
    m_candidate_move = mHalfMove(pos);
    m_game.insert(++m_index, pos);

    Q_EMIT (hasWhiteToMove() ? whiteToMove(pos)
                             : blackToMove(pos));
}

MPosition MGameStore::
onCandidateMoveConfirmed()
{
    // Assumption: The move is already visible on the board, hence we only add
    // the transitional position to the position list.
    MPosition pos = MPosition(m_candidate_move.getPosition());

    onCandidateMoveConfirmed(pos);

    return pos;
}

void MGameStore::
onPieceSelected(MPiece *piece)
{
    // If invalid piece was selected => reset selection, flashing.
    // Currently, most piece selections are "valid" (because the logic analyzer
    // doesnt know how to handle that yet)
    Q_ASSERT(isValidPosition(m_index));

    // Only allow selection if piece has the correct colour.
    MPosition pos = m_game[m_index];
    if (piece && piece->getColour() == pos.getColourToMove())
    {
        const bool was_already_selected = m_candidate_move.isSelected(piece);
        m_candidate_move.undo();
        m_candidate_move.deSelect();

        // Don't re-select the same piece - see b.m.o bug #7868.
        if (!was_already_selected)
        {
            m_candidate_move = mHalfMove(pos);
            m_candidate_move.select(piece->mapToCell());
        }

        Q_EMIT candidatePieceSelected();
    }
    // Must be an attempt to capture an enemy piece. Let's forward it then!
    // Note: From my current understanding, this is the only case where
    // onTargetSelected is called when target points to a piece. Usually,
    // onTargetSelected is always only called when target points to an empty
    // cell.
    else if (piece && m_candidate_move.isValid())
    {
        onTargetSelected(piece->mapToCell());
    }

    // TODO: complain here
}

void MGameStore::
onTargetSelected(const QPoint &target)
{
    if (m_candidate_move.isUndoRequest(target))
    {
        m_candidate_move.undo(); // but do not deselect!
        Q_EMIT moveDiscarded();
    }
    else if (m_candidate_move.applyToTarget(target))
        Q_EMIT moveConfirmationRequested();
    else
        Q_EMIT invalidTargetSelected();
}

bool MGameStore::
isValidPosition(int index) const
{
    return (!m_game.empty() && -1 < index && index < m_game.size());
}
