/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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
#include <qlightsensor.h>
#include <qgyroscope.h>

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

        for (int minor = 1; minor <= 2; minor++) {
            qmlRegisterType<QAccelerometer>("QtMobility.sensors", 1, minor, "Accelerometer");
            qmlRegisterType<QAccelerometerReading>("QtMobility.sensors", 1, minor, "AccelerometerReading");

            qmlRegisterType<QAmbientLightSensor>("QtMobility.sensors", 1, minor, "AmbientLightSensor");
            qmlRegisterType<QAmbientLightReading>("QtMobility.sensors", 1, minor, "AmbientLightReading");

            qmlRegisterType<QCompass>("QtMobility.sensors", 1, minor, "Compass");
            qmlRegisterType<QCompassReading>("QtMobility.sensors", 1, minor, "CompassReading");

            qmlRegisterType<QMagnetometer>("QtMobility.sensors", 1, minor, "Magnetometer");
            qmlRegisterType<QMagnetometerReading>("QtMobility.sensors", 1, minor, "MagnetometerReading");

            qmlRegisterType<QOrientationSensor>("QtMobility.sensors", 1, minor, "OrientationSensor");
            qmlRegisterType<QOrientationReading>("QtMobility.sensors", 1, minor, "OrientationReading");

            qmlRegisterType<QProximitySensor>("QtMobility.sensors", 1, minor, "ProximitySensor");
            qmlRegisterType<QProximityReading>("QtMobility.sensors", 1, minor, "ProximityReading");

            qmlRegisterType<QRotationSensor>("QtMobility.sensors", 1, minor, "RotationSensor");
            qmlRegisterType<QRotationReading>("QtMobility.sensors", 1, minor, "RotationReading");

            qmlRegisterType<QTapSensor>("QtMobility.sensors", 1, minor, "TapSensor");
            qmlRegisterType<QTapReading>("QtMobility.sensors", 1, minor, "TapReading");
        }

        qmlRegisterType<QLightSensor>("QtMobility.sensors", 1, 2, "LightSensor");
        qmlRegisterType<QLightReading>("QtMobility.sensors", 1, 2, "LightReading");

        qmlRegisterType<QGyroscope>("QtMobility.sensors", 1, 2, "Gyroscope");
        qmlRegisterType<QGyroscopeReading>("QtMobility.sensors", 1, 2, "GyroscopeReading");
    }
};

QT_END_NAMESPACE

#include "sensors.moc"

Q_EXPORT_PLUGIN2(qsensorsdeclarativemodule, QT_PREPEND_NAMESPACE(QSensorsDeclarativeModule));

// =====================================================================

/*!
    \qmlclass Accelerometer QAccelerometer
    \brief The Accelerometer element wraps the QAccelerometer class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAccelerometer, QAccelerometerReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass AccelerometerReading QAccelerometerReading
    \brief The AccelerometerReading element wraps the QAccelerometerReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAccelerometer, QAccelerometerReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass AmbientLightSensor QAmbientLightSensor
    \brief The AmbientLightSensor element wraps the QAmbientLightSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAmbientLightSensor, QAmbientLightReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass AmbientLightReading QAmbientLightReading
    \brief The AmbientLightReading element wraps the QAmbientLightReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAmbientLightSensor, QAmbientLightReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass Compass QCompass
    \brief The Compass element wraps the QCompass class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QCompass, QCompassReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass CompassReading QCompassReading
    \brief The CompassReading element wraps the QCompassReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QCompass, QCompassReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass Magnetometer QMagnetometer
    \brief The Magnetometer element wraps the QMagnetometer class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QMagnetometer, QMagnetometerReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass MagnetometerReading QMagnetometerReading
    \brief The MagnetometerReading element wraps the QMagnetometerReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QMagnetometer, QMagnetometerReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass OrientationSensor QOrientationSensor
    \brief The OrientationSensor element wraps the QOrientationSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QOrientationSensor, QOrientationReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass OrientationReading QOrientationReading
    \brief The OrientationReading element wraps the QOrientationReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QOrientationSensor, QOrientationReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass ProximitySensor QProximitySensor
    \brief The ProximitySensor element wraps the QProximitySensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QProximitySensor, QProximityReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass ProximityReading QProximityReading
    \brief The ProximityReading element wraps the QProximityReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QProximitySensor, QProximityReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass RotationSensor QRotationSensor
    \brief The RotationSensor element wraps the QRotationSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QRotationSensor, QRotationReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass RotationReading QRotationReading
    \brief The RotationReading element wraps the QRotationReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QRotationSensor, QRotationReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass TapSensor QTapSensor
    \brief The TapSensor element wraps the QTapSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QTapSensor, QTapReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass TapReading QTapReading
    \brief The TapReading element wraps the QTapReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QTapSensor, QTapReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass LightSensor QLightSensor
    \brief The LightSensor element wraps the QLightSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QLightSensor, QLightReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass LightReading QLightReading
    \brief The LightReading element wraps the QLightReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QLightSensor, QLightReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass Gyroscope QGyroscope
    \brief The Gyroscope element wraps the QGyroscope class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QGyroscope, QGyroscopeReading, {Sensors QML Limitations}
*/

/*!
    \qmlclass GyroscopeReading QGyroscopeReading
    \brief The GyroscopeReading element wraps the QGyroscopeReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QGyroscope, QGyroscopeReading, {Sensors QML Limitations}
*/

