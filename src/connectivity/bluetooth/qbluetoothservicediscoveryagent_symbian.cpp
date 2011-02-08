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

#include <QUrl>

#include <arpa/inet.h>
#include <netinet/in.h>

QTM_BEGIN_NAMESPACE

QBluetoothServiceDiscoveryAgentPrivate::QBluetoothServiceDiscoveryAgentPrivate(const QBluetoothAddress &address)
    : error(QBluetoothServiceDiscoveryAgent::NoError)
    , state(Inactive)
    , deviceAddress(address)
    , deviceDiscoveryAgent(0)
    , mode(QBluetoothServiceDiscoveryAgent::MinimalDiscovery)
    , m_sdpAgent(NULL)
    , m_filter(NULL)
    , m_attributes(NULL)
{
    TRAPD(err,
        m_filter = CSdpSearchPattern::NewL();
        m_attributes = CSdpAttrIdMatchList::NewL();
        )

    if (!err)
        TRAP(err, m_attributes->AddL(KAttrRangeAll);)
        
    if (err != KErrNone)
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
}

QBluetoothServiceDiscoveryAgentPrivate::~QBluetoothServiceDiscoveryAgentPrivate()
{
    delete m_filter;
    delete m_attributes;
    delete m_sdpAgent;
}

void QBluetoothServiceDiscoveryAgentPrivate::start(const QBluetoothAddress &address)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);
    TRAPD(err, startL(address));
    if (err != KErrNone) {
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::startL(const QBluetoothAddress &address)
{
    initAgentL(address);
    if (m_filter) {
        m_filter->Reset();
        if (uuidFilter.isEmpty()) {
            m_filter->AddL(QBluetoothUuid::PublicBrowseGroup);
        } else {
            foreach (const QBluetoothUuid &uuid, uuidFilter) {
                if (uuid.minimumSize() == 2) {
                    TUUID sUuid(uuid.toUInt16());
                    m_filter->AddL(sUuid);
                } else if (uuid.minimumSize() == 4) {
                    TUUID sUuid(uuid.toUInt32());
                    m_filter->AddL(sUuid);
                } else if (uuid.minimumSize() == 16) {
                    TUint32 *dataPointer = (TUint32*)uuid.toUInt128().data;
                    TUint32 lL = *(dataPointer++);
                    TUint32 lH = *(dataPointer++);
                    TUint32 hL = *(dataPointer++);
                    TUint32 hH = *(dataPointer);
                    TUUID sUuid(hH, hL, lH, lL);
                    m_filter->AddL(sUuid);
                } else {
                    // filter size can be 0 on error cases, searching all services
                    m_filter->AddL(QBluetoothUuid::PublicBrowseGroup);
                }
            }
        }
        m_sdpAgent->SetRecordFilterL(*m_filter);
        m_sdpAgent->NextRecordRequestL();
    } else
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
}

void QBluetoothServiceDiscoveryAgentPrivate::stop()
{
    if (m_sdpAgent) {
        m_sdpAgent->Cancel();
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::initAgentL(const QBluetoothAddress &address)
{
    TBTDevAddr btAddress(address.toUInt64());

    if (m_sdpAgent) {
        delete m_sdpAgent;
        m_sdpAgent = NULL;
    }
    //Trapped in Start
    m_sdpAgent = CSdpAgent::NewL(*this, btAddress);
}

bool QBluetoothServiceDiscoveryAgentPrivate::quickDiscovery(const QBluetoothAddress &address, const QBluetoothDeviceInfo &info)
{
    //not needed
    return false;
}

void QBluetoothServiceDiscoveryAgentPrivate::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    if (aError == KErrNone && aTotalRecordsCount > 0 && m_sdpAgent) {
        // request attributes
        TRAPD(err, m_sdpAgent->AttributeRequestL(aHandle, *m_attributes));
        if (err) {
            error = QBluetoothServiceDiscoveryAgent::UnknownError;
            emit q->error(error);
        }
    } else if (aError == KErrEof) {
        _q_serviceDiscoveryFinished();
    } else {
        _q_serviceDiscoveryFinished();
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::AttributeRequestResult(TSdpServRecordHandle, TSdpAttributeID aAttrID, CSdpAttrValue *aAttrValue)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);
    m_currentAttributeId = aAttrID;
    TRAPD(err, aAttrValue->AcceptVisitorL(*this));
    delete aAttrValue;
    if (m_stack.size() != 1) {
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
        return;
    }

    m_serviceInfo.setAttribute(aAttrID, m_stack.pop());

    if (err != KErrNone) {
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    if (aError == KErrNone && m_sdpAgent) {
        m_serviceInfo.setDevice(discoveredDevices.at(0));
        discoveredServices.append(m_serviceInfo);
        m_serviceInfo = QBluetoothServiceInfo();
        TRAPD(err, m_sdpAgent->NextRecordRequestL());
        if (err != KErrNone) {
            error = QBluetoothServiceDiscoveryAgent::UnknownError;
            emit q->error(error);
        }
    } else if (aError != KErrEof) {
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
    }

    if (m_serviceInfo.isValid())
        emit q->serviceDiscovered(discoveredServices.last());
}

void QBluetoothServiceDiscoveryAgentPrivate::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
{
    QVariant var;
    TUint datasize = aValue.DataSize();
    
    switch (aType) {
    case ETypeNil:
        break;
    case ETypeUint:
        if (datasize == 8) {
            TUint64 value;
            aValue.Uint64(value);
            var = QVariant::fromValue(value);
        } else
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
        m_stack.push(QVariant::fromValue(QBluetoothServiceInfo::Sequence()));
        break;
    case ETypeDEA:
        m_stack.push(QVariant::fromValue(QBluetoothServiceInfo::Alternative()));
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
        if (m_stack.size() == 0) {
            // single value attribute, just push onto stack
            m_stack.push(var);
        } else if (m_stack.size() >= 1) {
            // sequence or alternate attribute, add non-DES -DEA values to DES or DEA
            if (m_stack.top().canConvert<QBluetoothServiceInfo::Sequence>()) {
                QBluetoothServiceInfo::Sequence *sequence = static_cast<QBluetoothServiceInfo::Sequence *>(m_stack.top().data());
                sequence->append(var);
            } else if (m_stack.top().canConvert<QBluetoothServiceInfo::Alternative>()) {
                QBluetoothServiceInfo::Alternative *alternative = static_cast<QBluetoothServiceInfo::Alternative *>(m_stack.top().data());
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
    if (m_stack.size() > 1) {
        // finished a sequence or alternative add it to the parent sequence or alternative
        QVariant var = m_stack.pop();
        if (m_stack.top().canConvert<QBluetoothServiceInfo::Sequence>()) {
            QBluetoothServiceInfo::Sequence *sequence = static_cast<QBluetoothServiceInfo::Sequence *>(m_stack.top().data());
            sequence->append(var);
        } else if (m_stack.top().canConvert<QBluetoothServiceInfo::Alternative>()) {
            QBluetoothServiceInfo::Alternative *alternative = static_cast<QBluetoothServiceInfo::Alternative *>(m_stack.top().data());
            alternative->append(var);
        } else {
            qWarning("Unknown type in the QVariant, should be either a QBluetoothServiceInfo::Sequence or an QBluetoothServiceInfo::Alternative");
        }
    }
}

QTM_END_NAMESPACE
