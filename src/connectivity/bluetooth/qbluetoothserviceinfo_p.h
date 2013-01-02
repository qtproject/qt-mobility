/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBLUETOOTHSERVICEINFO_P_H
#define QBLUETOOTHSERVICEINFO_P_H

#include "qbluetoothuuid.h"
#include "qbluetoothdeviceinfo.h"

#include <QMap>
#include <QVariant>

#ifdef QTM_SYMBIAN_BLUETOOTH
#include <btsdp.h>
#endif

class OrgBluezServiceInterface;

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothServiceInfo;

class QBluetoothServiceInfoPrivate
{
    Q_DECLARE_PUBLIC(QBluetoothServiceInfo)
public:
    QBluetoothServiceInfoPrivate();
    ~QBluetoothServiceInfoPrivate();

    void setRegisteredAttribute(quint16 attributeId, const QVariant &value) const;
    void removeRegisteredAttribute(quint16 attributeId) const;

    bool ensureSdpConnection() const;

#ifdef QTM_BLUEZ_BLUETOOTH
    bool registerService() const;
#endif

    QBluetoothDeviceInfo deviceInfo;
    QMap<quint16, QVariant> attributes;

#ifdef QTM_SYMBIAN_BLUETOOTH
private:
    void setRegisteredAttributeL(quint16 attributeId, const QVariant &value) const;

public:
    mutable RSdp sdpSession;
    mutable RSdpDatabase sdpDatabase;
    mutable TSdpServRecordHandle serviceRecord;
#endif

#ifdef QTM_BLUEZ_BLUETOOTH
    mutable OrgBluezServiceInterface *service;
    mutable quint32 serviceRecord;
    mutable bool registered;
#endif
    
private:
    QBluetoothServiceInfo *q_ptr;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif
