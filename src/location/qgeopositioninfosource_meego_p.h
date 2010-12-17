#ifndef LOGFILEPOSITIONSOURCE_H
#define LOGFILEPOSITIONSOURCE_H

#include <qgeopositioninfosource.h>
#include <geoclue/geoclue-master.h>

QTM_USE_NAMESPACE

QT_BEGIN_NAMESPACE
class QFile;
class QTimer;
QT_END_NAMESPACE

class QGeoPositionInfoSourceMeego : public QGeoPositionInfoSource
{
    Q_OBJECT
public:
    void positionChanged(GeocluePosition      *position,
                  GeocluePositionFields fields,
                  int                   timestamp,
                  double                latitude,
                  double                longitude,
                  double                altitude,
                  GeoclueAccuracy      *accuracy);
    QGeoPositionInfoSourceMeego(QObject *parent = 0);
    int init();
    ~QGeoPositionInfoSourceMeego();

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;

    PositioningMethods supportedPositioningMethods() const;
    int minimumUpdateInterval() const;

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();

    virtual void requestUpdate(int timeout = 5000);

private:
	 GeoclueMasterClient *client;
	 GeocluePosition *pos;
    QGeoPositionInfo lastPosition;
};

#endif
