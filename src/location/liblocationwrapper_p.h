/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef LIBLOCATIONWRAPPER_H
#define LIBLOCATINWRAPPER_H

// INCLUDES
#include <QDebug>
#include <QFile>

#include "qgeocoordinate.h"
#include "qgeopositioninfo.h"
#include "qgeosatelliteinfo.h"

#include "gconfitem_p.h"

extern "C" {
   #include <glib.h>
   #include <location/location-gpsd-control.h>
   #include <location/location-gps-device.h>
   #include <location/location-misc.h>
   #include <location/location-distance-utils.h>
}

QTM_BEGIN_NAMESPACE

class LiblocationWrapper : public QObject
{
    Q_OBJECT

public:
    static LiblocationWrapper *instance();
    LiblocationWrapper();
    ~LiblocationWrapper();

    void start();
    void stop();
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    bool inited();
    bool isActive();
    QList<QGeoSatelliteInfo> satellitesInView();
    QList<QGeoSatelliteInfo> satellitesInUse();

private:
    QFile *file;
    LocationGPSDControl *locationControl;
    LocationGPSDevice *locationDevice;

    static void locationError(LocationGPSDevice *device, gint code, gpointer data);
    static void locationChanged(LocationGPSDevice *device, gpointer data);

    int errorHandlerId;
    int posChangedId;
    int origUpdateInterval;
    int startcounter;
    QGeoPositionInfo lastUpdate;
    QGeoPositionInfo lastSatUpdate;
    bool validLastUpdate;
    bool validLastSatUpdate;
    bool fromSatellite;

    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    QList<QGeoSatelliteInfo> satsInView;
    QList<QGeoSatelliteInfo> satsInUse;

    enum LocationState {
        Undefined = 0,
        Inited = 1,
        Started = 2,
        Stopped = 4,
        RequestActive = 8,
        RequestSingleShot = 16
    };
    int locationState;

signals:
    void positionUpdated(const QGeoPositionInfo &position);
    void error();
};

QTM_END_NAMESPACE
#endif // LIBLOCATIONWRAPPER_H
