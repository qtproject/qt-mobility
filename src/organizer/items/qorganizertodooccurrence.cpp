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

#include "qorganizertodooccurrence.h"

QTM_USE_NAMESPACE

void QOrganizerTodoOccurrence::setNotBeforeDateTime(const QDateTime& notBeforeDateTime)
{
    Q_UNUSED(notBeforeDateTime);
}

QDateTime QOrganizerTodoOccurrence::notBeforeDateTime() const
{
    return QDateTime();
}

void QOrganizerTodoOccurrence::setDueDateTime(const QDateTime& dueDateTime)
{
    Q_UNUSED(dueDateTime);
}

QDateTime QOrganizerTodoOccurrence::dueDateTime() const
{
    return QDateTime();
}

void QOrganizerTodoOccurrence::setParentItemLocalId(const QOrganizerItemLocalId& parentLocalId)
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    origin.setParentLocalId(parentLocalId);
    saveDetail(&origin);
}

QOrganizerItemLocalId QOrganizerTodoOccurrence::parentItemLocalId() const
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    return origin.parentLocalId();
}

void QOrganizerTodoOccurrence::setOriginalDate(const QDate& date)
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    origin.setOriginalDate(date);
    saveDetail(&origin);
}

QDate QOrganizerTodoOccurrence::originalDate() const
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    return origin.originalDate();
}

void QOrganizerTodoOccurrence::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

QOrganizerItemPriority::Priority QOrganizerTodoOccurrence::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

void QOrganizerTodoOccurrence::setProgressPercentage(int percentage)
{
    // XXX TODO: make the API more consistent with that in QOrganizerItemTodoProgress.
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    tp.setPercentageComplete(percentage);
    saveDetail(&tp);
}

int QOrganizerTodoOccurrence::progressPercentage() const
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    return tp.percentageComplete();
}

void QOrganizerTodoOccurrence::setStatus(QOrganizerItemTodoProgress::Status status)
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    tp.setStatus(status);
    saveDetail(&tp);
}

QOrganizerItemTodoProgress::Status QOrganizerTodoOccurrence::status() const
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    return tp.status();
}

void QOrganizerTodoOccurrence::setStartedDateTime(const QDateTime& startedDateTime)
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    tp.setStartedDateTime(startedDateTime);
    saveDetail(&tp);
}

QDateTime QOrganizerTodoOccurrence::startedDateTime() const
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    return tp.startedDateTime();
}

void QOrganizerTodoOccurrence::setFinishedDateTime(const QDateTime& finishedDateTime)
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    tp.setFinishedDateTime(finishedDateTime);
    saveDetail(&tp);
}

QDateTime QOrganizerTodoOccurrence::finishedDateTime() const
{
    QOrganizerItemTodoProgress tp = detail<QOrganizerItemTodoProgress>();
    return tp.finishedDateTime();
}
