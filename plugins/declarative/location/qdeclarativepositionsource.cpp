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

#include "qdeclarativepositionsource_p.h"
#include "qdeclarativeposition_p.h"
#include "qdeclarative.h"
#include <qnmeapositioninfosource.h>
#include <QFile>
#include <QTimer>
#include <QApplication>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass PositionSource

    \brief The PositionSource element allows you to get information about your current position.
    \inherits QObject

    The PositionSource element allows you to get information about your current position.
    You can receive information about things such as \l Position::latitude,
    \l Position::longtitude, \l Position::speed, and
    \l Position::altitude. This element is part of the \bold{QtMobility.location 1.0} module.

    Support for location sources are platform dependant. When declaring a PositionSource element, a
    default PositionSource source shall be created. Supported positioning methods are held in
    \l PositioningMethods. As a development convinience, one may also set data file as a source (NMEA format).
    Location updates are not necessarily started automatically upon element declaration, see \l start \l stop \l active
    and \l update.

    Here is very simple self-contained example QML to illustrate the usage:

    \qml
    import Qt 4.7
    import QtMobility.location 1.0

    Rectangle {
        id: page
        width: 350
        height: 350
        color: "olive"

        Text {
            id: title
            text: "Simple position test app"
            font {pointSize: 12; bold: true}
        }
        PositionSource {
            id: positionSource
            active: true
            updateInterval: 1000
        }
        Column {
            id: data
            anchors {top: title.bottom; left: title.left}
            Text {text: "<==== PositionSource ====>"}
            Text {text: "positioningMethod: "  + printableMethod(positionSource.positioningMethod)}
            Text {text: "nmeaSource: "         + positionSource.nmeaSource}
            Text {text: "updateInterval: "     + positionSource.updateInterval}
            Text {text: "active: "     + positionSource.active}
            Text {text: "<==== Position ====>"}
            Text {text: "latitude: "   + positionSource.position.latitude}
            Text {text: "longtitude: "   + positionSource.position.longtitude}
            Text {text: "altitude: "   + positionSource.position.altitude}
            Text {text: "speed: " + positionSource.position.speed}
            Text {text: "timestamp: "  + positionSource.position.timestamp}
            Text {text: "altitudeValid: "  + positionSource.position.altitudeValid}
            Text {text: "longtitudeValid: "  + positionSource.position.longtitudeValid}
            Text {text: "latitudeValid: "  + positionSource.position.latitudeValid}
            Text {text: "speedValid: "     + positionSource.position.speedValid}
        }
        function printableMethod(method) {
            if (method == PositionSource.SatellitePositioningMethod)
                return "Satellite";
            else if (method == PositionSource.NoPositioningMethod)
                return "Not available"
            else if (method == PositionSource.NonSatellitePositioningMethod)
                return "Non-satellite"
            else if (method == PositionSource.AllPositioningMethods)
                return "All/multiple"
            return "source error";
        }
    }
    \endqml

    \sa {QGeoPositionInfoSource}, {QGeoPositionInfo}, {QNmeaPositionInfoSource}, {QGeoCoordinate}

*/

/*!
    \internal
    \class QDeclarativePositionSource
    \brief The QDeclarativePositionSource provides positional data.
*/

QDeclarativePositionSource::QDeclarativePositionSource()
    : m_positionSource(0), m_positioningMethod(QDeclarativePositionSource::NoPositioningMethod),
      m_active(false), m_singleUpdate(false), m_updateInterval(0)
{
    m_positionSource = QGeoPositionInfoSource::createDefaultSource(this);
    if (m_positionSource) {
        connect(m_positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdateReceived(QGeoPositionInfo)));
        m_positioningMethod = positioningMethod();
    }
#ifdef QDECLARATIVE_POSITION_DEBUG
    if (m_positionSource)
        qDebug("QDeclarativePositionSource QGeoPositionInfoSource::createDefaultSource SUCCESS");
    else
        qDebug("QDeclarativePositionSource QGeoPositionInfoSource::createDefaultSource FAILURE");
#endif
}

QDeclarativePositionSource::~QDeclarativePositionSource()
{
    delete m_positionSource;
}

void QDeclarativePositionSource::setNmeaSource(const QUrl& nmeaSource)
{
    if (nmeaSource == m_nmeaSource) {
        return;
    }
    // The current position source needs to be deleted in any case,
    // because QNmeaPositionInfoSource can be bound only to a one file.
    if (m_positionSource) {
        delete m_positionSource;
        m_positionSource = 0;
    }
    m_nmeaSource = nmeaSource;
    // Create the NMEA source based on the given data. QML has automatically set QUrl
    // type to point to correct path. If the file is not found, check if the file actually
    // was an embedded resource file. QUrl loses the ':' so it is added here and checked if
    // it is available.
    QString localFileName = nmeaSource.toLocalFile();
    QFile* file = new QFile(localFileName);
    if (!file->exists()) {
        localFileName.prepend(":");
        file->setFileName(localFileName);
    }
    if (file->exists()) {
#ifdef QDECLARATIVE_POSITION_DEBUG
        qDebug() << "QDeclarativePositionSource NMEA File was found: " << localFileName;
#endif
        m_positionSource = new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode);
        (qobject_cast<QNmeaPositionInfoSource*>(m_positionSource))->setDevice(file);
        connect(m_positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(positionUpdateReceived(QGeoPositionInfo)));
    }
#ifdef QDECLARATIVE_POSITION_DEBUG
     else {
        qDebug() << "QDeclarativePositionSource NMEA File was not found: " << localFileName;
    }
#endif
    if (m_positioningMethod != positioningMethod()) {
        m_positioningMethod = positioningMethod();
        emit positioningMethodChanged(m_positioningMethod);
    }
    emit this->nmeaSourceChanged(m_nmeaSource);
}

void QDeclarativePositionSource::setUpdateInterval(int updateInterval)
{
    if (m_updateInterval == updateInterval)
        return;

    m_updateInterval = updateInterval;
    if (m_positionSource) {
        m_positionSource->setUpdateInterval(updateInterval);
    }
    emit updateIntervalChanged(m_updateInterval);
}

/*!
    \qmlproperty url PositionSource::nmeaSource

    This property holds the source for NMEA data (file). One purpose of this
    property is to be of development convinience.

    Setting this property will override any other position source. Currently only
    files local to the .qml -file are supported. Nmea source is created in simulation mode,
    meaning that the data and time information in the NMEA source data is used to provide
    positional updates at the rate at which the data was originally recorded.

    If nmeaSource has been set for a PositionSource element, there is no way to revert
    back to non-file sources.

    For example if there is a file called "nmealog.txt" in the same folder as the .qml file:

    \qml
    PositionSource {
        id: positionSource
        nmeaSource: "nmealog.txt"
    }
    \endqml
*/

QUrl QDeclarativePositionSource::nmeaSource() const
{
    return m_nmeaSource;
}

/*!
    \qmlproperty bool PositionSource::updateInterval

    This property holds the desired interval between updates (milliseconds).

    \sa {QGeoPositionInfoSource::updateInterval()}

*/

int QDeclarativePositionSource::updateInterval() const
{
    return m_updateInterval;
}

/*!
    \qmlproperty enumeration PositionSource::PositioningMethods

    This property holds the supported positioning methods of the
    current source.

    \list
    \o NoPositioningMethod - No positioning methods supported (no source).
    \o SatellitePositioningMethod - Satellite-based positioning methods such as GPS is supported.
    \o NonSatellitePositioningMethod - Non satellite methods are supported.
    \o AllPositioningMethods - Combination of methods are supported.
    \endlist

*/

QDeclarativePositionSource::PositioningMethod QDeclarativePositionSource::positioningMethod() const
{
    if (m_positionSource) {
        QGeoPositionInfoSource::PositioningMethods methods = m_positionSource->supportedPositioningMethods();
        if (methods & QGeoPositionInfoSource::SatellitePositioningMethods) {
            return QDeclarativePositionSource::SatellitePositioningMethod; }
        else if (methods & QGeoPositionInfoSource::NonSatellitePositioningMethods) {
            return QDeclarativePositionSource::NonSatellitePositioningMethod; }
        else if (methods & QGeoPositionInfoSource::AllPositioningMethods) {
            return QDeclarativePositionSource::AllPositioningMethods; }
    }
    return QDeclarativePositionSource::NoPositioningMethod;
}

/*!
    \qmlmethod PositionSource::start()

    Requests updates from the location source.
    Uses \l updateInterval if set, default interval otherwise.
    If there is no source available, this method has no effect.

    \sa stop, update, active
*/

void QDeclarativePositionSource::start() {
    if (m_positionSource) {
        // Safe to set, setting zero means using default value
        m_positionSource->setUpdateInterval(m_updateInterval);
        m_positionSource->startUpdates();
        if (!m_active) {
            m_active = true;
            emit activeChanged(m_active);
        }
    }
}

/*!
    \qmlmethod PositionSource::update()

    A convinience method to request single update from the location source.
    If there is no source available, this method has no effect.

    \sa start, stop, active
*/

void QDeclarativePositionSource::update() {
    if (m_positionSource) {
        // Use default timeout value
        m_positionSource->requestUpdate();
        if (!m_active) {
            m_active = true;
            m_singleUpdate = true;
            emit activeChanged(m_active);
        }
    }
}

/*!
    \qmlmethod PositionSource::stop()

    Stops updates from the location source.
    If there is no source available or it is not active,
    this method has no effect.

    \sa start, update, active
*/

void QDeclarativePositionSource::stop() {
    if (m_positionSource) {
        m_positionSource->stopUpdates();
        if (m_active) {
            m_active = false;
            emit activeChanged(m_active);
        }
    }
}

/*!
    \qmlsignal PositionSource::positionChanged()

    This signal is sent when a position update has been received
    from the location source. Upon receiving this signal, at least
    \l Position::latitude, \l Position::longtitude, and \l Position::timestamp
    members of the \l position have been update.

    \sa updateInterval

*/

/*!
    \qmlproperty bool PositionSource::active

    This property indicates whether the position source is (/should be)
    active. Setting this property to false equals calling \l stop, and
    setting this property true equals calling \l start.

    \sa start, stop, update

*/

void QDeclarativePositionSource::setActive(bool active)
{
    if (active == m_active)
        return;

    if (active)
        QTimer::singleShot(0, this, SLOT(start())); // delay ensures all properties have been set
    else
        stop();
}

bool QDeclarativePositionSource::isActive() const
{
    return m_active;
}

/*!
    \qmlproperty Position PositionSource::position

    This property holds the last known positional data.

    The Position element has different positional member variables,
    whose validity can be checked with appropriate validity functions
    (e.g. sometimes an update does not have speed or altitude data).

    However, whenever a \l positionChanged signal has been received, at least
    \l Position::latitude, \l Position::longtitude, and \l Position::timestamp can
    be assumed to be valid.

    \sa start, stop, update

*/

QObject* QDeclarativePositionSource::position()
{
    return &m_position;
}

void QDeclarativePositionSource::positionUpdateReceived(const QGeoPositionInfo& update)
{
    if (update.isValid()) {
        m_position.setTimestamp(update.timestamp());
        m_position.setLatitude(update.coordinate().latitude());
        m_position.setLongtitude(update.coordinate().longitude());

        if (update.coordinate().type() == QGeoCoordinate::Coordinate3D) {
            m_position.setAltitude(update.coordinate().altitude());
        }

        if (update.hasAttribute(QGeoPositionInfo::GroundSpeed)) {
            m_position.setSpeed(update.attribute(QGeoPositionInfo::GroundSpeed));
        }

        emit positionChanged();
    }
    if (m_singleUpdate && m_active) {
        m_active = false;
        m_singleUpdate = false;
        emit activeChanged(m_active);
    }
}

#include "moc_qdeclarativepositionsource_p.cpp"

QTM_END_NAMESPACE


