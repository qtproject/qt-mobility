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
#include "qmessageservice.h"


QTM_BEGIN_NAMESPACE

QMessageService::QMessageService(QObject *parent)
    : QObject(parent)
{
}

QMessageService::~QMessageService()
{
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    Q_UNUSED(filter);
    Q_UNUSED(body);
    Q_UNUSED(matchFlags);
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageService::countMessages(const QMessageFilter &filter)
{
    // TODO: Implement this
    Q_UNUSED(filter);
    return false;
}

bool QMessageService::send(QMessage &message)
{
    Q_UNUSED(message)
    return false; // stub
}

bool QMessageService::compose(const QMessage &message)
{
    Q_UNUSED(message)
    return false; // stub
}

bool QMessageService::retrieveHeader(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::retrieveBody(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    Q_UNUSED(messageId)
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::show(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id)
    return false; // stub
}

QMessageService::State QMessageService::state() const
{
    return InactiveState; // stub
}

void QMessageService::cancel()
{
}

QMessageManager::Error QMessageService::error() const
{
    return QMessageManager::NoError;
}

QTM_END_NAMESPACE
