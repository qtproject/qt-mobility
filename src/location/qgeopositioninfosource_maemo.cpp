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
#include "gconfitem.h" // Temporarily here.
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
#ifdef Q_WS_MAEMO_5
    time_interval_ = LOCATION_INTERVAL_DEFAULT;
    availableMethods = SatellitePositioningMethods;
#else
    time_interval_ = 5000;
    availableMethods = AllPositioningMethods;    
#endif
    distance_interval_ = 10;
    registered_ = false;
    validLastUpdate = false;
    validLastSatUpdate = false;
}

#ifdef Q_WS_MAEMO_5
QGeoPositionInfoSourceMaemo::~QGeoPositionInfoSourceMaemo()
{
    if(locationDevice)
        g_object_unref(locationDevice);
    if(locationControl)
        g_object_unref(locationControl);
}
#endif

int QGeoPositionInfoSourceMaemo::init()
{
#ifdef Q_WS_MAEMO_5
    g_type_init();

    locationControl = location_gpsd_control_get_default();
    if(locationControl) {        
        g_object_set(G_OBJECT(locationControl),
                     "preferred-method", LOCATION_METHOD_USER_SELECTED,
                     "preferred-interval", time_interval_,
                     NULL);
    } else {
        return -1;
    }

    locationDevice = 
            (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, 
                                             NULL);

    if(locationDevice) {
        errorHandlerId =
            g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                             G_CALLBACK(&locationError), 
                             static_cast<void*>(this));
        posChangedId =
            g_signal_connect(G_OBJECT(locationDevice), "changed",
                             G_CALLBACK(&locationChanged), 
                             static_cast<void*>(this));
        connectedId =
            g_signal_connect(G_OBJECT(locationDevice), "connected",
                             G_CALLBACK(&deviceConnected), 
                             static_cast<void*>(this));
        disconnectedId = 
            g_signal_connect(G_OBJECT(locationDevice), "disconnected",
                             G_CALLBACK(&deviceDisconnected), 
                             static_cast<void*>(this));
        return 0;
    } else {
        return -1;
    }
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

#ifdef Q_WS_MAEMO_5
    QGeoPositionInfo posInfo;
    QGeoCoordinate coordinate;
    double time;
    double latitude;    
    double longitude;
    double altitude;
    double speed;
    double track;
    double climb;
    
    GConfItem lastKnownPositionTime("/system/nokia/location/lastknown/time");
    GConfItem lastKnownPositionLatitude("/system/nokia/location/lastknown/latitude");
    GConfItem lastKnownPositionLongitude("/system/nokia/location/lastknown/longitude");
    GConfItem lastKnownPositionAltitude("/system/nokia/location/lastknown/altitude");
    GConfItem lastKnownPositionSpeed("/system/nokia/location/lastknown/speed");
    GConfItem lastKnownPositionTrack("/system/nokia/location/lastknown/track");
    GConfItem lastKnownPositionClimb("/system/nokia/location/lastknown/climb");
    
    time = lastKnownPositionTime.value().toDouble();
    latitude = lastKnownPositionLatitude.value().toDouble();
    longitude = lastKnownPositionLongitude.value().toDouble();
    altitude = lastKnownPositionAltitude.value().toDouble();
    speed = lastKnownPositionSpeed.value().toDouble();
    track = lastKnownPositionTrack.value().toDouble();
    climb = lastKnownPositionClimb.value().toDouble();
    
    if (time) {
        posInfo.setDateTime(QDateTime::fromTime_t(time));
    }
    
    if (longitude && latitude) {
        coordinate.setLongitude(longitude);
        coordinate.setLatitude(latitude);
        if (altitude) {
            coordinate.setAltitude(altitude);
        }
        posInfo.setCoordinate(coordinate);
    }
        
    if (speed) {
        posInfo.setAttribute(QGeoPositionInfo::GroundSpeed, speed);
    }
    
    if (track) {
        posInfo.setAttribute(QGeoPositionInfo::Direction, track);
    }
    
    if (climb) {
        posInfo.setAttribute(QGeoPositionInfo::VerticalSpeed, climb);        
    }
    
    return posInfo;
#else  
    return QGeoPositionInfo();
#endif
}


QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceMaemo::supportedPositioningMethods() const
{
    return availableMethods;
}


void QGeoPositionInfoSourceMaemo::setUpdateInterval(int msec)
{
    msec = (msec < MinimumUpdateInterval) ? MinimumUpdateInterval : msec;

#ifdef Q_WS_MAEMO_5
    time_interval_ = mapUpdateInterval(msec);
    QGeoPositionInfoSource::setUpdateInterval(time_interval_*100);    
    if(locationControl) {
        g_object_set(G_OBJECT(locationControl),
                     "preferred-method", LOCATION_METHOD_USER_SELECTED,
                     "preferred-interval", time_interval_,
                     NULL) ;      
    }
#else
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
    if(locationControl) {
        if(!errorHandlerId) {
            errorHandlerId =
                g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                                 G_CALLBACK(&locationError), 
                                 static_cast<void*>(this));            
        }

        if(!posChangedId) {
            posChangedId =
                g_signal_connect(G_OBJECT(locationDevice), "changed",
                                 G_CALLBACK(&locationChanged), 
                                 static_cast<void*>(this));            
        }
        
        if(!connectedId) {
            connectedId = 
                g_signal_connect(G_OBJECT(locationDevice), "connected",
                                 G_CALLBACK(&deviceConnected), 
                                 static_cast<void*>(this));            
        }
        
        if(!disconnectedId) {
            disconnectedId = 
                g_signal_connect(G_OBJECT(locationDevice), "disconnected",
                                 G_CALLBACK(&deviceDisconnected), 
                                 static_cast<void*>(this));                        
        }
        
        location_gpsd_control_start(locationControl);
    } else {
        
    }
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
    if(locationControl) {
        if(errorHandlerId)
            g_signal_handler_disconnect(G_OBJECT(locationControl), 
                                        errorHandlerId);
        if(posChangedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice), 
                                        posChangedId);
        if(connectedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice), 
                                        connectedId);
        if(disconnectedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice), 
                                        disconnectedId);
        errorHandlerId = 0;
        posChangedId = 0;
        connectedId = 0;
        disconnectedId = 0;        
        location_gpsd_control_stop(locationControl);
    }
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

        if (device->fix->fields & LOCATION_GPS_DEVICE_CLIMB_SET) {
            posInfo.setAttribute(QGeoPositionInfo::VerticalSpeed,
                                 device->fix->climb);
        }
  
        if (device->fix->fields & LOCATION_GPS_DEVICE_TRACK_SET) {
            posInfo.setAttribute(QGeoPositionInfo::Direction,
                                 device->fix->track);            
        }

    }
   
    if(device->status == LOCATION_GPS_DEVICE_STATUS_FIX) {
        posInfo.setCoordinate(coordinate);        
        object->setLocation(posInfo);
    }
}

void QGeoPositionInfoSourceMaemo::setLocation(const QGeoPositionInfo &update)
{
    lastSatUpdate = update;
    validLastSatUpdate = true;
    emit positionUpdated(update);
}

int QGeoPositionInfoSourceMaemo::mapUpdateInterval(int msec) {
    if (msec < 1500)
        return LOCATION_INTERVAL_1S;
    else if ((msec >= 1500) && (msec < 3500))
        return LOCATION_INTERVAL_2S;
    else if ((msec >= 3500) && (msec < 7500))
        return LOCATION_INTERVAL_5S;
    else if ((msec >= 7500) && (msec < 15000))
        return LOCATION_INTERVAL_10S;
    else if ((msec >= 15000) && (msec < 25000))
        return LOCATION_INTERVAL_20S;
    else if ((msec >= 25000) && (msec < 45000))
        return LOCATION_INTERVAL_30S;
    else if ((msec >= 45000) && (msec < 90000))
        return LOCATION_INTERVAL_60S;
    else if (msec >= 90000)
        return LOCATION_INTERVAL_120S;
    else
        return LOCATION_INTERVAL_DEFAULT;
}
    
#endif // Q_WS_MAEMO_5


#include "moc_qgeopositioninfosource_maemo_p.cpp"
QTM_END_NAMESPACE

