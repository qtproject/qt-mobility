/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessageid_p.h"
#include <QByteArray>
#include <QDataStream>
#include <MAPIUtil.h>
#include <QDebug>
#include <messagingutil_p.h>

QTM_BEGIN_NAMESPACE

#ifdef _WIN32_WCE

QMessageId QMessageIdPrivate::from(const MapiEntryId &storeKey, const MapiEntryId &entryId, const MapiRecordKey &messageKey, const MapiEntryId &folderKey)
{
    QMessageId result;
    if (!result.d_ptr)
        result.d_ptr = new QMessageIdPrivate(&result);

    result.d_ptr->_storeRecordKey = storeKey;
    result.d_ptr->_entryId = entryId;
    result.d_ptr->_folderRecordKey = folderKey;
    result.d_ptr->_messageRecordKey = messageKey;

    return result;
}

MapiEntryId QMessageIdPrivate::storeRecordKey(const QMessageId &id)
{
    if (id.isValid()) {
        return id.d_ptr->_storeRecordKey;
    }

    return MapiEntryId();
}

MapiEntryId QMessageIdPrivate::folderRecordKey(const QMessageId &id)
{
    if (id.isValid()) {
        if (!id.d_ptr->_folderRecordKey.isEmpty()) {
            return id.d_ptr->_folderRecordKey;
        } else {
            // Look up the folder record key for this ID
            QMessageManager::Error ignoredError(QMessageManager::NoError);
            MapiSessionPtr session(MapiSession::createSession(&ignoredError));
            if (ignoredError == QMessageManager::NoError) {
                return session->folderEntryId(&ignoredError, id);
            }
        }
    }

    return MapiEntryId();
}

#else

QMessageId QMessageIdPrivate::from(const MapiRecordKey &storeKey, const MapiEntryId &entryId, const MapiRecordKey &messageKey, const MapiRecordKey &folderKey)
{
    QMessageId result;
    if (!result.d_ptr)
        result.d_ptr = new QMessageIdPrivate(&result);

    result.d_ptr->_storeRecordKey = storeKey;
    result.d_ptr->_entryId = entryId;
    result.d_ptr->_folderRecordKey = folderKey;
    result.d_ptr->_messageRecordKey = messageKey;

    return result;
}

MapiRecordKey QMessageIdPrivate::folderRecordKey(const QMessageId &id)
{
    MapiRecordKey result;

    if (id.isValid()) {
        if (id.d_ptr->_folderRecordKey.isEmpty()) {
            // Look up the folder record key for this ID
            QMessageManager::Error ignoredError(QMessageManager::NoError);
            MapiSessionPtr session(MapiSession::createSession(&ignoredError));
            if (ignoredError == QMessageManager::NoError) {
                id.d_ptr->_folderRecordKey = session->folderRecordKey(&ignoredError, id);
            }
        }

        result = id.d_ptr->_folderRecordKey;
    }

    return result;
}

MapiRecordKey QMessageIdPrivate::storeRecordKey(const QMessageId &id)
{
    if (id.isValid()) {
        return id.d_ptr->_storeRecordKey;
    }

    return MapiRecordKey();
}

#endif

MapiEntryId QMessageIdPrivate::entryId(const QMessageId &id)
{
    if (id.isValid()) {
        return id.d_ptr->_entryId;
    }

    return MapiEntryId();
}

MapiRecordKey QMessageIdPrivate::messageRecordKey(const QMessageId &id)
{
    MapiRecordKey result;

    if (id.isValid()) {
        if (id.d_ptr->_messageRecordKey.isEmpty()) {
            // Look up the message record key for this ID
            QMessageManager::Error ignoredError(QMessageManager::NoError);
            MapiSessionPtr session(MapiSession::createSession(&ignoredError));
            if (ignoredError == QMessageManager::NoError) {
                id.d_ptr->_messageRecordKey = session->messageRecordKey(&ignoredError, id);
            }
        }

        result = id.d_ptr->_messageRecordKey;
    }

    return result;
}

QMessageId::QMessageId()
    : d_ptr(0)
{
}

QMessageId::QMessageId(const QMessageId& other)
    : d_ptr(0)
{
    this->operator=(other);
}

QMessageId::QMessageId(const QString& id)
    : d_ptr(new QMessageIdPrivate(this))
{
	QDataStream idStream(QByteArray::fromBase64(MessagingUtil::stripIdPrefix(id).toLatin1()));

#ifdef _WIN32_WCE
    idStream >> d_ptr->_entryId;
#else
    idStream >> d_ptr->_messageRecordKey;
#endif

    idStream >> d_ptr->_folderRecordKey;
    idStream >> d_ptr->_storeRecordKey;

#ifndef _WIN32_WCE
    QMessageManager::Error ignoredError(QMessageManager::NoError);
    MapiSessionPtr session(MapiSession::createSession(&ignoredError));
    if (ignoredError == QMessageManager::NoError) {
        d_ptr->_entryId = session->messageEntryId(&ignoredError, d_ptr->_storeRecordKey, d_ptr->_folderRecordKey, d_ptr->_messageRecordKey);
    }
#endif
}

QMessageId::~QMessageId()
{
    delete d_ptr;
}

QMessageId& QMessageId::operator=(const QMessageId& other)
{
    if (&other != this) {
        if (other.isValid()) {
            if (!d_ptr) {
                d_ptr = new QMessageIdPrivate(this);
            }
            d_ptr->_messageRecordKey = other.d_ptr->_messageRecordKey;
            d_ptr->_folderRecordKey = other.d_ptr->_folderRecordKey;
            d_ptr->_storeRecordKey = other.d_ptr->_storeRecordKey;
            d_ptr->_entryId = other.d_ptr->_entryId;
        } else {
            delete d_ptr;
            d_ptr = 0;
        }
    }

    return *this;
}

bool QMessageId::operator==(const QMessageId& other) const
{
    if (isValid()) {
        if (other.isValid()) {
            bool result(d_ptr->_storeRecordKey == other.d_ptr->_storeRecordKey);
                if (result) {
                    QMessageManager::Error ignoredError(QMessageManager::NoError);
                    MapiSessionPtr session(MapiSession::createSession(&ignoredError));
                    if (ignoredError == QMessageManager::NoError) {
                        result &= session->equal(d_ptr->_entryId, other.d_ptr->_entryId);
                    } else {
                        result = false;
                        qWarning() << "Unable to compare entry IDs.";
                    }
                }
            return result;
        }
        return false;
    } else {
        return !other.isValid();
    }
}

bool QMessageId::operator<(const QMessageId& other) const
{
    if (d_ptr && other.d_ptr) {
        if (operator==(other)) {
            return false;
        }

        return (d_ptr->_entryId < other.d_ptr->_entryId);
    }

    if (d_ptr) {
        return false;
    } else if (other.d_ptr) {
        return true;
    }

    return false;
}

QString QMessageId::toString() const
{
    if (!isValid())
        return MessagingUtil::addIdPrefix(QString());

#ifndef _WIN32_WCE
    if (d_ptr->_messageRecordKey.isEmpty())
        d_ptr->_messageRecordKey = QMessageIdPrivate::messageRecordKey(*this);
#endif

    if (d_ptr->_folderRecordKey.isEmpty())
        d_ptr->_folderRecordKey = QMessageIdPrivate::folderRecordKey(*this);

    QByteArray encodedId;
    QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
#ifdef _WIN32_WCE
    encodedIdStream << d_ptr->_entryId;
#else
    encodedIdStream << d_ptr->_messageRecordKey;
#endif
    encodedIdStream << d_ptr->_folderRecordKey;
    encodedIdStream << d_ptr->_storeRecordKey;

	return MessagingUtil::addIdPrefix(encodedId.toBase64());
}

bool QMessageId::isValid() const
{
    return (d_ptr && !d_ptr->_storeRecordKey.isEmpty() && !d_ptr->_entryId.isEmpty());
}

uint qHash(const QMessageId &id)
{
    return qHash(id.d_ptr->_storeRecordKey) ^ qHash(id.d_ptr->_entryId);
}


QTM_END_NAMESPACE
