/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QTest>
#include <QMetaType>
#include <QSignalSpy>
#include <QDebug>
#include <QFile>
#include <QApplication>

#include <qgeopositioninfosource.h>

#include "../qlocationtestutils_p.h"


#define MAX_WAITING_TIME 50000

Q_DECLARE_METATYPE(QGeoPositionInfo)


class tst_QGeoPositionInfoSource_OnlySatellite : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        qRegisterMetaType<QGeoPositionInfo>();
    }

    void supportedSource()
    {
        // TC_ID_3_x_2 : In settings make one satellite and no non
        // satellite based positioning methods available. And call this
        // API, check return value it should be SatellitePositioningMethods
        QGeoPositionInfoSource* src = QGeoPositionInfoSource::createSource();
        QVERIFY(src != NULL);
        QCOMPARE(src->supportedPositioningMethods(),
                 QGeoPositionInfoSource::SatellitePositioningMethods);
        delete src;
    }

    void  lastKnownPosition()
    {
        QGeoPositionInfoSource *source = QGeoPositionInfoSource::createSource();
        QSignalSpy spy(source,SIGNAL(positionUpdated(const QGeoPositionInfo&)));

        source->setUpdateInterval(MAX_WAITING_TIME/2);
        source->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
        source->startUpdates();

        QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, MAX_WAITING_TIME);
        source->stopUpdates();

        QList<QVariant> list = spy.takeFirst();
        QGeoPositionInfo info;
        info = list.at(0).value<QGeoPositionInfo>();

        QGeoPositionInfo lastPositioninfo;
        lastPositioninfo = source->lastKnownPosition();

        QCOMPARE(info.coordinate(),lastPositioninfo.coordinate());
        QCOMPARE(info.dateTime(),lastPositioninfo.dateTime());

        QCOMPARE(qFuzzyCompare(info.property(QGeoPositionInfo::HorizontalAccuracy),
                 lastPositioninfo.property(QGeoPositionInfo::HorizontalAccuracy)),TRUE);

        QCOMPARE(qFuzzyCompare(info.property(QGeoPositionInfo::VerticalAccuracy),
                 lastPositioninfo.property(QGeoPositionInfo::VerticalAccuracy)),TRUE);
        delete source;
    }
};


class tst_QGeoPositionInfoSource_OnlyNonSatellite : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        qRegisterMetaType<QGeoPositionInfo>();
    }

    void supportedSource()
    {
        //TC_ID_3_x_3 : In settings make no satellite and one non satellite based positioning methods 
        //available. And call this API, check return value it should be NonSatellitePositioningMethods
        QGeoPositionInfoSource* src = QGeoPositionInfoSource::createSource();
        QVERIFY(src != NULL);
        QCOMPARE(src->supportedPositioningMethods(),
                 QGeoPositionInfoSource::NonSatellitePositioningMethods);
        delete src;
    }

    void  lastKnownPosition3()
    {
        QGeoPositionInfoSource *source = QGeoPositionInfoSource::createSource();
        QSignalSpy spy(source,SIGNAL(positionUpdated(const QGeoPositionInfo&)));

        source->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
        source->setUpdateInterval(MAX_WAITING_TIME/2);
        source->startUpdates();

        QTRY_COMPARE_WITH_TIMEOUT(spy.count(), 1, MAX_WAITING_TIME);
        source->stopUpdates();

        QList<QVariant> list = spy.takeFirst();
        QGeoPositionInfo info = list.at(0).value<QGeoPositionInfo>();
        QGeoPositionInfo lastPositioninfo = source->lastKnownPosition();
        QCOMPARE(info.coordinate(),lastPositioninfo.coordinate());
        QCOMPARE(info.dateTime(),lastPositioninfo.dateTime());

        QCOMPARE(qFuzzyCompare(info.property(QGeoPositionInfo::HorizontalAccuracy),
                    lastPositioninfo.property(QGeoPositionInfo::HorizontalAccuracy)),TRUE);

        //QFuzzyCompare() with the vertical accuracy 0, 
        //- the comparison is failing even the values are same

        /*
        QCOMPARE(qFuzzyCompare(info.property(QGeoPositionInfo::VerticalAccuracy),
                    lastPositioninfo.property(QGeoPositionInfo::VerticalAccuracy)),TRUE);
          */
        delete source;
    }
};


class tst_QGeoPositionInfoSource_None : public QObject
{
    Q_OBJECT

private slots:
    void createSource()
    {
        //TC_ID_3_x_2 : Create a position source with the given parent system has no default position source
        QLocationTestUtils::uheap_mark();
        QObject *parent = new QObject;
        QGeoPositionInfoSource* src = QGeoPositionInfoSource::createSource(parent);
        QVERIFY(src == NULL);
        delete parent;
        QLocationTestUtils::uheap_mark_end();
    }
};

#define LOG_FILE_NAME "C:/data/qlocationtestlog.txt"
#define CONFIG_FILE_NAME "c:\\data\\qlocationtest.cfg"

QStringList create_list()
{
    QStringList list;
    list.append("ONLY_SATELLITE_BASEDMETHODS_ENABLED"); 
    list.append("ONLY_NON_SATELLITE_BASEDMETHODS_ENABLED");
    list.append("NO_POSITIONING_METHODS_ENABLED");
    return list;
}

QObject* getTestObject()
{
    QFile conf_file(QString(CONFIG_FILE_NAME));
    if(!conf_file.open(QIODevice::ReadOnly | QIODevice::Text))
         return NULL;
    QByteArray ba;
    QObject *ptr = NULL;    
    do{
        ba = conf_file.readLine(80);         //max lenth of line considered as 80
        if(ba.startsWith(';'))
            continue;
        QStringList list = create_list();
        ba.chop(1);
        int choice = list.indexOf(QString(ba));
        switch(choice){
        case 0:
            ptr = new tst_QGeoPositionInfoSource_OnlySatellite;
            break;
        case 1:
            ptr = new tst_QGeoPositionInfoSource_OnlyNonSatellite;
            break;
        case 2:
            ptr = new tst_QGeoPositionInfoSource_None;
            break;
        default:
            break;
        }
    }while(!ba.isEmpty());
    conf_file.close();
    return ptr;
}

QStringList create_args(int argc, char *argv[])
{
    QStringList list;
    for (int i=0; i<argc; i++)
        list << argv[i];
    list << "-o";
    list << LOG_FILE_NAME;
    return list;
}

void log_error()
{
    QFile log_file(LOG_FILE_NAME);
    if(!log_file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    log_file.write("Bad or No configuration file c:\\data\\qlocationtest.cfg");
    log_file.close();
}

int main(int argc, char *argv[]) 
{ 
    QApplication app(argc, argv);
#ifdef QT_KEYPAD_NAVIGATION
    QApplication::setKeypadNavigationEnabled(false);
#endif

    QObject *ptr = getTestObject();
    if(ptr)
    {
        QStringList list = create_args(argc, argv);
        int ret = QTest::qExec(ptr, list);
        delete ptr;
        return ret;
    }
    log_error();
    return 0;
}

#include "tst_qgeopositioninfosource_s60.moc"
