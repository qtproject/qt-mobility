#ifndef QGEOSATELLITEINFOSOURCE_MAEMO_H
#define QGEOSATELLITEINFOSOURCE_MAEMO_H

#include "qsatelliteinfosource.h"
#include "qsatelliteinfo.h"
#include "dbuscomm_maemo.h"

class QGeoSatelliteInfoSourceMaemo : public QGeoSatelliteInfoSource
{
    Q_OBJECT
public:
    explicit QGeoSatelliteInfoSourceMaemo(QObject *parent = 0);
    int init();

private:
    DBusComm* dbusComm;
    int client_id_;
    void dbusMessages(const QByteArray &msg);

public slots:
    virtual void startUpdates();
    void stopUpdates();

    void requestUpdate(int timeout = 5000);

signals:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &satellites);
    void requestTimeout();

private:
    Q_DISABLE_COPY(QGeoSatelliteInfoSourceMaemo)
};

#endif

