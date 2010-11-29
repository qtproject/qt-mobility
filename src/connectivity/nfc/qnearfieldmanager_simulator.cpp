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

#include "qnearfieldmanager_simulator_p.h"
#include "qnearfieldmanager.h"
#include "qnearfieldtagtype1.h"
#include "qndefmessage.h"

#include <mobilityconnection_p.h>
#include <QtGui/private/qsimulatordata_p.h>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

namespace Simulator {

class TagType1 : public QNearFieldTagType1
{
public:
    TagType1(const QByteArray &uid, QObject *parent);
    virtual ~TagType1();

    QByteArray uid() const;

    AccessMethods accessMethods() const;

    QByteArray sendCommand(const QByteArray &command);

private:
    QByteArray m_uid;
};

TagType1::TagType1(const QByteArray &uid, QObject *parent)
:   QNearFieldTagType1(parent), m_uid(uid)
{
}

TagType1::~TagType1()
{
}

QByteArray TagType1::uid() const
{
    return m_uid;
}

QNearFieldTarget::AccessMethods TagType1::accessMethods() const
{
    return NdefAccess | TagTypeSpecificAccess;
}

QByteArray TagType1::sendCommand(const QByteArray &command)
{
    if (command.isEmpty())
        return QByteArray();

    quint16 crc = qNfcChecksum(command.constData(), command.length());

    MobilityConnection *connection = MobilityConnection::instance();
    QByteArray response =
        RemoteMetacall<QByteArray>::call(connection->sendSocket(), WaitSync, "nfcSendCommand",
                                         command + char(crc & 0xff) + char(crc >> 8));

    if (response.isEmpty())
        return QByteArray();

    // check crc
    if (qNfcChecksum(response.constData(), response.length()) != 0)
        return QByteArray();

    response.chop(2);
    return response;
}

class NfcConnection : public QObject
{
    Q_OBJECT

public:
    NfcConnection();
    virtual ~NfcConnection();

signals:
    void targetInRange(const QByteArray &uid);
};

NfcConnection::NfcConnection()
:   QObject(MobilityConnection::instance())
{
    MobilityConnection *connection = MobilityConnection::instance();
    connection->addMessageHandler(this);

    RemoteMetacall<void>::call(connection->sendSocket(), NoSync, "setRequestsNfc");
}

NfcConnection::~NfcConnection()
{
}

}

QNearFieldManagerPrivateImpl::QNearFieldManagerPrivateImpl()
:   nfcConnection(new Simulator::NfcConnection)
{
    QObject::connect(nfcConnection, SIGNAL(targetInRange(QByteArray)),
                     this, SLOT(targetInRange(QByteArray)));
}

QNearFieldManagerPrivateImpl::~QNearFieldManagerPrivateImpl()
{
    delete nfcConnection;
}

void QNearFieldManagerPrivateImpl::startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
{
    detectTargetTypes = targetTypes;
}

void QNearFieldManagerPrivateImpl::stopTargetDetection()
{
    detectTargetTypes.clear();
}

void QNearFieldManagerPrivateImpl::targetInRange(const QByteArray &uid)
{
    if (detectTargetTypes.contains(QNearFieldTarget::NfcTagType1) ||
        detectTargetTypes.contains(QNearFieldTarget::AnyTarget)) {
        Simulator::TagType1 *target = new Simulator::TagType1(uid, this);
        emit targetDetected(target);
    }
}

#include "qnearfieldmanager_simulator.moc"
#include "moc_qnearfieldmanager_simulator_p.cpp"

QTM_END_NAMESPACE
