#include "tst_qgeosearchreply.h"

QTM_USE_NAMESPACE

void SearchReply::initTestCase()
{

    reply = new SubSearchReply();
}

void SearchReply::cleanupTestCase()
{

    delete reply;
    delete qgeoplace;
}

void SearchReply::init()
{
    qRegisterMetaType<QGeoSearchReply::Error>("Error");
    signalerror = new QSignalSpy(reply, SIGNAL(error(QGeoSearchReply::Error,const QString)));
    signalfinished = new QSignalSpy(reply, SIGNAL(finished()));
}

void SearchReply::cleanup()
{
    delete signalerror;
    delete signalfinished;
}

void SearchReply::t_qgsrep_constructor1()
{
    QVERIFY(!reply->isFinished());
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);
}

void SearchReply::t_qgsrep_constructor2()
{
    QFETCH(QGeoSearchReply::Error,error);
    QFETCH(QString,msg);

    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QGeoSearchReply *qgeosearchreplycopy = new QGeoSearchReply (error,msg,0);

    QCOMPARE(typeid(*qgeosearchreplycopy).name(),"N10QtMobility15QGeoSearchReplyE");
    QCOMPARE(signalerror->count(),0);
    QCOMPARE(signalfinished->count(),0);
    QCOMPARE (qgeosearchreplycopy->errorString(),msg);
    delete qgeosearchreplycopy;
}

void SearchReply::t_qgsrep_constructor2_data()
{
    QTest::addColumn<QGeoSearchReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoSearchReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoSearchReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoSearchReply::CommunicationError << "Communication Error.";
    QTest::newRow("error4") << QGeoSearchReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoSearchReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoSearchReply::UnknownError << "Unknown Error.";

}

void SearchReply::t_qgsrep_abort()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->callSetFinished(true);
    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->abort();
    reply->callSetFinished(false);
    reply->abort();

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),2);
}

void SearchReply::t_qgsrep_error()
{
    QFETCH(QGeoSearchReply::Error,error);
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

void SearchReply::t_qgsrep_error_data()
{
    QTest::addColumn<QGeoSearchReply::Error>("error");
    QTest::addColumn<QString>("msg");

    QTest::newRow("error1") << QGeoSearchReply::NoError << "No error.";
    QTest::newRow("error2") << QGeoSearchReply::EngineNotSetError << "Engine Not Set Error.";
    QTest::newRow("error3") << QGeoSearchReply::CommunicationError << "Communication Error.";
    QTest::newRow("error4") << QGeoSearchReply::ParseError << "Parse Error.";
    QTest::newRow("error5") << QGeoSearchReply::UnsupportedOptionError << "Unsupported Option Error.";
    QTest::newRow("error6") << QGeoSearchReply::UnknownError << "Unknown Error.";
}

void SearchReply::t_qgsrep_finished()
{
    QVERIFY( signalerror->isValid() );
    QVERIFY( signalfinished->isValid() );

    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),0);

    reply->callSetFinished(true);
    QVERIFY(reply->isFinished());
    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(false);

    QVERIFY(!reply->isFinished());
    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),1);

    reply->callSetFinished(true);

    QVERIFY(reply->isFinished());
    QCOMPARE(signalerror->count(),0);
    QCOMPARE (signalfinished->count(),2);
}



void SearchReply::t_qgsrep_limit()
{
    int limit =30;
    reply->callSetLimit(limit);
    QCOMPARE(reply->limit(),limit);
}

void SearchReply::t_qgsrep_offset()
{
    int offset = 2;
    reply->callSetOffset(offset);
    QCOMPARE(reply->offset(),offset);
}

void SearchReply::t_qgsrep_places()
{
    QList <QGeoPlace> geoplaces;
    geoplaces = reply->places();

    QCOMPARE(geoplaces.size(),0);

    QGeoAddress *qgeoaddress = new QGeoAddress ();
    qgeoaddress->setCity("Berlin");

    QGeoCoordinate *qgeocoordinate = new QGeoCoordinate (12.12 , 54.43);

    qgeoplace = new QGeoPlace ();
    qgeoplace->setAddress(*qgeoaddress);
    qgeoplace->setCoordinate(*qgeocoordinate);

    reply->callAddPlace(*qgeoplace);

    geoplaces = reply->places();
    QCOMPARE(geoplaces.size(),1);
    QCOMPARE(geoplaces.at(0),*qgeoplace);

    QGeoPlace *qgeoplacecopy = new QGeoPlace (*qgeoplace);

    QList <QGeoPlace> qgeoplaces;
    qgeoplaces.append(*qgeoplace);
    qgeoplaces.append(*qgeoplacecopy);

    reply->callSetPlaces(qgeoplaces);

    geoplaces = reply->places();

    QCOMPARE(geoplaces.size(),qgeoplaces.size());
    for (int i = 0 ; i < geoplaces.size(); i++)
    {
        QCOMPARE(geoplaces.at(i),qgeoplaces.at(i));
    }

    delete qgeoaddress;
    delete qgeocoordinate;
    delete qgeoplacecopy;
}

void SearchReply::t_qgsrep_viewport()
{
    QGeoCoordinate *qgeocoordinate = new QGeoCoordinate (12.12 , 54.43);

    qgeoboundingbox = new QGeoBoundingBox (*qgeocoordinate, 0.5 , 0.5);

    reply->callSetViewport(qgeoboundingbox);

    QCOMPARE (reply->viewport(),qgeoboundingbox);

    delete qgeocoordinate;
    delete qgeoboundingbox;
}

QTEST_MAIN(SearchReply);
