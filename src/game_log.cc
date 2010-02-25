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

#include <game_log.h>
#include <QtCore>
#include <QtGui>

MGameLog::
MGameLog(QObject *parent)
: QObject(parent),
  m_log_filter(0),
  m_log_widget(0)
{}

MGameLog::
~MGameLog()
{}

void MGameLog::
append(const QString &msg, const mLogLevelFlags &flags)
{
    m_log.append(mLogItem(msg, flags));
}

void MGameLog::
showLog(QWidget *parent)
{
    QMainWindow *game_log = new QMainWindow(parent);

    game_log->setWindowTitle(tr("Game Log"));
    game_log->setAttribute(Qt::WA_Maemo5StackedWindow);
    game_log->setAttribute(Qt::WA_DeleteOnClose);
    connect(game_log, SIGNAL(destroyed()), parent, SLOT(show()));

    QVBoxLayout *vbox = new QVBoxLayout;

    m_log_filter = new QComboBox;
    m_log_filter->setWindowTitle("Select log level to show");
    m_log_filter->insertItem(-1, QString("Show errors"), QVariant(static_cast<int>(MGameLog::ERROR)));
    m_log_filter->insertItem(-1, QString("Show critical warnings"), QVariant(static_cast<int>(MGameLog::CRITICAL)));
    m_log_filter->insertItem(-1, QString("Show warnings"), QVariant(static_cast<int>(MGameLog::WARNING)));
    m_log_filter->insertItem(-1, QString("Show information"), QVariant(static_cast<int>(MGameLog::INFO)));
    m_log_filter->insertItem(-1, QString("Show all"), QVariant(-1));
    m_log_filter->setCurrentIndex(0);
    connect(m_log_filter, SIGNAL(currentIndexChanged(int)), this, SLOT(onFilterChanged(int)));

    vbox->addWidget(m_log_filter);

    m_log_widget = new QTextEdit;
    vbox->addWidget(m_log_widget);

    QPushButton *copy_all = new QPushButton(tr("Copy all"));
    // This depends on the order of slot execution, of course.
    connect(copy_all, SIGNAL(pressed()), m_log_widget, SLOT(selectAll()));
    connect(copy_all, SIGNAL(pressed()), m_log_widget, SLOT(copy()));

    vbox->addWidget(copy_all);

    QWidget *central = new QWidget;
    central->setLayout(vbox);
    game_log->setCentralWidget(central);

    onFilterChanged(m_log_filter->currentIndex());

    game_log->show();
}

void MGameLog::
onFilterChanged(int filter_index)
{
    mLogLevelFlags flags;
    QVariant val = m_log_filter->itemData(filter_index).toInt();

    if (-1 == val.toInt())
    {
        flags = mLogLevelFlags(ERROR | CRITICAL | WARNING | MESSAGE | INFO | DEBUG);
    }
    else
    {
        flags = static_cast<MGameLog::mLogLevel>(m_log_filter->itemData(filter_index).toInt());
    }

    print(flags);
}

void MGameLog::
print(const mLogLevelFlags &flags) const
{
    m_log_widget->clear();

    for (mLogList::const_iterator iter = m_log.begin();
         iter != m_log.end();
         ++iter)
    {
        if (iter->getLogLevelFlags() & flags)
        {
            m_log_widget->textCursor().insertText(QString("%1 - %2").arg(iter->getTime())
                                                                    .arg(iter->getMessage()));
        }
    }
}

MGameLog::mLogItem::
mLogItem(const QString &msg, const mLogLevelFlags &flags)
: m_time(QTime::currentTime()),
  m_message(msg),
  m_log_level_flags(flags)
{}

MGameLog::mLogItem::
~mLogItem()
{}

QString MGameLog::mLogItem::
getTime() const
{
    return m_time.toString("hh:mm:ss");
}

QString MGameLog::mLogItem::
getMessage() const
{
    return m_message;
}

MGameLog::mLogLevelFlags MGameLog::mLogItem::
getLogLevelFlags() const
{
    return m_log_level_flags;
}
