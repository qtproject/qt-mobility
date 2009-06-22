/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qsatelliteinfo.h>

#include <QMetaType>
#include <QObject>
#include <QDebug>
#include <QTest>

Q_DECLARE_METATYPE(QSatelliteInfo)
Q_DECLARE_METATYPE(QSatelliteInfo::Property)

class tst_QSatelliteInfo : public QObject
{
    Q_OBJECT

private:
    QSatelliteInfo updateWithProperty(QSatelliteInfo::Property property, qreal value)
    {
        QSatelliteInfo info;
        info.setDoubleProperty(property, value);
        return info;
    }

    void addtestdata_update()
    {
        QTest::addColumn<QSatelliteInfo>("info");

        QSatelliteInfo info1;
        info1.setPrnNumber(10);
        QTest::newRow("prn") << info1;

        QSatelliteInfo info2;
        info2.setSignalStrength(99);
        QTest::newRow("signal strength") << info2; 

        QTest::newRow("Elevation") << updateWithProperty(QSatelliteInfo::Elevation, 0.0);
        QTest::newRow("Azimuth") << updateWithProperty(QSatelliteInfo::Azimuth, 0.0);
    }

private slots:
    void constructor_copy()
    {
        QFETCH(QSatelliteInfo, info);

        QCOMPARE(QSatelliteInfo(info), info);
    }

    void constructor_copy_data()
    {
        addtestdata_update();
    }

    void operator_comparison()
    {
        QFETCH(QSatelliteInfo, info);

        QVERIFY(info == info);
        QCOMPARE(info != info, false);
        QCOMPARE(info == QSatelliteInfo(), false);
        QCOMPARE(info != QSatelliteInfo(), true);

        QVERIFY(QSatelliteInfo() == QSatelliteInfo());
    }

    void operator_comparison_data()
    {
        addtestdata_update();
    }

    void setPrnNumber()
    {
        QFETCH(int, prn);

        QSatelliteInfo info;
        QCOMPARE(info.prnNumber(), -1);

        info.setPrnNumber(prn);
        QCOMPARE(info.prnNumber(), prn);
    }

    void setPrnNumber_data()
    {
        QTest::addColumn<int>("prn");
        QTest::newRow("-10") << -10;
        QTest::newRow("zero") << 0;
        QTest::newRow("100") << 100;
        QTest::newRow("100000") << 100000;
    }

    void setSignalStrength()
    {
        QFETCH(int, signal);

        QSatelliteInfo info;
        QCOMPARE(info.signalStrength(), -1);

        info.setSignalStrength(signal);
        QCOMPARE(info.signalStrength(), signal);
    }

    void setSignalStrength_data()
    {
        QTest::addColumn<int>("signal");
        QTest::newRow("-10") << -10;
        QTest::newRow("zero") << 0;
        QTest::newRow("100") << 100;
        QTest::newRow("100000") << 100000;
    }

    void setDoubleProperty()
    {
        QFETCH(QSatelliteInfo::Property, property);
        QFETCH(qreal, value);

        QSatelliteInfo u;

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
        QTest::addColumn<QSatelliteInfo::Property>("property");
        QTest::addColumn<qreal>("value");

        QList<QSatelliteInfo::Property> props;
        props << QSatelliteInfo::Elevation
              << QSatelliteInfo::Azimuth;
        for (int i=0; i<props.count(); i++) {
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(-1.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(0.0);
            QTest::newRow(QTest::toString("property " + props[i])) << props[i] << qreal(1.0);
        }
    }
};


QTEST_MAIN(tst_QSatelliteInfo)
#include "tst_qsatelliteinfo.moc"
