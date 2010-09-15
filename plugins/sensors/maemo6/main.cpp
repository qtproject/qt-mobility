/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "maemo6accelerometer.h"
#include "maemo6als.h"
#include "maemo6compass.h"
#include "maemo6magnetometer.h"
#include "maemo6orientationsensor.h"
#include "maemo6proximitysensor.h"
#include "maemo6rotationsensor.h"
#include "maemo6tapsensor.h"
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QDebug>

class maemo6SensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
    {
        QSensorManager::registerBackend(QAccelerometer::type, maemo6accelerometer::id, this);
        QSensorManager::registerBackend(QAmbientLightSensor::type, maemo6als::id, this);
        QSensorManager::registerBackend(QCompass::type, maemo6compass::id, this);
        QSensorManager::registerBackend(QMagnetometer::type, maemo6magnetometer::id, this);
        QSensorManager::registerBackend(QOrientationSensor::type, maemo6orientationsensor::id, this);
        QSensorManager::registerBackend(QProximitySensor::type, maemo6proximitysensor::id, this);
        QSensorManager::registerBackend(QRotationSensor::type, maemo6rotationsensor::id, this);
        QSensorManager::registerBackend(QTapSensor::type, maemo6tapsensor::id, this);
        qDebug() << "Loaded the Maemo 6 sensor plugin";
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == maemo6accelerometer::id)
            return new maemo6accelerometer(sensor);
        else if (sensor->identifier() == maemo6als::id)
            return new maemo6als(sensor);
        else if (sensor->identifier() == maemo6compass::id)
            return new maemo6compass(sensor);
        else if (sensor->identifier() == maemo6magnetometer::id)
            return new maemo6magnetometer(sensor);
        else if (sensor->identifier() == maemo6orientationsensor::id)
            return new maemo6orientationsensor(sensor);
        else if (sensor->identifier() == maemo6proximitysensor::id)
            return new maemo6proximitysensor(sensor);
        else if (sensor->identifier() == maemo6rotationsensor::id)
            return new maemo6rotationsensor(sensor);
        else if (sensor->identifier() == maemo6tapsensor::id)
            return new maemo6tapsensor(sensor);

        return 0;
    }
};

Q_EXPORT_PLUGIN2(libsensors_maemo6, maemo6SensorPlugin)

#include "main.moc"
