/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativeposition_p.h"
#include "qdeclarative.h"
#include <qnmeapositioninfosource.h>
#include <QFile>
#include <QApplication>

QTM_BEGIN_NAMESPACE

QDeclarativePosition::QDeclarativePosition()
    : m_latitude(0), m_latitudeValid(false), m_longtitude(0), m_longtitudeValid(false),
    m_altitude(0), m_altitudeValid(false), m_speed(0), m_speedValid(false)
{
}

QDeclarativePosition::~QDeclarativePosition()
{
}


/*!
    \qmlproperty bool PositionSource::latitudeValid

    This property is true if \l latitude has been set.

    \sa latitude

*/

bool QDeclarativePosition::isLatitudeValid() const
{
    return m_latitudeValid;
}


/*!
    \qmlproperty bool PositionSource::longtitudeValid

    This property is true if \l longtitude has been set.

    \sa longtitude

*/

bool QDeclarativePosition::isLongtitudeValid() const
{
    return m_longtitudeValid;
}


/*!
    \qmlproperty bool PositionSource::speedValid

    This property is true if \l speed has been set.

    \sa speed

*/

bool QDeclarativePosition::isSpeedValid() const
{
    return m_speedValid;
}

/*!
    \qmlproperty bool Position::altitudeValid

    This property is true if \l altitude has been set.

    \sa altitude

*/

bool QDeclarativePosition::isAltitudeValid() const
{
    return m_altitudeValid;
}

/*!
    \qmlproperty double Position::speed

    This property holds the value of speed (groundspeed, metres / second).

    \sa speedValid altitide longtitude latitude

*/

void QDeclarativePosition::setSpeed(double speed)
{
    m_speed = speed;
    if (!m_speedValid) {
        m_speedValid = true;
        emit speedValidChanged(m_speedValid);
    }
    emit speedChanged(m_speed);
}

double QDeclarativePosition::speed() const {
    return m_speed;
}

/*!
    \qmlproperty double Position::altitude

    This property holds the value of altitude (metres above sea level).

    \sa speedValid longtitude latitude speed

*/

void QDeclarativePosition::setAltitude(double altitude)
{
    m_altitude = altitude;
    if (!m_altitudeValid) {
        m_altitudeValid = true;
        emit altitudeValidChanged(m_altitudeValid);
    }
    emit altitudeChanged(m_altitude);
}

double QDeclarativePosition::altitude() const {
    return m_altitude;
}

/*!
    \qmlproperty double Position::longtitude

    This property holds the longtitude value of the geographical position
    (decimal degrees). A positive longitude indicates the Eastern Hemisphere,
    and a negative longitude indicates the Western Hemisphere

    \sa latitude speed altitude

*/

void QDeclarativePosition::setLongtitude(double longtitude)
{
    m_longtitude = longtitude;
    if (!m_longtitudeValid) {
        m_longtitudeValid = true;
        emit longtitudeValidChanged(m_longtitudeValid);
    }
    emit longtitudeChanged(m_longtitude);
}

double QDeclarativePosition::longtitude() const {
    return m_longtitude;
}

/*!
    \qmlproperty double Position::latitude

    This property holds latitude value of the geographical position
    (decimal degrees). A positive latitude indicates the Northern Hemisphere,
    and a negative latitude indicates the Southern Hemisphere.

    \sa longtitude speed altitude

*/

void QDeclarativePosition::setLatitude(double latitude)
{
    m_latitude = latitude;
    if (!m_latitudeValid) {
        m_latitudeValid = true;
        emit latitudeValidChanged(m_latitudeValid);
    }
    emit latitudeChanged(m_latitude);
}

double QDeclarativePosition::latitude() const {
   return m_latitude;
}

/*!
    \qmlproperty date Position::timestamp

    This property holds the timestamp when this position
    was received.

*/

void QDeclarativePosition::setTimestamp(const QDateTime& timestamp)
{
    m_timestamp = timestamp;
    emit timestampChanged(m_timestamp);
}

QDateTime QDeclarativePosition::timestamp() const
{
    return m_timestamp;
}

#include "moc_qdeclarativeposition_p.cpp"

QTM_END_NAMESPACE
