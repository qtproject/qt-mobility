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

// =====================================================================

/*!
    \qmlclass Accelerometer QAccelerometer
    \brief The Accelerometer element wraps the QAccelerometer class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAccelerometer, QAccelerometerReading
*/

/*!
    \qmlclass AccelerometerReading QAccelerometerReading
    \brief The AccelerometerReading element wraps the QAccelerometerReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAccelerometer, QAccelerometerReading
*/

/*!
    \qmlclass AmbientLightSensor QAmbientLightSensor
    \brief The AmbientLightSensor element wraps the QAmbientLightSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAmbientLightSensor, QAmbientLightReading
*/

/*!
    \qmlclass AmbientLightReading QAmbientLightReading
    \brief The AmbientLightReading element wraps the QAmbientLightReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QAmbientLightSensor, QAmbientLightReading
*/

/*!
    \qmlclass Compass QCompass
    \brief The Compass element wraps the QCompass class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QCompass, QCompassReading
*/

/*!
    \qmlclass CompassReading QCompassReading
    \brief The CompassReading element wraps the QCompassReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QCompass, QCompassReading
*/

/*!
    \qmlclass Magnetometer QMagnetometer
    \brief The Magnetometer element wraps the QMagnetometer class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QMagnetometer, QMagnetometerReading
*/

/*!
    \qmlclass MagnetometerReading QMagnetometerReading
    \brief The MagnetometerReading element wraps the QMagnetometerReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QMagnetometer, QMagnetometerReading
*/

/*!
    \qmlclass OrientationSensor QOrientationSensor
    \brief The OrientationSensor element wraps the QOrientationSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QOrientationSensor, QOrientationReading
*/

/*!
    \qmlclass OrientationReading QOrientationReading
    \brief The OrientationReading element wraps the QOrientationReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QOrientationSensor, QOrientationReading
*/

/*!
    \qmlclass ProximitySensor QProximitySensor
    \brief The ProximitySensor element wraps the QProximitySensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QProximitySensor, QProximityReading
*/

/*!
    \qmlclass ProximityReading QProximityReading
    \brief The ProximityReading element wraps the QProximityReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QProximitySensor, QProximityReading
*/

/*!
    \qmlclass RotationSensor QRotationSensor
    \brief The RotationSensor element wraps the QRotationSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QRotationSensor, QRotationReading
*/

/*!
    \qmlclass RotationReading QRotationReading
    \brief The RotationReading element wraps the QRotationReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QRotationSensor, QRotationReading
*/

/*!
    \qmlclass TapSensor QTapSensor
    \brief The TapSensor element wraps the QTapSensor class.
    \ingroup qml-sensors_type

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QTapSensor, QTapReading
*/

/*!
    \qmlclass TapReading QTapReading
    \brief The TapReading element wraps the QTapReading class.
    \ingroup qml-sensors_reading

    This element is part of the \bold{QtMobility.sensors 1.1} module.

    \sa QTapSensor, QTapReading
*/

