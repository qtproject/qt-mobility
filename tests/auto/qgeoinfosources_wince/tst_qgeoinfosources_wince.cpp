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

//TESTED_COMPONENT=src/location

#include <QTest>
#include <QMetaType>
#include <QSignalSpy>
#include <QObject>

#include <qgeopositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qgeosatelliteinfosource.h>
#include <qgeosatelliteinfo.h>

#include "../qlocationtestutils_p.h"

QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QGeoPositionInfo)
Q_DECLARE_METATYPE(QList<QGeoSatelliteInfo>)

class tst_QGeoInfoSourcesWinCE : public QObject
{
    Q_OBJECT

public:
    tst_QGeoInfoSourcesWinCE(QObject *parent = 0);

private slots:
    void initTestCase();

    void positioningMethods();
    void simultaneousUpdates();
    void satTriggerTimeoutSignal();

private:
    QGeoPositionInfoSource *posSource;
    QGeoSatelliteInfoSource *satSource;
};

tst_QGeoInfoSourcesWinCE::tst_QGeoInfoSourcesWinCE(QObject *parent) : QObject(parent) {}

void tst_QGeoInfoSourcesWinCE::initTestCase()
{
    posSource = QGeoPositionInfoSource::createDefaultSource(this);
    satSource = QGeoSatelliteInfoSource::createDefaultSource(this);

    if (posSource == 0 || satSource == 0)
        QSKIP("These tests require valid default sources for position and satellite data on WinCE.",
              SkipAll);

    qRegisterMetaType<QGeoPositionInfo>();
    qRegisterMetaType<QList<QGeoSatelliteInfo> >();
}

void tst_QGeoInfoSourcesWinCE::positioningMethods()
{
    if (posSource == 0)
        QSKIP("This test requires a valid default source for position data on WinCE.",
              SkipAll);

    QCOMPARE(posSource->supportedPositioningMethods(),
             QGeoPositionInfoSource::SatellitePositioningMethods);

    posSource->setPreferredPositioningMethods(QGeoPositionInfoSource::AllPositioningMethods);
    QCOMPARE(posSource->preferredPositioningMethods(),
             QGeoPositionInfoSource::SatellitePositioningMethods);

    posSource->setPreferredPositioningMethods(QGeoPositionInfoSource::SatellitePositioningMethods);
    QCOMPARE(posSource->preferredPositioningMethods(),
             QGeoPositionInfoSource::SatellitePositioningMethods);

    posSource->setPreferredPositioningMethods(QGeoPositionInfoSource::NonSatellitePositioningMethods);
    QCOMPARE(posSource->preferredPositioningMethods(),
             QGeoPositionInfoSource::SatellitePositioningMethods);
}

void tst_QGeoInfoSourcesWinCE::simultaneousUpdates()
{
    if (posSource == 0 || satSource == 0)
        QSKIP("This test requires valid default sources for position and satellite data on WinCE.",
              SkipAll);

    QSignalSpy spyPos(posSource, SIGNAL(positionUpdated(const QGeoPositionInfo&)));
    QSignalSpy spySatView(satSource,
                          SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    QSignalSpy spySatUse(satSource,
                         SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));

    posSource->setUpdateInterval(0);

    posSource->startUpdates();
    satSource->startUpdates();

    EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

    QTRY_VERIFY_WITH_TIMEOUT((spyPos.count() > 0) && (spySatView.count() > 0)
                             && (spySatUse.count() > 0), 20000);

    spyPos.clear();
    spySatView.clear();
    spySatUse.clear();

    for (int i = 0; i < 2; i++) {
        EXPECT_FAIL_WINCE_SEE_MOBILITY_337;

        QTRY_VERIFY_WITH_TIMEOUT((spyPos.count() > 0) && (spySatView.count() > 0)
                                 && (spySatUse.count() > 0), 10000);
        spyPos.clear();
        spySatView.clear();
        spySatUse.clear();
    }

    posSource->stopUpdates();
    satSource->stopUpdates();
}

// this is to provide code coverage for request timeouts
// - the fact that this will timeout is potentitally specific to the Win CE backend
// - a test for position timeouts would have been added, but the minimumInterval behaviours
//   make a timeout harder to force
void tst_QGeoInfoSourcesWinCE::satTriggerTimeoutSignal()
{
    QSignalSpy spyTimeout(satSource, SIGNAL(requestTimeout()));

    satSource->requestUpdate(1);

    QTRY_COMPARE_WITH_TIMEOUT(spyTimeout.count(), 1, 2000);
}

QTEST_MAIN(tst_QGeoInfoSourcesWinCE)
#include "tst_qgeoinfosources_wince.moc"
