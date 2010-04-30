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

#include <QtCore>
#include <qaccelerometer.h>

QTM_USE_NAMESPACE

namespace arrows{
    static const QString ARROW_UP="^", ARROW_DOWN="v", ARROW_LEFT="<",ARROW_RIGHT=">", ARROW_TO=".", ARROW_FROM="x";
}

class AccelerometerFilter : public QAccelerometerFilter
{
public:

    bool filter(QAccelerometerReading *reading)
    {
        QString arrow = getArrowKey(reading->x(), reading->y(), reading->z());
        if (arrow!=exArrow){
            qDebug() << "direction:" << arrow;
            exArrow = arrow;
        }
        return false; // don't store the reading in the sensor
    }

private:
    QString exArrow;
    static const qreal THRESHOLD = 0.3;

    QString getArrowKey(qreal x, qreal y, qreal z){

        // axis_z: TO or FROM
        if (abs(y)<THRESHOLD && abs(x)<THRESHOLD) return z>0?(arrows::ARROW_FROM):(arrows::ARROW_TO);

        // axis_x: LEFT or RIGHT
        if (abs(x)>abs(y)) return x>0?(arrows::ARROW_LEFT):(arrows::ARROW_RIGHT);

        // axis_y: UP or DOWN
        return y>0?(arrows::ARROW_DOWN):(arrows::ARROW_UP);
    }

    static qreal abs(qreal value) {return value<0?-value:value;}
};


int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QAccelerometer sensor;
    if (!sensor.connectToBackend()) {
        qWarning("No Accelerometer available!");
        return 1;
    }
    AccelerometerFilter filter;
    sensor.addFilter(&filter);
    sensor.start();

    return app.exec();
}
