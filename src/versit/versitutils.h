/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef VERSITUTILS_H
#define VERSITUTILS_H

#include <QByteArray>
#include <QPair>
#include <QString>
#include <QStringList>
#include <QMultiHash>

#include "qtversitglobal.h"

QTM_BEGIN_NAMESPACE

class VersitUtils
{
public:
    static QByteArray fold(QByteArray& text, int maxChars);
    static QByteArray unfold(QByteArray& text);
    static int countLeadingWhiteSpaces(const QByteArray& text, int pos=0);
    static bool quotedPrintableEncode(QByteArray& text);
    static void decodeQuotedPrintable(QByteArray& text);
    static bool backSlashEscape(QByteArray& text);
    static void removeBackSlashEscaping(QByteArray& text);
    static int findHardLineBreakInQuotedPrintable(const QByteArray& encoded);
    static QPair<QStringList,QString> extractPropertyGroupsAndName(
        const QByteArray& property);
    static QByteArray extractPropertyValue(const QByteArray& property);
    static QMultiHash<QString,QString> extractVCard21PropertyParams(
        const QByteArray& property);
    static QMultiHash<QString,QString> extractVCard30PropertyParams(
        const QByteArray& property);

private:
    
    static QList<QByteArray> extractParams(const QByteArray& property);
    static QList<QByteArray> extractParts(const QByteArray& text, char separator);
    static QByteArray extractPart(
        const QByteArray& text,
        int startPosition, 
        int length=-1);
    static QByteArray paramName(const QByteArray& parameter);
    static QByteArray paramValue(const QByteArray& parameter);
    static bool shouldBeQuotedPrintableEncoded(char chr);
    
private:
    
    friend class UT_VersitUtils;
};

QTM_END_NAMESPACE

#endif // VERSITUTILS_H
