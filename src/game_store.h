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

#ifndef GAME_STORE_H__
#define GAME_STORE_H__

#include <half_move.h>
#include <position.h>
#include <QtCore>

namespace Miniature
{

/*! This class stores the half moves of a game (and maybe even *games*, at a later point) and
 *  provides signals for position changes (e.g., when navigating through a game or setting up a
 *  position). It is unaware of the UI.
 */
class MGameStore
: public QObject
{
    Q_OBJECT

public:
    typedef QList<MPosition> MPositionList;

    explicit MGameStore(QObject *parent = 0);
    virtual ~MGameStore();

    //! Empties game store and resets to \a start_position.
    void reset(const MPosition &start_position);

    //! Returns the index of the current position.
    int getIndex() const;

    /*!
     *  Creates a MPosition with the default chess start position and stores it
     *  in m_game.
     */
    void setupStartPosition(bool isWhiteAtBottom = true);

    //! Returns whose turn it is.
    bool hasWhiteToMove() const;

    //! \sa hasWhiteToMove - added for completeness wrt. signals.
    bool hasBlackToMove() const;

Q_SIGNALS:
    void whiteToMove(const MPosition &position);
    void blackToMove(const MPosition &position);

    void candidatePieceSelected();
    void moveDiscarded();
    void moveConfirmationRequested();
    void invalidTargetSelected();

public Q_SLOTS:
    //! Sets position to index and emits {white,black}ToMove.
    void onPositionRequested(int index);

    /*!
     *  Adds the selected candidate move to the store and emits {white,
     *  black}ToMove.
     */
    MPosition onCandidateMoveConfirmed();
    void onCandidateMoveConfirmed(MPosition &pos);

    /*!
     *  Evaluates the selected piece and emits candidatePieceSelected,
     *  moveDiscarded, moveConfirmationRequested, invalidTargetSelected.
     *  @param[in] piece The selected piece.
     */
    void onPieceSelected(MPiece *piece);

    /*!
     *  Evaluates the selected target and emits moveDiscarded,
     *  moveConfirmationRequested, invalidTargetSelected.
     *  @param[in] target The selected target cell.
     */
    void onTargetSelected(const QPoint &target);

private:
    /*!
     *  \brief Returns whether the position exists in m_game.
     *  @param[in] half_move_index The index of the queried position.
     */
    bool isValidPosition(int half_move_index) const;

    MPositionList m_game; /*!< Stores the game history as a list of positions. >*/
    mHalfMove m_candidate_move; /*!< Encapsulates all transitional data
        for a potential half move. Remember that our UI requires a player to
        confirm moves so the UI needs to be able to show candidate moves that
        will never be part of the game history. >*/
    int m_index; /*!< An index to the positions stored in m_game. >*/
};

}

#endif // GAME_STORE_H__
