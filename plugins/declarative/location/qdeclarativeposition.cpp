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

#include "qdeclarativeposition.h"
#include "qdeclarative.h"
#include <qnmeapositioninfosource.h>
#include <QApplication>

// Define this to get usefuld debug messages
#define QDECLARATIVE_POSITION_DEBUG

#ifdef QDECLARATIVE_POSITION_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Position
    \brief The Position element allows you to get information about your current position.
    \inherits QObject

    The Position element allows you to get information about your current position.
    You can receive information about things such as \l latitude, \l longtitude, \l speed,
    and \l altitude. This element is part of the \bold{QtMobility.location 1.0} module.

    Support for location sources are platform dependant. When declaring a Position element, a
    default positioning source shall be created. Supported positioning methods are held in
    \l PositioningMethods. As a development convinience, one may also set data file as a source (NMEA format).
    Location updates are not started automatically upon element declaration,
    see \l startUpdates.

    Here is very simple, self-contained, example QML to illustrate the usage:

    \qml
    import Qt 4.7
    import QtMobility.location 1.0

    Rectangle {
        id: page
        width: 500
        height: 350
        color: "green"

        Text {
            id: title
            text: "Current position is: "
            font {pointSize: 12; bold: true}
        }
        Position {
            id: position
        }
        Column {
            id: data
            anchors {top: title.bottom; left: title.left}
            Text {text: "*Click* here to start updates.";
                  MouseArea {anchors.fill: parent; onClicked: position.startUpdates()}
            }
            Text {text: "*Click* here to stop updates.";
                  MouseArea {anchors.fill: parent; onClicked: position.stopUpdates()}
            }
            Text {text: "latitude: "   + position.latitude}
            Text {text: "longtitude: "   + position.longtitude}
            Text {text: "altitude: "   + position.altitude}
            Text {text: "speed: " + position.speed}
            Text {text: "timestamp: "  + position.timestamp}
            Text {text: "positionLatest: "  + position.positionLatest}
            Text {text: "altitudeLatest: "  + position.altitudeLatest}
            Text {text: "speedLatest: "     + position.speedLatest}
            Text {text: "positioningMethod: "     + printableMethod(position.positioningMethod)}
            Text {text: "nmeaSource: "     + position.nmeaSource}
            Text {text: "updateInterval: "     + position.updateInterval}
        }

        function printableMethod(method) {
            if (method == Position.SatellitePositioningMethod)
                return "Satellite";
            else if (method == Position.NoPositioningMethod)
                return "Not available"
            else if (method == Position.NonSatellitePositioningMethod)
                return "Non-satellite"
            else if (method == Position.AllPositioningMethods)
                return "All/multiple"
            return "source error";
        }
    }
    \endqml

    \sa {QGeoPositionInfoSource} {QGeoPositionInfo} {QNmeaPositionInfoSource} {QGeoCoordinate}

*/

/*!
    \internal
    \class QDeclarativePosition
    \brief The QDeclarativePosition provides positional data.
*/

QDeclarativePosition::QDeclarativePosition()
    : m_positionSource(0), m_latitude(0), m_longtitude(0), m_altitude(0), m_speed(0),
      m_updatesOngoing(false), m_positionLatest(false), m_altitudeLatest(false),
      m_speedLatest(false), m_updateInterval(0)
{
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_positionSource) {
#ifdef QDECLARATIVE_POSITION_DEBUG
        qDebug("QDeclarativePosition QGeoPositionInfoSource::createDefaultSource SUCCESS");
#endif
        connect(m_positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdateReceived(QGeoPositionInfo)));
    }
    else {
#ifdef QDECLARATIVE_POSITION_DEBUG
        qDebug("QDeclarativePosition QGeoPositionInfoSource::createDefaultSource FAILURE");
#endif
    }
}

QDeclarativePosition::~QDeclarativePosition()
{
    delete m_positionSource;
}

void QDeclarativePosition::setNmeaSource(const QUrl& nmeaSource)
{
    // The current position source needs to be deleted in any case,
    // because QNmeaPositionInfoSource can be bound only to a one file.
    if (m_positionSource) {
        delete m_positionSource;
        m_positionSource = 0;
    }
    m_nmeaSource = nmeaSource;
    // Create the NMEA source based on the given data. A fine feature in QML:
    // it automatically sets QUrl type to point to correct path.
    QFile* file = new QFile(nmeaSource.toLocalFile());
    if (file->exists()) {
#ifdef QDECLARATIVE_POSITION_DEBUG
        qDebug() << "QDeclarativePosition NMEA File was found: " << nmeaSource.toLocalFile();
#endif
        m_positionSource = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode);
        (qobject_cast<QNmeaPositionInfoSource*>(m_positionSource))->setDevice(file);
        connect(m_positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdateReceived(QGeoPositionInfo)));
    } else {
#ifdef QDECLARATIVE_POSITION_DEBUG
        qDebug("QDeclarativePosition NMEA File was not found, creating default source.");
#endif
        m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
    }
}

void QDeclarativePosition::setUpdateInterval(int updateInterval)
{
    m_updateInterval = updateInterval;
    if (m_positionSource) {
        m_positionSource->setUpdateInterval(updateInterval);
    }
}

/*!
    \qmlproperty url Position::nmeaSource

    This property holds the source for NMEA data (file). One purpose of this
    property is to be of development convinience. Setting this property
    will override any other position source. If source is changed, all previous
    data is invalidated.

    For example if there is a file called "nmealog.txt" in the same folder as the .qml file:

    \qml
    Position {
        id: position
        nmeaSource: "nmealog.txt"
    }
    \endqml
*/

QUrl QDeclarativePosition::nmeaSource() const
{
    return m_nmeaSource;
}

/*!
    \qmlproperty date Position::timestamp

    This property holds the timestamp of the previously received
    valid update.

    \sa positionLatest altitudeLatest speedLatest

*/

QDateTime QDeclarativePosition::timestamp() const
{
    return m_timestamp;
}

/*!
    \qmlproperty double Position::latitude

    This property holds the last known latitude value of the geographical position
    (decimal degrees). A positive latitude indicates the Northern Hemisphere,
    and a negative latitude indicates the Southern Hemisphere.

    \sa positionLatest

*/

double QDeclarativePosition::latitude() const {
   return m_latitude;
}

/*!
    \qmlproperty double Position::longtitude

    This property holds the last known longtitude value of the geographical position
    (decimal degrees). A positive longitude indicates the Eastern Hemisphere,
    and a negative longitude indicates the Western Hemisphere

    \sa positionLatest

*/

double QDeclarativePosition::longtitude() const {
    return m_longtitude;
}

/*!
    \qmlproperty double Position::altitude

    This property holds the value of last known altitude (metres above sea level).

    \sa altitudeLatest

*/

double QDeclarativePosition::altitude() const {
    return m_altitude;
}

/*!
    \qmlproperty double Position::speed

    This property holds the value of last known speed (groundspeed, metres / second).

    \sa speedLatest

*/

double QDeclarativePosition::speed() const {
    return m_speed;
}

/*!
    \qmlproperty bool Position::positionLatest

    This property is true if \l latitude and \l longtitude data were
    updated at current \l timestamp.

*/

bool QDeclarativePosition::isPositionLatest() const
{
    return m_positionLatest;
}

/*!
    \qmlproperty bool Position::altitudeLatest

    This property is true if \l altitude has been updated at current \l timestamp.

*/

bool QDeclarativePosition::isAltitudeLatest() const
{
    return m_altitudeLatest;
}

/*!
    \qmlproperty bool Position::speedLatest

    This property is true if \l speed has been updated at current \l timestamp.

*/

bool QDeclarativePosition::isSpeedLatest() const
{
    return m_speedLatest;
}

/*!
    \qmlproperty bool Position::updateInterval

    This property holds the desired interval between updates (milliseconds).

    \sa {QGeoPositionInfoSource::updateInterval()}

*/

int QDeclarativePosition::updateInterval() const
{
    return m_updateInterval;
}

/*!
    \qmlproperty enumeration Position::PositioningMethods

    This property holds the supported positioning methods of the
    current source.

    \list
    \o NoPositioningMethod - No positioning methods supported.
    \o SatellitePositioningMethod - Satellite-based positioning methods such as GPS is supported.
    \o NonSatellitePositioningMethod - Non satellite methods are supported.
    \o AllPositioningMethods - Combination of methods are supported.
    \endlist

*/

QDeclarativePosition::PositioningMethod QDeclarativePosition::positioningMethod() const
{
    if (m_positionSource) {
        QGeoPositionInfoSource::PositioningMethods methods = m_positionSource->supportedPositioningMethods();
        if (methods & QGeoPositionInfoSource::SatellitePositioningMethods) {
            return QDeclarativePosition::SatellitePositioningMethod; }
        else if (methods & QGeoPositionInfoSource::NonSatellitePositioningMethods) {
            return QDeclarativePosition::NonSatellitePositioningMethod; }
        else if (methods & QGeoPositionInfoSource::AllPositioningMethods) {
            return QDeclarativePosition::AllPositioningMethods; }
    }
    return QDeclarativePosition::NoPositioningMethod;
}

/*!
    \qmlmethod Position::startUpdates()

    Requests updates from the location source.
    Uses \l updateInterval if set, default otherwise.

    \sa stopUpdates
*/

void QDeclarativePosition::startUpdates() {
    if (m_positionSource) {
        m_positionSource->setUpdateInterval(m_updateInterval);
        m_positionSource->startUpdates();
    }
}

/*!
    \qmlmethod Position::update()

    Requests one update from the location source.

    \sa startUpdates stopUpdates
*/

void QDeclarativePosition::update() {
    if (m_positionSource) {
        // Use default timeout-value
        m_positionSource->requestUpdate();
    }
}

/*!
    \qmlmethod Position::stopUpdates()

    Stops updates from the location source. Does not invalidate any
    previously received data.

    \sa startUpdates

*/

void QDeclarativePosition::stopUpdates() {
    if (m_positionSource) {
        m_positionSource->stopUpdates();
    }
}

/*!
    \qmlsignal Position::positionUpdated()

    This signal is sent when a position update has been received
    from the location source, regardless if any valid data has been received
    or any of the values changed.

    \sa updateInterval

*/

void QDeclarativePosition::positionUpdateReceived(const QGeoPositionInfo& update)
{
    m_positionInfo = update;
    bool timestampChanged = false;
    bool positionChanged = false;
    bool speedChanged = false;
    bool altitudeChanged = false;
    bool positionLatestChanged = false;
    bool speedLatestChanged = false;
    bool altitudeLatestChanged = false;

    // Before sending any signals, set all data.
    updateTimestamp(update, timestampChanged);
    positionLatestChanged = updatePosition(update, positionChanged);
    speedLatestChanged = updateSpeed(update, speedChanged);
    altitudeLatestChanged = updateAltitude(update, altitudeChanged);

    // Send signals, all data is up-to-date already
    if (timestampChanged)
        emit this->timestampChanged(m_timestamp);
    if (positionChanged) {
        emit latitudeChanged(m_latitude);
        emit longtitudeChanged(m_longtitude);
    }
    if (positionLatestChanged)
        emit this->positionLatestChanged(m_positionLatest);
    if (speedChanged)
        emit this->speedChanged(m_speed);
    if (speedLatestChanged)
        emit this->speedLatestChanged(m_speedLatest);
    if (altitudeChanged)
        emit this->altitudeChanged(m_speed);
    if (altitudeLatestChanged)
        emit this->altitudeLatestChanged(m_altitudeLatest);
    // Unconditional emit, whether something changed or not
    emit positionUpdated();
}

// Updates the data and returns whether 'latest' changed
void QDeclarativePosition::updateTimestamp(const QGeoPositionInfo& update, bool& timestampChanged)
{
    timestampChanged = false;
    if (update.isValid()) {
        m_timestamp = update.timestamp();
        timestampChanged = true;
    }
}

// Updates the data and returns whether 'latest' changed
bool QDeclarativePosition::updatePosition(const QGeoPositionInfo& update, bool& positionChanged)
{
    positionChanged = false;
    if (update.isValid()) {
        m_latitude = update.coordinate().latitude();
        m_longtitude = update.coordinate().longitude();
        positionChanged = true;
        if (!m_positionLatest) {
            m_positionLatest = true;
            return true;
        }
    } else if (m_positionLatest) {
        m_positionLatest = false;
        return true;
    }
    return false;
}

// Updates the data and returns whether 'latest' changed
bool QDeclarativePosition::updateAltitude(const QGeoPositionInfo& update, bool& altitudeChanged)
{
    altitudeChanged = false;
    if (update.coordinate().type() == QGeoCoordinate::Coordinate3D) {
        m_altitude = update.coordinate().altitude();
        altitudeChanged = true;
        if (!m_altitudeLatest) {
            m_altitudeLatest = true;
            return true;
        }
    } else if (m_altitudeLatest) {
        m_altitudeLatest = false;
        return true;
    }
    return false;
}


// Updates the data and returns whether 'latest' changed
bool QDeclarativePosition::updateSpeed(const QGeoPositionInfo& update, bool& speedChanged)
{
    speedChanged = false;
    if (update.hasAttribute(QGeoPositionInfo::GroundSpeed)) {
        m_speed = update.attribute(QGeoPositionInfo::GroundSpeed);
        speedChanged = true;
        if (!m_speedLatest) {
            m_speedLatest = true;
            return true;
        }
    } else if (m_speedLatest) {
        m_speedLatest = false;
        return true;

    }
    return false;
}

#include "moc_qdeclarativeposition.cpp"

QTM_END_NAMESPACE


