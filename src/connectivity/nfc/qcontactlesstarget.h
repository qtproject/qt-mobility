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


#ifndef QCONTACTLESSTARGET_H
#define QCONTACTLESSTARGET_H

#include "qmobilityglobal.h"

#include <QtCore/QList>

class QString;
class QUrl;
class QVariant;
class QVariantList;

class QContactlessTargetPrivate;
class QNdefMessage;

class Q_CONNECTIVITY_EXPORT QContactlessTarget
{
public:
    enum TagType {
        Proprietary,
        NfcTagType1,
        NfcTagType2,
        NfcTagType3,
        NfcTagType4,
        Mifare
    };

    enum TagAccessMethod {
        NdefAccess,
        AdpuAccess,
        TagTypeSpecificAccess
    };
    Q_DECLARE_FLAGS(TagAccessMethods, TagAccessMethod)

    QContactlessTarget();

    QString uid() const;
    QUrl url() const;

    TagType type() const;
    TagAccessMethods accessMethods() const;

    // might need to make async
    QNdefMessage ndefMessage() const;
    void setNdefMessage(const QNdefMessage &message);

    // might need to make async
    QByteArray sendApduCommand(const QByteArray &command);
    QList<QByteArray> sendApduCommands(const QList<QByteArray> &commands);

    // might need to make async
    QVariant sendCommand(const QVariant &command);
    QVariantList sendCommands(const QVariantList &commands);

private:
    QContactlessTargetPrivate *d;
};

#endif // QCONTACTLESSTARGET_H
