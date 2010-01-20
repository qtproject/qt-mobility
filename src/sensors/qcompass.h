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

#ifndef QCOMPASS_H
#define QCOMPASS_H

#include <qsensor.h>

QTM_BEGIN_NAMESPACE

class QCompassReadingPrivate;

class Q_SENSORS_EXPORT QCompassReading : public QSensorReading
{
    Q_OBJECT
    DECLARE_READING(QCompassReading)
public:
    enum CalibrationLevel {
        Undefined = 0,
        Low       = 1,
        Middle    = 2,
        High      = 3
    };
    Q_ENUMS(CalibrationLevel)

    Q_PROPERTY(qreal azimuth READ azimuth WRITE setAzimuth)
    qreal azimuth() const;
    void setAzimuth(qreal azimuth);

    Q_PROPERTY(CalibrationLevel calibrationLevel READ calibrationLevel WRITE setCalibrationLevel)
    CalibrationLevel calibrationLevel() const;
    void setCalibrationLevel(CalibrationLevel calibrationLevel);
};

DECLARE_FILTER(QCompassFilter, QCompassReading);

class Q_SENSORS_EXPORT QCompass : public QSensor
{
    Q_OBJECT
    DECLARE_SENSOR(QCompass, QCompassFilter, QCompassReading)
};

QTM_END_NAMESPACE

#endif

