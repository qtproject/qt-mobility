/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qorganizertodo.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerTodo
  \brief The QOrganizerTodo class provides a task which should be completed
  \inmodule QtOrganizer
  \ingroup organizer-items

  A todo is an item which contains information about a task which has to
  be completed.  It might be associated with a particular point in time
  (for example, water the garden tomorrow evening) or it might have no
  specific temporal association (for example, climb Mount Everest someday).

  A todo can reoccur (for example, water the garden every evening) or it
  can occur only once.

  Todos can be used to schedule agenda items or tasks in a meaningful manner.
 */

/*!
  Sets the date time at which the task should be started to \a startDateTime
 */
void QOrganizerTodo::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    ttr.setStartDateTime(startDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time at which the task should be started
 */
QDateTime QOrganizerTodo::startDateTime() const
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    return ttr.startDateTime();
}

/*!
  Sets the date time by which the task should be completed to \a dueDateTime
 */
void QOrganizerTodo::setDueDateTime(const QDateTime& dueDateTime)
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    ttr.setDueDateTime(dueDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time by which the task should be completed
 */
QDateTime QOrganizerTodo::dueDateTime() const
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    return ttr.dueDateTime();
}

/*! Sets whether the time component of the start datetime or end datetime are significant. */
void QOrganizerTodo::setTimeSpecified(bool isTimeSpecified)
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    ttr.setTimeSpecified(isTimeSpecified);
    saveDetail(&ttr);
}

/*! Returns whether the time component of the start datetime or end datetime are significant. */
bool QOrganizerTodo::isTimeSpecified() const
{
    QOrganizerTodoTimeRange ttr = detail<QOrganizerTodoTimeRange>();
    return ttr.isTimeSpecified();
}

/*!
  Sets the dates on which the todo reoccurs to \a rdates
 */
void QOrganizerTodo::setRecurrenceDates(const QList<QDate>& rdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceDates(rdates);
    saveDetail(&rec);
}

/*!
  Returns the dates on which the todo reoccurs, which have been explicitly set
  by calling \l setRecurrenceDates()
 */
QList<QDate> QOrganizerTodo::recurrenceDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceDates();
}

/*!
  Sets the recurrence rules which define when the todo occurs to \a rrules
 */
void QOrganizerTodo::setRecurrenceRules(const QList<QOrganizerItemRecurrenceRule>& rrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceRules(rrules);
    saveDetail(&rec);
}

/*!
  Returns the list of recurrence rules which define when the todo occurs
 */
QList<QOrganizerItemRecurrenceRule> QOrganizerTodo::recurrenceRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceRules();
}

/*!
  Sets the dates on which the todo does not occur despite the date
  fulfilling the recurrence rules of the todo, to \a exdates
 */
void QOrganizerTodo::setExceptionDates(const QList<QDate>& exdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionDates(exdates);
    saveDetail(&rec);
}

/*!
  Returns the dates on which the todo does not occur, where it otherwise
  would occur as described by the recurrence rules.
 */
QList<QDate> QOrganizerTodo::exceptionDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionDates();
}

/*!
  Sets the recurrence rules which describe the dates on which the todo does
  not occur, where it otherwise would occur as described by the recurrence rules
  set with \l setRecurrenceRules(), to \a exrules
 */
void QOrganizerTodo::setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& exrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionRules(exrules);
    saveDetail(&rec);
}

/*!
  Returns the recurrence rules which describe the dates on which the todo
  does not occur, where it otherwise would occur as described by the recurrence rules
  set the \l setRecurrenceRules().
 */
QList<QOrganizerItemRecurrenceRule> QOrganizerTodo::exceptionRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionRules();
}

/*!
  Sets the priority of the todo to \a priority
 */
void QOrganizerTodo::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*!
  Returns the priority of the task.
 */
QOrganizerItemPriority::Priority QOrganizerTodo::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*!
  Sets the percentage of progress completed on the task described
  by the todo item to \a percentage
 */
void QOrganizerTodo::setProgressPercentage(int percentage)
{
    // XXX TODO: make the API more consistent with that in QOrganizerTodoProgress.
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setPercentageComplete(percentage);
    saveDetail(&tp);
}

/*!
  Returns the percentage of progress completed on the task described
  by the todo.
 */
int QOrganizerTodo::progressPercentage() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.percentageComplete();
}

/*!
  Sets the progress status of the task to \a status
 */
void QOrganizerTodo::setStatus(QOrganizerTodoProgress::Status status)
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setStatus(status);
    saveDetail(&tp);
}

/*!
  Returns the progress status of the task described by the todo
 */
QOrganizerTodoProgress::Status QOrganizerTodo::status() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.status();
}

/*!
  Sets the date and time at which the task was completed to \a finishedDateTime
 */
void QOrganizerTodo::setFinishedDateTime(const QDateTime& finishedDateTime)
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    tp.setFinishedDateTime(finishedDateTime);
    saveDetail(&tp);
}

/*!
  Returns the date and time at which the task was completed, if known.
 */
QDateTime QOrganizerTodo::finishedDateTime() const
{
    QOrganizerTodoProgress tp = detail<QOrganizerTodoProgress>();
    return tp.finishedDateTime();
}
