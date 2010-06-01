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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeopositioninfosource_maemo_p.h"
#include <iostream>
#include <QDateTime>

using namespace std;

QTM_BEGIN_NAMESPACE

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent): QGeoPositionInfoSource(parent)
{
    time_interval_ = DefaultUpdateInterval;
    availableMethods = AllPositioningMethods;
}


int QGeoPositionInfoSourceMaemo::init()
{
    int status;
    dbusComm = new DBusComm();
    if ((status = dbusComm->init()) == 0) {
        QObject::connect(dbusComm, SIGNAL(receivedPositionUpdate(const QGeoPositionInfo &)),
                         this, SLOT(newPositionUpdate(const QGeoPositionInfo &)));
    }
    return status;
}


void QGeoPositionInfoSourceMaemo::newPositionUpdate(const QGeoPositionInfo &update)
{
    emit positionUpdated(update);
}


QGeoPositionInfo QGeoPositionInfoSourceMaemo::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    static QGeoPositionInfo lastUpdate;
    lastUpdate = dbusComm->requestLastKnownPosition(fromSatellitePositioningMethodsOnly); 

    return lastUpdate;
}


QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceMaemo::supportedPositioningMethods() const
{
    return availableMethods;
}


void QGeoPositionInfoSourceMaemo::setUpdateInterval(int msec)
{
    msec = (msec < MinimumUpdateInterval) ? MinimumUpdateInterval : msec;
    QGeoPositionInfoSource::setUpdateInterval(msec);

    dbusComm->sendConfigRequest(dbusComm->CommandSetInterval, 0, msec);
}


void QGeoPositionInfoSourceMaemo::setPreferredPositioningMethods(PositioningMethods sources)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(sources);
    dbusComm->sendConfigRequest(dbusComm->CommandSetMethods, sources, 0);
}


int QGeoPositionInfoSourceMaemo::minimumUpdateInterval() const
{
    return MinimumUpdateInterval;
}


void QGeoPositionInfoSourceMaemo::startUpdates()
{
    dbusComm->sendConfigRequest(DBusComm::CommandStart, 0, time_interval_);
}


void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    dbusComm->sendConfigRequest(dbusComm->CommandStop, 0, 0);
}


void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    dbusComm->sendConfigRequest(dbusComm->CommandOneShot, 0, timeout);
}

#include "moc_qgeopositioninfosource_maemo_p.cpp"
QTM_END_NAMESPACE

