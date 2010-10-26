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

#ifndef QNEARFIELDTAGTYPE1SYMBIAN_H
#define QNEARFIELDTAGTYPE1SYMBIAN_H

#include <qnearfieldtagtype1.h>
#include "nearfieldndeftarget_symbian.h"
#include "nearfieldtagtype1_symbian.h"

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QNearFieldTagType1Symbian : public QNearFieldTagType1
{
    Q_OBJECT

public:

    explicit QNearFieldTagType1Symbian(CNearFieldTarget *tag, QObject *parent = 0);
    
    ~QNearFieldTagType1Symbian();

    virtual QByteArray uid() const;

    virtual QNearFieldTarget::AccessMethods accessMethods() const
    {
        return mAccessMethods;
    }

    // DIGPROTO
    virtual QByteArray readIdentification();

    // static memory functions
    virtual QByteArray readAll();
    virtual quint8 readByte(quint8 address);
    virtual bool writeByte(quint8 address, quint8 data, WriteMode mode = EraseAndWrite);

    // dynamic memory functions
    virtual QByteArray readSegment(quint8 segmentAddress);
    virtual QByteArray readBlock(quint8 blockAddress);
    virtual bool writeBlock(quint8 blockAddress, const QByteArray &data,
                            WriteMode mode = EraseAndWrite);

    virtual QByteArray sendCommand(const QByteArray &command);
    virtual QList<QByteArray> sendCommands(const QList<QByteArray> &commands);
    
    void setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
    {
        mAccessMethods = accessMethods;
    }

    bool hasNdefMessage();
    QList<QNdefMessage> ndefMessages();
    void setNdefMessages(const QList<QNdefMessage> &messages);

private:
    // Own
    CNearFieldTarget * mTag;
    QNearFieldTarget::AccessMethods mAccessMethods;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QNEARFIELDTAGTYPE1SYMBIAN_H
