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

#include "qgeopositioninfosource_maemo_p.h"
#include <iostream>

#ifdef Q_WS_MAEMO_5
#include <QDateTime>
#endif
#if 0 
void dumpQByteArray(const QByteArray &msg)
{
    std::cout << "QByteArray dump:\n";
    std::cout << msg.size() << "  \n";
    int i =  msg.size();
    for (int k = 0; k < i ;k++)
        printf("msg[%d]=%2.2x\n", k, (unsigned char)msg[k]);
}
#endif

using namespace std;

QTM_BEGIN_NAMESPACE

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent): QGeoPositionInfoSource(parent)
{
    // default values
    time_interval_ = 5000;
    distance_interval_ = 10;
    registered_ = false;
    validLastUpdate = false;
    validLastSatUpdate = false;
    availableMethods = AllPositioningMethods;
}

int QGeoPositionInfoSourceMaemo::init()
{
#ifdef Q_WS_MAEMO_5
    g_type_init();

    locationControl = location_gpsd_control_get_default();
    g_object_set(G_OBJECT(locationControl),
                 "preferred-method", LOCATION_METHOD_USER_SELECTED,
                 "preferred-interval", LOCATION_INTERVAL_DEFAULT,
                 NULL);

    locationDevice = 
            (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, 
                                             NULL);

    g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                     G_CALLBACK(&locationError), static_cast<void*>(this));
    g_signal_connect(G_OBJECT(locationDevice), "changed",
                     G_CALLBACK(&locationChanged), static_cast<void*>(this));
    g_signal_connect(G_OBJECT(locationDevice), "connected",
                     G_CALLBACK(&deviceConnected), static_cast<void*>(this));
    g_signal_connect(G_OBJECT(locationDevice), "disconnected",
                     G_CALLBACK(&deviceDisconnected), static_cast<void*>(this));

    location_gpsd_control_start(locationControl);
    return 0;
#else
    int status;

    dbusComm = new DBusComm();
    status = dbusComm->init();

    QObject::connect(dbusComm, SIGNAL(receivedMessage(const QByteArray &)),
                     this, SLOT(dbusMessages(const QByteArray &)));

    QObject::connect(dbusComm, SIGNAL(receivedPositionUpdate(const QGeoPositionInfo &)),
                     this, SLOT(newPositionUpdate(const QGeoPositionInfo &)));
    return status;
#endif // Q_WS_MAEMO_5
}

void QGeoPositionInfoSourceMaemo::dbusMessages(const QByteArray &msg)
{
    Q_UNUSED(msg)
    // stub

    return;
}


void QGeoPositionInfoSourceMaemo::newPositionUpdate(const QGeoPositionInfo &update)
{
    lastSatUpdate = update;
    validLastSatUpdate = true;
    emit positionUpdated(update);
}


QGeoPositionInfo QGeoPositionInfoSourceMaemo::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    if (validLastSatUpdate)
        return lastSatUpdate;

    if (!fromSatellitePositioningMethodsOnly)
        if (validLastUpdate)
            return lastUpdate;

    return QGeoPositionInfo();
}


QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceMaemo::supportedPositioningMethods() const
{
    return availableMethods;
}


void QGeoPositionInfoSourceMaemo::setUpdateInterval(int msec)
{
#ifdef Q_WS_MAEMO_5
    if(locationControl)
        g_object_set(G_OBJECT(locationControl),
                     "preferred-method", LOCATION_METHOD_USER_SELECTED,
                     "preferred-interval", msec,
                     NULL);
#else
    msec = (msec<MinimumUpdateInterval)?MinimumUpdateInterval:msec;

    QGeoPositionInfoSource::setUpdateInterval(msec);

    if (registered_ == false)
        registered_ = dbusComm->sendDBusRegister();
    dbusComm->sessionConfigRequest(dbusComm->CmdSetInterval, 0, msec);
#endif
}

void QGeoPositionInfoSourceMaemo::setPreferredPositioningMethods(PositioningMethods sources)
{
#ifdef Q_WS_MAEMO_5    
    return;
#else
    QGeoPositionInfoSource::setPreferredPositioningMethods(sources);
    if (registered_ == false)
        registered_ = dbusComm->sendDBusRegister();
    dbusComm->sessionConfigRequest(dbusComm->CmdSetMethods, sources, 0);
#endif
}


int QGeoPositionInfoSourceMaemo::minimumUpdateInterval() const
{
    return MinimumUpdateInterval;
}

// public slots:
void QGeoPositionInfoSourceMaemo::startUpdates()
{
#ifdef Q_WS_MAEMO_5
    if(locationControl)
        location_gpsd_control_start(locationControl);
#else
    if (registered_ == false)
        registered_ = dbusComm->sendDBusRegister();

    int cmd = dbusComm->CmdStart;
    dbusComm->sessionConfigRequest(cmd, 222, time_interval_);
#endif
}


void QGeoPositionInfoSourceMaemo::stopUpdates()
{
#ifdef Q_WS_MAEMO_5
    if(locationControl)
        location_gpsd_control_stop(locationControl);
#else
    if (registered_ == false)
        return; // nothing to stop
    dbusComm->sessionConfigRequest(dbusComm->CmdStop, 0, 0);
#endif
}

// Stub

void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    if (timeout) {}
}

#ifdef Q_WS_MAEMO_5
void QGeoPositionInfoSourceMaemo::locationError(LocationGPSDevice *device,
                                                gint errorCode, gpointer data)
{
    QString locationError;

    switch (errorCode) {
        case LOCATION_ERROR_USER_REJECTED_DIALOG:
            locationError = "User didn't enable requested methods";
            break;
        case LOCATION_ERROR_USER_REJECTED_SETTINGS:
            locationError = "User changed settings, which disabled location.";
            break;
        case LOCATION_ERROR_BT_GPS_NOT_AVAILABLE:
            locationError = "Problems with BT GPS";
            break;
        case LOCATION_ERROR_METHOD_NOT_ALLOWED_IN_OFFLINE_MODE:
            locationError = "Requested method is not allowed in offline mode";
            break;
        case LOCATION_ERROR_SYSTEM:
            locationError = "System error.";
            break;
        default:
            locationError = "Unknown error.";
    }

    qDebug() << "Location error:" << locationError;
}

void QGeoPositionInfoSourceMaemo::deviceDisconnected(LocationGPSDevice *device,
                                                     gpointer data)
{
    Q_UNUSED(device)

    if (!data)
        return;

    QGeoPositionInfoSourceMaemo *object = (QGeoPositionInfoSourceMaemo *)data;
    object->setOnline(false);
}

void QGeoPositionInfoSourceMaemo::deviceConnected(LocationGPSDevice *device,
                                                  gpointer data)
{
    Q_UNUSED(device)

    if (!data)
        return;

    QGeoPositionInfoSourceMaemo *object = (QGeoPositionInfoSourceMaemo *)data;
    object->setOnline(true);
}

void QGeoPositionInfoSourceMaemo::setOnline(bool status)
{
    emit online(status);
}

bool QGeoPositionInfoSourceMaemo::isOnline() const
{
    if (!locationDevice) {
        return false;
    }

    return locationDevice->online == 1;
}

void QGeoPositionInfoSourceMaemo::locationChanged(LocationGPSDevice *device,
                                                 gpointer data)
{
    QGeoPositionInfo posInfo;
    QGeoCoordinate coordinate;
    QGeoPositionInfoSourceMaemo *object;

    if (!data || !device)
        return;

    object = (QGeoPositionInfoSourceMaemo *)data;

    if (device && device->fix) {
        if (device->fix->fields & LOCATION_GPS_DEVICE_TIME_SET) {
            posInfo.setDateTime(QDateTime::fromTime_t(device->fix->time));
        }

        if (device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET) {
            coordinate.setLatitude(device->fix->latitude);
            coordinate.setLongitude(device->fix->longitude);
            posInfo.setAttribute(QGeoPositionInfo::HorizontalAccuracy,
                                 device->fix->eph);
            posInfo.setAttribute(QGeoPositionInfo::VerticalAccuracy,
                                 device->fix->epv);

        }

        if (device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET) {
            coordinate.setAltitude(device->fix->altitude);
        }

        if (device->fix->fields & LOCATION_GPS_DEVICE_SPEED_SET) {
            posInfo.setAttribute(QGeoPositionInfo::GroundSpeed,
                                 device->fix->speed);
        }

        if (device->fix->fields & LOCATION_GPS_DEVICE_TRACK_SET) {
            posInfo.setAttribute(QGeoPositionInfo::Direction,
                                 device->fix->track);            
        }

    }
    posInfo.setCoordinate(coordinate);
    object->setLocation(posInfo);
}

void QGeoPositionInfoSourceMaemo::setLocation(const QGeoPositionInfo &update)
{
    lastSatUpdate = update;
    validLastSatUpdate = true;

    emit positionUpdated(update);
}
#endif // Q_WS_MAEMO_5


#include "moc_qgeopositioninfosource_maemo_p.cpp"
QTM_END_NAMESPACE

