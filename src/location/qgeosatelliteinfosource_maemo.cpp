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

#include "qgeosatelliteinfosource_maemo_p.h"

QTM_BEGIN_NAMESPACE

#ifdef Q_WS_MAEMO_5
#define LOCATION_INTERVAL_MIN LOCATION_INTERVAL_1S*100        
#endif        
QGeoSatelliteInfoSourceMaemo::QGeoSatelliteInfoSourceMaemo(QObject *parent) : QGeoSatelliteInfoSource(parent)
{
    client_id_ = -1;
#ifdef Q_WS_MAEMO_5
    errorHandlerId = 0;
    signalHandlerId = 0;
    requestUpdateTimeout = 0;
    satelliteState = QGeoSatelliteInfoSourceMaemo::Undefined;    
#endif    
}

#ifdef Q_WS_MAEMO_5
QGeoSatelliteInfoSourceMaemo::~QGeoSatelliteInfoSourceMaemo()
{
    if(satelliteDevice)
        g_object_unref(satelliteDevice);
    if(satelliteControl)
        g_object_unref(satelliteControl);
}
#endif

int QGeoSatelliteInfoSourceMaemo::init()
{
#ifdef Q_WS_MAEMO_5
    g_type_init();
    satelliteControl = location_gpsd_control_get_default();
    if(satelliteControl) {
        g_object_set(G_OBJECT(satelliteControl),
                     "preferred-method", LOCATION_METHOD_USER_SELECTED,
                     "preferred-interval", LOCATION_INTERVAL_DEFAULT,
                     NULL);
    } else {
        return -1;
    }
    
    satelliteDevice =
            (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE,
                                             NULL);

    if(satelliteDevice) {
        errorHandlerId =
            g_signal_connect(G_OBJECT(satelliteControl), "error-verbose",
                             G_CALLBACK(&satelliteError), 
                             static_cast<void*>(this));
        signalHandlerId =
            g_signal_connect(G_OBJECT(satelliteDevice), "changed",
                             G_CALLBACK(&infoChanged), 
                             static_cast<void*>(this));
        satelliteState = QGeoSatelliteInfoSourceMaemo::Inited;                
        return 0;
    } else {
        return -1;
    }

#else
    int status;

    dbusComm = new DBusComm();
    status = dbusComm->init();

    QObject::connect(dbusComm, SIGNAL(npeMessage(const QByteArray &)),
                     this, SLOT(npeMessages(const QByteArray &)));

    return status;
#endif // Q_WS_MAEMO_5
    return 0;
}

// This method receives messages from DBus.

void QGeoSatelliteInfoSourceMaemo::dbusMessages(const QByteArray &msg)
{
    Q_UNUSED(msg)

    return;
}


void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
#ifdef Q_WS_MAEMO_5
    if(satelliteState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot)
        satelliteState &= ~QGeoSatelliteInfoSourceMaemo::RequestSingleShot;
    
    if ((satelliteState & QGeoSatelliteInfoSourceMaemo::Inited) &&
        !(satelliteState & QGeoSatelliteInfoSourceMaemo::Started)) {
        if(!errorHandlerId) {
            errorHandlerId =
                g_signal_connect(G_OBJECT(satelliteControl), "error-verbose",
                                 G_CALLBACK(&satelliteError), 
                                 static_cast<void*>(this));            
        }
        if(!signalHandlerId) {
            signalHandlerId =
                g_signal_connect(G_OBJECT(satelliteDevice), "changed",
                                 G_CALLBACK(&infoChanged), 
                                 static_cast<void*>(this));            
        }
        location_gpsd_control_start(satelliteControl);
        
        satelliteState |= QGeoSatelliteInfoSourceMaemo::Started;
        satelliteState &= ~QGeoSatelliteInfoSourceMaemo::Stopped;        
    }
#else
#if 0
    int len = npe.NewStartTrackingMsg(&msg, client_id_, NpeIf::MethodAll,
                                      NpeIf::OptionNone , 1);

    // cout << "ISI Message len " << len << "\n";
    dbusComm->sendIsiMessage(msg, len);
    delete [] msg;
#endif

#if 0 // Test !
    QList<QGeoSatelliteInfo> list;
    QGeoSatelliteInfo tmp;

    tmp.setPrnNumber(33);
    tmp.setSignalStrength(15);
    tmp.setProperty(QGeoSatelliteInfo::Azimuth, 45.0);
    tmp.setProperty(QGeoSatelliteInfo::Elevation, 25.5);
    list.append(tmp);
    list.append(tmp);
    list.append(tmp);

    emit satellitesInViewUpdated(list);
    emit satellitesInUseUpdated(list);
#endif
#endif // Q_WS_MAEMO_5

}


void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{
#ifdef Q_WS_MAEMO_5
    if ((satelliteState & (QGeoSatelliteInfoSourceMaemo::Started |
                          QGeoSatelliteInfoSourceMaemo::Inited)) &&
        !(satelliteState & QGeoSatelliteInfoSourceMaemo::Stopped)) {
        if(errorHandlerId)
            g_signal_handler_disconnect(G_OBJECT(satelliteControl), 
                                        errorHandlerId);
        if(signalHandlerId)
            g_signal_handler_disconnect(G_OBJECT(satelliteDevice), 
                                        signalHandlerId); 
        errorHandlerId = 0;
        signalHandlerId = 0;
        location_gpsd_control_stop(satelliteControl);

        satelliteState &= ~QGeoSatelliteInfoSourceMaemo::Started;
        satelliteState |= QGeoSatelliteInfoSourceMaemo::Stopped;        
    }
#endif

}

void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
{
#ifdef Q_WS_MAEMO_5
    if (QGeoSatelliteInfoSourceMaemo::Inited) {
        if (satelliteState & QGeoSatelliteInfoSourceMaemo::RequestActive)
            return;
            
        if (!timeout) {
            requestUpdateTimeout = LOCATION_INTERVAL_DEFAULT;
        } else if (timeout < LOCATION_INTERVAL_MIN) {
            satelliteState &= ~(QGeoSatelliteInfoSourceMaemo::RequestActive |
                                QGeoSatelliteInfoSourceMaemo::RequestSingleShot);
            requestUpdateTimeout = 0;
            emit requestTimeout();
            return;
        } else {
            requestUpdateTimeout = this->mapUpdateInterval(timeout);
        }
    
        if (!(satelliteState & QGeoSatelliteInfoSourceMaemo::Started)) {
            this->setUpdateInterval(requestUpdateTimeout);
            startUpdates();
            satelliteState |= (QGeoSatelliteInfoSourceMaemo::RequestActive |
                              QGeoSatelliteInfoSourceMaemo::RequestSingleShot);

        } else if (satelliteState & QGeoSatelliteInfoSourceMaemo::Started) {            
            if (requestUpdateTimeout < LOCATION_INTERVAL_DEFAULT) {
                satelliteState |= (QGeoSatelliteInfoSourceMaemo::RequestActive);
                this->setUpdateInterval(requestUpdateTimeout);
            }
        }
    }
#else        
    int a;
    a = timeout + 1;
#endif
}

#ifdef Q_WS_MAEMO_5
void QGeoSatelliteInfoSourceMaemo::setUpdateInterval(int liblocation_interval)
{
    if(satelliteState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        if(liblocation_interval <= LOCATION_INTERVAL_DEFAULT) {
            satelliteState &= ~(QGeoSatelliteInfoSourceMaemo::RequestActive |
                               QGeoSatelliteInfoSourceMaemo::RequestSingleShot);
        } else {
            return;
        }
    } 

    if(satelliteControl) {
        g_object_set(G_OBJECT(satelliteControl),
                     "preferred-method", LOCATION_METHOD_USER_SELECTED,
                     "preferred-interval", liblocation_interval,
                     NULL) ;      
    }
}

void QGeoSatelliteInfoSourceMaemo::satelliteError(LocationGPSDevice *device,
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

void QGeoSatelliteInfoSourceMaemo::infoChanged(LocationGPSDevice *device,
                                               gpointer data)
{
    QGeoSatelliteInfo satInfo;
    QGeoSatelliteInfoSourceMaemo *object;

    if (!data || !device)
        return;
     
    object = (QGeoSatelliteInfoSourceMaemo *)data;

    if (device && device->satellites_in_view) {
        QList<QGeoSatelliteInfo> satsInView;
        QList<QGeoSatelliteInfo> satsInUse;
        int i;
        for(i=0;i<device->satellites->len;i++) {
            LocationGPSDeviceSatellite *satData =
                    (LocationGPSDeviceSatellite *)g_ptr_array_index(device->satellites,
                                                                    i);
            satInfo.setSignalStrength(satData->signal_strength);
            satInfo.setPrnNumber(satData->prn);
            satInfo.setAttribute(QGeoSatelliteInfo::Elevation, satData->elevation);
            satInfo.setAttribute(QGeoSatelliteInfo::Azimuth, satData->azimuth);

            satsInView.append(satInfo);
            if(satData->in_use)
                satsInUse.append(satInfo);
        }
        
        if(!satsInView.isEmpty())
            object->satellitesInView(satsInView);
        
        if(!satsInUse.isEmpty())
            object->satellitesInUse(satsInUse);
    }    
}

void QGeoSatelliteInfoSourceMaemo::satellitesInView(const QList<QGeoSatelliteInfo> &satellites)
{
    emit satellitesInViewUpdated(satellites);

#if Q_WS_MAEMO_5    
    if(satelliteState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        satelliteState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;

        if(satelliteState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot) {
            stopUpdates();
            satelliteState &= ~QGeoSatelliteInfoSourceMaemo::RequestSingleShot;
        }        
        setUpdateInterval(LOCATION_INTERVAL_DEFAULT);
    }
#endif
}

void QGeoSatelliteInfoSourceMaemo::satellitesInUse(const QList<QGeoSatelliteInfo> &satellites)
{
    emit satellitesInUseUpdated(satellites);
    
#if Q_WS_MAEMO_5    
    if(satelliteState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        satelliteState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;

        if(satelliteState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot) {
            stopUpdates();
            satelliteState &= ~QGeoSatelliteInfoSourceMaemo::RequestSingleShot;
        }        
        setUpdateInterval(LOCATION_INTERVAL_DEFAULT);
    }
#endif    
}

int QGeoSatelliteInfoSourceMaemo::mapUpdateInterval(int msec) {
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

#if 0
void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
void requestTimeout();
#endif

#include "moc_qgeosatelliteinfosource_maemo_p.cpp"
QTM_END_NAMESPACE

