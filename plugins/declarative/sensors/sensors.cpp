/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include <qsensorbackend.h>
#include <qaccelerometer.h>
#include <qambientlightsensor.h>
#include <qcompass.h>
#include <qmagnetometer.h>
#include <qorientationsensor.h>
#include <qproximitysensor.h>
#include <qrotationsensor.h>
#include <qtapsensor.h>

QT_BEGIN_NAMESPACE

QTM_USE_NAMESPACE

class QSensorsDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMobility.sensors"));

        qmlRegisterType<QSensorReading>();

        qmlRegisterType<QAccelerometer>("QtMobility.sensors", 1, 1, "Accelerometer");
        qmlRegisterType<QAccelerometerReading>("QtMobility.sensors", 1, 1, "AccelerometerReading");

        qmlRegisterType<QAmbientLightSensor>("QtMobility.sensors", 1, 1, "AmbientLightSensor");
        qmlRegisterType<QAmbientLightReading>("QtMobility.sensors", 1, 1, "AmbientLightReading");

        qmlRegisterType<QCompass>("QtMobility.sensors", 1, 1, "Compass");
        qmlRegisterType<QCompassReading>("QtMobility.sensors", 1, 1, "CompassReading");

        qmlRegisterType<QMagnetometer>("QtMobility.sensors", 1, 1, "Magnetometer");
        qmlRegisterType<QMagnetometerReading>("QtMobility.sensors", 1, 1, "MagnetometerReading");

        qmlRegisterType<QOrientationSensor>("QtMobility.sensors", 1, 1, "OrientationSensor");
        qmlRegisterType<QOrientationReading>("QtMobility.sensors", 1, 1, "OrientationReading");

        qmlRegisterType<QProximitySensor>("QtMobility.sensors", 1, 1, "ProximitySensor");
        qmlRegisterType<QProximityReading>("QtMobility.sensors", 1, 1, "ProximityReading");

        qmlRegisterType<QRotationSensor>("QtMobility.sensors", 1, 1, "RotationSensor");
        qmlRegisterType<QRotationReading>("QtMobility.sensors", 1, 1, "RotationReading");

        qmlRegisterType<QTapSensor>("QtMobility.sensors", 1, 1, "TapSensor");
        qmlRegisterType<QTapReading>("QtMobility.sensors", 1, 1, "TapReading");
    }
};

QT_END_NAMESPACE

#include "sensors.moc"

Q_EXPORT_PLUGIN2(qsensorsdeclarativemodule, QT_PREPEND_NAMESPACE(QSensorsDeclarativeModule));

