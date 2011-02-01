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

#include "meegoaccelerometer.h"
#include "meegoals.h"
#include "meegocompass.h"
#include "meegomagnetometer.h"
#include "meegoorientationsensor.h"
#include "meegoproximitysensor.h"
#include "meegorotationsensor.h"
#include "meegotapsensor.h"
#include "meegogyroscope.h"
#include "meegolightsensor.h"

#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QDebug>
#include <QSettings>

class meegoSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
    
public:
            
    void registerSensors()
    {
        // if no default - no support either, uses Sensors.conf
        QSettings settings(QSettings::SystemScope, QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.beginGroup(QLatin1String("Default"));
        QStringList keys = settings.allKeys();
        for (int i=0,l=keys.size(); i<l; i++){
            QString type = keys.at(i);
            QSensorManager::registerBackend(type.toAscii(), settings.value(type).toByteArray(), this);
        }
        qDebug() << "Loaded the MeeGo sensor plugin";
    }


    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == meegoaccelerometer::id)
            return new meegoaccelerometer(sensor);
        if (sensor->identifier() == meegoals::id)
            return new meegoals(sensor);
        if (sensor->identifier() == meegocompass::id)
            return new meegocompass(sensor);
        if (sensor->identifier() == meegomagnetometer::id)
            return new meegomagnetometer(sensor);
        if (sensor->identifier() == meegoorientationsensor::id)
            return new meegoorientationsensor(sensor);
        if (sensor->identifier() == meegoproximitysensor::id)
            return new meegoproximitysensor(sensor);
        if (sensor->identifier() == meegorotationsensor::id)
            return new meegorotationsensor(sensor);
        if (sensor->identifier() == meegotapsensor::id)
            return new meegotapsensor(sensor);
        if (sensor->identifier() == meegogyroscope::id)
            return new meegogyroscope(sensor);
        if (sensor->identifier() == meegolightsensor::id)
            return new meegolightsensor(sensor);
        return 0;
    }
};

Q_EXPORT_PLUGIN2(libsensors_meego, meegoSensorPlugin)

#include "main.moc"
