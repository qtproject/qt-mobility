#ifndef QGEOPOSITIONINFOSOURCEMEEGO_H
#define QGEOPOSITIONINFOSOURCEMEEGO_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgeopositioninfosource.h>
#include <geoclue/geoclue-master.h>

#define Q_LOCATION_GEOCLUE_DEBUG

QTM_BEGIN_NAMESPACE

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
    ~QGeoPositionInfoSourceMeego();

    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    PositioningMethods supportedPositioningMethods() const;
    int minimumUpdateInterval() const;
    int init();

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout = 5000);

private:
    GeoclueMasterClient *client;
    GeocluePosition *pos;
    QGeoPositionInfo lastPosition;
};

QTM_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCEMEEGO_H
