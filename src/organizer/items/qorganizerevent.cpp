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

QOrganizerEvent::QOrganizerEvent()
{
    // XXX TODO: set the QOrganizerItemType detail to == Event.
}

QOrganizerEvent::QOrganizerEvent(const QOrganizerEvent& other)
    : QOrganizerItem(other)
{
}
QOrganizerEvent& QOrganizerEvent::operator=(const QOrganizerEvent& other)
{
    QOrganizerItem::operator =(other);
    return *this;
}

void QOrganizerEvent::setStartDateTime(const QDateTime& startDateTime)
{
    Q_UNUSED(startDateTime);
}

QDateTime QOrganizerEvent::startDateTime() const
{
    return QDateTime();
}

void QOrganizerEvent::setEndDateTime(const QDateTime& endDateTime)
{
    Q_UNUSED(endDateTime);
}

QDateTime QOrganizerEvent::endDateTime() const
{
    return QDateTime();
}

void QOrganizerEvent::setRecurrence(const QOrganizerItemRecurrence& recurrence) const
{
    Q_UNUSED(recurrence);
}
QOrganizerItemRecurrence QOrganizerEvent::recurrence() const
{
    return QOrganizerItemRecurrence();
}

void QOrganizerEvent::setPriority(QOrganizerItemPriority::Priority priority)
{
    Q_UNUSED(priority);
}

QOrganizerItemPriority::Priority QOrganizerEvent::priority() const
{
    return QOrganizerItemPriority::UnknownPriority;
}

QString QOrganizerEvent::locationName() const
{
    return QString();
}
void QOrganizerEvent::setLocationName(const QString& locationName)
{
    Q_UNUSED(locationName);
}

QString QOrganizerEvent::locationAddress() const
{
    return QString();
}
void QOrganizerEvent::setLocationAddress(const QString& locationAddress)
{
    Q_UNUSED(locationAddress);
}

QString QOrganizerEvent::locationGeoCoordinates() const
{
    return QString();
}
void QOrganizerEvent::setLocationGeoCoordinates(const QString& locationCoordinates)
{
    Q_UNUSED(locationCoordinates);
}
