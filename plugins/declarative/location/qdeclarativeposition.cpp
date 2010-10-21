/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativeposition_p.h"
#include "qdeclarative.h"
#include <qnmeapositioninfosource.h>
#include <QFile>
#include <QApplication>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Position

    \brief The Position element holds various positional data, such as coordinate
    (longitude, latitude, altitude) and speed.
    \inherits QObject
    \ingroup qml-location

    The Position element holds various positional data, such as \l coordinate,
    (longitude, latitude, and altitude) and \l speed, as well as \l timestamp.

    Not all of these are always available (e.g. latitude and longitude may be valid,
    but speed update has not been received or set manually), and hence there are
    corresponding validity attributes which can be used when writing applications.

    At the time being the Position element is a read-only element (meaning it does
    not have exposed write accessors), primarily used by \l PositionSource.

    The Position element is part of the \bold{QtMobility.location 1.1} module.

    \sa PositionSource, Coordinate

*/

QDeclarativePosition::QDeclarativePosition(QObject* parent)
        : QObject(parent), m_latitudeValid(false), m_longitudeValid(false),
        m_altitudeValid(false), m_speedValid(false)
{
}

QDeclarativePosition::~QDeclarativePosition()
{
}

/*!
    \qmlproperty Coordinate Position::coordinate

    This property holds the latitude, longitude, and altitude
    value of the Position.

    Note: due to its inherent changing nature, the coordinateChanged()
    - signal is emitted also when the values of the coordinate changes,
    which enables easier data binding to Coordinate element instead of
    directly connecting to e.g. its latitude and longitude.

    \sa longitudeValid latitudeValid altitudeValid

*/

void QDeclarativePosition::setCoordinate(QGeoCoordinate coordinate)
{
    m_coordinate.setCoordinate(coordinate);

    if (coordinate.type() == QGeoCoordinate::Coordinate3D && !m_altitudeValid) {
        m_altitudeValid = true;
        emit altitudeValidChanged();
    } else if (m_altitudeValid) {
        m_altitudeValid = false;
        emit altitudeValidChanged();
    }
    if (coordinate.isValid()) {
        if (!m_longitudeValid) {
            m_longitudeValid = true;
            emit longitudeValidChanged();
        }
        if (!m_latitudeValid) {
            m_latitudeValid = true;
            emit latitudeValidChanged();
        }
    } else {
        if (m_longitudeValid) {
            m_longitudeValid = false;
            emit longitudeValidChanged();
        }
        if (m_latitudeValid) {
            m_latitudeValid = false;
            emit latitudeValidChanged();
        }
    }
    emit coordinateChanged();
}

QDeclarativeCoordinate* QDeclarativePosition::coordinate()
{
    return &m_coordinate;
}

/*!
    \qmlproperty bool Position::latitudeValid

    This property is true if coordinate's latitude has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa coordinate

*/

bool QDeclarativePosition::isLatitudeValid() const
{
    return m_latitudeValid;
}


/*!
    \qmlproperty bool Position::longitudeValid

    This property is true if coordinate's longitude has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa coordinate

*/

bool QDeclarativePosition::isLongitudeValid() const
{
    return m_longitudeValid;
}


/*!
    \qmlproperty bool Position::speedValid

    This property is true if \l speed has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa speed

*/

bool QDeclarativePosition::isSpeedValid() const
{
    return m_speedValid;
}

/*!
    \qmlproperty bool Position::altitudeValid

    This property is true if coordinate's altitude has been set
    (to indicate whether that data has been received or not, as every update
    does not necessarily contain all data).

    \sa coordinate

*/

bool QDeclarativePosition::isAltitudeValid() const
{
    return m_altitudeValid;
}

/*!
    \qmlproperty double Position::speed

    This property holds the value of speed (groundspeed, metres / second).
    Currently a read-only property.

    \sa speedValid, coordinate
*/

void QDeclarativePosition::setSpeed(double speed)
{
    if (speed == m_speed)
        return;
    m_speed = speed;
    if (!m_speedValid) {
        m_speedValid = true;
        emit speedValidChanged();
    }
    emit speedChanged();
}

double QDeclarativePosition::speed() const
{
    return m_speed;
}

/*!
    \qmlproperty date Position::timestamp

    This property holds the timestamp when this position
    was received. If the property has not been set, it is invalid.

*/

void QDeclarativePosition::setTimestamp(const QDateTime& timestamp)
{
    if (timestamp == m_timestamp)
        return;
    m_timestamp = timestamp;
    emit timestampChanged();
}

QDateTime QDeclarativePosition::timestamp() const
{
    return m_timestamp;
}

#include "moc_qdeclarativeposition_p.cpp"

QTM_END_NAMESPACE
