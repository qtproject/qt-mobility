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

#include "qorganizerevent.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerEvent
  \brief The QOrganizerEvent class provides an event in time which may reoccur
  \inmodule QtOrganizer
  \ingroup organizer-items

  A QOrganizerEvent is an item which occurs at a particular point in time
  and may be associated with a location or have other details.  It may have
  a set of recurrence rules or dates on which the event occurs associated
  with it, and also exceptions to those recurrences.
 */

/*! Sets the start date time of the event to \a startDateTime */
void QOrganizerEvent::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    etr.setStartDateTime(startDateTime);
    saveDetail(&etr);
}

/*! Returns the date time at which the first instance of the event starts */
QDateTime QOrganizerEvent::startDateTime() const
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    return etr.startDateTime();
}

/*! Sets the end date time of the event to \a endDateTime */
void QOrganizerEvent::setEndDateTime(const QDateTime& endDateTime)
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    etr.setEndDateTime(endDateTime);
    saveDetail(&etr);
}

/*! Returns the date time at which the first instance of the event ends */
QDateTime QOrganizerEvent::endDateTime() const
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    return etr.endDateTime();
}

/*! Sets whether the time component of the start datetime or end datetime are significant. */
void QOrganizerEvent::setTimeSpecified(bool isTimeSpecified)
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    etr.setTimeSpecified(isTimeSpecified);
    saveDetail(&etr);
}

/*! Returns whether the time component of the start datetime or end datetime are significant. */
bool QOrganizerEvent::isTimeSpecified() const
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    return etr.isTimeSpecified();
}

/*! Sets the list of dates \a rdates to be dates on which the event occurs */
void QOrganizerEvent::setRecurrenceDates(const QList<QDate>& rdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceDates(rdates);
    saveDetail(&rec);
}

/*! Returns the list of dates which have been explicitly set as dates on which the event occurs */
QList<QDate> QOrganizerEvent::recurrenceDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceDates();
}

/*! Sets the list of recurrence rules \a rrules to be the rules which define when the event occurs, other than those dates
    specified explicitly via setRecurrenceDates(). */
void QOrganizerEvent::setRecurrenceRules(const QList<QOrganizerItemRecurrenceRule>& rrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceRules(rrules);
    saveDetail(&rec);
}

/*! Returns the list of recurrence rules which define when the event occurs */
QList<QOrganizerItemRecurrenceRule> QOrganizerEvent::recurrenceRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceRules();
}

/*! Sets the given list of dates \a exdates to be dates on which the event explicitly does not occur,
    when the recurrence rules suggest that the event should occur on those dates. */
void QOrganizerEvent::setExceptionDates(const QList<QDate>& exdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionDates(exdates);
    saveDetail(&rec);
}

/*! Returns the list of dates on which the event explicitly does not occur despite
    the recurrence rules for the event */
QList<QDate> QOrganizerEvent::exceptionDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionDates();
}

/*! Sets the given list of recurrence rules \a exrules to be the rules which define when
    the event does not occur. */
void QOrganizerEvent::setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& exrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionRules(exrules);
    saveDetail(&rec);
}

/*! Returns the list of exception rules for the event */
QList<QOrganizerItemRecurrenceRule> QOrganizerEvent::exceptionRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionRules();
}

/*! Sets the priority of this event to \a priority */
void QOrganizerEvent::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*! Returns the priority of the event */
QOrganizerItemPriority::Priority QOrganizerEvent::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*! Returns the name of the location at which the event occurs, if known */
QString QOrganizerEvent::locationName() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.locationName();
}

/*! Sets the name of the location at which the event occurs to \a locationName */
void QOrganizerEvent::setLocationName(const QString& locationName)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setLocationName(locationName);
    saveDetail(&ld);
}

/*! Returns the address of the location at which the event occurs, if known */
QString QOrganizerEvent::locationAddress() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.address();
}

/*! Sets the address of the location at which the event occurs to \a locationAddress */
void QOrganizerEvent::setLocationAddress(const QString& locationAddress)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setAddress(locationAddress);
    saveDetail(&ld);
}

/*! Returns the geo-coordinates of the location at which the event occurs, if known */
QString QOrganizerEvent::locationGeoCoordinates() const
{
    // XXX TODO: consistency with QOILocation API ?
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.geoLocation();
}

/*!
    \preliminary
    Sets the geo-coordinates of the location at which the event occurs to \a locationCoordinates.
    The coordinates should be in the format "latitude;longitude"
 */
void QOrganizerEvent::setLocationGeoCoordinates(const QString& locationCoordinates)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setGeoLocation(locationCoordinates);
    saveDetail(&ld);
}
