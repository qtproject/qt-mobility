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
#include <geoclue/geoclue-velocity.h>
#include <QTimer>

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

    void singleUpdateFailed();
    void singleUpdateSucceeded(QGeoPositionInfo info);
    void regularUpdateFailed();
    void regularUpdateSucceeded(QGeoPositionInfo info);
    void velocityUpdateFailed();
    void velocityUpdateSucceeded(double speed);

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();
    virtual void requestUpdate(int timeout = 5000);

private slots:
    void requestUpdateTimeout();
    void startUpdatesTimeout();

private:
    int configurePositionSource();

private:
    int m_updateInterval;
    GeoclueResourceFlags m_preferredResources;
    GeoclueMasterClient *m_client;
    GeocluePosition *m_pos;
    GeoclueVelocity* m_vel;
    QTimer m_updateTimer;
    QTimer m_requestTimer;
    bool m_lastPositionIsFresh;
    bool m_lastVelocityIsFresh;
    double m_lastVelocity;
    QGeoPositionInfo m_lastPosition;
};

QTM_END_NAMESPACE

#endif // QGEOPOSITIONINFOSOURCE_GEOCLUEMASTER_H
