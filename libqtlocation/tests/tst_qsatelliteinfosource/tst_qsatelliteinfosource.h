#ifndef TST_QGEOSATELLITEINFOSOURCE_H
#define TST_QGEOSATELLITEINFOSOURCE_H

#include <QtCore/QObject>
#include <QtCore/QCoreApplication>
#include <iostream>

#include "qgeosatelliteinfosource.h"
#include "qgeopositioninfosource.h"

class TestApp : public QObject
{
    Q_OBJECT

public:
    TestApp();
    void start();
    int i, count;
    void printSatInfo(const QGeoSatelliteInfo &sat) const;

public Q_SLOTS:
    void viewInfo(const QList<QGeoSatelliteInfo> &satellites);
    void usedInfo(const QList<QGeoSatelliteInfo> &satellites);

Q_SIGNALS:
    void startTracking();
    void stopTracking();
    void theEnd();

private:
    Q_DISABLE_COPY(TestApp)

};

#endif

