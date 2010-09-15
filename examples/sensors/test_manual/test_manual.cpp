/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QObject>
#include <QTest>
#include <QDebug>
#include <QSettings>
#include <iostream>


#include <qsensor.h>
#include <qorientationsensor.h>
#include <qaccelerometer.h>
#include <qrotationsensor.h>
#include <qmagnetometer.h>
#include <qcompass.h>
#include "sensorslotclass.h"

QTM_USE_NAMESPACE

        static QOrientationReading::Orientation o2;
static QOrientationSensor orientationSensor;
static QAccelerometer accelerometer;
static QRotationSensor rotationSensor;
static QAccelerometerReading* acceReading;
static QRotationReading* rotReading;
static QMagnetometer magnetometer;
static QMagnetometerReading* maggeReading;
static QCompass compass;
static QCompassReading* compassReading;


static QString dataRateString;
static QString dataRangeString;
static QString resolutionString;
static int counter;
static bool isStationary = false;

///////////////////////////////////////////
//cpp file
///////////////////////////////////////////


SensorSlotClass::SensorSlotClass()
{
    int rateMax =0;
    connect(&orientationSensor, SIGNAL(readingChanged()), this, SLOT(slotOrientationData()));
    if (orientationSensor.availableDataRates().size()>0){
        rateMax = orientationSensor.availableDataRates().at(0).second;
        orientationSensor.setDataRate(rateMax);
    }
    orientationSensor.start();
    connect(&accelerometer, SIGNAL(readingChanged()), this, SLOT(slotAccelerationData()));
    if (accelerometer.availableDataRates().size()>0){
        rateMax = accelerometer.availableDataRates().at(0).second;
        accelerometer.setDataRate(rateMax);
    }
    accelerometer.start();
    connect(&rotationSensor, SIGNAL(readingChanged()), this, SLOT(slotRotationData()));
    if (rotationSensor.availableDataRates().size()>0){
        rateMax = rotationSensor.availableDataRates().at(0).second;
        rotationSensor.setDataRate(rateMax);
    }
    rotationSensor.start();
    magnetometer.setProperty("returnGeoValues", true);
    connect(&magnetometer, SIGNAL(readingChanged()), this, SLOT(slotMagnetometerData()));
    if (magnetometer.availableDataRates().size()>0){
        rateMax = magnetometer.availableDataRates().at(0).second;
        magnetometer.setDataRate(rateMax);
    }
    magnetometer.start();

    connect(&compass, SIGNAL(readingChanged()), this, SLOT(slotCompassData()));
    if (compass.availableDataRates().size()>0){
        rateMax = compass.availableDataRates().at(0).second;
        compass.setDataRate(rateMax);
    }
    compass.start();

    m_x=0; m_y=0; m_z=0;
}

SensorSlotClass::~SensorSlotClass(){
    disconnect(&compass);
    compass.stop();

    disconnect(&orientationSensor);
    orientationSensor.stop();

    disconnect(&rotationSensor);
    rotationSensor.stop();

    disconnect(&magnetometer);
    magnetometer.stop();

    disconnect(&accelerometer);
    accelerometer.stop();

}


void SensorSlotClass::slotOrientationData(){
    o2 = orientationSensor.reading()->orientation();
    checkRange(&orientationSensor, o2);
    checkRate(&orientationSensor, m_orientationTimestamp);
}


void SensorSlotClass::slotAccelerationData(){

    acceReading = accelerometer.reading();
    checkRange(&accelerometer, acceReading->x());
    checkRange(&accelerometer, acceReading->y());
    checkRange(&accelerometer, acceReading->z());

    checkRate(&accelerometer, m_accelerometerTimestamp);

    if (isStationary) checkResolution(&accelerometer, acceReading);

}

void SensorSlotClass::slotRotationData(){
    rotReading = rotationSensor.reading();
    checkRange(&rotationSensor, rotReading->x());
    checkRange(&rotationSensor, rotReading->y());
    checkRange(&rotationSensor, rotReading->z());

    checkRate(&rotationSensor, m_rotationTimestamp);
}

void SensorSlotClass::slotMagnetometerData(){

    maggeReading = magnetometer.reading();
    checkRange(&magnetometer, maggeReading->x());
    checkRange(&magnetometer, maggeReading->y());
    checkRange(&magnetometer, maggeReading->z());

    checkRate(&magnetometer, m_magnetometerTimestamp);

}

void SensorSlotClass::slotCompassData(){
    compassReading = compass.reading();
    checkRange(&compass, (qreal)compassReading->azimuth());

    checkRate(&compass, m_compassTimestamp);
}


void SensorSlotClass::checkRange(QSensor* sensor, qreal value){
    qreal min = sensor->outputRanges().at(sensor->outputRange()).minimum;
    qreal max = sensor->outputRanges().at(sensor->outputRange()).maximum;

    if (min>value || value> max){
        QString num;
        dataRangeString.append(sensor->type());
        dataRangeString.append(": range=[");
        dataRangeString.append(num.setNum(min));
        dataRangeString.append(",");
        dataRangeString.append(num.setNum(max));
        dataRangeString.append("], value =");
        dataRangeString.append(num.setNum(value));
        dataRangeString.append("\n");
    }

}


void SensorSlotClass::checkRate(QSensor* sensor, int &exTimestamp){
    int timestamp = sensor->reading()->timestamp();
    int diff = timestamp - exTimestamp;
    int rate = sensor->dataRate();
    if (rate==0) return;
    if (diff < (1000/rate)*0.9){
        dataRateString.append(sensor->type());
        dataRateString.append(": rate=");
        dataRateString.append(sensor->dataRate());
        dataRateString.append(", measured rate=");
        dataRateString.append(1000/diff);
        dataRateString.append("\n");
    }

    exTimestamp = timestamp;
}


void SensorSlotClass::checkResolution(QSensor* sensor, QAccelerometerReading* reading){
    qreal x = reading->x();
    qreal y = reading->y();
    qreal z = reading->z();


    qreal resolution = sensor->outputRanges().at(sensor->outputRange()).accuracy;

    if (m_x!=0){
        qreal diff = qAbs(x-m_x);
        checkDiff(diff, resolution, ": x resolution=");
        diff = qAbs(y-m_y);
        checkDiff(diff, resolution, ": y resolution=");
        diff = qAbs(z-m_z);
        checkDiff(diff, resolution, ": z resolution=");
    }
    m_x = x;
    m_y = y;
    m_z = z;

}

void SensorSlotClass::checkDiff(qreal diff, qreal resolution, QString msg){
    if (diff==0) return;
    qreal resolutionMax = 3*resolution;


    if (qAbs(diff)<resolution || qAbs(diff)>resolutionMax){
        QString num;
        resolutionString.append("\n");
        resolutionString.append("accelerometer:");
        resolutionString.append(msg);
        num.setNum(resolution);
        resolutionString.append(num);
        resolutionString.append("!=");
        num.setNum(qAbs(diff));
        resolutionString.append(num);
    }

}



namespace tests{

    static QOrientationReading::Orientation orientation[] = {
        QOrientationReading::TopUp,
        QOrientationReading::TopDown,
        QOrientationReading::LeftUp,
        QOrientationReading::RightUp,
        QOrientationReading::FaceUp,
        QOrientationReading::FaceDown
    };


    static const float GRAVITY_EARTH = 9.80665;
    static const float THRESHOLD = GRAVITY_EARTH/4;

    static void pressAnyKey()
    {
        std::string value;
        qDebug()<< "Press any key: \n";
        std::getline(std::cin, value);
    }


    static QString debugMessage(QString title, qreal expected, qreal measured, qreal threshold){
        QString msg;
        msg.append(title);
        QString num;
        num.setNum(measured);
        msg.append(num);
        msg.append(" was not in range [");
        num.setNum(expected - threshold);
        msg.append(num);
        msg.append(",");
        num.setNum(expected + threshold);
        msg.append(num);
        msg.append("]");
        msg.append("\n");
        return msg;
    }


    static void drawDevice(QOrientationReading::Orientation orientation){
        switch (orientation) {
        case QOrientationReading::TopUp:
            std::cout<<"    ___________\n";
            std::cout<<"    | _______ |\n";
            std::cout<<"    | |     | |\n";
            std::cout<<"    | |     | |\n";
            std::cout<<"    | |_____| |\n";
            std::cout<<"    | _______ |\n";
            std::cout<<"    | |_|_|_| |\n";
            std::cout<<"    | |_|_|_| |\n";
            std::cout<<"    | |_|_|_| |\n";
            std::cout<<"    |_________|\n";
            std::cout<< "Top up \n"; break;
            break;
        case QOrientationReading::TopDown:
            std::cout<<"     _________ \n";
            std::cout<<"    | _______ |\n";
            std::cout<<"    | |_|_|_| |\n";
            std::cout<<"    | |_|_|_| |\n";
            std::cout<<"    | |_|_|_| |\n";
            std::cout<<"    | _______ |\n";
            std::cout<<"    | |     | |\n";
            std::cout<<"    | |     | |\n";
            std::cout<<"    | |_____| |\n";
            std::cout<<"    |_________|\n";
            std::cout<<"Top down \n"; break;
        case QOrientationReading::LeftUp:
            std::cout<<"     _________________\n";
            std::cout<<"    | _______ ______  |\n";
            std::cout<<"    | |_|_|_| |     | |\n";
            std::cout<<"    | |_|_|_| |     | |\n";
            std::cout<<"    | |_|_|_| |_____| |\n";
            std::cout<<"    |_________________|\n";
            std::cout<<"Left up  \n"; break;
        case QOrientationReading::RightUp:
            std::cout<<"     _________________\n";
            std::cout<<"    | _______ ______  |\n";
            std::cout<<"    | |     | |_|_|_| |\n";
            std::cout<<"    | |     | |_|_|_| |\n";
            std::cout<<"    | |_____| |_|_|_| |\n";
            std::cout<<"    |_________________|\n";
            std::cout<<"Right up \n"; break;
        case QOrientationReading::FaceUp:
            std::cout<<"     _________\n";
            std::cout<<"    |  ' ' '  |\n";
            std::cout<<"    |_________|\n";
            std::cout<<"Face up  \n"; break;
        case QOrientationReading::FaceDown:
            std::cout<<"     _________\n";
            std::cout<<"    |         |\n";
            std::cout<<"    |__._._.__|\n";
            std::cout<<"Face down\n"; break;
        case QOrientationReading::Undefined: std::cout<<"Undefined\n"; break;
        default: std::cout<<"Invalid enum value\n";
        }
    }


    static const char* orientationAsString(QOrientationReading::Orientation orientation){
        switch (orientation) {
        case QOrientationReading::TopUp:     return "Top up   ";
        case QOrientationReading::TopDown:   return "Top down ";
        case QOrientationReading::LeftUp:    return  "Left up  ";
        case QOrientationReading::RightUp:   return "Right up ";
        case QOrientationReading::FaceUp:    return "Face up  ";
        case QOrientationReading::FaceDown:  return "Face down";
        case QOrientationReading::Undefined: return "Undefined";
        default: return "Invalid enum value";
        }
    }


    static qreal acce_array[6][3]=
    {
        {0, tests::GRAVITY_EARTH, 0},
        {0, -tests::GRAVITY_EARTH, 0},
        {-tests::GRAVITY_EARTH, 0, 0},
        {tests::GRAVITY_EARTH, 0, 0},
        {0, 0, tests::GRAVITY_EARTH},
        {0, 0, -tests::GRAVITY_EARTH}};


    static qreal rot_array[6][2]=
    {
        {90,0},
        {-90, 0},
        {0,90},
        {0,-90},
        {0, 0},
        {0, 180}};

    static const qreal ROT_THRESHOLD = 20;


    static bool confirm()
    {
        std::string value;
        while (true){
            qDebug()<< "Answer(y/N):";
            std::getline(std::cin, value);
            if (value.compare("Y")==0 || value.compare("y")==0 ) return true;
            if (value.compare("N")==0 || value.compare("n")==0 || value.compare("")==0 || value.compare(" ")==0 || value.compare("\n")==0) return false;
        }
    }

    static bool testMagge(qreal val1, qreal val2, qreal threshold, QString &msg){
        bool isOk = true;

        if (val1+val2>threshold){
            isOk = false;
            QString num;
            msg.append(num.setNum(val1));
            msg.append("!=");
            msg.append(num.setNum(val2));
            msg.append("\n");
        }
        return isOk;
    }



    static bool testMagge2(qreal valBig, qreal valSmall1, qreal valSmall2, QString &msg){
        bool isOk = true;
        if (qAbs(valBig)< qAbs(valSmall1) || qAbs(valBig)< qAbs(valSmall2)){
            isOk = false;
            QString num;
            msg.append(num.setNum(valBig));
            msg.append(",");
            msg.append(num.setNum(valSmall1));
            msg.append(",");
            msg.append(num.setNum(valSmall2));
            msg.append("\n");
        }
        return isOk;
    }


    static bool testCompass(qreal val1, qreal val2, QString &msg){
        bool isOk = true;
        int threshold = 30;


        if (qAbs(val1 - val2)> threshold){
            if (val1>360-threshold)
                if ((360 - val1 + val2)< threshold) return isOk;
            if (val2>360-threshold)
                if ((360 - val2 + val1)< threshold) return isOk;
            isOk = false;
            msg.append("Compass failure:");
            QString num;
            msg.append(num.setNum(val1));
            msg.append("!=");
            msg.append(num.setNum(val2));
            msg.append("\n");
        }
        return isOk;
    }


    static bool testRot(qreal azimuth, qreal rotZ, QString &msg){
        bool isOk = true;


        qreal tmpAzimuth = rotZ>0 ? 360 -rotZ : -rotZ;
        int threshold = 30;


        if (qAbs(azimuth - tmpAzimuth)> threshold){
            if (tmpAzimuth> 360-threshold)
                if ((360 - tmpAzimuth + azimuth)< threshold) return isOk;
            if (azimuth> 360-threshold)
                if ((360 - azimuth + tmpAzimuth)< threshold) return isOk;

            isOk=false;
            msg.append("Rot/compass comparison failed: ");
            QString num;
            msg.append(num.setNum(azimuth));
            msg.append("!=");
            msg.append(num.setNum(tmpAzimuth));
            msg.append("\n");
        }

        return isOk;
    }



}


class test_manual: public QObject{
    Q_OBJECT
    private slots:
    void testOrientation();
    void testDataRate();
    void testOutputRange();
    void testResolution();
};


void test_manual::testOrientation()

{

    SensorSlotClass slotClass;

    bool hasZ = rotationSensor.property("hasZ").toBool();

    int j=0,k=0,l=0;


    qreal magge_x[6];
    qreal magge_y[6];
    qreal magge_z[6];
    qreal rot_x[6];
    qreal rot_y[6];
    qreal rot_z[6];
    qreal compass_azimuth[6];
    qreal compass_level[6];

    QString tmp("\n");


    for (; counter<7; counter++){
        qDebug()<<"Put the device in following position, try to preserve compass direction:\n";
        if (counter==4) qDebug()<<"Put the device on the table on stationary position!:\n";
        QOrientationReading::Orientation o1 = tests::orientation[counter%6];
        tests::drawDevice(o1);
        tests::pressAnyKey();
        QTest::qWait(50);   // DO NOT REMOVE - does not work without this!

        if (counter==0) continue;     //prevent from first UNDEFINED VALUE


        if (o1!=o2){
            j++;
            tmp.append("Orientation: expected/measured: \n");
            tmp.append(tests::orientationAsString(o1));
            tmp.append("!=");
            tmp.append(tests::orientationAsString(o2));
            tmp.append("\n");
        }


        //acceleration
        qreal tmpX = tests::acce_array[counter%6][0];
        qreal tmpY = tests::acce_array[counter%6][1];
        qreal tmpZ = tests::acce_array[counter%6][2];
        qreal x = acceReading->x();
        qreal y = acceReading->y();
        qreal z = acceReading->z();

        QString accTmp;
        int tmpK = k;
        if (qAbs(x-tmpX)>tests::THRESHOLD){
            k++;
            accTmp.append(tests::debugMessage("Acceleration X: ",tmpX, x, tests::THRESHOLD));
        }

        if (qAbs(y-tmpY)>tests::THRESHOLD){
            k++;
            accTmp.append(tests::debugMessage("Acceleration Y: ",tmpY, y, tests::THRESHOLD));
        }

        if (qAbs(z-tmpZ)>tests::THRESHOLD){
            k++;
            accTmp.append(tests::debugMessage("Acceleration Z: ",tmpZ, z, tests::THRESHOLD));
        }
        if (k>tmpK){
            tmp.append("Acceleration failure when in ");
            tmp.append(tests::orientationAsString(o2));
            tmp.append("\n");
            tmp.append(accTmp);
        }

        //rotation
        QString rotTmp;
        tmpX = tests::rot_array[counter%6][0];
        tmpY = tests::rot_array[counter%6][1];
        x = rotReading->x();
        y = rotReading->y();
        if (hasZ){
            rot_x[counter-1]=rotReading->x();
            rot_y[counter-1]=rotReading->y();
            rot_z[counter-1]=rotReading->z();
        }
        int tmpL = l;
        // not as straight-forward
        if (qAbs(x-tmpX)>tests::ROT_THRESHOLD){
            l++;
            rotTmp.append(tests::debugMessage("Rotation X: ", tmpX, x, tests::ROT_THRESHOLD));
        }

        if (tmpY<y-tests::ROT_THRESHOLD || tmpY>y+tests::ROT_THRESHOLD){
            l++;
            rotTmp.append(tests::debugMessage("Rotation Y: ", tmpY, y, tests::ROT_THRESHOLD));
        }
        if (l>tmpL){
            tmp.append("Rotation failure when in ");
            tmp.append(tests::orientationAsString(o2));
            tmp.append("\n");
            tmp.append(rotTmp);
        }


        //magnetometer
        magge_x[counter-1]=maggeReading->x();
        magge_y[counter-1]=maggeReading->y();
        magge_z[counter-1]=maggeReading->z();

        //        qDebug()<<" magge x="<<magge_x[i-1]<<", y="<<magge_y[i-1]<<", z="<<magge_z[i-1];

        //compass
        compass_azimuth[counter-1] = compassReading->azimuth();
        compass_level[counter-1] = compassReading->calibrationLevel();
        //        qDebug()<<" compass = "<< compass_azimuth[i-1];
        //        if (hasZ){
        //            qDebug()<<" rotation = x "<<rot_x[i-1];
        //            qDebug()<<" rotation = y "<<rot_y[i-1];
        //            qDebug()<<" rotation = z "<<rot_z[i-1];
        //        }


        if (counter==4){
            isStationary = true;
            qDebug()<<"Keep the device on the table on stationary position!:\n";
            qDebug()<<"Test continues after a while";            
            QTest::qWait(2000);   // DO NOT REMOVE - does not work without this!
            isStationary = false;
        }
    }


    //magnetometer: same magnitude, opposite values
    // x-axis comparison, 1 and 2
    // y-axis comparison, 0 and 5
    // z-axis comparison, 3 and 4
    int rangeIndex = magnetometer.outputRange();
    qreal threshold = (magnetometer.outputRanges().at(rangeIndex).maximum - magnetometer.outputRanges().at(rangeIndex).minimum)/10;

    QString msg;
    if (!tests::testMagge(magge_x[1], magge_x[2], threshold, msg)){
        tmp.append("Magnetometer failure (x-axis):");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge(magge_y[0], magge_y[5], threshold, msg)){
        tmp.append("Magnetometer failure (y-axis):");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge(magge_z[3], magge_z[4], threshold, msg)){
        tmp.append("Magnetometer failure (z-axis):");
        tmp.append(msg);
        msg.clear();
        j++;
    }

    //magnetometer: one big value, two small values
    // x-axis comparison, 1 and 2
    // y-axis comparison, 0 and 5
    // z-axis comparison, 3 and 4
    if (!tests::testMagge2(magge_x[1], magge_y[1], magge_z[1], msg)){
        tmp.append("Magnetometer x: not bigger than y, z :");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge2(magge_x[2], magge_y[2], magge_z[2], msg)){
        tmp.append("Magnetometer x: not bigger than y, z :");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge2(magge_y[0], magge_x[0], magge_z[0], msg)){
        tmp.append("Magnetometer y: not bigger than x, z :");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge2(magge_y[5], magge_x[5], magge_z[5], msg)){
        tmp.append("Magnetometer y: not bigger than x, z :");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge2(magge_z[3], magge_x[3], magge_y[3], msg)){
        tmp.append("Magnetometer z: not bigger than x, y :");
        tmp.append(msg);
        msg.clear();
        j++;
    }
    if (!tests::testMagge2(magge_z[4], magge_x[4], magge_y[4], msg)){
        tmp.append("Magnetometer z: not bigger than x, y :");
        tmp.append(msg);
        msg.clear();
        j++;
    }

    //rotation
    if (hasZ){
        qDebug()<<"Is magnetic north pole rotation sensor's external entity";
        if (tests::confirm()){
            for (int i=0; i<6; i++){
                if (!(tests::testRot(compass_azimuth[i], rot_z[i], msg))){
                    tmp.append(msg);
                    msg.clear();
                }
            }
        }
    }

    // compass: 1&2, 3&4
    // azimuth will be projected to horizontal plane
    // top up & top down (5&0): too much variation, NOT tested
    // left up & right up (1&2): 180 diff
    // face up & face down (3&4): should be the same
    // take calibration level into account
    int index[6]={1,2,3,4};

    for (int i=0; i<3; i++){
        if (compass_level[index[i*2]]< 0.1 || compass_level[index[i*2+1]]< 0.1)
            continue;

        qreal val1 = compass_azimuth[index[i*2]];
        qreal val2 = compass_azimuth[index[i*2+1]];
        //        QString num;
        //        qDebug()<<" Compass testing val1 = "<<num.setNum(val1)<< " val2 = "<<num.setNum(val2);

        if (i==0){
            val1 = ((int)val1+180)%360;
        }

        if (!(tests::testCompass(val1, val2, msg))){
            tmp.append(msg);
            msg.clear();
            j++;
        }
    }

    qDebug()<< "Do you have a real compass in use: \n";
    bool isYes = tests::confirm();
    if (isYes){
        qDebug()<<"Put the device in following position, and head it towards compass north:\n";
        QOrientationReading::Orientation o1 = tests::orientation[4];
        tests::drawDevice(o1);
        tests::pressAnyKey();
        QTest::qWait(50);   // DO NOT REMOVE - does not work without this!
        qreal azimuth = compassReading->azimuth();

        if (azimuth>20 && azimuth<340){
            j++;
            tmp.append("Instead of being 0, compass north gives ");
            QString num;
            num.setNum(azimuth);
            tmp.append(num);
            tmp.append("\n");
        }
    }



    QVERIFY2(tmp.size()<2, tmp.toLatin1().data());
}


void test_manual::testDataRate(){
    QVERIFY2(dataRateString.size()==0, dataRateString.toLatin1().data());
    dataRateString.clear();
}

void test_manual::testOutputRange(){
    QVERIFY2(dataRangeString.size()==0, dataRangeString.toLatin1().data());
    dataRangeString.clear();
}

void test_manual::testResolution(){
    QVERIFY2(resolutionString.size()==0, resolutionString.toLatin1().data());
    resolutionString.clear();
}



QTEST_MAIN(test_manual)

#include "test_manual.moc"
