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

#include "qorganizereventoccurrence.h"

QTM_USE_NAMESPACE

/*!
  \class QOrganizerEventOccurrence
  \brief An occurrence of an event
  \inmodule QtOrganizer
  \ingroup organizer-items

  An event occurrence is an occurrence of an event where the occurrence
  differs from the generating event in some way.  An occurrence which is
  retrieved from a manager may not actually be persisted in that manager
  (for example, it may be generated automatically from the recurrence rule
  of the parent event stored in the manager), in which case it will have a
  zero-id and differ from the parent event only in its start date.
  Alternatively, it may be persisted in the manager (that is, the client
  has saved the occurrence previously) where it is stored as an exception
  to its parent event.
 */

/*!
  Sets the start date time of the event occurrence to \a startDateTime
 */
void QOrganizerEventOccurrence::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    etr.setStartDateTime(startDateTime);
    saveDetail(&etr);
}

/*!
  Returns the date time at which the event occurrence begins
 */
QDateTime QOrganizerEventOccurrence::startDateTime() const
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    return etr.startDateTime();
}

/*!
  Sets the end date time of the event occurrence to \a endDateTime
 */
void QOrganizerEventOccurrence::setEndDateTime(const QDateTime& endDateTime)
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    etr.setEndDateTime(endDateTime);
    saveDetail(&etr);
}

/*!
  Returns the date time at which the event occurrence ends
 */
QDateTime QOrganizerEventOccurrence::endDateTime() const
{
    QOrganizerEventTimeRange etr = detail<QOrganizerEventTimeRange>();
    return etr.endDateTime();
}

/*!
  Sets the event occurrence's parent to be the event identified by the
  given \a parentLocalId
 */
void QOrganizerEventOccurrence::setParentLocalId(const QOrganizerItemLocalId& parentLocalId)
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    origin.setParentLocalId(parentLocalId);
    saveDetail(&origin);
}

/*!
  Returns the local id of the event which is this occurrence's parent
 */
QOrganizerItemLocalId QOrganizerEventOccurrence::parentLocalId() const
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    return origin.parentLocalId();
}

/*!
  Sets the date at which this occurrence was originally going to occur,
  to the given \a date.
 */
void QOrganizerEventOccurrence::setOriginalDate(const QDate& date)
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    origin.setOriginalDate(date);
    saveDetail(&origin);
}

/*!
  Returns the date at which the occurrence was originally going to occur.
 */
QDate QOrganizerEventOccurrence::originalDate() const
{
    QOrganizerItemInstanceOrigin origin = detail<QOrganizerItemInstanceOrigin>();
    return origin.originalDate();
}

/*!
  Sets the priority of the event occurrence to \a priority
 */
void QOrganizerEventOccurrence::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

/*!
  Returns the priority of the event occurrence
 */
QOrganizerItemPriority::Priority QOrganizerEventOccurrence::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

/*!
  Returns the name of the location at which the event occurrence is held, if known
 */
QString QOrganizerEventOccurrence::locationName() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.locationName();
}

/*!
  Sets the name of the location at which the event occurrence is held to \a locationName
 */
void QOrganizerEventOccurrence::setLocationName(const QString& locationName)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setLocationName(locationName);
    saveDetail(&ld);
}

/*!
  Returns the address of the location at which the event occurrence is held, if known
 */
QString QOrganizerEventOccurrence::locationAddress() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.address();
}

/*!
  Sets the address of the location at which the event occurrence is held to \a locationAddress
 */
void QOrganizerEventOccurrence::setLocationAddress(const QString& locationAddress)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setAddress(locationAddress);
    saveDetail(&ld);
}

/*!
  Returns the geo-coordinates of the location at which the event occurrence is held, if known
 */
QString QOrganizerEventOccurrence::locationGeoCoordinates() const
{
    // XXX TODO: consistency with QOILocation API ?
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.geoLocation();
}

/*!
  \preliminary
  Sets the geo-coordinates of the location at which the event occurrence is held to \a locationCoordinates.
  The coordinates must be in the form "latitude;longitude".
 */
void QOrganizerEventOccurrence::setLocationGeoCoordinates(const QString& locationCoordinates)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setGeoLocation(locationCoordinates);
    saveDetail(&ld);
}

