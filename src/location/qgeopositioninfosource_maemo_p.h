
#ifndef QGEOPOSITIONINFOSOURCEMAEMO_H
#define QGEOPOSITIONINFOSOURCEMAEMO_H

#include "qgeopositioninfosource.h"
#include "dbuscomm.h"

class DBusComm;
class QGeoPositionInfoSourceMaemo : public QGeoPositionInfoSource
{
    Q_OBJECT
public:

    QGeoPositionInfoSourceMaemo(QObject *parent = 0);
    int init();

    virtual void setUpdateInterval(int interval);
    virtual void setPreferredPositioningMethods(PositioningMethods sources);
    //virtual QPositioninfo QGeoPositionInfoSource::lastUpdate(QFlags<QGeoPositionInfoSource::PositionSourceType>) const;
    virtual QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    virtual PositioningMethods supportedPositioningMethods() const;
    virtual int minimumUpdateInterval() const;

private:
    void dumpNMEA(const char *msg);
    DBusComm* dbusComm;
    QGeoPositionInfo lastUpdate;
    QGeoPositionInfo lastSatUpdate;
    bool validLastUpdate;
    bool validLastSatUpdate;
    static const int MinimumUpdateInterval = 1000;
    int time_interval_;
    int distance_interval_;
    bool registered_;
    PositioningMethods availableMethods;

public slots:
    void startUpdates();
    void stopUpdates();
    void requestUpdate(int timeout = 5000);
    void dbusMessages(const QByteArray &msg);
    void newPositionUpdate(const QGeoPositionInfo &update);
};

#endif
