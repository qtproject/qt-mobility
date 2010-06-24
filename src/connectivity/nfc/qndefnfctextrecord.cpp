/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qndefnfctextrecord.h>

#include <QtCore/QDebug>

QTM_BEGIN_NAMESPACE

QLocale QNdefNfcTextRecord::locale() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QLocale();

    quint8 status = p.at(0);

    quint8 codeLength = status & 0x3f;

    return QLocale(QString::fromAscii(p.constData() + 1, codeLength));
}

QString QNdefNfcTextRecord::text() const
{
    const QByteArray p = payload();

    if (p.isEmpty())
        return QString();

    quint8 status = p.at(0);

    bool utf16 = status & 0x80;
    quint8 codeLength = status & 0x3f;

    if (utf16) {
        return QString::fromUtf16(reinterpret_cast<const ushort *>(p.constData() + 1 + codeLength),
                                  p.length() - 1 - codeLength);
    } else {
        return QString::fromUtf8(p.constData() + 1 + codeLength, p.length() - 1 - codeLength);
    }
}

QTM_END_NAMESPACE
