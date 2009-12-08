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

#include "action_area.h"

#include <QVBoxLayout>

using namespace Miniature;

const MActionArea::Colours MActionArea::m_colours =
{
    QString("#c88010"),
    QString("#c88410"),
    QString("#f8d068"),

    QString("#00ff66"),
    QString("#218121"),
    QString("#00f966"),

    QString("#80bcff"),
    QString("#8080ff"),
    QString("#80c4ff"),

    QString("#ae00ff")
};

MActionArea::MActionArea()
: m_button(new QPushButton()),
  m_state(NONE)
{}

MActionArea::~MActionArea()
{}

void MActionArea::setState(State s)
{
    if (m_state == s)
    {
        return;
    }

    m_state = s;
    switch(m_state)
    {
        default:
        case NONE:
            m_button->setStyleSheet(createButtonStyleFlat()); break;
        case TURN_STARTED:
            m_button->setStyleSheet(createButtonStyleBorderOnly()); break;
        case PIECE_SELECTED:
            m_button->setStyleSheet(createButtonStyleHalfFilled()); break;
        case TARGET_SELECTED:
            m_button->setStyleSheet(createButtonStyle()); break;
    }
}

QString MActionArea::createButtonStyleBase() const
{
    return QString(
        "QPushButton {"
            "margin: 3px 2px;"
            "width:472px;"
            "height: 64px;"
            "border-radius: 10px;"
            "border-style: outset;"
            "border-width: 1px;"
            "border-color: %1;"
            "background-image: url('%2');"
            "background-position: %3;"
            "background-repeat: no-repeat;"
            "background-attachment: fixed;"
            "background-color: %4;"
        "}"
    );
}

QString MActionArea::createButtonStylePressedBase() const
{
    return QString(
        "QPushButton:pressed"
        "{"
            "border-color: %1;"
            "background-color: %2;"
        "}"
    );
}

QString MActionArea::createButtonStyleFlat() const
{
    return createButtonStyleBase().arg("none").arg("").arg("").arg("none");
}

QString MActionArea::createButtonStyleBorderOnly() const
{
    return createButtonStyleBase().arg(m_colours.maemo5_orange_border).arg("").arg("").arg("black");
}

QString MActionArea::createButtonStyleHalfFilled() const
{
    const QString gradient1 =  QString("qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1 stop: 1 %2)").arg(m_colours.blue_gradient1)
                                                                                                            .arg(m_colours.blue_gradient2);
    const QString gradient2 =  QString("qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1 stop: 1 %2)").arg(m_colours.maemo5_orange_gradient1)
                                                                                                            .arg(m_colours.maemo5_orange_gradient2);

    return QString("%1 %2").arg(createButtonStyleBase().arg(m_colours.blue_border).arg(":/blackout.png").arg("top right").arg(gradient1))
                           .arg(createButtonStylePressedBase().arg(m_colours.maemo5_orange_border).arg(gradient2));

}

QString MActionArea::createButtonStyle() const
{
    const QString gradient1 =  QString("qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1 stop: 1 %2)").arg(m_colours.green_gradient1)
                                                                                                            .arg(m_colours.green_gradient2);
    const QString gradient2 =  QString("qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 %1 stop: 1 %2)").arg(m_colours.maemo5_orange_gradient1)
                                                                                                            .arg(m_colours.maemo5_orange_gradient2);

    return QString("%1 %2").arg(createButtonStyleBase().arg(m_colours.green_border).arg("").arg("").arg(gradient1))
                           .arg(createButtonStylePressedBase().arg(m_colours.maemo5_orange_border).arg(gradient2));
}

QGraphicsProxyWidget* MActionArea::createActionAreaProxyWidget(const QString& name)
{
    Q_CHECK_PTR(m_button);

    QGraphicsProxyWidget *proxy_widget = new QGraphicsProxyWidget();
    QWidget *box = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    box->setLayout(layout);
    proxy_widget->setWidget(box); // TODO: ownership?

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_button);
    m_button->setText(name);

    m_button->setStyleSheet(createButtonStyleFlat());
    m_button->show();

    return proxy_widget;
}
