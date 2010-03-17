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
#include "n900lightsensor.h"
#include "n900proximitysensor.h"
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QDebug>

class n900SensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
    {
        qDebug() << "loaded the N900 plugin";
        QSensorManager::registerBackend(QAccelerometer::type, n900accelerometer::id, this);
        QSensorManager::registerBackend(QAmbientLightSensor::type, n900lightsensor::id, this);
        QSensorManager::registerBackend(QProximitySensor::type, n900proximitysensor::id, this);
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == n900accelerometer::id)
            return new n900accelerometer(sensor);

        if (sensor->identifier() == n900lightsensor::id)
            return new n900lightsensor(sensor);

        if (sensor->identifier() == n900proximitysensor::id)
            return new n900proximitysensor(sensor);

        return 0;
    }
};

Q_EXPORT_PLUGIN2(libsensors_n900, n900SensorPlugin)

#include "main.moc"

