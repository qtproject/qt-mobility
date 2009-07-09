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

Q_DECLARE_METATYPE(QGeoCoordinate)
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QGeoPositionInfo::Property)

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

        QDateTime dt = QDateTime::currentDateTime();

        QTest::newRow("coord") << QGeoPositionInfo(QGeoCoordinate(1,1,1), QDateTime());
        QTest::newRow("datetime") << QGeoPositionInfo(QGeoCoordinate(), dt);

        QTest::newRow("Heading") << updateWithProperty(QGeoPositionInfo::Heading, 0.0);
        QTest::newRow("GroundSpeed") << updateWithProperty(QGeoPositionInfo::GroundSpeed, 0.0);
        QTest::newRow("VerticalSpeed") << updateWithProperty(QGeoPositionInfo::VerticalSpeed, 0.0);
        QTest::newRow("MagneticVariation") << updateWithProperty(QGeoPositionInfo::MagneticVariation, 0.0);
        QTest::newRow("HorizontalAccuracy") << updateWithProperty(QGeoPositionInfo::HorizontalAccuracy, 0.0);
        QTest::newRow("VerticalAccuracy") << updateWithProperty(QGeoPositionInfo::VerticalAccuracy, 0.0);

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

    void setProperty()
    {
        QFETCH(QGeoPositionInfo::Property, property);
        QFETCH(qreal, value);

        QGeoPositionInfo u;
        
        QVERIFY(!u.hasProperty(property));
        QCOMPARE(u.property(property), qreal(-1.0));

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));
        QCOMPARE(u.property(property), qreal(-1.0));

        u.setProperty(property, value);
        QVERIFY(u.hasProperty(property));
        QCOMPARE(u.property(property), value);

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));
        QCOMPARE(u.property(property), qreal(-1.0));
    }

    void setProperty_data()
    {
        QTest::addColumn<QGeoPositionInfo::Property>("property");
        QTest::addColumn<qreal>("value");

        QList<QGeoPositionInfo::Property> props;
        props << QGeoPositionInfo::Heading
              << QGeoPositionInfo::GroundSpeed
              << QGeoPositionInfo::VerticalSpeed
              << QGeoPositionInfo::MagneticVariation
              << QGeoPositionInfo::HorizontalAccuracy
              << QGeoPositionInfo::VerticalSpeed;
        for (int i=0; i<props.count(); i++) {
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(-1.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(0.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(1.0);
        }
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
};


QTEST_MAIN(tst_QGeoPositionInfo)
#include "tst_qgeopositioninfo.moc"
