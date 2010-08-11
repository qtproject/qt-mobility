/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
    static const qreal THRESHOLD;

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

const qreal AccelerometerFilter::THRESHOLD = 0.3;

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
