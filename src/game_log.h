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

#ifndef GAME_LOG_H__
#define GAME_LOG_H__

#include <QObject>
#include <QString>
#include <QList>
#include <QComboBox>
#include <QTextEdit>
#include <QMainWindow>
#include <QTime>

/*! \brief Logging facility for Miniature.
 *
 *  This class allows other components to append messages to the game log via
 *  the append slot. Different log levels can be used which has an effect on
 *  *how* the messages are reported.
 */
class MGameLog
: public QObject
{
    Q_OBJECT

public:
    enum mLogLevel
    {
        ERROR    = 0x000,
        CRITICAL = 0x001,
        WARNING  = 0x002,
        MESSAGE  = 0x004,
        INFO     = 0x008,
        DEBUG    = 0x010
    };

    typedef QFlags<mLogLevel> mLogLevelFlags;

private:
    class mLogItem;

public:
    typedef QList<mLogItem> mLogList;

    explicit MGameLog(QObject *parent = 0);
    virtual ~MGameLog();


public Q_SLOTS:
    /*!
     *  Creates a log item from the message and the flags and appends it to the log.
     */
    void append(const QString &msg, const mLogLevelFlags &flags);

    /*!
     *  Shows the log window, in the context of parent.
     */
    void showLog(QWidget *parent);

    /*!
     *  Slot for changing the log level filter.
     */
    void onFilterChanged(int filter_index);

private:
    void print(const mLogLevelFlags &flags) const;

    mLogList m_log; /*!< The list that contains all log items. >*/
    QComboBox *m_log_filter; /*!< The combo box used for filtering. >*/
    QTextEdit *m_log_widget; /*!< The widget used to print the log list. >*/

    /*!
     *  A helper class that is put in the log's QList.
     */
    class mLogItem
    {
    public:
        explicit mLogItem(const QString &msg, const mLogLevelFlags &flags);
        virtual ~mLogItem();

        QString getTime() const;
        QString getMessage() const;
        mLogLevelFlags getLogLevelFlags() const;

    private:
        QTime m_time;
        QString m_message;
        mLogLevelFlags m_log_level_flags;
    };
};

#endif // GAME_LOG_H__
