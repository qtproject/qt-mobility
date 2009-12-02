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

#ifndef QSENSOR_H
#define QSENSOR_H

#include <qmobilityglobal.h>
#include <QObject>

QTM_BEGIN_NAMESPACE

class QSensorListener;
class QSensorEvent;

class Q_SENSORS_EXPORT QSensor : public QObject
{
public:
    // Types of sensors that the API supports
    enum Type {
        Orientation,
        Rotation,
        AngularAcceleration,
        Acceleration,
        DoubleTap,
        Proximity,
        MagneticNorth,
        Magnetometer,
        AmbientLight,

        // Non-standard sensor types
        UserSensor = 128
    };

    enum Sensitivity {
        // These use pre-determined timing intervals, as set by the sensor
        OccasionalUpdates, // When the system feels like it
        InfrequentUpdates, // Every now and then
        FrequentUpdates,   // Often (eg. for gaming controls)

        // For more control
        TimedUpdates,      // Every x milliseconds (may not be supported by all sensors)
        RealtimeUpdates    // As often as polled (may not be supported by all sensors)
    };

    // Try to 'grab' the sensor (some sensors have ownership issues)
    virtual bool open() = 0;

    // Release the sensor
    virtual void close() = 0;

    // Set the desired sensitivity (default is defined by the sensor)
    // Use documentation to determine the sensitivities that the sensor
    // supports.
    void setSensitivity(Sensitivity sensitivity, int interval = 0);

    // Register a listener (that will receive sensor events as they come in)
    void addListener(QSensorListener *listener);

};

class QSensorListener
{
public:
    virtual void sensorEvent(QSensorEvent *event) = 0;
};

class Q_SENSORS_EXPORT QSensorEvent
{
public:
    quint64 timestamp;
};

QTM_END_NAMESPACE

#endif

