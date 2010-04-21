/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMAILNAMESPACE_H
#define QMAILNAMESPACE_H

#include "qmailglobal.h"
#include <QDate>
#include <QPair>
#include <QString>
#include <QTime>

#if !defined(Q_OS_WIN) || !defined(_WIN32_WCE)
class QSqlDatabase;
#endif

namespace QMail
{
    QTOPIAMAIL_EXPORT QString lastSystemErrorMessage();
    QTOPIAMAIL_EXPORT void usleep(unsigned long usecs);
    QTOPIAMAIL_EXPORT QString dataPath();
    QTOPIAMAIL_EXPORT QString tempPath();
    QTOPIAMAIL_EXPORT QString pluginsPath();
    QTOPIAMAIL_EXPORT QString sslCertsPath();
    QTOPIAMAIL_EXPORT QString messageServerPath();
    QTOPIAMAIL_EXPORT QString messageSettingsPath();
    QTOPIAMAIL_EXPORT QString mimeTypeFromFileName(const QString& filename);
    QTOPIAMAIL_EXPORT QStringList extensionsForMimeType(const QString& mimeType);

#if !defined(Q_OS_WIN) || !defined(_WIN32_WCE)
    QTOPIAMAIL_EXPORT int fileLock(const QString& filePath);
    QTOPIAMAIL_EXPORT bool fileUnlock(int id);

    QTOPIAMAIL_EXPORT QSqlDatabase createDatabase();
#endif

    QTOPIAMAIL_EXPORT QString baseSubject(const QString& subject, bool *replyOrForward);
    QTOPIAMAIL_EXPORT QStringList messageIdentifiers(const QString& str);

    template<typename StringType>
    StringType unquoteString(const StringType& src)
    {
        // If a string has double-quote as the first and last characters, return the string
        // between those characters
        int length = src.length();
        if (length)
        {
            typename StringType::const_iterator const begin = src.constData();
            typename StringType::const_iterator const last = begin + length - 1;

            if ((last > begin) && (*begin == '"' && *last == '"'))
                return src.mid(1, length - 2);
        }

        return src;
    }

    template<typename StringType>
    StringType quoteString(const StringType& src)
    {
        StringType result("\"\"");

        // Return the input string surrounded by double-quotes, which are added if not present
        int length = src.length();
        if (length)
        {
            result.reserve(length + 2);

            typename StringType::const_iterator begin = src.constData();
            typename StringType::const_iterator last = begin + length - 1;

            if (*begin == '"')
                begin += 1;

            if ((last >= begin) && (*last == '"'))
                last -= 1;

            if (last >= begin)
                result.insert(1, StringType(begin, (last - begin + 1)));
        }

        return result;
    }

}

#endif
