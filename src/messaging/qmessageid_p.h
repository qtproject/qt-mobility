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
#ifndef QMESSAGEIDPRIVATE_H
#define QMESSAGEIDPRIVATE_H
#include "qmessageid.h"
#if defined(Q_OS_WIN)
#include "winhelpers_p.h"
#endif


QTM_BEGIN_NAMESPACE

class QMessageIdPrivate
{
    Q_DECLARE_PUBLIC(QMessageId)

public:
    QMessageIdPrivate(QMessageId *id)
        :q_ptr(id)
    {
    }

    QMessageId *q_ptr;

#if defined(Q_OS_WIN)
    mutable MapiRecordKey _messageRecordKey;
    MapiEntryId _entryId;

#ifdef _WIN32_WCE
    MapiEntryId _folderRecordKey;
    MapiEntryId _storeRecordKey;
    static QMessageId from(const MapiEntryId &storeId, const MapiEntryId &entryId, const MapiRecordKey &messageKey = MapiRecordKey(), const MapiEntryId &folderId  = MapiEntryId());
    static MapiEntryId folderRecordKey(const QMessageId &id);
    static MapiEntryId storeRecordKey(const QMessageId &id);
#else
    mutable MapiRecordKey _folderRecordKey;
    MapiRecordKey _storeRecordKey;
    static QMessageId from(const MapiRecordKey &storeKey, const MapiEntryId &entryId, const MapiRecordKey &messageKey = MapiRecordKey(), const MapiRecordKey &folderKey  = MapiRecordKey());
    static MapiRecordKey folderRecordKey(const QMessageId &id);
    static MapiRecordKey storeRecordKey(const QMessageId &id);
#endif

    static MapiEntryId entryId(const QMessageId &id);
    static MapiRecordKey messageRecordKey(const QMessageId &id);
#endif
};

QTM_END_NAMESPACE
#endif
