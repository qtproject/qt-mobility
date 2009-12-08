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

#ifndef QORIENTATIONSENSOR_H
#define QORIENTATIONSENSOR_H

#include <qsensor.h>

QTM_BEGIN_NAMESPACE

class Q_SENSORS_EXPORT QOrientationSensorValue : public QSensorValue
{
public:
    enum OrientationFlag {
        Unknown   = 0x00,
        Portrait  = 0x01,
        Landscape = 0x02,
        Inverted  = 0x04
    };
    Q_DECLARE_FLAGS(Orientation, OrientationFlag)

    Orientation orientation;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QOrientationSensorValue::Orientation)

class Q_SENSORS_EXPORT QOrientationSensor : public QSensor
{
    Q_OBJECT
public:
    explicit QOrientationSensor(QSensor *sensor);
    explicit QOrientationSensor(const QSensorID &id, QObject *parent = 0);

    static const QString type;

    QOrientationSensorValue::Orientation currentOrientation() const
    {
        return static_cast<QOrientationSensorValue*>(currentValue())->orientation;
    }

signals:
    void orientationChanged(QOrientationSensorValue::Orientation orientation);

};

QTM_END_NAMESPACE

#endif

