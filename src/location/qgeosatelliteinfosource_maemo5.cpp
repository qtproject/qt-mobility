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

#include "qgeosatelliteinfosource_maemo5_p.h"
#include "liblocationwrapper_p.h"

QTM_BEGIN_NAMESPACE
        
QGeoSatelliteInfoSourceMaemo::QGeoSatelliteInfoSourceMaemo(QObject *parent) 
    : QGeoSatelliteInfoSource(parent)
{
    client_id_ = -1;
    timerInterval = DEFAULT_UPDATE_INTERVAL;
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(satelliteStatus()));

    requestTimer = new QTimer(this);
    requestTimer->setSingleShot(true);
    connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimeoutElapsed()));

    satelliteInfoState = QGeoSatelliteInfoSourceMaemo::Undefined;
}

int QGeoSatelliteInfoSourceMaemo::init()
{
    if (LiblocationWrapper::instance()->inited()) {
        satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Stopped;
        return INIT_OK;
    } else {
        return INIT_FAILED;
    }
}

void QGeoSatelliteInfoSourceMaemo::setUpdateInterval(int msec)
{
    bool updateTimerInterval = false;
    
    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive)
        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)
            updateTimerInterval = true;
    
    satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::TogglePowersave |
                            QGeoSatelliteInfoSourceMaemo::PowersaveActive);
    
    timerInterval = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;

    if (timerInterval >= POWERSAVE_THRESHOLD) {
        satelliteInfoState |= (QGeoSatelliteInfoSourceMaemo::PowersaveActive |
                               QGeoSatelliteInfoSourceMaemo::TogglePowersave);
    } else {
        satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::TogglePowersave |
                                QGeoSatelliteInfoSourceMaemo::PowersaveActive);
    }  

    if (updateTimerInterval || (satelliteInfoState &
                                QGeoSatelliteInfoSourceMaemo::Started))
        activateTimer();    
}

void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
    if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive)) {
        startLocationDaemon();
        satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::RequestSingleShot |
                                QGeoSatelliteInfoSourceMaemo::TogglePowersave);
    }
    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{
    if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot)) {    
        updateTimer->stop();
        requestTimer->stop();    
        satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;
        if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped))
            LiblocationWrapper::instance()->stop();
        satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::Started;
        satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Stopped;
    }
}

void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
{
    int timeoutForRequest = 0;
    
    if (!timeout) {
        if (LiblocationWrapper::instance()->isActive())
            timeoutForRequest = DEFAULT_UPDATE_INTERVAL;
        else
            timeoutForRequest = POWERSAVE_POWERON_PERIOD;
    } else if (timeout < MINIMUM_UPDATE_INTERVAL) {
        if (satelliteInfoState & (QGeoSatelliteInfoSourceMaemo::RequestActive |
                                 QGeoSatelliteInfoSourceMaemo::RequestSingleShot))
            return;
        else 
            satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::RequestActive |
                                    QGeoSatelliteInfoSourceMaemo::RequestSingleShot);
        emit requestTimeout();
        return;
    } else {
        timeoutForRequest = timeout;
    }

    updateTimer->stop();
    requestTimer->stop();

    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::RequestActive;
    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)
       satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::RequestSingleShot;

    startLocationDaemon();

    activateTimer();
    requestTimer->start(timeoutForRequest);
}

void QGeoSatelliteInfoSourceMaemo::satelliteStatus()
{
    QList<QGeoSatelliteInfo> satellitesInView = 
            LiblocationWrapper::instance()->satellitesInView();
    QList<QGeoSatelliteInfo> satellitesInUse = 
            LiblocationWrapper::instance()->satellitesInUse();

    if (satellitesInView.length()) {
        // If there are satellites in view, also in use can be emitted.
        emit satellitesInViewUpdated(satellitesInView);
        emit satellitesInUseUpdated(satellitesInUse);
     }

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;
    
        requestTimer->stop();
        
        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot) {
            satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestSingleShot;

            if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive))            
                return;
        }
    }
    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::requestTimeoutElapsed()
{
    updateTimer->stop();
    emit requestTimeout();
    
    satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestActive;

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot) {
        satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestSingleShot;
        stopUpdates();
        if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive))
            return;
    }
    activateTimer();
}

void QGeoSatelliteInfoSourceMaemo::activateTimer() {
    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)
        return;
    
    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestActive) {
        updateTimer->start(MINIMUM_UPDATE_INTERVAL);
    } else {
        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::PowersaveActive) {
            if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::TogglePowersave)) {
                startLocationDaemon();
                satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::TogglePowersave;
                updateTimer->start(POWERSAVE_POWERON_PERIOD);
            } else if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::TogglePowersave) {
                stopUpdates();
                satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::TogglePowersave;
                updateTimer->start(timerInterval - POWERSAVE_POWERON_PERIOD);
            }
        } else {
            updateTimer->start(timerInterval);
        }
    }
}

void QGeoSatelliteInfoSourceMaemo::startLocationDaemon() {
    if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Started))
        LiblocationWrapper::instance()->start();
    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Started;
    satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::Stopped;
}

#include "moc_qgeosatelliteinfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

