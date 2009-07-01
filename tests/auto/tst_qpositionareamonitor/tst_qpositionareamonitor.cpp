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
#include <qpositionareamonitor.h>

#include <QTest>
#include <QMetaType>

#include <limits.h>

Q_DECLARE_METATYPE(QCoordinate)

class MyPositionAreaMonitor : public QPositionAreaMonitor
{
    Q_OBJECT
public:
    ~MyPositionAreaMonitor() {}
};

class tst_QPositionAreaMonitor : public QObject
{
    Q_OBJECT

private slots:
    void setMonitoredArea()
    {
        QFETCH(QCoordinate, coord);
        QFETCH(int, radius);

        MyPositionAreaMonitor mon;
        mon.setMonitoredArea(coord, radius);
        QCOMPARE(mon.coordinate(), coord);
        QCOMPARE(mon.radius(), radius);
    }

    void setMonitoredArea_data()
    {
        QTest::addColumn<QCoordinate>("coord");
        QTest::addColumn<int>("radius");

        QList<int> radii = QList<int>() << INT_MIN << -1 << 0 << 1 << INT_MAX;
        for (int i=0; i<radii.count(); i++) {
            QTest::newRow(qPrintable(QString("null coord, radius %1").arg(radii[i])))
                    << QCoordinate() << radii[i];
        }

        QTest::newRow("valid coord, 0 radius") << QCoordinate(1, 1, 1) << 0;
        QTest::newRow("invalid coord, 0 radius") << QCoordinate(-200, 1, 1) << 0;
    }

    void coordinate()
    {
        MyPositionAreaMonitor mon;
        QCOMPARE(mon.coordinate(), QCoordinate());
    }

    void radius()
    {
        MyPositionAreaMonitor mon;
        QCOMPARE(mon.radius(), 0);
    }

    void createMonitor()
    {
        QVERIFY(QPositionAreaMonitor::createMonitor() == 0);
    }
};


QTEST_MAIN(tst_QPositionAreaMonitor)
#include "tst_qpositionareamonitor.moc"
