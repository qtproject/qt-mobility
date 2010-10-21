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
#include "qnearfieldtagtype1_symbian_p.h"

QTM_BEGIN_NAMESPACE

QNearFieldTagType1Symbian::QNearFieldTagType1Symbian(CNearFieldTagType1 *tag, QObject *parent)
                                : QNearFieldTagType1(parent), mTag(tag)
{
}

QByteArray QNearFieldTagType1Symbian::uid() const
{
}

QNearFieldTarget::AccessMethods QNearFieldTagType1Symbian::accessMethods() const
{
}

QByteArray QNearFieldTagType1Symbian::readIdentification()
{
}

QByteArray QNearFieldTagType1Symbian::readAll()
{
}

quint8 QNearFieldTagType1Symbian::readByte(quint8 address)
{
}

bool QNearFieldTagType1Symbian::writeByte(quint8 address, quint8 data, WriteMode mode)
{
}

QByteArray QNearFieldTagType1Symbian::readSegment(quint8 segmentAddress)
{
}

QByteArray QNearFieldTagType1Symbian::readBlock(quint8 blockAddress)
{
}

bool QNearFieldTagType1Symbian::writeBlock(quint8 blockAddress, const QByteArray &data,
                        WriteMode mode)
{
}

QByteArray QNearFieldTagType1Symbian::sendCommand(const QByteArray &command)
{
}

QList<QByteArray> QNearFieldTagType1Symbian::sendCommands(const QList<QByteArray> &commands)
{
}

#include "moc_qnearfieldtagtype1_symbian_p.cpp"

QTM_END_NAMESPACE
