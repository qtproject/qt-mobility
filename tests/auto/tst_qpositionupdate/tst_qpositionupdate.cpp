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
#include <qpositionupdate.h>

#include <QMetaType>
#include <QObject>
#include <QDebug>
#include <QTest>

Q_DECLARE_METATYPE(QCoordinate)
Q_DECLARE_METATYPE(QPositionUpdate)
Q_DECLARE_METATYPE(QPositionUpdate::Property)

class tst_QPositionUpdate : public QObject
{
    Q_OBJECT

private:
    QPositionUpdate updateWithProperty(QPositionUpdate::Property property, qreal value)
    {
        QPositionUpdate u;
        u.setDoubleProperty(property, value);
        return u;
    }

    void addTestData_update()
    {
        QTest::addColumn<QPositionUpdate>("u");

        QDateTime dt = QDateTime::currentDateTime();

        QTest::newRow("coord") << QPositionUpdate(QCoordinate(1,1,1), QDateTime());
        QTest::newRow("datetime") << QPositionUpdate(QCoordinate(), dt);

        QTest::newRow("Heading") << updateWithProperty(QPositionUpdate::Heading, 0.0);
        QTest::newRow("GroundSpeed") << updateWithProperty(QPositionUpdate::GroundSpeed, 0.0);
        QTest::newRow("VerticalSpeed") << updateWithProperty(QPositionUpdate::VerticalSpeed, 0.0);
        QTest::newRow("MagneticVariation") << updateWithProperty(QPositionUpdate::MagneticVariation, 0.0);
        QTest::newRow("HorizontalAccuracy") << updateWithProperty(QPositionUpdate::HorizontalAccuracy, 0.0);
        QTest::newRow("VerticalAccuracy") << updateWithProperty(QPositionUpdate::VerticalAccuracy, 0.0);

    }

private slots:
    void isValid()
    {
        QPositionUpdate u;
        QVERIFY(!u.isValid());
        QVERIFY(!u.coordinate().isValid());
        QVERIFY(u.updateTime().isNull());
    }

    void constructor_coord_dateTime()
    {
        QFETCH(QCoordinate, coord);
        QFETCH(QDateTime, dateTime);

        QPositionUpdate u(coord, dateTime);
        QCOMPARE(u.coordinate(), coord);
        QCOMPARE(u.updateTime(), dateTime);
    }

    void constructor_coord_dateTime_data()
    {
        QTest::addColumn<QCoordinate>("coord");
        QTest::addColumn<QDateTime>("dateTime");

        QTest::newRow("both null") << QCoordinate() << QDateTime();
        QTest::newRow("both valid") << QCoordinate(1,1) << QDateTime::currentDateTime();
        QTest::newRow("valid coord") << QCoordinate(1,1) << QDateTime();
        QTest::newRow("valid datetime") << QCoordinate() << QDateTime::currentDateTime();
    }

    void constructor_copy()
    {
        QFETCH(QPositionUpdate, u);

        QCOMPARE(QPositionUpdate(u), u);
    }

    void constructor_copy_data()
    {
        addTestData_update();
    }

    void operator_comparison()
    {
        QFETCH(QPositionUpdate, u);

        QVERIFY(u == u); 
        QCOMPARE(u != u, false); 
        QCOMPARE(u == QPositionUpdate(), false);
        QCOMPARE(u != QPositionUpdate(), true);

        QVERIFY(QPositionUpdate() == QPositionUpdate());
    }

    void operator_comparison_data()
    {
        addTestData_update();
    }

    void setUpdateTime()
    {
        QFETCH(QDateTime, dateTime);

        QPositionUpdate u;
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
        QPositionUpdate u;
        QVERIFY(u.updateTime().isNull());
    }

    void setCoordinate()
    {

        QFETCH(QCoordinate, coord);

        QPositionUpdate u;
        u.setCoordinate(coord);
        QCOMPARE(u.coordinate(), coord);
    }

    void setCoordinate_data()
    {
        QTest::addColumn<QCoordinate>("coord");

        QTest::newRow("invalid") << QCoordinate();
        QTest::newRow("valid") << QCoordinate(30,30);
    }

    void setDoubleProperty()
    {
        QFETCH(QPositionUpdate::Property, property);
        QFETCH(qreal, value);

        QPositionUpdate u;
        
        QVERIFY(!u.hasProperty(property));
        QCOMPARE(u.doubleProperty(property), qreal(-1.0));

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));
        QCOMPARE(u.doubleProperty(property), qreal(-1.0));

        u.setDoubleProperty(property, value);
        QVERIFY(u.hasProperty(property));
        QCOMPARE(u.doubleProperty(property), value);

        u.removeProperty(property);
        QVERIFY(!u.hasProperty(property));
        QCOMPARE(u.doubleProperty(property), qreal(-1.0));
    }

    void setDoubleProperty_data()
    {
        QTest::addColumn<QPositionUpdate::Property>("property");
        QTest::addColumn<qreal>("value");

        QList<QPositionUpdate::Property> props;
        props << QPositionUpdate::Heading
              << QPositionUpdate::GroundSpeed
              << QPositionUpdate::VerticalSpeed
              << QPositionUpdate::MagneticVariation
              << QPositionUpdate::HorizontalAccuracy
              << QPositionUpdate::VerticalSpeed;
        for (int i=0; i<props.count(); i++) {
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(-1.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(0.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(1.0);
        }
    }

    void datastream()
    {
        QFETCH(QPositionUpdate, u);

        QByteArray ba;
        QDataStream out(&ba, QIODevice::WriteOnly);
        out << u;

        QDataStream in(&ba, QIODevice::ReadOnly);
        QPositionUpdate inUpdate;
        in >> inUpdate;
        QCOMPARE(inUpdate, u);
    }

    void datastream_data()
    {
        addTestData_update();
    }
};


QTEST_MAIN(tst_QPositionUpdate)
#include "tst_qpositionupdate.moc"
