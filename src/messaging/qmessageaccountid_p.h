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
#ifndef QMESSAGEACCOUNTIDPRIVATE_H
#define QMESSAGEACCOUNTIDPRIVATE_H
#include "qmessageaccountid.h"
#if defined(Q_OS_WIN)
#include "winhelpers_p.h"
#endif

QTM_BEGIN_NAMESPACE

class QMessageAccountIdPrivate
{
    Q_DECLARE_PUBLIC(QMessageAccountId)

public:
    QMessageAccountIdPrivate(QMessageAccountId *accountId)
        :q_ptr(accountId)
    {
    }

    QMessageAccountId *q_ptr;
#if defined(Q_OS_WIN)

#ifdef _WIN32_WCE
    MapiEntryId _storeRecordKey;
    static QMessageAccountId from(const MapiEntryId &storeKey);
    static MapiEntryId storeRecordKey(const QMessageAccountId &id);
#else
    MapiRecordKey _storeRecordKey;
    static QMessageAccountId from(const MapiRecordKey &storeKey);
    static MapiRecordKey storeRecordKey(const QMessageAccountId &id);
#endif

#endif
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QString _id;
#endif    
};

QTM_END_NAMESPACE
#endif
