/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
    int status;

    dbusComm = new DBusComm();
    status = dbusComm->init();

    QObject::connect(dbusComm, SIGNAL(receivedMessage(const QByteArray &)),
                     this, SLOT(dbusMessages(const QByteArray &)));

    QObject::connect(dbusComm, SIGNAL(receivedPositionUpdate(const QGeoPositionInfo &)),
                     this, SLOT(newPositionUpdate(const QGeoPositionInfo &)));

    return status;
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
    msec = (msec < MinimumUpdateInterval) ? MinimumUpdateInterval : msec;

    QGeoPositionInfoSource::setUpdateInterval(msec);
    if (registered_ == false)
        registered_ = dbusComm->sendDBusRegister();
    dbusComm->sessionConfigRequest(dbusComm->CmdSetInterval, 0, msec);
}

void QGeoPositionInfoSourceMaemo::setPreferredPositioningMethods(PositioningMethods sources)
{
    QGeoPositionInfoSource::setPreferredPositioningMethods(sources);
    if (registered_ == false)
        registered_ = dbusComm->sendDBusRegister();
    dbusComm->sessionConfigRequest(dbusComm->CmdSetMethods, sources, 0);
}


int QGeoPositionInfoSourceMaemo::minimumUpdateInterval() const
{
    return MinimumUpdateInterval;
}


// public slots:

void QGeoPositionInfoSourceMaemo::startUpdates()
{
    if (registered_ == false)
        registered_ = dbusComm->sendDBusRegister();

    int cmd = dbusComm->CmdStart;
    dbusComm->sessionConfigRequest(cmd, 222, time_interval_);
}


void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    if (registered_ == false) return; // nothing to stop
    dbusComm->sessionConfigRequest(dbusComm->CmdStop, 0, 0);
}

// Stub

void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    if (timeout) {}
}

#include "moc_qgeopositioninfosource_maemo_p.cpp"
QTM_END_NAMESPACE

