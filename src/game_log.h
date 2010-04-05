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
#include <QMap>
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
    enum mLogDomain
    {
        ERROR    = 0x000,
        ENGINE   = 0x001,
        GRAPHICS = 0x002,
        PREGAME  = 0x004,
        GAME     = 0x008,
        P2P      = 0x010,
        FICS     = 0x020,
        CHAT     = 0x040,
        FEN      = 0x800
    };

    typedef QFlags<mLogDomain> mLogDomainFlags;
    typedef QMap<mLogDomain, QString> mLogDomainMap;

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
    void append(const QString &msg, const mLogDomainFlags &flags);

    /*!
     *  Shows the log window, in the context of parent.
     */
    void showLog(QWidget *parent);

    /*!
     *  Slot for changing the log level filter.
     */
    void onFilterChanged(int filter_index);

private:
    mLogDomainFlags getAllLogDomainFlags() const;
    void print(const mLogDomainFlags &flags) const;

    mLogList m_log; /*!< The list that contains all log items. >*/
    mLogDomainMap m_log_domain_lookup; /*!< Stores the display names of log domains. >*/
    QComboBox *m_log_filter; /*!< The combo box used for filtering. >*/
    QTextEdit *m_log_widget; /*!< The widget used to print the log list. >*/

    /*!
     *  A helper class that is put in the log's QList.
     */
    class mLogItem
    {
    public:
        explicit mLogItem(const QString &msg, const mLogDomainFlags &flags);
        virtual ~mLogItem();

        QString getTime() const;
        QString getMessage() const;
        mLogDomainFlags getLogDomainFlags() const;

    private:
        QTime m_time;
        QString m_message;
        mLogDomainFlags m_log_domain_flags;
    };
};

#endif // GAME_LOG_H__
