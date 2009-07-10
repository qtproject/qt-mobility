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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qgeopositioninfo.h>

#include <QMetaType>
#include <QObject>
#include <QDebug>
#include <QTest>

#include <float.h>

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QGeoPositionInfo::Property)

QByteArray tst_qgeopositioninfo_debug;

void tst_qgeopositioninfo_messageHandler(QtMsgType type, const char *msg)
{
    switch(type) {
        case QtDebugMsg :
            tst_qgeopositioninfo_debug = QByteArray(msg);
            break;
        default:
            break;
    }
}

QList<qreal> tst_qgeopositioninfo_qrealTestValues()
{
    QList<qreal> values;
    if (qreal(DBL_MIN) == DBL_MIN)
        values << DBL_MIN;

    values << FLT_MIN;
    values << -1.0 << 0.0 << 1.0;
    values << FLT_MAX;

    if (qreal(DBL_MAX) == DBL_MAX)
        values << DBL_MAX;
    return values;
}

QList<QGeoPositionInfo::Property> tst_qgeopositioninfo_getProperties()
{
    QList<QGeoPositionInfo::Property> properties;
    properties << QGeoPositionInfo::Heading
            << QGeoPositionInfo::GroundSpeed
            << QGeoPositionInfo::VerticalSpeed
            << QGeoPositionInfo::MagneticVariation
            << QGeoPositionInfo::HorizontalAccuracy
            << QGeoPositionInfo::VerticalAccuracy;
    return properties;
}


class tst_QGeoPositionInfo : public QObject
{
    Q_OBJECT

private:
    QGeoPositionInfo updateWithProperty(QGeoPositionInfo::Property property, qreal value)
    {
        QGeoPositionInfo u;
        u.setProperty(property, value);
        return u;
    }

    void addTestData_update()
    {
        QTest::addColumn<QGeoPositionInfo>("u");

        QTest::newRow("coord") << QGeoPositionInfo(QGeoCoordinate(-27.3422,150.2342), QDateTime());
        QTest::newRow("datetime") << QGeoPositionInfo(QGeoCoordinate(), QDateTime::currentDateTime());

        QList<QGeoPositionInfo::Property> properties = tst_qgeopositioninfo_getProperties();
        QList<qreal> values = tst_qgeopositioninfo_qrealTestValues();
        for (int i=0; i<properties.count(); i++) {
            QTest::newRow(qPrintable(QString("Property %1 = %2").arg(properties[i]).arg(values[i])))
                    << updateWithProperty(properties[i], values[i]);
        }
    }

private slots:
    void isValid()
    {
        QGeoPositionInfo u;
        QVERIFY(!u.isValid());
        QVERIFY(!u.coordinate().isValid());
        QVERIFY(u.updateTime().isNull());
    }

    void constructor_coord_dateTime()
    {
        QFETCH(QGeoCoordinate, coord);
        QFETCH(QDateTime, dateTime);

        QGeoPositionInfo u(coord, dateTime);
        QCOMPARE(u.coordinate(), coord);
        QCOMPARE(u.updateTime(), dateTime);
    }

    void constructor_coord_dateTime_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");
        QTest::addColumn<QDateTime>("dateTime");

        QTest::newRow("both null") << QGeoCoordinate() << QDateTime();
        QTest::newRow("both valid") << QGeoCoordinate(1,1) << QDateTime::currentDateTime();
        QTest::newRow("valid coord") << QGeoCoordinate(1,1) << QDateTime();
        QTest::newRow("valid datetime") << QGeoCoordinate() << QDateTime::currentDateTime();
    }

    void constructor_copy()
    {
        QFETCH(QGeoPositionInfo, u);

        QCOMPARE(QGeoPositionInfo(u), u);
    }

    void constructor_copy_data()
    {
        addTestData_update();
    }

    void operator_comparison()
    {
        QFETCH(QGeoPositionInfo, u);

        QVERIFY(u == u); 
        QCOMPARE(u != u, false); 
        QCOMPARE(u == QGeoPositionInfo(), false);
        QCOMPARE(u != QGeoPositionInfo(), true);

        QVERIFY(QGeoPositionInfo() == QGeoPositionInfo());
    }

    void operator_comparison_data()
    {
        addTestData_update();
    }

    void setUpdateTime()
    {
        QFETCH(QDateTime, dateTime);

        QGeoPositionInfo u;
        u.setUpdateTime(dateTime);
        QCOMPARE(u.updateTime(), dateTime);
    }

    void setUpdateTime_data()
    {
        QTest::addColumn<QDateTime>("dateTime");
        QTest::newRow("invalid") << QDateTime();
        QTest::newRow("now") << QDateTime::currentDateTime();
    }

    void updateTime()
    {
        QGeoPositionInfo u;
        QVERIFY(u.updateTime().isNull());
    }

    void setCoordinate()
    {

        QFETCH(QGeoCoordinate, coord);

        QGeoPositionInfo u;
        u.setCoordinate(coord);
        QCOMPARE(u.coordinate(), coord);
    }

    void setCoordinate_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");

        QTest::newRow("invalid") << QGeoCoordinate();
        QTest::newRow("valid") << QGeoCoordinate(30,30);
    }

    void property()
    {
        QFETCH(QGeoPositionInfo::Property, property);
        QFETCH(qreal, value);

        QGeoPositionInfo u;
        QCOMPARE(u.property(property), qreal(-1.0));

        u.setProperty(property, value);
        QCOMPARE(u.property(property), value);

        u.removeProperty(property);
        QCOMPARE(u.property(property), qreal(-1.0));
    }

    void property_data()
    {
        QTest::addColumn<QGeoPositionInfo::Property>("property");
        QTest::addColumn<qreal>("value");

        QList<QGeoPositionInfo::Property> properties = tst_qgeopositioninfo_getProperties();
        QList<qreal> values = tst_qgeopositioninfo_qrealTestValues();
        for (int i=0; i<properties.count(); i++) {
            QTest::newRow(qPrintable(QString("Property %1 = %2").arg(properties[i]).arg(values[i])))
                    << properties[i] << values[i];
        }
    }

    void hasProperty()
    {
        QFETCH(QGeoPositionInfo::Property, property);
        QFETCH(qreal, value);

        QGeoPositionInfo u;
        QVERIFY(!u.hasProperty(property));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));
    }

    void hasProperty_data()
    {
        property_data();
    }

    void removeProperty()
    {
        QFETCH(QGeoPositionInfo::Property, property);
        QFETCH(qreal, value);

        QGeoPositionInfo u;
        QVERIFY(!u.hasProperty(property));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));
    }

    void removeProperty_data()
    {
        property_data();
    }

    void datastream()
    {
        QFETCH(QGeoPositionInfo, u);

        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out << u;

        QDataStream in(&ba, QIODevice::ReadOnly);
        QGeoPositionInfo inUpdate;
        in >> inUpdate;
        QCOMPARE(inUpdate, u);
    }

    void datastream_data()
    {
        addTestData_update();
    }

    void debug()
    {
        QFETCH(QGeoPositionInfo, info);
        QFETCH(QByteArray, debugStringEnd);

        qInstallMsgHandler(tst_qgeopositioninfo_messageHandler);
        qDebug() << info;
        qInstallMsgHandler(0);

        // use endsWith() so we don't depend on QDateTime's debug() implementation
        QVERIFY(tst_qgeopositioninfo_debug.endsWith(debugStringEnd));
    }

    void debug_data()
    {
        QTest::addColumn<QGeoPositionInfo>("info");
        QTest::addColumn<QByteArray>("debugStringEnd");

        QTest::newRow("no values") << QGeoPositionInfo()
                << QString("QGeoCoordinate(?, ?))").toLatin1();

        QGeoCoordinate coord(1, 1);
        QTest::newRow("coord, time") << QGeoPositionInfo(coord, QDateTime::currentDateTime())
                << QByteArray("QGeoCoordinate(1, 1))");

        QGeoPositionInfo info;
        info.setProperty(QGeoPositionInfo::Heading, 1.1);
        info.setProperty(QGeoPositionInfo::GroundSpeed, 2.1);
        info.setProperty(QGeoPositionInfo::VerticalSpeed, 3.1);
        info.setProperty(QGeoPositionInfo::MagneticVariation, 4.1);
        info.setProperty(QGeoPositionInfo::HorizontalAccuracy, 5.1);
        info.setProperty(QGeoPositionInfo::VerticalAccuracy, 6.1);
        QTest::newRow("all properties") << info
                << QByteArray("QGeoCoordinate(?, ?), Heading=1.1, GroundSpeed=2.1, VerticalSpeed=3.1, MagneticVariation=4.1, HorizontalAccuracy=5.1, VerticalAccuracy=6.1)");
    }
};


QTEST_MAIN(tst_QGeoPositionInfo)
#include "tst_qgeopositioninfo.moc"
