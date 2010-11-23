#ifndef TST_SearchReply_H
#define TST_SeacrhReply_H

#endif // TST_SearchReply_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QMetaType>
#include <QSignalSpy>

#include <typeinfo>

#include <QGeoSearchReply>
#include <QGeoPlace>
#include <QGeoAddress>
#include <QGeoCoordinate>

#include <QGeoBoundingBox>

QTM_USE_NAMESPACE
class SubSearchReply :public QGeoSearchReply
{
    Q_OBJECT
public:
    SubSearchReply():QGeoSearchReply() {}

    void  callAddPlace ( const QGeoPlace & place ) {addPlace(place);}
    void  callSetError ( Error error, const QString & errorString ) {setError(error, errorString);}
    void  callSetFinished ( bool finished ) {setFinished(finished);}
    void  callSetLimit ( int limit ) {setLimit(limit);}
    void  callSetOffset ( int offset ) {setOffset(offset);}
    void  callSetPlaces ( const QList<QGeoPlace> & places ) {setPlaces(places);}
    void  callSetViewport ( QGeoBoundingArea * viewport ) {setViewport(viewport);}

};

class SearchReply :public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    //Start Unit Test for QGeoSearchReply
private slots:
    void t_qgsrep_constructor1();
    void t_qgsrep_constructor2();
    void t_qgsrep_constructor2_data();
    void t_qgsrep_abort();
    void t_qgsrep_error();
    void t_qgsrep_error_data();
    void t_qgsrep_finished();
    void t_qgsrep_limit();
    void t_qgsrep_offset();
    void t_qgsrep_places();
    void t_qgsrep_viewport();

    //End Unit Test for QGeoSearchReply



private:
    QSignalSpy *signalerror;
    QSignalSpy *signalfinished;
    SubSearchReply* reply;
    QGeoPlace *qgeoplace;
    QGeoBoundingBox *qgeoboundingbox;
};

Q_DECLARE_METATYPE( QList<double>);
Q_DECLARE_METATYPE( QGeoSearchReply::Error);


