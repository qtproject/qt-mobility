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

/*!
  \class QOrganizerTodoOccurrence
  \brief An occurrence of a task which should be completed
  \inmodule QtOrganizer
  \ingroup organizer-items

  A todo occurrence is a specific instance of a todo item.  An occurrence
  which is retrieved from a manager may not actually be persisted in that
  manager (for example, it may be generated automatically from the
  recurrence rule of the parent todo stored in the manager), in which case
  it will have a zero-id and differ from the parent todo only in its start
  date.  Alternatively, it may be persisted in the manager (that is, the
  client has saved the occurrence previously) where it is stored as an exception
  to its parent todo.
 */

/*!
  Sets the date time at which the task should be started to \a startDateTime
 */
void QOrganizerTodoOccurrence::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    ttr.setStartDateTime(startDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time at which the task should be started
 */
QDateTime QOrganizerTodoOccurrence::startDateTime() const
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    return ttr.startDateTime();
}

/*!
  Sets the date time by which the task should be completed to \a dueDateTime
 */
void QOrganizerTodoOccurrence::setDueDateTime(const QDateTime& dueDateTime)
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    ttr.setDueDateTime(dueDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time by which the task should be completed
 */
QDateTime QOrganizerTodoOccurrence::dueDateTime() const
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    return ttr.dueDateTime();
}

/*!
  Sets the todo occurrence's parent to be the todo identified by the
  given \a parentLocalId
 */
void QOrganizerTodoOccurrence::setParentLocalId(const QOrganizerItemLocalId& parentLocalId)
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    origin.setParentLocalId(parentLocalId);
    saveDetail(&origin);
}

/*!
  Returns the local id of the todo which is this occurrence's parent
 */
QOrganizerItemLocalId QOrganizerTodoOccurrence::parentLocalId() const
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    return origin.parentLocalId();
}

/*!
  Sets the date at which this occurrence was originally going to occur,
  to the given \a date.
 */
void QOrganizerTodoOccurrence::setOriginalDate(const QDate& date)
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    origin.setOriginalDate(date);
    saveDetail(&origin);
}

/*!
  Returns the date at which the occurrence was originally going to occur.
 */
QDate QOrganizerTodoOccurrence::originalDate() const
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    return origin.originalDate();
}

/*!
  Sets the priority of the todo occurrence to \a priority
 */
void QOrganizerTodoOccurrence::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*!
  Returns the priority of the todo occurrence.
 */
QOrganizerItemPriority::Priority QOrganizerTodoOccurrence::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*!
  Sets the percentage of progress completed on the task described
  by the todo occurrence item to \a percentage
 */
void QOrganizerTodoOccurrence::setProgressPercentage(int percentage)
{
    // XXX TODO: make the API more consistent with that in QOrganizerTodoProgress.
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setPercentageComplete(percentage);
    saveDetail(&tp);
}

/*!
  Returns the percentage of progress completed on the task described
  by the todo occurrence.
 */
int QOrganizerTodoOccurrence::progressPercentage() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.percentageComplete();
}

/*!
  Sets the progress status of the todo occurrence to \a status
 */
void QOrganizerTodoOccurrence::setStatus(QOrganizerTodoProgress::Status status)
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setStatus(status);
    saveDetail(&tp);
}

/*!
  Returns the progress status of the task described by the todo occurrence
 */
QOrganizerTodoProgress::Status QOrganizerTodoOccurrence::status() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.status();
}

/*!
  Sets the date and time at which the task described by the todo occurrence was completed to \a finishedDateTime
 */
void QOrganizerTodoOccurrence::setFinishedDateTime(const QDateTime& finishedDateTime)
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setFinishedDateTime(finishedDateTime);
    saveDetail(&tp);
}

/*!
  Returns the date and time at which the task described by the todo occurrence was completed, if known.
 */
QDateTime QOrganizerTodoOccurrence::finishedDateTime() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.finishedDateTime();
}
