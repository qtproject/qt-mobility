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

#include "liblocationwrapper.h"

#define LL_INTERVAL_TO_MSECS 100
#define MIN_UPDATE_INTERVAL  (LOCATION_INTERVAL_1S * LL_INTERVAL_TO_MSECS)
#define TO_SECS              1000

using namespace std;

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(LiblocationWrapper, LocationEngine)
        
LiblocationWrapper *LiblocationWrapper::instance()
{
    return LocationEngine();
}

LiblocationWrapper::~LiblocationWrapper() 
{
    if (locationDevice)
        g_object_unref(locationDevice);
    if (locationControl)
        g_object_unref(locationControl);
}

bool LiblocationWrapper::inited()
{
    if (!(locationState & LiblocationWrapper::Inited)) {
        llCurrentInterval = LOCATION_INTERVAL_DEFAULT;
        g_type_init();

        locationControl = location_gpsd_control_get_default();

        if (locationControl) {
            g_object_set(G_OBJECT(locationControl),
                         "preferred-method", LOCATION_METHOD_USER_SELECTED,
                         "preferred-interval", llCurrentInterval,
                         NULL);
            locationDevice = 
                    (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, 
                                                     NULL);
        
            if (locationDevice) {
                errorHandlerId =
                    g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                                     G_CALLBACK(&locationError), 
                                     static_cast<void*>(this));
                posChangedId =
                    g_signal_connect(G_OBJECT(locationDevice), "changed",
                                     G_CALLBACK(&locationChanged), 
                                     static_cast<void*>(this));
                locationState = LiblocationWrapper::Inited;
            }
        }
    }
    return true;
}

void LiblocationWrapper::locationError(LocationGPSDevice *device,
                                       gint errorCode, gpointer data)
{
    Q_UNUSED(device);
    Q_UNUSED(data);
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

int LiblocationWrapper::setUpdateInterval(int wantedInterval)
{
    if (!LiblocationWrapper::Inited)
        return 0;
    
    wantedInterval = (wantedInterval < MIN_UPDATE_INTERVAL) ? 
                        MIN_UPDATE_INTERVAL : wantedInterval;

    if (locationState & LiblocationWrapper::RequestActive) {
        if(wantedInterval <= (llCurrentInterval * LL_INTERVAL_TO_MSECS)) {
            locationState &= ~(LiblocationWrapper::RequestActive |
                               LiblocationWrapper::RequestSingleShot);   
            origUpdateInterval = 0;
        } else {
            // Set user requested interval to wanted value after request is
            // complete (if greater than request timeout).
            origUpdateInterval = wantedInterval;
            return (mapUpdateInterval(wantedInterval) * LL_INTERVAL_TO_MSECS);
        }
    } 
    
    llCurrentInterval = mapUpdateInterval(wantedInterval);
    
    g_object_set(G_OBJECT(locationControl),
                 "preferred-method", LOCATION_METHOD_USER_SELECTED,
                 "preferred-interval", llCurrentInterval,
                 NULL) ;      
    
    return (llCurrentInterval*LL_INTERVAL_TO_MSECS);
}

bool LiblocationWrapper::requestUpdate(int timeout)
{
    int requestTimeout = 0;

    if (locationState & LiblocationWrapper::Inited) {
        if (locationState & LiblocationWrapper::RequestActive)
            return true;
    
        if (!timeout) {
            requestTimeout = (LOCATION_INTERVAL_DEFAULT * LL_INTERVAL_TO_MSECS);
        } else if (timeout < MIN_UPDATE_INTERVAL) {
            locationState &= ~(LiblocationWrapper::RequestActive |
                               LiblocationWrapper::RequestSingleShot);
            return false;
        } else {
            requestTimeout = timeout;
        }
    
        if (!(locationState & LiblocationWrapper::Started)) {
            origUpdateInterval = (llCurrentInterval * LL_INTERVAL_TO_MSECS);
            setUpdateInterval(requestTimeout);
            start();
            locationState |= (LiblocationWrapper::RequestActive |
                              LiblocationWrapper::RequestSingleShot);

        } else if (locationState & LiblocationWrapper::Started) {
            QGeoPositionInfo lastKnown = lastKnownPosition(true);

            if (lastKnown.dateTime().isValid()) {
                QDateTime dt = QDateTime::currentDateTime();
                int timeFromFix = dt.secsTo(lastKnown.dateTime());
                
                if ((requestTimeout/TO_SECS) <
                    ((llCurrentInterval/10) - timeFromFix)) {
                    locationState |= LiblocationWrapper::RequestActive;
                    origUpdateInterval = 
                            (llCurrentInterval * LL_INTERVAL_TO_MSECS);                    
                    setUpdateInterval(requestTimeout);
                }
            }
        }
    }
    return true;
}

void LiblocationWrapper::locationChanged(LocationGPSDevice *device,
                                                 gpointer data)
{
    QGeoPositionInfo posInfo;
    QGeoCoordinate coordinate;
    QGeoSatelliteInfo satInfo;
    
    LiblocationWrapper *object;

    if (!data || !device)
        return;

    object = (LiblocationWrapper *)data;

    if (device) {
        if (device->fix) {
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
        
        if (device->satellites_in_view) {
            QList<QGeoSatelliteInfo> satsInView;
            QList<QGeoSatelliteInfo> satsInUse;
            int i;
            for (i=0;i<device->satellites->len;i++) {
                LocationGPSDeviceSatellite *satData =
                    (LocationGPSDeviceSatellite *)g_ptr_array_index(device->satellites,
                                                                    i);
                satInfo.setSignalStrength(satData->signal_strength);
                satInfo.setPrnNumber(satData->prn);
                satInfo.setAttribute(QGeoSatelliteInfo::Elevation, 
                                     satData->elevation);
                satInfo.setAttribute(QGeoSatelliteInfo::Azimuth, 
                                     satData->azimuth);
    
                satsInView.append(satInfo);
                if (satData->in_use)
                    satsInUse.append(satInfo);
            }
            
            if (!satsInView.isEmpty())
                object->satellitesInView(satsInView);
            
            if (!satsInUse.isEmpty())
                object->satellitesInUse(satsInUse);
        }        
    }
       
    // Only position updates with time (3D) are provided.
    if (device->fix->mode == LOCATION_GPS_DEVICE_MODE_3D) {
        posInfo.setCoordinate(coordinate);        
        object->setLocation(posInfo);
    }
}

void LiblocationWrapper::setLocation(const QGeoPositionInfo &update)
{    
    lastSatUpdate = update;
    validLastSatUpdate = true;
    emit positionUpdated(update);
    
    if (locationState & LiblocationWrapper::RequestActive) {
        locationState &= ~LiblocationWrapper::RequestActive;

        if(locationState & LiblocationWrapper::RequestSingleShot) {
            stop();
            locationState &= ~LiblocationWrapper::RequestSingleShot;
        }
        
        if (origUpdateInterval) {
            setUpdateInterval(origUpdateInterval);
        }        
    }
}

int LiblocationWrapper::mapUpdateInterval(int msec) {     
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

QGeoPositionInfo LiblocationWrapper::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
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
    
    if (validLastSatUpdate)
        return lastSatUpdate;

    if (!fromSatellitePositioningMethodsOnly)
        if (validLastUpdate)
            return lastUpdate;
    
    time = lastKnownPositionTime.value().toDouble();
    latitude = lastKnownPositionLatitude.value().toDouble();
    longitude = lastKnownPositionLongitude.value().toDouble();
    altitude = lastKnownPositionAltitude.value().toDouble();
    speed = lastKnownPositionSpeed.value().toDouble();
    track = lastKnownPositionTrack.value().toDouble();
    climb = lastKnownPositionClimb.value().toDouble();
        
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

    // Only positions with time (3D) are provided.
    if (time) {
        posInfo.setDateTime(QDateTime::fromTime_t(time));
        return posInfo;
    }

    return QGeoPositionInfo();
}

void LiblocationWrapper::deviceDisconnected(LocationGPSDevice *device,
                                            gpointer data)
{
    Q_UNUSED(device)

    if (!data)
        return;

    LiblocationWrapper *object = (LiblocationWrapper *)data;
    object->setOnline(false);
}

void LiblocationWrapper::deviceConnected(LocationGPSDevice *device,
                                         gpointer data)
{
    Q_UNUSED(device)

    if (!data)
        return;

    LiblocationWrapper *object = (LiblocationWrapper *)data;
    object->setOnline(true);
}

void LiblocationWrapper::setOnline(bool status)
{
    emit online(status);
}

bool LiblocationWrapper::isOnline() const
{
    if (!locationDevice) {
        return false;
    } else {
        return locationDevice->online == 1;
    }
}

void LiblocationWrapper::satellitesInView(const QList<QGeoSatelliteInfo> &satellites)
{
    emit satellitesInViewUpdated(satellites);
}

void LiblocationWrapper::satellitesInUse(const QList<QGeoSatelliteInfo> &satellites)
{
    emit satellitesInUseUpdated(satellites);
}

void LiblocationWrapper::start() {
    if (locationState & LiblocationWrapper::RequestSingleShot)
        locationState &= ~LiblocationWrapper::RequestSingleShot;
    
    if ((locationState & LiblocationWrapper::Inited) &&
        !(locationState & LiblocationWrapper::Started)) {
        if (!errorHandlerId) {
            errorHandlerId =
                g_signal_connect(G_OBJECT(locationControl), "error-verbose",
                                 G_CALLBACK(&locationError), 
                                 static_cast<void*>(this));
        }

        if (!posChangedId) {
            posChangedId =
                g_signal_connect(G_OBJECT(locationDevice), "changed",
                                 G_CALLBACK(&locationChanged), 
                                 static_cast<void*>(this));
        }

        location_gpsd_control_start(locationControl);
        
        locationState |= LiblocationWrapper::Started;
        locationState &= ~LiblocationWrapper::Stopped;
    }
}

void LiblocationWrapper::stop() {
    if ((locationState & (LiblocationWrapper::Started |
                          LiblocationWrapper::Inited)) &&
        !(locationState & LiblocationWrapper::Stopped)) {
        if (errorHandlerId)
            g_signal_handler_disconnect(G_OBJECT(locationControl), 
                                        errorHandlerId);
        if (posChangedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice), 
                                        posChangedId);
        if (connectedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice), 
                                        connectedId);
        if (disconnectedId)
            g_signal_handler_disconnect(G_OBJECT(locationDevice), 
                                        disconnectedId);
        errorHandlerId = 0;
        posChangedId = 0;
        connectedId = 0;
        disconnectedId = 0;

        location_gpsd_control_stop(locationControl);

        locationState &= ~LiblocationWrapper::Started;
        locationState |= LiblocationWrapper::Stopped;
    }
}

#include "moc_liblocationwrapper.cpp"
QTM_END_NAMESPACE

