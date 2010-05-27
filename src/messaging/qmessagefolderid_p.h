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
#ifndef QMESSAGEFOLDERIDPRIVATE_H
#define QMESSAGEFOLDERIDPRIVATE_H
#include "qmessagefolderid.h"
#if defined(Q_OS_WIN)
#include "winhelpers_p.h"
#endif

#define FOLDER_PREFIX_SMS   "SMS_"

#define FOLDER_ID_INBOX     "SMS_InboxFolder"
#define FOLDER_ID_OUTBOX    "SMS_OutboxFolder"
#define FOLDER_ID_DRAFTS    "SMS_DraftsFolder"
#define FOLDER_ID_SENT      "SMS_SentFolder"
#define FOLDER_ID_TRASH     "SMS_TrashFolder"

QTM_BEGIN_NAMESPACE

class QMessageFolderIdPrivate
{
    Q_DECLARE_PUBLIC(QMessageFolderId)

public:
    QMessageFolderIdPrivate(QMessageFolderId *folderId)
        :q_ptr(folderId)
    {
    }

public:
    QMessageFolderId *q_ptr;
#if defined(Q_OS_WIN)

#ifdef _WIN32_WCE
    MapiEntryId _storeRecordKey;
    static QMessageFolderId from(const MapiRecordKey &folderKey, const MapiEntryId &storeKey, const MapiEntryId &entryId);
    static MapiEntryId storeRecordKey(const QMessageFolderId &id);
#else
    MapiRecordKey _storeRecordKey;
    static QMessageFolderId from(const MapiRecordKey &folderKey, const MapiRecordKey &storeKey, const MapiEntryId &entryId = MapiEntryId());
    static MapiRecordKey storeRecordKey(const QMessageFolderId &id);
#endif

    bool _valid;
    MapiEntryId _entryId;
    MapiRecordKey _folderRecordKey;
    static MapiRecordKey folderRecordKey(const QMessageFolderId &id);
    static MapiEntryId entryId(const QMessageFolderId &id);

#endif
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QString _id;
#endif    
};

QTM_END_NAMESPACE

#endif
