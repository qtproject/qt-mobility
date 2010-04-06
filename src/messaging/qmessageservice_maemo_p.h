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

#include "qmessageservice.h"

QTM_BEGIN_NAMESPACE

class QMessageServicePrivate
{
public:
    enum EnginesToCall
    {
        EnginesToCallModest    = 0x1,
        EnginesToCallTelepathy = 0x2,
        // Extensible
        EnginesToCallAll = 0xFFFFFFFF,
    };

    QMessageServicePrivate(QMessageService* parent);
    ~QMessageServicePrivate();

    static QMessageServicePrivate* implementation(const QMessageService &service);

    bool queryMessages(QMessageService &messageService, const QMessageFilter &filter,
                       const QMessageSortOrder &sortOrder, uint limit, uint offset,
                       EnginesToCall enginesToCall = EnginesToCallAll);
    bool queryMessages(QMessageService &messageService, const QMessageFilter &filter,
                       const QString &body, QMessageDataComparator::MatchFlags matchFlags,
                       const QMessageSortOrder &sortOrder, uint limit, uint offset,
                       EnginesToCall enginesToCall = EnginesToCallAll);
    bool countMessages(QMessageService &messageService, const QMessageFilter &filter,
                       EnginesToCall enginesToCall = EnginesToCallAll);

    void setFinished(bool successful);
    void stateChanged(QMessageService::State state);
    void messagesFound(const QMessageIdList &ids, bool isFiltered, bool isSorted);
    void messagesCounted(int count);
    void progressChanged(uint value, uint total);

public:
    QMessageService* q_ptr;
    QMessageService::State _state;
    QMessageManager::Error _error;
    bool _active;
    int _actionId;
    int _pendingRequestCount;

    QMessageIdList _ids;
    int _count;
    bool _sorted;
    bool _filtered;

    QMessageFilter _filter;
    QMessageSortOrder _sortOrder;
    int _limit;
    int _offset;
};

QTM_END_NAMESPACE
