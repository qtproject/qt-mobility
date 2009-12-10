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

#ifndef QROTATIONSENSOR_H
#define QROTATIONSENSOR_H

#include <qsensor.h>
#include <QtGlobal>

QTM_BEGIN_NAMESPACE

class Q_SENSORS_EXPORT QRotationValue : public QSensorValue
{
public:
    qreal x;
    qreal y;
    qreal z;
};


class Q_SENSORS_EXPORT QRotationSensor : public QSensor
{
public:
    explicit QRotationSensor(const QSensorId &id, QObject *parent = 0);

    static const QString TYPE;
    QString type() const { return TYPE; };

    qreal currentXRotation() const
    {
        return static_cast<QRotationValue*>(currentValue())->x;
    }
    qreal currentYRotation() const
    {
        return static_cast<QRotationValue*>(currentValue())->y;
    }
    qreal currentZRotation() const
    {
        return static_cast<QRotationValue*>(currentValue())->z;
    }
};

QTM_END_NAMESPACE

#endif

