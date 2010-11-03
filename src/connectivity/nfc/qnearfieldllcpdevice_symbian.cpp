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

#include "qnearfieldllcpdevice_symbian_p.h"

QTM_BEGIN_NAMESPACE

QNearFieldLlcpDeviceSymbian::QNearFieldLlcpDeviceSymbian(RNfcServer& nfcServer, QObject *parent)
                                  : mNfcServer(nfcServer), QNearFieldTarget(parent)
{
}

QByteArray QNearFieldLlcpDeviceSymbian::uid() const
{
    return QByteArray();
}

/*!
    \fn QNearFieldTarget::Type QNearFieldTarget::type() const = 0

    Returns the type of tag type of this near field target.
*/
QNearFieldTarget::Type QNearFieldLlcpDeviceSymbian::type() const
{
    return QNearFieldTarget::AnyTarget;
}

/*!
    \fn QNearFieldTarget::AccessMethods QNearFieldTarget::accessMethods() const = 0

    Returns the access methods support by this near field target.
*/
QNearFieldTarget::AccessMethods QNearFieldLlcpDeviceSymbian::accessMethods() const
{
    return QNearFieldTarget::LlcpAccess;
}

void QNearFieldLlcpDeviceSymbian::setAccessMethods(const QNearFieldTarget::AccessMethods& accessMethods)
{
}

#include "moc_qnearfieldllcpdevice_symbian_p.cpp"

QTM_END_NAMESPACE
