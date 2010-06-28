/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

///////////////////////////////////////////
//cpp file
///////////////////////////////////////////


SensorSlotClass::SensorSlotClass()
{
    connect(&orientationSensor, SIGNAL(readingChanged()), this, SLOT(slotOrientationData()));
    orientationSensor.start();

    connect(&accelerometer, SIGNAL(readingChanged()), this, SLOT(slotAccelerationData()));
    accelerometer.start();

    connect(&rotationSensor, SIGNAL(readingChanged()), this, SLOT(slotRotationData()));
    rotationSensor.start();

    magnetometer.setProperty("returnGeoValues", true);
    connect(&magnetometer, SIGNAL(readingChanged()), this, SLOT(slotMagnetometerData()));
    magnetometer.start();

    connect(&compass, SIGNAL(readingChanged()), this, SLOT(slotCompassData()));
    compass.start();
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
}


void SensorSlotClass::slotAccelerationData(){
    acceReading = accelerometer.reading();
}

void SensorSlotClass::slotRotationData(){
    rotReading = rotationSensor.reading();
}

void SensorSlotClass::slotMagnetometerData(){
    maggeReading = magnetometer.reading();
}


void SensorSlotClass::slotCompassData(){
    compassReading = compass.reading();
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

    for (int i=0; i<7; i++){
        qDebug()<<"Put the device in following position, try to preserve compass direction:\n";
        QOrientationReading::Orientation o1 = tests::orientation[i%6];
        tests::drawDevice(o1);
        tests::pressAnyKey();
        QTest::qWait(50);   // DO NOT REMOVE - does not work without this!

        if (i==0) continue;     //prevent from first UNDEFINED VALUE


        if (o1!=o2){
            j++;
            tmp.append("Orientation: expected/measured: \n");
            tmp.append(tests::orientationAsString(o1));
            tmp.append("!=");
            tmp.append(tests::orientationAsString(o2));
            tmp.append("\n");
        }


        //acceleration
        qreal tmpX = tests::acce_array[i%6][0];
        qreal tmpY = tests::acce_array[i%6][1];
        qreal tmpZ = tests::acce_array[i%6][2];
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
        tmpX = tests::rot_array[i%6][0];
        tmpY = tests::rot_array[i%6][1];
        x = rotReading->x();
        y = rotReading->y();
        if (hasZ){
            rot_x[i-1]=rotReading->x();
            rot_y[i-1]=rotReading->y();
            rot_z[i-1]=rotReading->z();
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
        magge_x[i-1]=maggeReading->x();
        magge_y[i-1]=maggeReading->y();
        magge_z[i-1]=maggeReading->z();

//        qDebug()<<" magge x="<<magge_x[i-1]<<", y="<<magge_y[i-1]<<", z="<<magge_z[i-1];

        //compass
        compass_azimuth[i-1] = compassReading->azimuth();
        compass_level[i-1] = compassReading->calibrationLevel();
//        qDebug()<<" compass = "<< compass_azimuth[i-1];
//        if (hasZ){
//            qDebug()<<" rotation = x "<<rot_x[i-1];
//            qDebug()<<" rotation = y "<<rot_y[i-1];
//            qDebug()<<" rotation = z "<<rot_z[i-1];
//        }
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

    // compass: 1&2, 4&3
    // azimuth will be projected to horizontal plane
    // top up & top down (5&0): too much variation
    // left up & right up: 180 diff
    // face up & face down: should be the same
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


QTEST_MAIN(test_manual)

#include "test_manual.moc"
