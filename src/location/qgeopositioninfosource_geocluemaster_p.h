#ifndef QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
#define QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H

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

class QGeoPositionInfoSourceGeoclueMaster : public QGeoPositionInfoSource
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
    QGeoPositionInfoSourceGeoclueMaster(QObject *parent = 0);
    ~QGeoPositionInfoSourceGeoclueMaster();

    // From QGeoPositionInfoSource
    void setUpdateInterval(int msec);
    QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;
    PositioningMethods supportedPositioningMethods() const;
    void setPreferredPositioningMethods(PositioningMethods methods);
    int minimumUpdateInterval() const;
    int init();

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout = 5000);

private:
    int configurePositionSource();

private:
    int m_updateInterval;
    GeoclueResourceFlags m_preferredResources;
    GeoclueMasterClient *m_client;
    GeocluePosition *m_pos;
    QGeoPositionInfo m_lastPosition;
};

QTM_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
