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
#include <qambientlightsensor.h>

QTM_USE_NAMESPACE

class AmbientLightFilter : public QAmbientLightFilter
{
public:
    qtimestamp stamp;
    bool filter(QAmbientLightReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QString output;
        switch (reading->lightLevel()) {
            case QAmbientLightReading::Dark:      output = "Dark    ";  break;
            case QAmbientLightReading::Twilight:  output = "Twilight";  break;
            case QAmbientLightReading::Light:     output = "Light   ";  break;
            case QAmbientLightReading::Bright:    output = "Bright  ";  break;
            case QAmbientLightReading::Sunny:     output = "Sunny   ";  break;
            case QAmbientLightReading::Undefined: output = "Undefined"; break;
            default: output = "Invalid enum value";
        }
        QTextStream out(stdout);
        out << "Ambient light level: " << output
            << QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 5).arg( 1000000.0 / diff, 3, 'f', 1) << endl;
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QAmbientLightSensor sensor;
    if (!sensor.connectToBackend()) {
        qWarning("No ambient light sensor available!");
        return 1;
    }
    AmbientLightFilter filter;
    sensor.addFilter(&filter);
    sensor.start();

    return app.exec();
}
