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

#include "qgeopositioninfosource_maemo5_p.h"
#include "liblocationwrapper_p.h"

using namespace std;

QTM_BEGIN_NAMESPACE

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent) 
    : QGeoPositionInfoSource(parent)
{
    // default values
    availableMethods = SatellitePositioningMethods;
    lowSignalLevelCount = 0;
    
    timerInterval = DEFAULT_UPDATE_INTERVAL;
    updateTimer = new QTimer(this);
    updateTimer->setSingleShot(true);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(newPositionUpdate()));

    requestTimer = new QTimer(this);
    requestTimer->setSingleShot(true);
    connect(requestTimer, SIGNAL(timeout()), this, SLOT(requestTimeoutElapsed()));

    positionInfoState = QGeoPositionInfoSourceMaemo::Undefined;
}

int QGeoPositionInfoSourceMaemo::init()
{
    if (LiblocationWrapper::instance()->inited()) {
        positionInfoState = QGeoPositionInfoSourceMaemo::Stopped;
        return INIT_OK;
    } else {
        return INIT_FAILED;
    }
}

QGeoPositionInfo QGeoPositionInfoSourceMaemo::lastKnownPosition(bool fromSatellitePositioningMethodsOnly) const
{
    return (LiblocationWrapper::instance()->lastKnownPosition(fromSatellitePositioningMethodsOnly));
}

QGeoPositionInfoSource::PositioningMethods QGeoPositionInfoSourceMaemo::supportedPositioningMethods() const
{
    return availableMethods;
}

void QGeoPositionInfoSourceMaemo::setUpdateInterval(int msec)
{
    bool updateTimerInterval = false;
    
    if (positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive)
        if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
            updateTimerInterval = true;
    
    positionInfoState &= ~(QGeoPositionInfoSourceMaemo::TogglePowersave |
                           QGeoPositionInfoSourceMaemo::PowersaveActive);

    if (!msec) {
        timerInterval = MINIMUM_UPDATE_INTERVAL;
        QGeoPositionInfoSource::setUpdateInterval(0);
    } else {    
        timerInterval = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;
        QGeoPositionInfoSource::setUpdateInterval(timerInterval);
    }

    if (timerInterval >= POWERSAVE_THRESHOLD) {
        positionInfoState |= (QGeoPositionInfoSourceMaemo::PowersaveActive |
                              QGeoPositionInfoSourceMaemo::TogglePowersave);
    } else {
        positionInfoState &= ~(QGeoPositionInfoSourceMaemo::TogglePowersave |
                               QGeoPositionInfoSourceMaemo::PowersaveActive);
    }
    
    if (updateTimerInterval || (positionInfoState & 
                                QGeoPositionInfoSourceMaemo::Started))
        activateTimer();
}

void QGeoPositionInfoSourceMaemo::setPreferredPositioningMethods(PositioningMethods sources)
{
    Q_UNUSED(sources)
    return;
}

int QGeoPositionInfoSourceMaemo::minimumUpdateInterval() const
{
    return MINIMUM_UPDATE_INTERVAL;
}

// public slots:
void QGeoPositionInfoSourceMaemo::startUpdates()
{
    if (!(positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive)) {
        startLocationDaemon();
        positionInfoState &= ~(QGeoPositionInfoSourceMaemo::RequestSingleShot |
                               QGeoPositionInfoSourceMaemo::TogglePowersave);
    }
    activateTimer();
}

void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    if (!(positionInfoState & QGeoPositionInfoSourceMaemo::RequestSingleShot)) {
        updateTimer->stop();
        requestTimer->stop();
        positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestActive;
        if (!(positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)) {
            LiblocationWrapper::instance()->stop();
        }
        positionInfoState &= ~QGeoPositionInfoSourceMaemo::Started;
        positionInfoState |= QGeoPositionInfoSourceMaemo::Stopped;
    }
}

void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    int timeoutForRequest = 0;

    if (!timeout) {
        if (LiblocationWrapper::instance()->isActive())
            timeoutForRequest = DEFAULT_UPDATE_INTERVAL;
        else
            timeoutForRequest = POWERSAVE_POWERON_PERIOD;
    } else if (timeout < MINIMUM_UPDATE_INTERVAL) {
        if (positionInfoState & (QGeoPositionInfoSourceMaemo::RequestActive |
                                 QGeoPositionInfoSourceMaemo::RequestSingleShot))
            return;
        else
            positionInfoState &= ~(QGeoPositionInfoSourceMaemo::RequestActive |
                                   QGeoPositionInfoSourceMaemo::RequestSingleShot);
        emit updateTimeout();
        return;
    } else {
        timeoutForRequest = timeout;
    }

    updateTimer->stop();
    requestTimer->stop();

    positionInfoState |= QGeoPositionInfoSourceMaemo::RequestActive;
    if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
       positionInfoState |= QGeoPositionInfoSourceMaemo::RequestSingleShot;

    startLocationDaemon();
    
    activateTimer();
    requestTimer->start(timeoutForRequest);
}

void QGeoPositionInfoSourceMaemo::newPositionUpdate()
{
    if (LiblocationWrapper::instance()->fixIsValid()) {             
        emit positionUpdated(LiblocationWrapper::instance()->position());
        
        if (positionInfoState & QGeoPositionInfoSourceMaemo::SignalLevelLow) {
            positionInfoState &= ~QGeoPositionInfoSourceMaemo::SignalLevelLow;
            lowSignalLevelCount = 0;
        }
        
        if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive) {
            positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestActive;
            requestTimer->stop();

            if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestSingleShot) {
                positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestSingleShot;
                if (!(positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive))
                {
                    return;
                }
            }
        }
    } else {
        if (lowSignalLevelCount++ > 2) {        
            if (!(positionInfoState & QGeoPositionInfoSourceMaemo::SignalLevelLow)) {
                emit updateTimeout();
                positionInfoState |= QGeoPositionInfoSourceMaemo::SignalLevelLow;
                lowSignalLevelCount = 0;
            }
        }
    }
    activateTimer();
}

void QGeoPositionInfoSourceMaemo::requestTimeoutElapsed()
{
    updateTimer->stop();
    emit updateTimeout();

    positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestActive;

    if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestSingleShot) {
        positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestSingleShot;
        stopUpdates();
        if (!(positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive))
            return;
    }
    activateTimer();
}

void QGeoPositionInfoSourceMaemo::activateTimer() {
    if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
        return;
    
    if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive) {
        updateTimer->start(MINIMUM_UPDATE_INTERVAL);
    } else {
        if (positionInfoState & QGeoPositionInfoSourceMaemo::PowersaveActive) {
            if (!(positionInfoState & QGeoPositionInfoSourceMaemo::TogglePowersave)) {
                startLocationDaemon();
                positionInfoState |= QGeoPositionInfoSourceMaemo::TogglePowersave;
                updateTimer->start(POWERSAVE_POWERON_PERIOD);
            } else if (positionInfoState & QGeoPositionInfoSourceMaemo::TogglePowersave) {
                stopUpdates();
                positionInfoState &= ~QGeoPositionInfoSourceMaemo::TogglePowersave;
                updateTimer->start(timerInterval - POWERSAVE_POWERON_PERIOD);
            }
        } else {
            updateTimer->start(timerInterval);
        }
    }
}

void QGeoPositionInfoSourceMaemo::startLocationDaemon() {
    if (!(positionInfoState & QGeoPositionInfoSourceMaemo::Started))
        LiblocationWrapper::instance()->start();
    positionInfoState |= QGeoPositionInfoSourceMaemo::Started;
    positionInfoState &= ~QGeoPositionInfoSourceMaemo::Stopped;
}

#include "moc_qgeopositioninfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

