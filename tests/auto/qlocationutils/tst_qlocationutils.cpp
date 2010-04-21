/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//#include "../../../src/location/qlocationutils_p.h"
#include "qlocationutils_p.h"
#include "qgeopositioninfo.h"

#include <QDebug>
#include <QTest>
#include <QObject>

#include <math.h>

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QGeoCoordinate)


class tst_QLocationUtils : public QObject
{
    Q_OBJECT
private:
    QString addChecksumEtc(const QString &sentence)
    {
        Q_ASSERT(sentence[0] == '$' && sentence[sentence.length()-1] == '*');

        // XOR byte value of all characters between '$' and '*'
        int result = 0;
        for (int i=1; i<sentence.length()-1; i++)
            result ^= sentence[i].toAscii();
        QString sum;
        sum.sprintf("%02x", result);
        return sentence + sum + "\r\n";
    }

    double nmeaDegreesToDecimal(double nmeaDegrees)
    {
        double deg;
        double min = 100.0 * modf(nmeaDegrees / 100.0, &deg);
        return deg + (min / 60.0);
    }

private slots:

    void getPosInfoFromNmea()
    {
        QFETCH(QString, nmea);
        QFETCH(QGeoPositionInfo, expectedInfo);
        QFETCH(bool, expectedHasFix);
        QFETCH(bool, expectedResult);

        QGeoPositionInfo info;
        bool hasFix;
        bool b = QLocationUtils::getPosInfoFromNmea(nmea.toAscii(), nmea.length(), &info, &hasFix);
        QCOMPARE(b, expectedResult);
        QCOMPARE(hasFix, expectedHasFix);
        QCOMPARE(info, expectedInfo);
    }

    void getPosInfoFromNmea_data()
    {
        QTest::addColumn<QString>("nmea");
        QTest::addColumn<QGeoPositionInfo>("expectedInfo");
        QTest::addColumn<bool>("expectedHasFix");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("empty") << QString() << QGeoPositionInfo() << false << false;

        // must use exact double values or comparisons fail
        double lat = nmeaDegreesToDecimal(2734.7964); 
        double lng = nmeaDegreesToDecimal(15306.0124);

        QList<double> dblValues;
        dblValues << -1.534 << -1.0 << 0.0 << 0.12123 << 3.23 << 123124.11;

        QGeoPositionInfo info;

        // all types
        QList<QString> types;
        types << "$GPGGA*" << "$GPGLL*" << "$GPRMC*" << "$GPVTG*" << "$GPZDA*";
        for (int i=0; i<types.count(); i++) {
            // the sentence type is valid even if it is empty so the method still returns true
            QTest::newRow(qPrintable(types[i])) << addChecksumEtc(types[i])
                    << QGeoPositionInfo() << false << true;
        }

        // bad type
        QTest::newRow("unknown type") << addChecksumEtc("$GPZZZ*") << QGeoPositionInfo() << false << false;

        // --GGA:

        QTest::newRow("GGA-time") << addChecksumEtc("$GPGGA,060613.626,,,,,,,,,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(), QTime(6, 6, 13, 626)))
                << false << true;

        QTest::newRow("GGA-lat/long") << addChecksumEtc("$GPGGA,,2734.7964,S,15306.0124,E,,,,,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(-lat, lng), QDateTime())
                << false << true;

        QTest::newRow("GGA-lat/long/alt") << addChecksumEtc("$GPGGA,,2734.7964,S,15306.0124,E,,,,26.8,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(-lat, lng, 26.8), QDateTime())
                << false << true;
    
        QTest::newRow("GGA alt but no lat/long") << addChecksumEtc("$GPGGA,,,,,,,,,26.8,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;

        for (int i=-1; i<10; i++) {
            bool hasFix = i > 0;
            QTest::newRow("GGA-fix") << addChecksumEtc(QString("$GPGGA,,,,,,%1,,,,,,,,*").arg(i))
                    << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                    << hasFix << true;
        }

        QTest::newRow("GGA-all") << addChecksumEtc("$GPGGA,060613.626,2734.7964,S,15306.0124,E,1,03,2.9,-26.8,M,36.8,M,1,0000*")
                << QGeoPositionInfo(QGeoCoordinate(-lat, lng, -26.8), QDateTime(QDate(), QTime(6,6,13,626)))
                << true << true;


        // --GLL:

        QTest::newRow("GLL-lat/long") << addChecksumEtc("$GPGLL,2734.7964,S,15306.0124,E,,,*")
                << QGeoPositionInfo(QGeoCoordinate(-lat, lng), QDateTime())
                << false << true;

        QTest::newRow("GLL-time") << addChecksumEtc("$GPGLL,,,,,060613.626,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(), QTime(6, 6, 13, 626)))
                << false << true;

        QTest::newRow("GLL-fix-ok") << addChecksumEtc("$GPGLL,,,,,,A,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << true << true;

        QTest::newRow("GLL-fix-bad") << addChecksumEtc("$GPGLL,,,,,,V,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;

        QTest::newRow("GLL-all") << addChecksumEtc("$GGGLL,2734.7964,S,15306.0124,E,220125.999,A,,*")
                << QGeoPositionInfo(QGeoCoordinate(-lat, lng), QDateTime(QDate(), QTime(22,1,25,999)))
                << true << true;

                
        // --RMC:

        QTest::newRow("RMC-time") << addChecksumEtc("$GPRMC,060613.626,,,,,,,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(), QTime(6, 6, 13, 626)))
                << false << true;
        
        QTest::newRow("RMC-fix-ok") << addChecksumEtc("$GPRMC,,A,,,,,,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << true << true;

        QTest::newRow("RMC-fix-bad") << addChecksumEtc("$GPRMC,,V,,,,,,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;

        QTest::newRow("RMC-lat/long") << addChecksumEtc("$GPRMC,,,2734.7964,S,15306.0124,E,,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(-lat, lng), QDateTime())
                << false << true;

        for (int i=0; i<dblValues.count(); i++) {
            info = QGeoPositionInfo();
            info.setAttribute(QGeoPositionInfo::Direction, dblValues[i]);
            QTest::newRow(qPrintable(QString("RMC-heading %1").arg(dblValues[i]))) << addChecksumEtc(QString("$GPRMC,,,,,,,,%1,,,,*").arg(dblValues[i], 0, 'g', 10)) 
                    << info << false << true;
        }

        for (int i=0; i<dblValues.count(); i++) {
            info = QGeoPositionInfo();
            info.setAttribute(QGeoPositionInfo::GroundSpeed, dblValues[i] * 1.852 / 3.6);
            QTest::newRow(qPrintable(QString("RMC-speed %1").arg(dblValues[i]))) << addChecksumEtc(QString("$GPRMC,,,,,,,%1,,,,,*").arg(dblValues[i], 0, 'g', 10)) 
                    << info << false << true;
        }

        for (int i=0; i<dblValues.count(); i++) {
            info = QGeoPositionInfo();
            info.setAttribute(QGeoPositionInfo::MagneticVariation, dblValues[i]);
            char direction = dblValues[i] > 0 ? 'E' : 'W';
            QTest::newRow(qPrintable(QString("RMC-magVar %1").arg(dblValues[i]))) << addChecksumEtc(QString("$GPRMC,,,,,,,,,,%1,%2,*").arg(qAbs(dblValues[i]), 0, 'g', 10).arg(direction)) 
                    << info << false << true;
        }

        QTest::newRow("RMC-date") << addChecksumEtc("$GPRMC,,,,,,,,,010520,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(2020,5,1), QTime(), Qt::UTC))
                << false << true;

        QTest::newRow("RMC-date bad format") << addChecksumEtc("$GPRMC,,,,,,,,,10520,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;

        info = QGeoPositionInfo(QGeoCoordinate(-lat, lng), QDateTime(QDate(2008,4,3), QTime(22,1,25,999), Qt::UTC));
        info.setAttribute(QGeoPositionInfo::GroundSpeed, 8.9 * 1.852 / 3.6);
        info.setAttribute(QGeoPositionInfo::Direction, 47.6);
        info.setAttribute(QGeoPositionInfo::MagneticVariation, -11.2);
        QTest::newRow("RMC-all") << addChecksumEtc("$GPRMC,220125.999,A,2734.7964,S,15306.0124,E,8.9,47.6,030408,11.2,W,A*")
                << info
                << true << true;

        // -- VTG:

        for (int i=0; i<dblValues.count(); i++) {
            info = QGeoPositionInfo();
            info.setAttribute(QGeoPositionInfo::Direction, dblValues[i]);
            QTest::newRow(qPrintable(QString("VTG-heading %1").arg(dblValues[i]))) << addChecksumEtc(QString("$GPVTG,%1,,,,,,,,*").arg(dblValues[i], 0, 'g', 10)) 
                    << info << false << true;
        }

        for (int i=0; i<dblValues.count(); i++) {
            info = QGeoPositionInfo();
            info.setAttribute(QGeoPositionInfo::GroundSpeed, dblValues[i] / 3.6);
            QTest::newRow(qPrintable(QString("VTG-speed %1").arg(dblValues[i]))) << addChecksumEtc(QString("$GPVTG,,,,,,,%1,,*").arg(dblValues[i], 0, 'g', 10)) 
                    << info << false << true;
        }

        info = QGeoPositionInfo();
        info.setAttribute(QGeoPositionInfo::Direction, 158.7);
        info.setAttribute(QGeoPositionInfo::GroundSpeed, 61.5 / 3.6);
        QTest::newRow("VTG-all") << addChecksumEtc("$GPVTG,158.7,T,169.9,M,33.2,N,61.5,K,A*")
                << info
                << false << true;

        // ZDA:

        QTest::newRow("ZDA-time") << addChecksumEtc("$GPZDA,152333.4,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(), QTime(15,23,33, 4), Qt::UTC))
                << false << true;

        QTest::newRow("ZDA-date day only") << addChecksumEtc("$GPZDA,,1,,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;

        QTest::newRow("ZDA-date month only") << addChecksumEtc("$GPZDA,,,12,,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;
        
        QTest::newRow("ZDA-date bad year only") << addChecksumEtc("$GPZDA,,,,2005,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime())
                << false << true;

        QTest::newRow("ZDA-date") << addChecksumEtc("$GPZDA,,1,12,2005,,*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(2005,12,1), QTime(), Qt::UTC))
                << false << true;

        QTest::newRow("ZDA-all") << addChecksumEtc("$GPZDA,052953.05,03,04,2008,00,00*")
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(2008, 4, 3), QTime(5, 29, 53, 5), Qt::UTC))
                << false << true;

        // test if multiple sentences are passed in, only first is parsed
        QTest::newRow("multiple") << (addChecksumEtc("$GPZDA,052953.05,03,04,2008,00,00*") + addChecksumEtc("$GPVTG,158.7,T,169.9,M,33.2,N,61.5,K,A*"))
                << QGeoPositionInfo(QGeoCoordinate(), QDateTime(QDate(2008, 4, 3), QTime(5, 29, 53, 5), Qt::UTC))
                << false << true;
    }

    void hasValidNmeaChecksum() {
        QFETCH(QString, nmea);
        QFETCH(bool, expectedResult);

        QCOMPARE(QLocationUtils::hasValidNmeaChecksum(nmea.toAscii().constData(), nmea.length()), expectedResult);
    }

    void hasValidNmeaChecksum_data()
    {
        QTest::addColumn<QString>("nmea");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("empty") << QString() << false;

        QString s = addChecksumEtc("$GPGGA,,,,,,,,,,,,,,*");
        QString trimmed = s.trimmed();
        QTest::newRow("ok") << s << true;
        QTest::newRow("too short") << trimmed.mid(0, trimmed.length()-1) << false;
        QTest::newRow("no asterisk") << s.replace("*", "") << false; // modifies test data!
    }

    void getNmeaTime()
    {
        QFETCH(QString, str);
        QFETCH(QTime, expectedTime);
        QFETCH(bool, expectedResult);

        QTime time;
        bool b = QLocationUtils::getNmeaTime(str.toAscii(), &time);
        QCOMPARE(time, expectedTime);
        QCOMPARE(b, expectedResult);
    }

    void getNmeaTime_data()
    {
        QTest::addColumn<QString>("str");
        QTest::addColumn<QTime>("expectedTime");
        QTest::addColumn<bool>("expectedResult");
        
        QTest::newRow("empty") << QString() << QTime() << false;
        QTest::newRow("no numbers") << QString("asdl") << QTime() << false;
        QTest::newRow("one number short") << QString("11223") << QTime() << false;

        QTest::newRow("112233") << QString("112233") << QTime(11,22,33) << true;
        QTest::newRow("000000") << QString("000000") << QTime(0,0,0) << true;
        QTest::newRow("235959") << QString("235959") << QTime(23,59,59) << true;
        QTest::newRow("235960") << QString("235960") << QTime() << false;
        QTest::newRow("236059") << QString("236059") << QTime() << false;
        QTest::newRow("245959") << QString("24959") << QTime() << false;

        QTest::newRow("112233.0") << QString("112233.0") << QTime(11,22,33) << true;
        QTest::newRow("112233.1") << QString("112233.1") << QTime(11,22,33, 1) << true;
        QTest::newRow("112233.12") << QString("112233.12") << QTime(11,22,33, 12) << true;
        QTest::newRow("112233.123") << QString("112233.123") << QTime(11,22,33, 123) << true;
        QTest::newRow("112233.999") << QString("112233.999") << QTime(11,22,33, 999) << true;
        QTest::newRow("112233.9990") << QString("112233.9990") << QTime(11,22,33,999) << true;
        QTest::newRow("112233.9991") << QString("112233.9991") << QTime(11,22,33,999) << true;
    }

    void getNmeaLatLong()
    {
        QFETCH(QString, str);
        QFETCH(double, expectedLat);
        QFETCH(double, expectedLng);
        QFETCH(bool, expectedResult);

        double lat, lng;
        QList<QByteArray> parts = str.toAscii().split(',');
        bool b = QLocationUtils::getNmeaLatLong(parts[0], parts[1][0], parts[2], parts[3][0], &lat, &lng);
        QCOMPARE(b, expectedResult);
        if (b) {
            QCOMPARE(lat, expectedLat);
            QCOMPARE(lng, expectedLng);
        }
    }

    void getNmeaLatLong_data()
    {
        QTest::addColumn<QString>("str");
        QTest::addColumn<double>("expectedLat");
        QTest::addColumn<double>("expectedLng");
        QTest::addColumn<bool>("expectedResult");

        QTest::newRow("0.0,S,0.0,E") << "0.0,S,0.0,E" << 0.0 << 0.0 << true;
        QTest::newRow("0.0,S,0.0,E") << "0.0,S,0.0,E" << 0.0 << 0.0 << true;
        QTest::newRow("0,N,0,W") << "0,N,0,W" << 0.0 << 0.0 << true;
        QTest::newRow("0,N,0,W") << "0,N,0,W" << 0.0 << 0.0 << true;

        QTest::newRow("bad N/S indicator") << "0.0,Z,0.0,E" << 0.0 << 0.0 << false;
        QTest::newRow("bad E/W indicator") << "0.0,S,0.0,P" << 0.0 << 0.0 << false;

        QTest::newRow("lat too big") << "9000.0001,N,0.0,E" << 0.0 << 0.0 << false; 
        QTest::newRow("lat almost too big") << "9000.0000,N,0.0,E" << 90.0 << 0.0 << true; 
        QTest::newRow("lat too small") << "9000.0001,S,0.0,E" << 0.0 << 0.0 << false; 
        QTest::newRow("lat almost too small") << "9000.0000,S,0.0,E" << -90.0 << 0.0 << true; 

        QTest::newRow("long too big") << "0.0,N,18000.0001,E" << 0.0 << 0.0 << false; 
        QTest::newRow("long almost too big") << "0.0,N,18000.0000,E" << 0.0 << 180.0 << true; 
        QTest::newRow("long too small") << "0.0,N,18000.0001,W" << 0.0 << 0.0 << false; 
        QTest::newRow("long almost too small") << "0.0,N,18000.0000,W" << 0.0 << -180.0 << true; 

        double lat = nmeaDegreesToDecimal(2734.7964); 
        double lng = nmeaDegreesToDecimal(15306.0124);

        QTest::newRow("Lat N, Long E")
                << "2734.7964,N,15306.0124,E" 
                << lat << lng << true;

        QTest::newRow("Lat S, Long E")
                << "2734.7964,S,15306.0124,E" 
                << -lat << lng << true;

        QTest::newRow("Lat N, Long W")
                << "2734.7964,N,15306.0124,W" 
                << lat << -lng << true;

        QTest::newRow("Lat S, Long W")
                << "2734.7964,S,15306.0124,W" 
                << -lat << -lng << true;
    }

};


QTEST_MAIN(tst_QLocationUtils)
#include "tst_qlocationutils.moc"

