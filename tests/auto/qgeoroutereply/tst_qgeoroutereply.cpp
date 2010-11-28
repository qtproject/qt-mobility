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

#include "tst_qgeoroutereply.h"

QTM_USE_NAMESPACE

void tst_QGeoRouteReply::initTestCase()
{
    qgeocoordinate1 = new QGeoCoordinate (43.5435 , 76.342);
    qgeocoordinate2 = new QGeoCoordinate (-43.5435 , 176.342);
    qgeocoordinate3 = new QGeoCoordinate (-13.5435 , +76.342);

    waypoints.append(*qgeocoordinate1);
    waypoints.append(*qgeocoordinate2);
    waypoints.append(*qgeocoordinate3);

    qgeorouterequest = new QGeoRouteRequest (waypoints);
    reply = new SubRouteReply(*qgeorouterequest);
}

void tst_QGeoRouteReply::cleanupTestCase()
{
    delete qgeocoordinate1;
    delete qgeocoordinate2;
    delete qgeocoordinate3;
    delete qgeorouterequest;
    delete reply;
}

void tst_QGeoRouteReply::init()
{
    qRegisterMetaType<QGeoRouteReply::Error>("Error");
    signalerror = new QSignalSpy(reply, SIGNAL(error(QGeoRouteReply::Error,const QString)));
    signalfinished = new QSignalSpy(reply, SIGNAL(finished()));
}

void tst_QGeoRouteReply::cleanup()
{
    delete signalerror;
    delete signalfinished;
}

void tst_QGeoRouteReply::constructor()
{
    QVERIFY(!reply->isFinished());
    QCOMPARE(reply->error(),QGeoRouteReply::NoError);
    QCOMPARE(reply->request(),*qgeorouterequest);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);
}

void tst_QGeoRouteReply::constructor_error()
{
    QFETCH(QGeoRouteReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QGeoRouteReply *qgeoroutereplycopy = new QGeoRouteReply (error,msg,0);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);

    QVERIFY(qgeoroutereplycopy->isFinished());
    QCOMPARE(qgeoroutereplycopy->error(),error);
    QCOMPARE (qgeoroutereplycopy->errorString(),msg);

    delete qgeoroutereplycopy;
}

void tst_QGeoRouteReply::constructor_error_data()
{
    QTest::addColumn<QGeoRouteReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoRouteReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoRouteReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoRouteReply::CommunicationError << "Comunication Error.";
    QTest::newRow("error4") << QGeoRouteReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoRouteReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoRouteReply::UnknownError << "Unknown Error.";

}

void tst_QGeoRouteReply::destructor()
{
    QGeoRouteReply *qgeoroutereplycopy;

    QFETCH(QGeoRouteReply::Error,error);
    QFETCH(QString,msg);

    QLocationTestUtils::uheap_mark();
    qgeoroutereplycopy = new QGeoRouteReply (error,msg,0);
    delete qgeoroutereplycopy;
    QLocationTestUtils::uheap_mark_end();

}

void tst_QGeoRouteReply::destructor_data()
{
    tst_QGeoRouteReply::constructor_error_data();
}

void tst_QGeoRouteReply::routes()
{
    QList<QGeoRoute> routes;
    QGeoRoute *route1 = new QGeoRoute ();
    QGeoRoute *route2 = new QGeoRoute ();

    route1->setDistance(15.12);
    route2->setDistance(20.12);

    routes.append(*route1);
    routes.append(*route2);

    reply->callSetRoutes(routes);

    QList<QGeoRoute> routescopy;
    routescopy = reply->routes();
    QCOMPARE(routescopy,routes);

    QCOMPARE(routescopy.at(0).distance(),route1->distance());
    QCOMPARE(routescopy.at(1).distance(),route2->distance());

    delete route1;
    delete route2;
}

void tst_QGeoRouteReply::finished()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->callSetFinished(true);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(false);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(true);

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),2);
}

void tst_QGeoRouteReply::abort()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->abort();
    reply->callSetFinished(false);
    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);
}

void tst_QGeoRouteReply::error()
{
    QFETCH(QGeoRouteReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );
    QCOMPARE(signalerror->count(),0);

    reply->callSetError(error,msg);

   QCOMPARE(signalerror->count(),1);
   QCOMPARE(signalfinished->count(),1);
   QCOMPARE(reply->errorString(),msg);
   QCOMPARE(reply->error(),error);


}

void tst_QGeoRouteReply::error_data()
{
    QTest::addColumn<QGeoRouteReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoRouteReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoRouteReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoRouteReply::CommunicationError << "Communication Error.";
    QTest::newRow("error4") << QGeoRouteReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoRouteReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoRouteReply::UnknownError << "Unknown Error.";
}

void tst_QGeoRouteReply::request()
{
    SubRouteReply *rr= new SubRouteReply(*qgeorouterequest);

    QCOMPARE(rr->request(),*qgeorouterequest);

    delete rr;
}

QTEST_MAIN(tst_QGeoRouteReply);
