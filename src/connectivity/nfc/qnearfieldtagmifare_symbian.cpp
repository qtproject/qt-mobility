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
#include "qnearfieldtagmifare_symbian_p.h"

QTM_BEGIN_NAMESPACE

/*!
    Constructs a new near field target with \a parent.
*/
QNearFieldTagMifareSymbian::QNearFieldTagMifareSymbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTarget(parent), QNearFieldTagImpl(tag)
{
}

QNearFieldTagMifareSymbian::~QNearFieldTagMifareSymbian()
{
    delete mTag;
}

QByteArray QNearFieldTagMifareSymbian::uid() const
{
    return _uid();
}

bool QNearFieldTagMifareSymbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

/*!
    Returns a list of all NDEF messages stored on the near field target.
*/
QList<QNdefMessage> QNearFieldTagMifareSymbian::ndefMessages()
{
    return _ndefMessages();
}

/*!
    Sets the NDEF messages stored on the near field target to \a messages.
*/
void QNearFieldTagMifareSymbian::setNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages);
}

/*!
    Sends \a command to the near field target and returns the result.
*/
QByteArray QNearFieldTagMifareSymbian::sendCommand(const QByteArray &command)
{
}

/*!
    Sends multiple \a commands to the near field target and returns the result.
*/
QList<QByteArray> QNearFieldTagMifareSymbian::sendCommands(const QList<QByteArray> &commands)
{
}

#include "moc_qnearfieldtagmifare_symbian_p.cpp"

QTM_END_NAMESPACE
