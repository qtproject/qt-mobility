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

#include "qorganizereventoccurrence.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerEventOccurrence
  \brief The QOrganizerEventOccurrence class provides an occurrence of an event.
  \inmodule QtOrganizer
  \ingroup organizer-items
  \since 1.1

  An event occurrence is where the occurrence differs from the generating
  event in some way.  An occurrence which is retrieved from a manager may not
  actually be persisted in that manager (for example, it may be generated
  automatically from the recurrence rule of the parent event stored in the
  manager), in which case it will have a zero-id and differ from the parent
  event only in its start date. Alternatively, it may be persisted in the
  manager (that is, the client has saved the occurrence previously) where it
  is stored as an exception to its parent event.
 */

/*!
  Sets the start date time of the event occurrence to \a startDateTime.
  instance).  For all-day events, the time part is meaningless.
  \since 1.1
 */
void QOrganizerEventOccurrence::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    etr.setStartDateTime(startDateTime);
    saveDetail(&etr);
}

/*!
  Returns the date time at which the event occurrence begins.
  For all-day events, the time part can be set to any valid value.
  \since 1.1
 */
QDateTime QOrganizerEventOccurrence::startDateTime() const
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    return etr.startDateTime();
}

/*!
  Sets the end date time of the event occurrence to \a endDateTime.
  For all-day events, the time part can be set to any valid value, and the date is to be interpreted
  as the inclusive end date.
  \since 1.1
 */
void QOrganizerEventOccurrence::setEndDateTime(const QDateTime& endDateTime)
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    etr.setEndDateTime(endDateTime);
    saveDetail(&etr);
}

/*!
  Returns the date time at which the event occurrence ends.
  For all-day events, the time part is meaningless, and the date is to be interpreted
  as the inclusive end date.
  \since 1.1
 */
QDateTime QOrganizerEventOccurrence::endDateTime() const
{
    QOrganizerEventTime etr = detail<QOrganizerEventTime>();
    return etr.endDateTime();
}

/*!
  Sets the event occurrence's parent to be the event identified by the
  given \a parentId
  \since 1.1
 */
void QOrganizerEventOccurrence::setParentId(const QOrganizerItemId& parentId)
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    origin.setParentId(parentId);
    saveDetail(&origin);
}

/*!
  Returns the id of the event which is this occurrence's parent
  \since 1.1
 */
QOrganizerItemId QOrganizerEventOccurrence::parentId() const
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    return origin.parentId();
}

/*!
  Sets the date at which this occurrence was originally going to occur,
  to the given \a date.
  \since 1.1
 */
void QOrganizerEventOccurrence::setOriginalDate(const QDate& date)
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    origin.setOriginalDate(date);
    saveDetail(&origin);
}

/*!
  Returns the date at which the occurrence was originally going to occur.
  \since 1.1
 */
QDate QOrganizerEventOccurrence::originalDate() const
{
    QOrganizerItemParent origin = detail<QOrganizerItemParent>();
    return origin.originalDate();
}

/*!
  Sets the priority of the event occurrence to \a priority
  \since 1.1
 */
void QOrganizerEventOccurrence::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*!
  Returns the priority of the event occurrence
  \since 1.1
 */
QOrganizerItemPriority::Priority QOrganizerEventOccurrence::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*!
  Returns the label of the location at which the event occurrence is held, if known
  \since 1.1
 */
QString QOrganizerEventOccurrence::location() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.label();
}

/*!
  Sets the label of the location at which the event occurrence is held to \a location
  \since 1.1
 */
void QOrganizerEventOccurrence::setLocation(const QString& location)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setLabel(location);
    saveDetail(&ld);
}
