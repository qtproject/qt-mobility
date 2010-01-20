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

#include "n900accelerometer.h"
#include "n900orientationsensor.h"
#include "n900lightsensor.h"
#include "n900proximitysensor.h"
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QFile>
#include <QDebug>

static QSensorBackend *create_sensor_backend_n900accelerometer(QSensor *sensor)
{
    if (QFile::exists(ACCELEROMETER_FILE))
        return new n900accelerometer(sensor);
    return 0;
}

static QSensorBackend *create_sensor_backend_n900orientationsensor(QSensor *sensor)
{
    if (QFile::exists(ACCELEROMETER_FILE))
        return new n900orientationsensor(sensor);
    return 0;
}

static QSensorBackend *create_sensor_backend_n900lightsensor(QSensor *sensor)
{
    if (QFile::exists(LIGHTSENSOR_FILE))
        return new n900lightsensor(sensor);
    return 0;
}

static QSensorBackend *create_sensor_backend_n900proximitysensor(QSensor *sensor)
{
    if (QFile::exists(PROXIMITY_FILE))
        return new n900proximitysensor(sensor);
    return 0;
}

class n900SensorPlugin : public QObject, public QSensorPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
    {
        qWarning() << "Loaded the N900 plugin";
        REGISTER_STATEMENT(n900accelerometer, "QAccelerometer", QByteArray("n900.accelerometer"));
        REGISTER_STATEMENT(n900orientationsensor, "QOrientationSensor", QByteArray("n900.orientation"));
        REGISTER_STATEMENT(n900lightsensor, "QAmbientLightSensor", QByteArray("n900.light"));
        REGISTER_STATEMENT(n900proximitysensor, "QProximitySensor", QByteArray("n900.proximity"));
    }
};

Q_EXPORT_PLUGIN2(libsensors_n900, n900SensorPlugin);

#include "main.moc"

