/* Miniature - A chess board that goes always with you, ready to let
 * you play and learn wherever you go.
 *
 * Copyright (C) 2009 Michael Hasselmann <michael@taschenorakel.de>
 * Copyright (C) 2009 Dennis Stötzel <kore@meeQ.de>
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

#ifndef LOGIC_ANALYZER_H__
#define LOGIC_ANALYZER_H__

#include "position.h"
#include <QObject>

namespace Miniature
{

typedef int MConstraintList;

/* This class performs logical checks on a given position and move.
 * Currently, it only validates simple moves.
 */
class MLogicAnalyzer
: public QObject
{
    Q_OBJECT

public:
    MLogicAnalyzer(const MConstraintList &constraints, QObject *parent = 0);
    virtual ~MLogicAnalyzer();

    bool verifyMove(const MPosition &pos, QPoint from, QPoint to) const;

Q_SIGNALS:
    void invalidMove(const MPosition &pos, QPoint from, QPoint to) const;

private:
    MConstraintList m_constraints;

    QList<QPoint> applyConStraight(const MPosition &pos, const QList<QPoint> &moveList, QPoint from) const;
};

}

#endif // LOGIC_ANALYZER_H__
