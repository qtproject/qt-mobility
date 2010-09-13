/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagefolderid_p.h"
#include "winhelpers_p.h"
#include <QByteArray>
#include <QDataStream>
#include <MAPIUtil.h>
#include <QDebug>
#include <messagingutil_p.h>

QTM_BEGIN_NAMESPACE

#ifdef _WIN32_WCE

QMessageFolderId QMessageFolderIdPrivate::from(const MapiRecordKey &folderKey, const MapiEntryId &storeKey, const MapiEntryId &entryId)
{
    QMessageFolderId result;
    result.d_ptr->_valid = true;
    result.d_ptr->_folderRecordKey = folderKey;
    result.d_ptr->_storeRecordKey = storeKey;
    result.d_ptr->_entryId = entryId;
    return result;
}

MapiEntryId QMessageFolderIdPrivate::storeRecordKey(const QMessageFolderId &id)
{
    return id.d_ptr->_storeRecordKey;
}

#else

QMessageFolderId QMessageFolderIdPrivate::from(const MapiRecordKey &folderKey, const MapiRecordKey &storeKey, const MapiEntryId &entryId)
{
    QMessageFolderId result;
    result.d_ptr->_valid = true;
    result.d_ptr->_folderRecordKey = folderKey;
    result.d_ptr->_storeRecordKey = storeKey;
    result.d_ptr->_entryId = entryId;
    return result;
}

MapiRecordKey QMessageFolderIdPrivate::storeRecordKey(const QMessageFolderId &id)
{
    return id.d_ptr->_storeRecordKey;
}

#endif

MapiRecordKey QMessageFolderIdPrivate::folderRecordKey(const QMessageFolderId &id)
{
    return id.d_ptr->_folderRecordKey;
}

MapiEntryId QMessageFolderIdPrivate::entryId(const QMessageFolderId &id)
{
    return id.d_ptr->_entryId;
}

QMessageFolderId::QMessageFolderId()
    : d_ptr(new QMessageFolderIdPrivate(this))
{
    d_ptr->_valid = false;
}

QMessageFolderId::QMessageFolderId(const QMessageFolderId& other)
    : d_ptr(new QMessageFolderIdPrivate(this))
{
    this->operator=(other);
}

QMessageFolderId::QMessageFolderId(const QString& id)
    : d_ptr(new QMessageFolderIdPrivate(this))
{
	QDataStream idStream(QByteArray::fromBase64(MessagingUtil::stripIdPrefix(id).toLatin1()));
    d_ptr->_valid = true;
#ifdef _WIN32_WCE
    idStream >> d_ptr->_entryId;
#else
    idStream >> d_ptr->_folderRecordKey;
#endif
    idStream >> d_ptr->_storeRecordKey;
    if (!idStream.atEnd())
#ifdef _WIN32_WCE
        idStream >> d_ptr->_folderRecordKey;
#else
        idStream >> d_ptr->_entryId;
#endif
}

QMessageFolderId::~QMessageFolderId()
{
    delete d_ptr;
}

QMessageFolderId& QMessageFolderId::operator=(const QMessageFolderId& other)
{
    if (&other != this) {
        d_ptr->_valid = other.d_ptr->_valid;
        d_ptr->_folderRecordKey = other.d_ptr->_folderRecordKey;
        d_ptr->_storeRecordKey = other.d_ptr->_storeRecordKey;
        d_ptr->_entryId = other.d_ptr->_entryId;
    }

    return *this;
}

bool QMessageFolderId::operator==(const QMessageFolderId& other) const
{
    if (isValid()) {
        if (other.isValid()) {
            bool result(true);
#ifdef _WIN32_WCE    
            QMessageManager::Error ignoredError(QMessageManager::NoError);
            MapiSessionPtr session(MapiSession::createSession(&ignoredError));
            if (ignoredError == QMessageManager::NoError) {
                result &= session->equal(d_ptr->_entryId, other.d_ptr->_entryId);
            } else {
                result = false;
                qWarning() << "Unable to compare entry IDs.";
            }
#else
            result &= (d_ptr->_folderRecordKey == other.d_ptr->_folderRecordKey);
#endif
            result &= (d_ptr->_storeRecordKey == other.d_ptr->_storeRecordKey);
            return result;
        }
        return false;
    } else {
        return !other.isValid();
    }
}

bool QMessageFolderId::operator<(const QMessageFolderId& other) const
{
    if (isValid() && other.isValid()) {
        if (d_ptr->_storeRecordKey == other.d_ptr->_storeRecordKey) {
            return (d_ptr->_folderRecordKey < other.d_ptr->_folderRecordKey);
        }

        return (d_ptr->_storeRecordKey < other.d_ptr->_storeRecordKey);
    } else if (!isValid() && other.isValid()) {
        return true; // Arbitrarily define invalid ids to be less than valid ids
    }

    return false;
}

QString QMessageFolderId::toString() const
{
    if (!isValid())
        return MessagingUtil::addIdPrefix(QString());
    QByteArray encodedId;
    QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
#ifdef _WIN32_WCE
    encodedIdStream << d_ptr->_entryId;
#else
    encodedIdStream << d_ptr->_folderRecordKey;
#endif
    encodedIdStream << d_ptr->_storeRecordKey;
#ifdef _WIN32_WCE
    if (d_ptr->_folderRecordKey.count())
        encodedIdStream << d_ptr->_folderRecordKey;
#else
    if (d_ptr->_entryId.count())
        encodedIdStream << d_ptr->_entryId;
#endif
	return MessagingUtil::addIdPrefix(encodedId.toBase64());
}

bool QMessageFolderId::isValid() const
{
#ifdef _WIN32_WCE
    return (d_ptr->_valid && !d_ptr->_entryId.isEmpty() && !d_ptr->_storeRecordKey.isEmpty());
#else
    return (d_ptr->_valid && !d_ptr->_folderRecordKey.isEmpty() && !d_ptr->_storeRecordKey.isEmpty());
#endif
}

uint QtMobility::qHash(const QTM_PREPEND_NAMESPACE(QMessageFolderId) &id)
{
#ifdef _WIN32_WCE
    return (QT_PREPEND_NAMESPACE(qHash)(id.d_ptr->_valid) ^ qHash(id.d_ptr->_entryId) ^ qHash(id.d_ptr->_storeRecordKey));
#else
    return (QT_PREPEND_NAMESPACE(qHash)(id.d_ptr->_valid) ^ qHash(id.d_ptr->_folderRecordKey) ^ qHash(id.d_ptr->_storeRecordKey));
#endif
}
QTM_END_NAMESPACE


