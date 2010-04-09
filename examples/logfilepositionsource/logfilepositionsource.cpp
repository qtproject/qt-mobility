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
#include <QtCore>

#include "logfilepositionsource.h"

LogFilePositionSource::LogFilePositionSource(QObject *parent)
    : QGeoPositionInfoSource(parent),
      logFile(new QFile(this)),
      timer(new QTimer(this))
{
    connect(timer, SIGNAL(timeout()), this, SLOT(readNextPosition()));

    logFile->setFileName(QCoreApplication::applicationDirPath()
            + QDir::separator() + "simplelog.txt");
    if (!logFile->open(QIODevice::ReadOnly))
        qWarning() << "Error: cannot open source file" << logFile->fileName();
}

QGeoPositionInfo LogFilePositionSource::lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly*/) const
{
    return lastPosition;
}

LogFilePositionSource::PositioningMethods LogFilePositionSource::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

int LogFilePositionSource::minimumUpdateInterval() const
{
    return 500;
}

void LogFilePositionSource::startUpdates()
{
    int interval = updateInterval();
    if (interval < minimumUpdateInterval())
        interval = minimumUpdateInterval();

    timer->start(interval);
}

void LogFilePositionSource::stopUpdates()
{
    timer->stop();
}

void LogFilePositionSource::requestUpdate(int /*timeout*/)
{
    // For simplicity, ignore timeout - assume that if data is not available
    // now, no data will be added to the file later
    if (logFile->canReadLine())
        readNextPosition();
    else
        emit updateTimeout();
}

void LogFilePositionSource::readNextPosition()
{
    QByteArray line = logFile->readLine().trimmed();
    if (!line.isEmpty()) {
        QList<QByteArray> data = line.split(' ');
        double latitude;
        double longitude;
        bool hasLatitude = false;
        bool hasLongitude = false;
        QDateTime timestamp = QDateTime::fromString(QString(data.value(0)), Qt::ISODate);
        latitude = data.value(1).toDouble(&hasLatitude);
        longitude = data.value(2).toDouble(&hasLongitude);

        if (hasLatitude && hasLongitude && timestamp.isValid()) {
            QGeoCoordinate coordinate(latitude, longitude);
            QGeoPositionInfo info(coordinate, timestamp);
            if (info.isValid()) {
                lastPosition = info;
                emit positionUpdated(info);
            }
        }
    }
}
