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

#ifndef SERIALISER_H
#define SERIALISER_H

#include "qtcontacts.h"

#include <QString>
#include <QStringList>

QTM_USE_NAMESPACE

class Serialiser
{
public:
    static QStringList convertContact(const QContact& contact);
    static QContact convertVcard(const QStringList& vcardLines);

private:
    static QString escaped(const QString& input);
    static QString convertDetail(const QContact& contact, const QContactDetail& detail, const QString& vcardField = QString());
    static QStringList findActionIdsFromCustomString(const QString& customString);
    static QString findLinkStringFromCustomString(const QString& customString);
    static QContactDetail convertCustomString(const QString& customString);
    static QStringList removeClutter(const QStringList& vcard);
    static QContactDetail parsePropertyType(const QString& line);
    static QStringList parseContext(const QString& line);
    static QString parseValue(const QString& line);
    static QString parseAttributes(const QString& line);
    static bool parsePreferred(const QString& line);
    static bool detailIsPreferredForAnything(const QContact& contact, const QContactDetail& detail);
};

#endif
