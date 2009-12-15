/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
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

#ifndef ACTION_AREA_H__
#define ACTION_AREA_H__

#include <QGraphicsWidget>
#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QPushButton>
#include <QGraphicsProxyWidget>

namespace Miniature
{

/* This class represents a player's action area, that is, an area that requires
 * user interaction besides moving the pieces on the board. It is also used to
 * indicate the current state when taking a turn.
 * This class is not theming-friendly ...
 */
class MActionArea
: public QObject
{
    Q_OBJECT

public:
    enum State {NONE, TURN_STARTED, PIECE_SELECTED, INVALID_PIECE_SELECTED, TARGET_SELECTED, INVALID_TARGET_SELECTED, TURN_ENDED};

    explicit MActionArea(QObject *parent = 0);
    ~MActionArea();
    QGraphicsProxyWidget* createActionAreaProxyWidget(const QString& name);
    QString getText() const;

public Q_SLOT:
    void setState(State s);
    void setText(const QString &text);

Q_SIGNALS:
    void moveConfirmed();
    void pieceSelectionCancelled();

private:
    // TODO: find how to properly create overlays so that I do not have to
    // mimick the Maemo5 style only for setting different background colors. I
    // guess I just missed some QPalette one-liner or so.
    QString createButtonStyleBase() const;
    QString createButtonStylePressedBase() const;
    QString createButtonStyleFlat() const;
    QString createButtonStyleBorderOnly() const;
    QString createButtonStyleHalfFilled() const;
    QString createButtonStyle() const;

    QPushButton *m_button;
    State m_state;

    struct Colours
    {
        const QString maemo5_orange_border;
        const QString maemo5_orange_gradient1;
        const QString maemo5_orange_gradient2;

        const QString green_border;
        const QString green_gradient1;
        const QString green_gradient2;

        const QString blue_border;
        const QString blue_gradient1;
        const QString blue_gradient2;

        const QString purple_border;
    };

    static const Colours m_colours;

private Q_SLOTS:
    /* Checks the current state and emit the corresponding signal whenever the
     * button was pressed. */
    void onActionTriggered();
};

} // namespace Miniature

#endif // ACTION_AREA_H__
