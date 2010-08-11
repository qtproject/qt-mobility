/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// QT Mobility Sensor API headers
#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>

// Internal Headers
#include "proximitysensorsym.h"
#include "ambientlightsensorsym.h"
#include "magnetometersensorsym.h"
#include "compasssym.h"
#include "orientationsym.h"
#include "accelerometersym.h"
#include "rotationsensorsym.h"
#include "tapsensorsym.h"

// QT Utility headers
#include <QDebug>

class SensorPluginSym : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
    {
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
        {
        qDebug() << "Loaded the symbian sensor plugins";
        QSensorManager::registerBackend(QProximitySensor::type, CProximitySensorSym::id, this);
        QSensorManager::registerBackend(QAmbientLightSensor::type, CAmbientLightSensorSym::id, this);
        QSensorManager::registerBackend(QMagnetometer::type, CMagnetometerSensorSym::id, this);
        QSensorManager::registerBackend(QCompass::type, CCompassSym::id, this);
        QSensorManager::registerBackend(QOrientationSensor::type, COrientationSensorSym::id, this);
        QSensorManager::registerBackend(QAccelerometer::type, CAccelerometerSensorSym::id, this);
        QSensorManager::registerBackend(QRotationSensor::type, CRotationSensorSym::id, this);
        QSensorManager::registerBackend(QTapSensor::type, CTapSensorSym::id, this);
        }

    QSensorBackend *createBackend(QSensor *sensor)
        {
        if (sensor->identifier() == CProximitySensorSym::id)
            {
            CProximitySensorSym *self = NULL;
            TRAPD(err,self = CProximitySensorSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == CAmbientLightSensorSym::id)
            {
            CAmbientLightSensorSym *self = NULL;
            TRAPD(err,self = CAmbientLightSensorSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == CMagnetometerSensorSym::id)
            {
            CMagnetometerSensorSym *self = NULL;
            TRAPD(err,self = CMagnetometerSensorSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == CCompassSym::id)
            {
            CCompassSym *self = NULL;
            TRAPD(err,self = CCompassSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == COrientationSensorSym::id)
            {
            COrientationSensorSym *self = NULL;
            TRAPD(err,self = COrientationSensorSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == CAccelerometerSensorSym::id)
            {
            CAccelerometerSensorSym *self = NULL;
            TRAPD(err,self = CAccelerometerSensorSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == CRotationSensorSym::id)
            {
            CRotationSensorSym *self = NULL;
            TRAPD(err,self = CRotationSensorSym::NewL(sensor));
            return self;
            }
        if (sensor->identifier() == CTapSensorSym::id)
            {
            CTapSensorSym *self = NULL;
            TRAPD(err,self = CTapSensorSym::NewL(sensor));
            return self;
            }
        return 0;
        }
    };

Q_EXPORT_PLUGIN2(qtsensors_sym, SensorPluginSym);

#include "main.moc"

