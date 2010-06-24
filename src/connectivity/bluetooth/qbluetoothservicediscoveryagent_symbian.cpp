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

#include "qbluetoothservicediscoveryagent_p.h"

#include <QDebug>

#include <QUrl>

#include <arpa/inet.h>
#include <netinet/in.h>

QTM_BEGIN_NAMESPACE

//#define MINIMAL_SERVICE_ATTRIBUTES
#define ALL_SERVICE_ATTRIBUTES

QBluetoothServiceDiscoveryAgentPrivate::QBluetoothServiceDiscoveryAgentPrivate(const QBluetoothAddress &address)
: error(QBluetoothServiceDiscoveryAgent::NoError), deviceAddress(address), state(Inactive), deviceDiscoveryAgent(0)
{
    filter = CSdpSearchPattern::NewL();

    attributes = CSdpAttrIdMatchList::NewL();
#if defined(MINIMAL_SERVICE_ATTRIBUTES)
    attributes->AddL(QBluetoothServiceInfo::ServiceName);
    attributes->AddL(QBluetoothServiceInfo::ServiceDescription);
    attributes->AddL(QBluetoothServiceInfo::ServiceProvider);
    attributes->AddL(QBluetoothServiceInfo::ProtocolDescriptorList);
#elif defined(ALL_SERVICE_ATTRIBUTES)
    attributes->AddL(KAttrRangeAll);
    attributes->RemoveL(787);     //attribute id 787 from my N958GB causes parser crash
#endif
}

QBluetoothServiceDiscoveryAgentPrivate::~QBluetoothServiceDiscoveryAgentPrivate()
{
}

void QBluetoothServiceDiscoveryAgentPrivate::start(const QBluetoothAddress &address)
{
    initAgent(address);

    filter->Reset();
    if (uuidFilter.isEmpty()) {
        filter->AddL(QBluetoothUuid::PublicBrowseGroup);
    } else {
        foreach (const QBluetoothUuid &uuid, uuidFilter) {
            /* need to support 16, 32 and 128 bit uuids */
            TUUID sUuid(uuid.toUInt16());
            filter->AddL(sUuid);
        }
    }
    sdpAgent->SetRecordFilterL(*filter);

    sdpAgent->NextRecordRequestL();
}

void QBluetoothServiceDiscoveryAgentPrivate::stop()
{
}

void QBluetoothServiceDiscoveryAgentPrivate::initAgent(const QBluetoothAddress &address)
{
    TBTDevAddr btAddress(address.toUInt64());

    sdpAgent = CSdpAgent::NewL(*this, btAddress);
}

void QBluetoothServiceDiscoveryAgentPrivate::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    if (aError == KErrNone && aTotalRecordsCount > 0) {
        // request attributes
        TRAPD(err, sdpAgent->AttributeRequestL(aHandle, *attributes));
        if (err) {
            qDebug() << "Error calling AttributeRequestL()";
        }
    } else if (aError == KErrEof) {
        _q_serviceDiscoveryFinished(false);
    } else {
        qDebug() << "Got error in" << __FUNCTION__ << aError;
        _q_serviceDiscoveryFinished(true);
    }
}

static QString attributeToString(quint16 attr)
{
    switch (attr) {
    case QBluetoothServiceInfo::ServiceName: return QLatin1String("Service Name");
    case QBluetoothServiceInfo::ServiceDescription: return QLatin1String("Service Description");
    case QBluetoothServiceInfo::ServiceProvider: return QLatin1String("Service Provider");
    case QBluetoothServiceInfo::ProtocolDescriptorList: return QLatin1String("Protocol Descriptor List");
    }

    return QString::number(attr);
}

void QBluetoothServiceDiscoveryAgentPrivate::AttributeRequestResult(TSdpServRecordHandle, TSdpAttributeID aAttrID, CSdpAttrValue *aAttrValue)
{
    currentAttributeId = aAttrID;
    TRAPD(err, aAttrValue->AcceptVisitorL(*this));

    if (stack.size() != 1) {
        qDebug() << "STACK SIZE NOT 1 for attribute" << aAttrID;
        return;
    }

    serviceInfo.setAttribute(aAttrID, stack.pop());

    if (err)
        qDebug() << "Got error in" << __FUNCTION__ << err;
}

void QBluetoothServiceDiscoveryAgentPrivate::AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    if (aError == KErrNone) {
        serviceInfo.setDevice(discoveredDevices.at(0));
        discoveredServices.append(serviceInfo);
        if (serviceInfo.isValid())
            emit q->serviceDiscovered(serviceInfo);
        else
            qDebug() << "Discovered service with no attributes.";

        serviceInfo = QBluetoothServiceInfo();

        TRAPD(err, sdpAgent->NextRecordRequestL());
        if (err) {
            qDebug() << "Error calling NextRecordRequestL()";
        }
    } else if (aError != KErrEof) {
        qDebug() << "Got error in" << __FUNCTION__ << aError;
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
{
    QVariant var;

    switch (aType) {
    case ETypeNil:
        break;
    case ETypeUint:
        var = QVariant::fromValue(aValue.Uint());
        break;
    case ETypeInt:
        var = QVariant::fromValue(aValue.Int());
        break;
    case ETypeUUID: {
        TPtrC8 shortForm(aValue.UUID().ShortestForm());
        if (shortForm.Size() == 2) {
            QBluetoothUuid uuid(ntohs(*reinterpret_cast<const quint16 *>(shortForm.Ptr())));
            var = QVariant::fromValue(uuid);
        } else if (shortForm.Size() == 4) {
            QBluetoothUuid uuid(ntohl(*reinterpret_cast<const quint32 *>(shortForm.Ptr())));
            var = QVariant::fromValue(uuid);
        } else if (shortForm.Size() == 16) {
            QBluetoothUuid uuid(*reinterpret_cast<const quint128 *>(shortForm.Ptr()));
            var = QVariant::fromValue(uuid);
        }
        break;
    }
    case ETypeString: {
        TPtrC8 stringBuffer = aValue.Des();
        var = QVariant::fromValue(QString::fromLocal8Bit(reinterpret_cast<const char *>(stringBuffer.Ptr()), stringBuffer.Size()));
        break;
    }
    case ETypeBoolean:
        var = QVariant::fromValue(static_cast<bool>(aValue.Bool()));
        break;
    case ETypeDES:
        stack.push(QVariant::fromValue(QBluetoothServiceInfo::Sequence()));
        break;
    case ETypeDEA:
        stack.push(QVariant::fromValue(QBluetoothServiceInfo::Alternative()));
        break;
    case ETypeURL: {
        TPtrC8 stringBuffer = aValue.Des();
        var = QVariant::fromValue(QUrl(QString::fromLocal8Bit(reinterpret_cast<const char *>(stringBuffer.Ptr()), stringBuffer.Size())));
        break;
    }
    case ETypeEncoded:
        qDebug() << "Don't know how to handle encoded type.";
        break;
    default:
        qDebug() << "Don't know how to handle type" << aType;
    }

    if (aType != ETypeDES && aType != ETypeDEA) {
        if (stack.size() == 0) {
            // single value attribute, just push onto stack
            stack.push(var);
        } else if (stack.size() >= 1) {
            // sequence or alternate attribute, add non-DES -DEA values to DES or DEA
            if (stack.top().canConvert<QBluetoothServiceInfo::Sequence>()) {
                QBluetoothServiceInfo::Sequence *sequence = static_cast<QBluetoothServiceInfo::Sequence *>(stack.top().data());
                sequence->append(var);
            } else if (stack.top().canConvert<QBluetoothServiceInfo::Alternative>()) {
                QBluetoothServiceInfo::Alternative *alternative = static_cast<QBluetoothServiceInfo::Alternative *>(stack.top().data());
                alternative->append(var);
            } else {
                qWarning("Unknown type in the QVariant, should be either a QBluetoothServiceInfo::Sequence or an QBluetoothServiceInfo::Alternative");
            }
        }
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::StartListL(CSdpAttrValueList &)
{
}

void QBluetoothServiceDiscoveryAgentPrivate::EndListL()
{
    if (stack.size() > 1) {
        // finished a sequence or alternative add it to the parent sequence or alternative
        QVariant var = stack.pop();
        if (stack.top().canConvert<QBluetoothServiceInfo::Sequence>()) {
            QBluetoothServiceInfo::Sequence *sequence = static_cast<QBluetoothServiceInfo::Sequence *>(stack.top().data());
            sequence->append(var);
        } else if (stack.top().canConvert<QBluetoothServiceInfo::Alternative>()) {
            QBluetoothServiceInfo::Alternative *alternative = static_cast<QBluetoothServiceInfo::Alternative *>(stack.top().data());
            alternative->append(var);
        } else {
            qWarning("Unknown type in the QVariant, should be either a QBluetoothServiceInfo::Sequence or an QBluetoothServiceInfo::Alternative");
        }
    }
}

QTM_END_NAMESPACE
