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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    ttr.setStartDateTime(startDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time at which the task should be started
 */
QDateTime QOrganizerTodo::startDateTime() const
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    return ttr.startDateTime();
}

/*!
  Sets the date time by which the task should be completed to \a dueDateTime
 */
void QOrganizerTodo::setDueDateTime(const QDateTime& dueDateTime)
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    ttr.setDueDateTime(dueDateTime);
    saveDetail(&ttr);
}

/*!
  Returns the date time by which the task should be completed
 */
QDateTime QOrganizerTodo::dueDateTime() const
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    return ttr.dueDateTime();
}

/*!
  Sets whether the time-of-day component of the todo's start date-time or end date-time is
  insignificant (eg. \a isAllDay is generally set to true for a birthday).  If \a isAllDay is true,
  the time-of-day component is considered insignificant, and the todo will be an all-day
  item.
 */
void QOrganizerTodo::setAllDay(bool isAllDay)
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    ttr.setAllDay(isAllDay);
    saveDetail(&ttr);
}

/*! Returns true if and only if the time component of the start date/time or end date/time are
 * insignificant. */
bool QOrganizerTodo::isAllDay() const
{
    QOrganizerTodoTime ttr = detail<QOrganizerTodoTime>();
    return ttr.isAllDay();
}

/*!
  Sets the dates on which the todo reoccurs to \a rdates
 */
void QOrganizerTodo::setRecurrenceDates(const QSet<QDate>& rdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceDates(rdates);
    saveDetail(&rec);
}

/*!
  Returns the dates on which the todo reoccurs, which have been explicitly set
  by calling \l setRecurrenceDates()
 */
QSet<QDate> QOrganizerTodo::recurrenceDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceDates();
}

/*!
  Clears the set of recurrence rules which define when the todo occurs, and replaces
  it will the single recurrence rule \a rrule.
 */
void QOrganizerTodo::setRecurrenceRule(const QOrganizerRecurrenceRule& rrule)
{
    setRecurrenceRules(QSet<QOrganizerRecurrenceRule>() << rrule);
}

/*!
  Sets the recurrence rules which define when the todo occurs to \a rrules
 */
void QOrganizerTodo::setRecurrenceRules(const QSet<QOrganizerRecurrenceRule>& rrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceRules(rrules);
    saveDetail(&rec);
}

/*!
  Returns a recurrence rule which defines when the todo occurs.
  If more than one recurrence rule has been set in the todo,
  one will be returned at random.
 */
QOrganizerRecurrenceRule QOrganizerTodo::recurrenceRule() const
{
    QSet<QOrganizerRecurrenceRule> rrules = recurrenceRules();
    if (!rrules.isEmpty())
        return *rrules.begin();
    return QOrganizerRecurrenceRule();
}

/*!
  Returns the list of recurrence rules which define when the todo occurs
 */
QSet<QOrganizerRecurrenceRule> QOrganizerTodo::recurrenceRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceRules();
}

/*!
  Sets the dates on which the todo does not occur despite the date
  fulfilling the recurrence rules of the todo, to \a exdates
 */
void QOrganizerTodo::setExceptionDates(const QSet<QDate>& exdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionDates(exdates);
    saveDetail(&rec);
}

/*!
  Returns the dates on which the todo does not occur, where it otherwise
  would occur as described by the recurrence rules.
 */
QSet<QDate> QOrganizerTodo::exceptionDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionDates();
}

/*!
  Clears the set of recurrence rules which describe the dates on which the todo does
  not occur, where it otherwise would occur as described by the recurrence rules, and
  inserts into the cleared list the single exception rule \a exrule.
 */
void QOrganizerTodo::setExceptionRule(const QOrganizerRecurrenceRule& exrule)
{
    setExceptionRules(QSet<QOrganizerRecurrenceRule>() << exrule);
}

/*!
  Sets the recurrence rules which describe the dates on which the todo does
  not occur, where it otherwise would occur as described by the recurrence rules
  set with \l setRecurrenceRules(), to \a exrules
 */
void QOrganizerTodo::setExceptionRules(const QSet<QOrganizerRecurrenceRule>& exrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionRules(exrules);
    saveDetail(&rec);
}

/*!
  Returns a recurrence rule which describe the dates on which the todo does not occur, where
  it otherwise would occur as described by the recurrence rules.  If more than one exception
  rule was set for the todo item, one of those exception rules will be returned at random.
 */
QOrganizerRecurrenceRule QOrganizerTodo::exceptionRule() const
{
    QSet<QOrganizerRecurrenceRule> exrules = exceptionRules();
    if (!exrules.isEmpty())
        return *exrules.begin();
    return QOrganizerRecurrenceRule();
}

/*!
  Returns the recurrence rules which describe the dates on which the todo
  does not occur, where it otherwise would occur as described by the recurrence rules
  set the \l setRecurrenceRules().
 */
QSet<QOrganizerRecurrenceRule> QOrganizerTodo::exceptionRules() const
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
