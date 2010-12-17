
#ifndef QGEOSATELLITEINFOSOURCE_MAEMO_H
#define QGEOSATELLITEINFOSOURCE_MAEMO_H

#include "qgeosatelliteinfosource.h"
#include "qgeosatelliteinfo.h"

#include <gypsy/gypsy-satellite.h>
QTM_BEGIN_NAMESPACE

class QGeoSatelliteInfoSourceMeego : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    explicit QGeoSatelliteInfoSourceMeego(QObject *parent = 0);
    int init();

public slots:
    virtual void startUpdates();
    void stopUpdates();

    void requestUpdate(int timeout = 5000);
    void satellitesChanged(GypsySatellite* satellite, GPtrArray* satellites);

signals:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void requestTimeout();

private:
    Q_DISABLE_COPY(QGeoSatelliteInfoSourceMeego)
	 GypsySatellite *satellite;
};

QTM_END_NAMESPACE

#endif

