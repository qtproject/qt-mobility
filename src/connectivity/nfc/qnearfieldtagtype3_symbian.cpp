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
#include <nfctag.h>
#include "qnearfieldtagtype3_symbian_p.h"

QNearFieldTagType3Symbian::QNearFieldTagType3Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTagType3(parent), QNearFieldTagImpl(tag)
{
}

QNearFieldTagType3Symbian::~QNearFieldTagType3Symbian()
{
}

QByteArray QNearFieldTagType3Symbian::uid() const
{
}

bool QNearFieldTagType3Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

QList<QNdefMessage> QNearFieldTagType3Symbian::ndefMessages()
{
    return _ndefMessages();
}

void QNearFieldTagType3Symbian::setNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages); 
}

QByteArray QNearFieldTagType3Symbian::sendCommand(const QByteArray &command)
{
}

QList<QByteArray> QNearFieldTagType3Symbian::sendCommands(const QList<QByteArray> &commands)
{
}

QByteArray QNearFieldTagType3Symbian::readBlock(quint8 blockAddress)
{
}

bool QNearFieldTagType3Symbian::writeBlock(quint8 blockAddress, const QByteArray &data)
{
}

bool QNearFieldTagType3Symbian::selectSector(quint8 sector)
{
}

#include "moc_qnearfieldtagtype3_symbian_p.cpp"
