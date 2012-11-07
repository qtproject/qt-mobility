/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

bool portraitOrientation;

class n900SensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
    {
        qDebug() << "loaded the N900 plugin";
        portraitOrientation = false;
        if (qgetenv("N900_PORTRAIT_SENSORS") == "1")
            portraitOrientation = true;
        QSensorManager::registerBackend(QAccelerometer::type, n900accelerometer::id, this);
        QSensorManager::registerBackend(QLightSensor::type, n900lightsensor::id, this);
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

Q_EXPORT_PLUGIN2(qtsensors_n900, n900SensorPlugin)

#include "main.moc"

