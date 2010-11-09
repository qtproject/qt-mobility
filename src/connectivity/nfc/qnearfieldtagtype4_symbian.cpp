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
#include "qnearfieldutility_symbian.h"
#include "qnearfieldtagtype4_symbian_p.h"
#include "qnearfieldutility_symbian.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QNearFieldTagType4Symbian
    \brief The QNearFieldTagType4Symbian class provides symbian backend implementation for communicating with an NFC Tag
           Type 1 tag.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity
*/

/*!
    Constructs a new tag type 1 near field target with \a tag and \a parent.
*/
QNearFieldTagType4Symbian::QNearFieldTagType4Symbian(MNearFieldTarget *tag, QObject *parent)
                                : QNearFieldTarget(parent), QNearFieldTagImpl(tag)
{
}

/*!
    Destructor
*/
QNearFieldTagType4Symbian::~QNearFieldTagType4Symbian()
{
    delete mTag;
}

QByteArray QNearFieldTagType4Symbian::uid() const
{
}
    
bool QNearFieldTagType4Symbian::hasNdefMessage()
{
    return _hasNdefMessage();
}

QList<QNdefMessage> QNearFieldTagType4Symbian::ndefMessages()
{
    return _ndefMessages();
}

void QNearFieldTagType4Symbian::setNdefMessages(const QList<QNdefMessage> &messages)
{
    _setNdefMessages(messages);
}

/*!
    \reimp
*/
QByteArray QNearFieldTagType4Symbian::sendCommand(const QByteArray &command)
{
}

/*!
    \reimp
*/
QList<QByteArray> QNearFieldTagType4Symbian::sendCommands(const QList<QByteArray> &commands)
{
}

QByteArray QNearFieldTagType4Symbian::sendAPDUCommand(const QByteArray &command)
{
}

/*!
    \reimp
*/
QList<QByteArray> QNearFieldTagType4Symbian::sendAPDUCommands(const QList<QByteArray> &commands)
{
}

#include "moc_qnearfieldtagtype4_symbian_p.cpp"

QTM_END_NAMESPACE
