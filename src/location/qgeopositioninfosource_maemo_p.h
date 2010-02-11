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

#ifndef QGEOPOSITIONINFOSOURCEMAEMO_H
#define QGEOPOSITIONINFOSOURCEMAEMO_H

#include "qgeopositioninfosource.h"
#include "dbuscomm_maemo_p.h"

#ifdef Q_WS_MAEMO_5
#include "qgeocoordinate.h"

extern "C" {
   #include <glib.h>
   #include <location/location-gpsd-control.h>
   #include <location/location-gps-device.h>
   #include <location/location-misc.h>
   #include <location/location-distance-utils.h>
}
#endif

QTM_BEGIN_NAMESPACE

class DBusComm;
class QGeoPositionInfoSourceMaemo : public QGeoPositionInfoSource
{
    Q_OBJECT

#ifdef Q_WS_MAEMO_5
    Q_PROPERTY(bool online READ isOnline)
#endif

public:

    QGeoPositionInfoSourceMaemo(QObject *parent = 0);
#ifdef Q_WS_MAEMO_5    
    ~QGeoPositionInfoSourceMaemo();
#endif
    int init();

    virtual void setUpdateInterval(int interval);
    virtual void setPreferredPositioningMethods(PositioningMethods sources);
    //virtual QPositioninfo QGeoPositionInfoSource::lastUpdate(QFlags<QGeoPositionInfoSource::PositionSourceType>) const;
    virtual QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    virtual PositioningMethods supportedPositioningMethods() const;
    virtual int minimumUpdateInterval() const;

#ifdef Q_WS_MAEMO_5
    bool isOnline() const;
#endif

private:
#ifdef Q_WS_MAEMO_5
    LocationGPSDControl *locationControl;
    LocationGPSDevice *locationDevice;
    static void locationError(LocationGPSDevice *device, gint code, gpointer data);
    static void deviceDisconnected(LocationGPSDevice *device, gpointer data);
    static void deviceConnected(LocationGPSDevice *device, gpointer data);
    static void locationChanged(LocationGPSDevice *device, gpointer data);
    int mapUpdateInterval(int msec);
    int errorHandlerId;
    int posChangedId;
    int connectedId;
    int disconnectedId;
    int tmpUpdateInterval;
    int requestTimeout;
    enum LocationState {
        Undefined = 0,
        Inited = 1,
        Started = 2,
        Stopped = 4,
        RequestActive = 8,
        RequestSingleShot = 16
    };
    int locationState;    
#endif
    void dumpNMEA(const char *msg);
    DBusComm* dbusComm;
    QGeoPositionInfo lastUpdate;
    QGeoPositionInfo lastSatUpdate;
    bool validLastUpdate;
    bool validLastSatUpdate;
    static const int MinimumUpdateInterval = 1000;
    int time_interval_;
    int distance_interval_;
    bool registered_;
    PositioningMethods availableMethods;

#ifdef Q_WS_MAEMO_5
signals:
    void online(bool status);

    Q_SIGNALS:
    void positionUpdated(const QGeoPositionInfo &update);

private Q_SLOTS:
    void setOnline(bool status);
    void setLocation(const QGeoPositionInfo &update);
#endif
public slots:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 5000);
    void dbusMessages(const QByteArray &msg);
    void newPositionUpdate(const QGeoPositionInfo &update);
};

QTM_END_NAMESPACE

#endif
