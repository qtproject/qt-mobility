/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qgeoareamonitor.h>

#include <QTest>
#include <QMetaType>

#include <float.h>

Q_DECLARE_METATYPE(QGeoCoordinate)

class MyPositionAreaMonitor : public QGeoAreaMonitor
{
    Q_OBJECT
public:
    MyPositionAreaMonitor(QObject *parent = 0) : QGeoAreaMonitor(parent) {}
};

class tst_QGeoAreaMonitor : public QObject
{
    Q_OBJECT

private slots:
    void setCenter()
    {
        QFETCH(QGeoCoordinate, coord);

        MyPositionAreaMonitor mon;
        mon.setCenter(coord);
        QCOMPARE(mon.center(), coord);
    }

    void setCenter_data()
    {
        QTest::addColumn<QGeoCoordinate>("coord");
        QTest::newRow("invalid coord") << QGeoCoordinate();
        QTest::newRow("invalid coord 2") << QGeoCoordinate(-200, 200);
        QTest::newRow("2d coord") << QGeoCoordinate(1, 1);
        QTest::newRow("3d coord") << QGeoCoordinate(1, 1, 1);
    }

    void center()
    {
        MyPositionAreaMonitor mon;
        QCOMPARE(mon.center(), QGeoCoordinate());
    }

    void setRadius()
    {
        QFETCH(qreal, radius);

        MyPositionAreaMonitor mon;
        mon.setRadius(radius);
        QCOMPARE(mon.radius(), radius);
    }

    void setRadius_data()
    {
        QTest::addColumn<qreal>("radius");

        if (qreal(DBL_MIN) == DBL_MIN)
            QTest::newRow("double min") << qreal(DBL_MIN);

        QTest::newRow("float min") << qreal(FLT_MIN);
        QTest::newRow("-1") << qreal(-1);
        QTest::newRow("0") << qreal(0);
        QTest::newRow("1") << qreal(0);
        QTest::newRow("float max") << qreal(FLT_MAX);

        if (qreal(DBL_MAX) == DBL_MAX)
            QTest::newRow("double max") << qreal(DBL_MAX);
    }

    void radius()
    {
        MyPositionAreaMonitor mon;
        QCOMPARE(mon.radius(), qreal(0.0));
    }

    void createDefaultMonitor()
    {
        QVERIFY(QGeoAreaMonitor::createDefaultMonitor(0) == 0);
    }
};


QTEST_MAIN(tst_QGeoAreaMonitor)
#include "tst_qgeoareamonitor.moc"
