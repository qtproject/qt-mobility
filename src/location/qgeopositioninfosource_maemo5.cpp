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

#define MINIMUM_UPDATE_INTERVAL 1000

QGeoPositionInfoSourceMaemo::QGeoPositionInfoSourceMaemo(QObject *parent) 
    : QGeoPositionInfoSource(parent)
{
    // default values
    availableMethods = SatellitePositioningMethods;
    positionInited = false;
}

int QGeoPositionInfoSourceMaemo::init()
{
    if (positionInited)
        return 0;
    
    if (LiblocationWrapper::instance()->inited()) {

        QObject::connect(LiblocationWrapper::instance(), 
                         SIGNAL(positionUpdated(const QGeoPositionInfo &)),
                         this, 
                         SLOT(newPositionUpdate(const QGeoPositionInfo &)));
        positionInited = true;
        return 0;
    }
    else {
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
    msec = (msec < MINIMUM_UPDATE_INTERVAL) ? MINIMUM_UPDATE_INTERVAL : msec;

    QGeoPositionInfoSource::setUpdateInterval(LiblocationWrapper::instance()->setUpdateInterval(msec));    
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
    if (positionInited)
        LiblocationWrapper::instance()->start();
}

void QGeoPositionInfoSourceMaemo::stopUpdates()
{
    if (positionInited)
        LiblocationWrapper::instance()->stop();
}

void QGeoPositionInfoSourceMaemo::requestUpdate(int timeout)
{
    if (positionInited)
        if (!LiblocationWrapper::instance()->requestUpdate(timeout))
            emit updateTimeout();
}

void QGeoPositionInfoSourceMaemo::newPositionUpdate(const QGeoPositionInfo &update)
{
    emit positionUpdated(update);
}

#include "moc_qgeopositioninfosource_maemo5_p.cpp"
QTM_END_NAMESPACE

