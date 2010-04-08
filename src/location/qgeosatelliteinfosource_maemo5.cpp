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
#include "liblocationwrapper.h"

QTM_BEGIN_NAMESPACE

#define MINIMUM_UPDATE_INTERVAL 1000
#define DEFAULT_UPDATE_INTERVAL 5000

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
        return 0;
    } else {
        return -1;
    }
}

void QGeoSatelliteInfoSourceMaemo::setUpdateInterval(int msec)
{
    msec = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;

    timerInterval = msec;
}

void QGeoSatelliteInfoSourceMaemo::startUpdates()
{
    LiblocationWrapper::instance()->start();
    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Started;
    satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::Stopped |
                            QGeoSatelliteInfoSourceMaemo::RequestSingleShot);
    if (!updateTimer->isActive())
        updateTimer->start(timerInterval);
}

void QGeoSatelliteInfoSourceMaemo::stopUpdates()
{
    if (updateTimer->isActive())
        updateTimer->stop();
    LiblocationWrapper::instance()->stop();
    satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::Started |
                            QGeoSatelliteInfoSourceMaemo::RequestActive |
                            QGeoSatelliteInfoSourceMaemo::RequestSingleShot);
    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::Stopped;
}

void QGeoSatelliteInfoSourceMaemo::requestUpdate(int timeout)
{
    int timeoutRequest = 0;

    if (!timeout) {
        timeoutRequest = MINIMUM_UPDATE_INTERVAL;
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
        timeoutRequest = timeout;
    }

    if (updateTimer->isActive())
        updateTimer->stop();

    if (requestTimer->isActive())
        requestTimer->stop();

    LiblocationWrapper::instance()->start();
    updateTimer->start(MINIMUM_UPDATE_INTERVAL);
    requestTimer->start(timeoutRequest);

    satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::RequestActive;

    if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::Stopped)
        satelliteInfoState |= QGeoSatelliteInfoSourceMaemo::RequestSingleShot;
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

        if (requestTimer->isActive())
            requestTimer->stop();

        if (satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot) {
            satelliteInfoState &= ~QGeoSatelliteInfoSourceMaemo::RequestSingleShot;
            return;
        }
    }
    updateTimer->start(timerInterval);
}

void QGeoSatelliteInfoSourceMaemo::requestTimeoutElapsed()
{
    emit requestTimeout();
    if (updateTimer->isActive())
        updateTimer->stop();

    if (!(satelliteInfoState & QGeoSatelliteInfoSourceMaemo::RequestSingleShot))
        updateTimer->start(timerInterval);

    satelliteInfoState &= ~(QGeoSatelliteInfoSourceMaemo::RequestActive |
                            QGeoSatelliteInfoSourceMaemo::RequestSingleShot);
}

#include "moc_qgeosatelliteinfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

