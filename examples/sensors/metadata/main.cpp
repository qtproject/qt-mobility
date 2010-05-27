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
