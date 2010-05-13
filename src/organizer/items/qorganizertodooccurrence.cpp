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

QOrganizerTodoOccurrence::QOrganizerTodoOccurrence()
{
}

QOrganizerTodoOccurrence::QOrganizerTodoOccurrence(const QOrganizerTodoOccurrence& other)
    : QOrganizerItem(other)
{
}

QOrganizerTodoOccurrence& QOrganizerTodoOccurrence::operator=(const QOrganizerTodoOccurrence& other)
{
    QOrganizerItem::operator =(other);
    return *this;
}

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

void QOrganizerTodoOccurrence::setParentItemId(const QOrganizerItemId& parentId) const
{
    Q_UNUSED(parentId);
}

QOrganizerItemId QOrganizerTodoOccurrence::parentItemId() const
{
    return QOrganizerItemId();
}

void QOrganizerTodoOccurrence::setOriginalDateTime(const QDateTime& dateTime)
{
    Q_UNUSED(dateTime);
}

QDateTime QOrganizerTodoOccurrence::originalDateTime() const
{
    return QDateTime();
}

void QOrganizerTodoOccurrence::setPriority(QOrganizerItemPriority::Priority priority)
{
    Q_UNUSED(priority);
}

QOrganizerItemPriority::Priority QOrganizerTodoOccurrence::priority() const
{
    return QOrganizerItemPriority::UnknownPriority;
}

void QOrganizerTodoOccurrence::setProgressPercentage(int percentage)
{
    Q_UNUSED(percentage);
}

int QOrganizerTodoOccurrence::progressPercentage() const
{
    return 0;
}

void QOrganizerTodoOccurrence::setStatus(QOrganizerItemTodoProgress::Status status)
{
    Q_UNUSED(status);
}

QOrganizerItemTodoProgress::Status QOrganizerTodoOccurrence::status() const
{
    return QOrganizerItemTodoProgress::StatusNotStarted;
}

void QOrganizerTodoOccurrence::setStartedDateTime(const QDateTime& startDateTime)
{
    Q_UNUSED(startDateTime);
}

QDateTime QOrganizerTodoOccurrence::startedDateTime() const
{
    return QDateTime();
}

void QOrganizerTodoOccurrence::setFinishedDateTime(const QDateTime& finishedDateTime)
{
    Q_UNUSED(finishedDateTime);
}

QDateTime QOrganizerTodoOccurrence::finishedDateTime() const
{
    return QDateTime();
}
