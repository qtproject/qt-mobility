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
#include <qambientlightsensor.h>
#include <qcompass.h>
#include <qmagnetometer.h>
#include <qorientationsensor.h>
#include <qproximitysensor.h>
#include <qrotationsensor.h>
#include <qtapsensor.h>

QTM_USE_NAMESPACE

bool checkSensor( QSensor *sensor )
{
    QTextStream out(stdout);
    bool conclusion = true;
    out << endl << "--- Metadata ---" << endl;

    // Get readings from functions

    // void     addFilter ( QSensorFilter * filter )
    qrangelist sen_datarates = sensor->availableDataRates();
    // bool     connectToBackend ()
    int sen_datarate = sensor->dataRate();
    QString sen_desc = sensor->description();
    int sen_error = sensor->error();
    QByteArray sen_ident = sensor->identifier();
    bool sen_active = sensor->isActive();
    bool sen_busy = sensor->isBusy();
    bool sen_isConn = sensor->isConnectedToBackend();
    int sen_outRange = sensor->outputRange();
    qoutputrangelist sen_outRList = sensor->outputRanges();
    QSensorReading *sen_reading = sensor->reading();
    // void 	removeFilter ( QSensorFilter * filter )
    // void 	setDataRate ( int rate )
    // void 	setIdentifier ( const QByteArray & identifier )
    // void 	setOutputRange ( int index )
    QByteArray sen_type = sensor->type();

    // Print findings

    out << "Identifier: " << sen_ident
        << " Type: " << sen_type << endl;
    out << "Description: " << sen_desc << endl;
    out << "Active: " << sen_active
        << " Busy: " << sen_busy
        << " Connected: " << sen_isConn
        << " Last error: " << sen_error << endl;

    out << "Using datarate: " << sen_datarate
        << " Out of:";
    for( int i = 0; i < sen_datarates.size(); ++i )
    {
        out << " [" << sen_datarates[i].first << "," << sen_datarates[i].second << "]";
    }
    out << endl;

    out << "Using output range: " << sen_outRange
        << " Out of (min,max,accuracy):";
    for( int i = 0; i < sen_outRList.size(); ++i )
    {
        out << " (" << sen_outRList[i].minimum << "," << sen_outRList[i].maximum << "," << sen_outRList[i].accuracy << ")";
    }
    out << endl;

    //out << sen_reading << endl; //QSensorReading *

    return conclusion;
}

int main( int argc, char **argv )
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);
    bool result = true;

    QSensor *sensors[8];
    sensors[0] = new QAccelerometer;
    sensors[1] = new QAmbientLightSensor;
    sensors[2] = new QCompass;
    sensors[3] = new QMagnetometer;
    sensors[4] = new QOrientationSensor;
    sensors[5] = new QProximitySensor;
    sensors[6] = new QRotationSensor;
    sensors[7] = new QTapSensor;

    for( int i = 0; i < 8; ++i )
    {
        if( ! sensors[i] )
        {
            out << "sensor number " << i << " unavailable" << endl;
            continue;
        }
        if( ! sensors[i]->connectToBackend() )
        {
            out << "connectToBackend failed" << endl;
            result = false;
        } else {
            if( ! checkSensor( sensors[i] ) )
                result = false;
        }
    }

    return result;
}
