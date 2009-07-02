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
#include <qpositionsource.h>
#include <qpositionupdate.h>

#include <QTest>
#include <QMetaType>

#include <limits.h>

Q_DECLARE_METATYPE(QPositionSource::PositioningMethod)
Q_DECLARE_METATYPE(QPositionSource::PositioningMethods)

class MyPositionSource : public QPositionSource
{
    Q_OBJECT
public:
    QPositionUpdate lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const
    {
        return QPositionUpdate();
    }

    virtual PositioningMethods supportedPositioningMethods() const { return 0; }
    virtual int minimumUpdateInterval() const { return 0; }

    virtual void startUpdates() {}
    virtual void stopUpdates() {}

    virtual void requestUpdate(int timeout = 5000) {}
};

class tst_QPositionSource : public QObject
{
    Q_OBJECT

private slots:

    void setUpdateInterval()
    {
        QFETCH(int, interval);

        MyPositionSource s;
        s.setUpdateInterval(interval);
        QCOMPARE(s.updateInterval(), interval);
    }

    void setUpdateInterval_data()
    {
        QTest::addColumn<int>("interval");
        QTest::newRow("INT_MIN") << INT_MIN;
        QTest::newRow("-1") << -1;
        QTest::newRow("0") << 0;
        QTest::newRow("1") << 1;
        QTest::newRow("INT_MAX") << INT_MAX;
    }

    void updateInterval()
    {
        MyPositionSource s;
        QCOMPARE(s.updateInterval(), 0);
    }

    void setPreferredPositioningMethods()
    {
        QFETCH(int, methods);

        MyPositionSource s;
        s.setPreferredPositioningMethods(static_cast<QPositionSource::PositioningMethods>(methods));
        QCOMPARE(s.preferredPositioningMethods(), static_cast<QPositionSource::PositioningMethods>(methods));
    }

    void setPreferredPositioningMethods_data()
    {
        QTest::addColumn<int>("methods");
        QTest::newRow("sat") << int(QPositionSource::SatellitePositioningMethods);
        QTest::newRow("sat | non-sat") << int(QPositionSource::SatellitePositioningMethods
                              | QPositionSource::NonSatellitePositioningMethods);
        QTest::newRow("all") << int(QPositionSource::AllPositioningMethods);
        QTest::newRow("all | sat") << int(QPositionSource::AllPositioningMethods
                                    | QPositionSource::NonSatellitePositioningMethods);
    }

    void preferredPositioningMethods()
    {
        MyPositionSource s;
        QCOMPARE(s.preferredPositioningMethods(), 0);
    }

    void createSource()
    {
        QVERIFY(QPositionSource::createSource() == 0);
    }
};


QTEST_MAIN(tst_QPositionSource)
#include "tst_qpositionsource.moc"
