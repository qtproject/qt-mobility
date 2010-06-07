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

#include "qorganizerevent.h"
#include "qtorganizer.h"

QTM_USE_NAMESPACE

void QOrganizerEvent::setStartDateTime(const QDateTime& startDateTime)
{
    QOrganizerItemEventTimeRange etr = detail<QOrganizerItemEventTimeRange>();
    etr.setStartDateTime(startDateTime);
    saveDetail(&etr);
}

QDateTime QOrganizerEvent::startDateTime() const
{
    QOrganizerItemEventTimeRange etr = detail<QOrganizerItemEventTimeRange>();
    return etr.startDateTime();
}

void QOrganizerEvent::setEndDateTime(const QDateTime& endDateTime)
{
    QOrganizerItemEventTimeRange etr = detail<QOrganizerItemEventTimeRange>();
    etr.setEndDateTime(endDateTime);
    saveDetail(&etr);
}

QDateTime QOrganizerEvent::endDateTime() const
{
    QOrganizerItemEventTimeRange etr = detail<QOrganizerItemEventTimeRange>();
    return etr.endDateTime();
}

void QOrganizerEvent::setRecurrenceDates(const QList<QDate>& rdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceDates(rdates);
    saveDetail(&rec);
}

QList<QDate> QOrganizerEvent::recurrenceDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceDates();
}

void QOrganizerEvent::setRecurrenceRules(const QList<QOrganizerItemRecurrenceRule>& rrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setRecurrenceRules(rrules);
    saveDetail(&rec);
}

QList<QOrganizerItemRecurrenceRule> QOrganizerEvent::recurrenceRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.recurrenceRules();
}

void QOrganizerEvent::setExceptionDates(const QList<QDate>& exdates)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionDates(exdates);
    saveDetail(&rec);
}

QList<QDate> QOrganizerEvent::exceptionDates() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionDates();
}

void QOrganizerEvent::setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& exrules)
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    rec.setExceptionRules(exrules);
    saveDetail(&rec);
}

QList<QOrganizerItemRecurrenceRule> QOrganizerEvent::exceptionRules() const
{
    QOrganizerItemRecurrence rec = detail<QOrganizerItemRecurrence>();
    return rec.exceptionRules();
}

void QOrganizerEvent::setPriority(QOrganizerItemPriority::Priority priority)
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    pd.setPriority(priority);
    saveDetail(&pd);
}

QOrganizerItemPriority::Priority QOrganizerEvent::priority() const
{
    QOrganizerItemPriority pd = detail<QOrganizerItemPriority>();
    return pd.priority();
}

QString QOrganizerEvent::locationName() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.locationName();
}

void QOrganizerEvent::setLocationName(const QString& locationName)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setLocationName(locationName);
    saveDetail(&ld);
}

QString QOrganizerEvent::locationAddress() const
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.address();
}

void QOrganizerEvent::setLocationAddress(const QString& locationAddress)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setAddress(locationAddress);
    saveDetail(&ld);
}

QString QOrganizerEvent::locationGeoCoordinates() const
{
    // XXX TODO: consistency with QOILocation API ?
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    return ld.geoLocation();
}

void QOrganizerEvent::setLocationGeoCoordinates(const QString& locationCoordinates)
{
    QOrganizerItemLocation ld = detail<QOrganizerItemLocation>();
    ld.setGeoLocation(locationCoordinates);
    saveDetail(&ld);
}
