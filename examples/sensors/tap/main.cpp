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
#include <qtapsensor.h>

QTM_USE_NAMESPACE

class TapSensorFilter : public QTapFilter
{
public:
    qtimestamp stamp;
    bool filter(QTapReading *reading)
    {
        int diff = ( reading->timestamp() - stamp );
        stamp = reading->timestamp();
        QString output;
        switch (reading->tapDirection()) {
            case QTapReading::X:         output = "X";         break;
            case QTapReading::Y:         output = "Y";         break;
            case QTapReading::Z:         output = "Z";         break;
            case QTapReading::X_Pos:     output = "X pos";     break;
            case QTapReading::Y_Pos:     output = "Y pos";     break;
            case QTapReading::Z_Pos:     output = "Z pos";     break;
            case QTapReading::X_Neg:     output = "X neg";     break;
            case QTapReading::Y_Neg:     output = "Y neg";     break;
            case QTapReading::Z_Neg:     output = "Z neg";     break;
            case QTapReading::Undefined: output = "Undefined"; break;
            default: output = "Invalid enum value";
        }
        QTextStream out(stdout);
        out << "Tap: ";
        if(reading->isDoubleTap())
            out << "Double ";
        else
            out << "Single ";
        out << QString(" (%1 ms since last, %2 Hz)").arg(diff / 1000, 5).arg( 1000000.0 / diff, 3, 'f', 1) << endl;
        return false; // don't store the reading in the sensor
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QTapSensor sensor;
    if (!sensor.connectToBackend()) {
        qWarning("No tap sensor available!");
        return 1;
    }
    TapSensorFilter filter;
    sensor.addFilter(&filter);
    sensor.start();

    return app.exec();
}
