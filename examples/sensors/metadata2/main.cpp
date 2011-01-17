/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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
#include <QFile>
#include <QDebug>

QTM_USE_NAMESPACE

const char* const MAX_BUFFER_SIZE = "maxBufferSize";
const char* const EFFICIENT_BUFFER_SIZE = "efficientBufferSize";



QString checkSensor( QSensor *sensor, bool isDefault)
{
    qrangelist sen_datarates = sensor->availableDataRates();
    QString sen_desc('"');
    sen_desc.append(sensor->description());
    sen_desc.append('"');
    QByteArray sen_ident = sensor->identifier();
    qoutputrangelist sen_outRList = sensor->outputRanges();
    QByteArray sen_type = sensor->type();


    QString datarates;

    for( int i = 0; i < sen_datarates.size(); ++i )
    {
        datarates.append("[");
        QString num;
        datarates.append(num.setNum(sen_datarates[i].first));
        datarates.append("..");
        datarates.append(num.setNum(sen_datarates[i].second));
        datarates.append("] ");
    }


    QString outputRanges;
    for( int i = 0; i < sen_outRList.size(); ++i )
    {
        outputRanges.append("[");
        QString num;
        outputRanges.append(num.setNum(sen_outRList[i].minimum));
        outputRanges.append("..");
        outputRanges.append(num.setNum(sen_outRList[i].maximum));
        outputRanges.append(";");
        outputRanges.append(num.setNum(sen_outRList[i].accuracy));
        outputRanges.append("] ");
    }

    // bufferSizes
    QVariant maxVariant = sensor->property(MAX_BUFFER_SIZE);
    QString bufferSizes("[1..");
    bufferSizes.append(QString::number(maxVariant.isValid()?maxVariant.toInt():1));
    bufferSizes.append("],");
    QVariant efficientVariant = sensor->property(EFFICIENT_BUFFER_SIZE);
    bufferSizes.append(QString::number(efficientVariant.isValid()?efficientVariant.toInt():1));

    QString metadata(sen_ident);
    if (isDefault) metadata.append("- default");
    metadata.append(",");
    metadata.append(sen_type);
    metadata.append(",");
    metadata.append(sen_desc);
    metadata.append(",");
    metadata.append(outputRanges);
    metadata.append(",");
    metadata.append(datarates);
    metadata.append(",");
    metadata.append(bufferSizes);
    metadata.append("\n");


    return metadata;
}

int main( int argc, char **argv )
{
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    QString fileName = args.size()>1?args.at(1):"metadata.csv";
    QFile file(fileName);

    bool result = true;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return result;

    QTextStream out(&file);

    out <<"Identifier,Type,Description,OutputRanges,DataRates,BufferSizes,EfficientBufferSize"<<endl;

    QList<QByteArray> types = QSensor::sensorTypes();
    for (int j=0, l= types.size();j<l; j++ ){

        const QByteArray type = types.at(j);
        QList<QByteArray> ids = QSensor::sensorsForType(type);
        

        QByteArray defaultSensor = QSensor::defaultSensorForType(type);
        
        for( int i = 0, ll=ids.size(); i < ll; ++i )
        {
            QSensor sensor(type);
            QByteArray id = ids.at(i);
            sensor.setIdentifier(id);
            if( ! sensor.connectToBackend() )
            {
                qDebug() << "connectToBackend failed" << id <<endl;
                result = false;
            } else {
                out << checkSensor(&sensor, id==defaultSensor);
            }
        }
    }

    return result;
}
