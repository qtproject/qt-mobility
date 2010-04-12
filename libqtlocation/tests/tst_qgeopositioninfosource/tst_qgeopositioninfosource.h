#ifndef TST_QGEOPOSITIONINFOSOURCE_H
#define TST_QGEOPOSITIONINFOSOURCE_H

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <iostream>

#include "qgeopositioninfosource.h"
#include "qgeopositioninfo.h"

class tst_QGeoPositionInfoSource : public QObject
{
    Q_OBJECT

public:
    tst_QGeoPositionInfoSource();
    void start();
    int init(int count, int interval, bool useGPS);
    int i, count, interval;
    QGeoPositionInfoSource* source;

public slots:
    void newPosition(const QGeoPositionInfo &update);

signals:
    void startTracking();
    void stopTracking();
    void the_end();

};

#endif

