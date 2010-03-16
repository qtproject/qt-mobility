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
#include "liblocationwrapper.h"

using namespace std;

QTM_BEGIN_NAMESPACE

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent) 
    : QGeoPositionInfoSource(parent)
{
    // default values
    availableMethods = SatellitePositioningMethods;
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
        positionInfoState |= QGeoPositionInfoSourceMaemo::Stopped;
        return 0;
    } else {
        return -1;
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
    if (!msec) {
        msec = MINIMUM_UPDATE_INTERVAL;
        timerInterval = msec;
        QGeoPositionInfoSource::setUpdateInterval(0);
    } else {    
        msec = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;
        timerInterval = msec;
        QGeoPositionInfoSource::setUpdateInterval(timerInterval);
    }
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
    LiblocationWrapper::instance()->start();
    positionInfoState |= QGeoPositionInfoSourceMaemo::Started;
    positionInfoState &= ~(QGeoPositionInfoSourceMaemo::Stopped |
                           QGeoPositionInfoSourceMaemo::RequestSingleShot);

    if (!updateTimer->isActive())
        updateTimer->start(timerInterval);
}

void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    if (updateTimer->isActive())
        updateTimer->stop();
    LiblocationWrapper::instance()->stop();
    positionInfoState &= ~(QGeoPositionInfoSourceMaemo::Started |
                           QGeoPositionInfoSourceMaemo::RequestActive |
                           QGeoPositionInfoSourceMaemo::RequestSingleShot);
    positionInfoState |= QGeoPositionInfoSourceMaemo::Stopped;
}

void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    int timeoutRequest = 0;

    if (!timeout) {
        timeoutRequest = MINIMUM_UPDATE_INTERVAL;
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
        timeoutRequest = timeout;
    }

    if (updateTimer->isActive())
         updateTimer->stop();

    if (requestTimer->isActive())
        requestTimer->stop();

    LiblocationWrapper::instance()->start();
    updateTimer->start(MINIMUM_UPDATE_INTERVAL);
    requestTimer->start(timeoutRequest);

    positionInfoState |= QGeoPositionInfoSourceMaemo::RequestActive;

    if (positionInfoState & QGeoPositionInfoSourceMaemo::Stopped)
       positionInfoState |= QGeoPositionInfoSourceMaemo::RequestSingleShot;
}

void QGeoPositionInfoSourceMaemo::newPositionUpdate()
{
    if (LiblocationWrapper::instance()->fixIsValid())
        emit positionUpdated(LiblocationWrapper::instance()->position());

    if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestActive) {
        positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestActive;

        if (requestTimer->isActive())
            requestTimer->stop();

        if (positionInfoState & QGeoPositionInfoSourceMaemo::RequestSingleShot) {
            positionInfoState &= ~QGeoPositionInfoSourceMaemo::RequestSingleShot;
            return;
        }
    }
    updateTimer->start(timerInterval);
}

void QGeoPositionInfoSourceMaemo::requestTimeoutElapsed()
{
    emit updateTimeout();
    if (updateTimer->isActive())
        updateTimer->stop();
    
    if (!(positionInfoState & QGeoPositionInfoSourceMaemo::RequestSingleShot))
        updateTimer->start(timerInterval);
        
    positionInfoState &= ~(QGeoPositionInfoSourceMaemo::RequestActive |
                            QGeoPositionInfoSourceMaemo::RequestSingleShot);
}


#include "moc_qgeopositioninfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

