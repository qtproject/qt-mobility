/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizertodooccurrence.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerTodoOccurrence
  \brief The QOrganizerTodoOccurrence class provides an occurrence of a task which should be completed
  \inmodule QtOrganizer
  \ingroup organizer-items
  \since 1.1

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
  Sets the date time at which the task should be started to \a startDateTime.  For all-day tasks,
  the time part can be set to any valid value.
  \since 1.1
 */
void QOrganizerTodoOccurrence::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    ttr.setStartDateTime(startDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time at which the task should be started.  For all-day tasks, the time part is
  meaningless.
  \since 1.1
 */
QDateTime QOrganizerTodoOccurrence::startDateTime() const
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    return ttr.startDateTime();
}

/*!
  Sets the date time by which the task should be completed to \a dueDateTime.  For all-day tasks,
  the time part can be set to any valid value.
  \since 1.1
 */
void QOrganizerTodoOccurrence::setDueDateTime(const QDateTime& dueDateTime)
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    ttr.setDueDateTime(dueDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time by which the task should be completed.  For all-day tasks, the time part is
  meaningless.
  \since 1.1
 */
QDateTime QOrganizerTodoOccurrence::dueDateTime() const
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    return ttr.dueDateTime();
}

/*!
  Sets the todo occurrence's parent to be the todo identified by the
  given \a parentId
  \since 1.1
 */
void QOrganizerTodoOccurrence::setParentId(const QOrganizerItemId& parentId)
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    origin.setParentId(parentId);
    saveDetail(&origin);
}

/*!
  Returns the id of the todo which is this occurrence's parent
  \since 1.1
 */
QOrganizerItemId QOrganizerTodoOccurrence::parentId() const
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    return origin.parentId();
}

/*!
  Sets the date at which this occurrence was originally going to occur,
  to the given \a date.
  \since 1.1
 */
void QOrganizerTodoOccurrence::setOriginalDate(const QDate& date)
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    origin.setOriginalDate(date);
    saveDetail(&origin);
}

/*!
  Returns the date at which the occurrence was originally going to occur.
  \since 1.1
 */
QDate QOrganizerTodoOccurrence::originalDate() const
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    return origin.originalDate();
}

/*!
  Sets the priority of the todo occurrence to \a priority
  \since 1.1
 */
void QOrganizerTodoOccurrence::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*!
  Returns the priority of the todo occurrence.
  \since 1.1
 */
QOrganizerItemPriority::Priority QOrganizerTodoOccurrence::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*!
  Sets the percentage of progress completed on the task described
  by the todo occurrence item to \a percentage
  \since 1.1
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
  \since 1.1
 */
int QOrganizerTodoOccurrence::progressPercentage() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.percentageComplete();
}

/*!
  Sets the progress status of the todo occurrence to \a status
  \since 1.1
 */
void QOrganizerTodoOccurrence::setStatus(QOrganizerTodoProgress::Status status)
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setStatus(status);
    saveDetail(&tp);
}

/*!
  Returns the progress status of the task described by the todo occurrence
  \since 1.1
 */
QOrganizerTodoProgress::Status QOrganizerTodoOccurrence::status() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.status();
}

/*!
  Sets the date and time at which the task described by the todo occurrence was completed to \a finishedDateTime
  \since 1.1
 */
void QOrganizerTodoOccurrence::setFinishedDateTime(const QDateTime& finishedDateTime)
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setFinishedDateTime(finishedDateTime);
    saveDetail(&tp);
}

/*!
  Returns the date and time at which the task described by the todo occurrence was completed, if known.
  \since 1.1
 */
QDateTime QOrganizerTodoOccurrence::finishedDateTime() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.finishedDateTime();
}
