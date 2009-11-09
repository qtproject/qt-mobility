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
#include "qmessageserviceaction.h"

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent)
{
}

QMessageServiceAction::~QMessageServiceAction()
{
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter);
    Q_UNUSED(ordering);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter);
    Q_UNUSED(body);
    Q_UNUSED(options);
    Q_UNUSED(ordering);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::countMessages(const QMessageFilter &filter) const
{
    Q_UNUSED(filter);
    return false;
}

bool QMessageServiceAction::send(QMessage &message)
{
    Q_UNUSED(message)
    return false; // stub
}

bool QMessageServiceAction::compose(const QMessage &message)
{
    Q_UNUSED(message)
    return false; // stub
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::show(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id)
    return false; // stub
}

QMessageServiceAction::State QMessageServiceAction::state() const
{
    return Pending; // stub
}

void QMessageServiceAction::cancelOperation()
{
}

QMessageStore::ErrorCode QMessageServiceAction::lastError() const
{
    return QMessageStore::NoError;
}
