/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2011 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef FRONTEND_H
#define FRONTEND_H

#include "dispatcher.h"
#include "commandline.h"
#include "linereader.h"
#include "abstractbackend.h"
#include "game.h"
#include "position.h"

#include <QtCore>

namespace Game { namespace Frontend {

QString fromColor(Color color);

class MiniaturePrivate;

//! This class communicates with the engine, handles the active game state
//! (together with the engine) and exports objects and properties to the
//! graphical frontend, our QML UI.
class Miniature
    : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Miniature)
    Q_DECLARE_PRIVATE(Miniature)
    Q_ENUMS(GameMode)

    Q_PROPERTY(GameMode gameMode READ gameMode
                                 WRITE setGameMode
                                 NOTIFY gameModeChanged)

    Q_PROPERTY(bool validMove READ validMove
                              NOTIFY validMoveChanged)

private:
    const QScopedPointer<MiniaturePrivate> d_ptr;

public:
    enum GameMode {
        TestFicsMode,
        FicsMode
    };

    explicit Miniature(Dispatcher *dispatcher,
                      QObject *parent = 0);
    virtual ~Miniature();
    virtual void show(const QUrl &ui);
    virtual void showBoard();

    virtual void handleRecord(const Record &r);
    virtual void handleSeek(const Seek &s);

    //! Sets game mode.
    Q_INVOKABLE void setGameMode(GameMode mode);
    Q_INVOKABLE GameMode gameMode() const;
    Q_SIGNAL void gameModeChanged(GameMode mode);

    Q_INVOKABLE void login(const QString &username,
                           const QString &password);

    //! Respond to a game advertisement. Will trigger a match if id is valid
    //! (and opponent accepts, in case of manual starts).
    //! @param id the game advertisement id.
    Q_INVOKABLE void play(uint id);

    //! Toggles whether a game advertisement is highlighted. Only one ad can be
    //! highlighted at a time.
    //! @param id the game advertisement id
    Q_INVOKABLE void toggleGameAdvertisementHighlighting(uint id);

    //! Selects a piece. Silently fails if piece was of wrong color or square is empty.
    //! @param target the target square of the selected piece [0-63].
    Q_INVOKABLE void selectPiece(int target);

    //! Performs a move. Will be sent once confirmed.
    //! @param origin the original square of the moved piece [0-63].
    //! @param target the target square of the moved piece [0-63].
    //! @param promotion the promotion to use (optional, defaults to queen).
    Q_INVOKABLE void movePiece(int origin,
                               int target,
                               const QString &promotion = QString("queen"));

    //! Undoes last move.
    Q_INVOKABLE void undoMove();

    //! Returns whether last move was valid.
    Q_INVOKABLE bool validMove() const;
    Q_SIGNAL void validMoveChanged(bool valid);

    //! Confirms current move.
    Q_INVOKABLE void confirmMove();


    //! Sets the active game.
    //! @param game the game.
    void setActiveGame(Game *game);

    //! Returns agive game, can be 0.
    Game * activeGame() const;

private:
    void sendCommand(AbstractCommand *command);
    Q_SLOT void onPositionChanged(const Position &position);
};

}} // namespace Game, Frontend

#endif // FRONTEND_H
