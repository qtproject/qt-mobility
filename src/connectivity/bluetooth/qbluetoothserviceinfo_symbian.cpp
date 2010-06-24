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

#include "qbluetoothserviceinfo.h"
#include "qbluetoothserviceinfo_p.h"

#include <QUrl>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QBluetoothServiceInfoPrivate::QBluetoothServiceInfoPrivate()
:serviceRecord(0)
{
}

QBluetoothServiceInfoPrivate::~QBluetoothServiceInfoPrivate()
{
    if (sdpDatabase.SubSessionHandle() != 0)
        sdpDatabase.Close();

    if (sdpSession.Handle() != 0)
        sdpSession.Close();
}

static void convert(MSdpElementBuilder *list, const QList<QVariant> &vList)
{
    list->StartListL();

    foreach (const QVariant &v, vList) {
        switch (v.type()) {
        case QMetaType::Void:
            list->BuildNilL();
            break;
        case QMetaType::UChar:
            list->BuildUintL(TSdpIntBuf<unsigned char>(v.toUInt()));
            break;
        case QMetaType::UShort:
            list->BuildUintL(TSdpIntBuf<unsigned short>(v.toUInt()));
            break;
        case QMetaType::UInt:
            list->BuildUintL(TSdpIntBuf<uint>(v.toUInt()));
            break;
        case QMetaType::Char:
            list->BuildIntL(TSdpIntBuf<char>(v.toInt()));
            break;
        case QMetaType::Short:
            list->BuildIntL(TSdpIntBuf<short>(v.toInt()));
            break;
        case QMetaType::Int:
            list->BuildIntL(TSdpIntBuf<int>(v.toInt()));
            break;
        case QMetaType::QString:
            list->BuildStringL(TPtrC8(reinterpret_cast<const quint8*>(v.toString().toLocal8Bit().constData())));
            break;
        case QMetaType::Bool:
            list->BuildBooleanL(v.toBool());
            break;
        case QMetaType::QUrl:
            list->BuildURLL(TPtrC8(reinterpret_cast<const quint8*>(v.toUrl().toString().toLocal8Bit().constData())));
            break;
        case QVariant::UserType:
            if (v.userType() == qMetaTypeId<QBluetoothUuid>()) {
                QBluetoothUuid uuid = v.value<QBluetoothUuid>();
                switch (uuid.minimumSize()) {
                case 2:
                    list->BuildUUIDL(uuid.toUInt16());
                    break;
                case 4:
                    list->BuildUUIDL(uuid.toUInt32());
                    break;
                case 16: {
                    quint128 uuid128 = uuid.toUInt128();
                    quint32 *data = reinterpret_cast<quint32 *>(uuid128.data);
                    list->BuildUUIDL(TUUID(data[0], data[1], data[2], data[3]));
                    break;
                }
                }
            } else if (v.userType() == qMetaTypeId<QBluetoothServiceInfo::Sequence>()) {
                convert(list->BuildDESL(), v.value<QBluetoothServiceInfo::Sequence>());
            } else if (v.userType() == qMetaTypeId<QBluetoothServiceInfo::Alternative>()) {
                convert(list->BuildDEAL(), v.value<QBluetoothServiceInfo::Alternative>());
            }
            break;
        default:
            qDebug() << __FUNCTION__ << "Don't know how to handle variant type" << v.type();
        }
    }

    list->EndListL();
}

void QBluetoothServiceInfoPrivate::setRegisteredAttribute(quint16 attributeId, const QVariant &value) const
{
    CSdpAttrValue *sdpValue = 0;

    switch (value.type()) {
    case QMetaType::Void:
        sdpValue = CSdpAttrValueNil::NewNilL();
        break;
    case QMetaType::UChar:
        sdpValue = CSdpAttrValueUint::NewUintL(TSdpIntBuf<unsigned char>(value.toUInt()));
        break;
    case QMetaType::UShort:
        sdpValue = CSdpAttrValueUint::NewUintL(TSdpIntBuf<unsigned short>(value.toUInt()));
        break;
    case QMetaType::UInt:
        sdpValue = CSdpAttrValueUint::NewUintL(TSdpIntBuf<uint>(value.toUInt()));
        break;
    case QMetaType::Char:
        sdpValue = CSdpAttrValueInt::NewIntL(TSdpIntBuf<char>(value.toInt()));
        break;
    case QMetaType::Short:
        sdpValue = CSdpAttrValueInt::NewIntL(TSdpIntBuf<short>(value.toInt()));
        break;
    case QMetaType::Int:
        sdpValue = CSdpAttrValueInt::NewIntL(TSdpIntBuf<int>(value.toInt()));
        break;
    case QMetaType::QString:
        sdpValue = CSdpAttrValueString::NewStringL(TPtrC8(reinterpret_cast<const quint8*>(value.toString().toLocal8Bit().constData())));
        break;
    case QMetaType::Bool:
        sdpValue = CSdpAttrValueBoolean::NewBoolL(value.toBool());
        break;
    case QMetaType::QUrl:
        sdpValue = CSdpAttrValueURL::NewURLL(TPtrC8(reinterpret_cast<const quint8*>(value.toUrl().toString().toLocal8Bit().constData())));
        break;
    case QVariant::UserType:
        if (value.userType() == qMetaTypeId<QBluetoothUuid>()) {
            QBluetoothUuid uuid = value.value<QBluetoothUuid>();
            switch (uuid.minimumSize()) {
            case 2:
                sdpValue = CSdpAttrValueUUID::NewUUIDL(uuid.toUInt16());
                break;
            case 4:
                sdpValue = CSdpAttrValueUUID::NewUUIDL(uuid.toUInt32());
                break;
            case 16: {
                quint128 uuid128 = uuid.toUInt128();
                quint32 *data = reinterpret_cast<quint32 *>(uuid128.data);
                sdpValue = CSdpAttrValueUUID::NewUUIDL(TUUID(data[0], data[1], data[2], data[3]));
                break;
            }
            }
        } else if (value.userType() == qMetaTypeId<QBluetoothServiceInfo::Sequence>()) {
            CSdpAttrValueDES *sequence = CSdpAttrValueDES::NewDESL(0);
            convert(sequence, value.value<QBluetoothServiceInfo::Sequence>());
            sdpValue = sequence;
        } else if (value.userType() == qMetaTypeId<QBluetoothServiceInfo::Alternative>()) {
            CSdpAttrValueDEA *alternative = CSdpAttrValueDEA::NewDEAL(0);
            convert(alternative, value.value<QBluetoothServiceInfo::Alternative>());
            sdpValue = alternative;
        }
        break;
    default:
        qDebug() << __FUNCTION__ << "Don't know how to handle variant type" << value.type();
    }

    if (sdpValue) {
        sdpDatabase.UpdateAttributeL(serviceRecord, attributeId, *sdpValue);
        delete sdpValue;
    }
}

void QBluetoothServiceInfoPrivate::removeRegisteredAttribute(quint16 attributeId) const
{
    sdpDatabase.DeleteAttribute(serviceRecord, attributeId);
}

bool QBluetoothServiceInfoPrivate::ensureSdpConnection() const
{
    if (sdpSession.Handle() == 0) {
        int error = sdpSession.Connect();
        if (error != KErrNone) {
            qWarning("Could not open connection to SDP session (error %d).", error);
            return false;
        }
    }

    if (sdpDatabase.SubSessionHandle() == 0) {
        int error = sdpDatabase.Open(sdpSession);
        if (error != KErrNone) {
            qWarning("Could not open connection to SDP database (error %d).", error);
            return false;
        }
    }

    return true;
}

bool QBluetoothServiceInfo::isRegistered() const
{
    Q_D(const QBluetoothServiceInfo);

    return d->sdpDatabase.SubSessionHandle() != 0;
}

bool QBluetoothServiceInfo::registerService() const
{
    Q_D(const QBluetoothServiceInfo);

    if (!d->ensureSdpConnection())
        return false;

    d->sdpDatabase.CreateServiceRecordL(0, d->serviceRecord);

    foreach (quint16 id, d->attributes.keys())
        d->setRegisteredAttribute(id, d->attributes[id]);

    return true;
}

bool QBluetoothServiceInfo::unregisterService() const
{
    Q_D(const QBluetoothServiceInfo);

    if (!d->ensureSdpConnection())
        return false;

    d->sdpDatabase.DeleteRecord(d->serviceRecord);

    return true;
}

QTM_END_NAMESPACE
