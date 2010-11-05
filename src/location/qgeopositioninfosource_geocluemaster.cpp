#include <QtCore>

#ifdef Q_LOCATION_GEOCLUE_DEBUG
#include <QDebug>
#endif

#include "qgeopositioninfosource_geocluemaster_p.h"

QTM_BEGIN_NAMESPACE

#define MINIMUM_UPDATE_INTERVAL 1000
#define UPDATE_TIMEOUT_COLD_START 120000

// Helper function to convert data into a QGeoPositionInfo
static QGeoPositionInfo geoclueToPositionInfo(GeocluePositionFields fields,
                                               int                   timestamp,
                                               double                latitude,
                                               double                longitude,
                                               double                altitude)
{
    QGeoCoordinate coordinate(latitude, longitude);
    QDateTime dateTime = QDateTime();
    dateTime.setTime_t(timestamp);
    if (fields & GEOCLUE_POSITION_FIELDS_ALTITUDE) {
        coordinate.setAltitude(altitude);
    }
    return QGeoPositionInfo(coordinate, dateTime);
}

// Callback for position-changed -signal
static void position_changed (GeocluePosition      *position,
                              GeocluePositionFields fields,
                              int                   timestamp,
                              double                latitude,
                              double                longitude,
                              double                altitude,
                              GeoclueAccuracy      *accuracy,
                              gpointer              userdata) // Ptr to this
{
    Q_UNUSED(position);
    Q_UNUSED(accuracy);
    if (!(fields & GEOCLUE_POSITION_FIELDS_LATITUDE &&
          fields & GEOCLUE_POSITION_FIELDS_LONGITUDE)) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "Position-changed from GeoClue master failed.";
#endif
        static_cast<QGeoPositionInfoSourceGeoclueMaster*>(userdata)->regularUpdateFailed();
    } else {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "Position-changed from GeoClue master, lat, lon, alt, time, fields: " <<
                latitude << longitude << altitude << timestamp << fields;
#endif
        static_cast<QGeoPositionInfoSourceGeoclueMaster*>(userdata)->regularUpdateSucceeded(
                    geoclueToPositionInfo(fields, timestamp, latitude, longitude, altitude));
    }
}

// Callback for velocity-changed -signal
static void velocity_changed (GeoclueVelocity *velocity,
                              GeoclueVelocityFields fields,
                              int timestamp,
                              double speed,
                              double direction,
                              double climb,
                              gpointer userdata) // Ptr to this
{
    Q_UNUSED(velocity)
    Q_UNUSED(timestamp)
    Q_UNUSED(direction)
    Q_UNUSED(climb)
    if (!(fields & GEOCLUE_VELOCITY_FIELDS_SPEED)) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "Velocity update from geoclue failed.";
#endif
        static_cast<QGeoPositionInfoSourceGeoclueMaster*>(userdata)->velocityUpdateFailed();
        return;
    }
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "Velocity changed, speed: " << speed;
#endif
    static_cast<QGeoPositionInfoSourceGeoclueMaster*>(userdata)->velocityUpdateSucceeded(speed);
}

// Callback for single async update
static void position_callback (GeocluePosition      *pos,
                   GeocluePositionFields fields,
                   int                   timestamp,
                   double                latitude,
                   double                longitude,
                   double                altitude,
                   GeoclueAccuracy      *accuracy,
                   GError               *error,
                   gpointer              userdata)
{
    Q_UNUSED(pos);
    Q_UNUSED(accuracy);
    if (error || !(fields & GEOCLUE_POSITION_FIELDS_LATITUDE &&
                   fields & GEOCLUE_POSITION_FIELDS_LONGITUDE)) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug("QGeoPositionInfoSourceGeoClueMaster error getting single update.");
#endif
        static_cast<QGeoPositionInfoSourceGeoclueMaster*>(userdata)->singleUpdateFailed();
        if (error)
            g_error_free (error);
    } else {
        static_cast<QGeoPositionInfoSourceGeoclueMaster*>(userdata)->singleUpdateSucceeded(
                    geoclueToPositionInfo(fields, timestamp, latitude, longitude, altitude));
    }
}

QGeoPositionInfoSourceGeoclueMaster::QGeoPositionInfoSourceGeoclueMaster(QObject *parent)
    : QGeoPositionInfoSource(parent), m_updateInterval(0), m_preferredResources(GEOCLUE_RESOURCE_ALL),
      m_client(0), m_pos(0), m_vel(0), m_lastPositionIsFresh(false), m_lastVelocityIsFresh(false),
      m_lastVelocity(0)
{
    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
    QObject::connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(startUpdatesTimeout()));
}

QGeoPositionInfoSourceGeoclueMaster::~QGeoPositionInfoSourceGeoclueMaster()
{
    if (m_pos)
        g_object_unref (m_pos);
    if (m_vel)
        g_object_unref(m_vel);
    if (m_client)
        g_object_unref (m_client);
}

void QGeoPositionInfoSourceGeoclueMaster::velocityUpdateFailed()
{
    // Set the velocitydata non-fresh.
    m_lastVelocityIsFresh = false;
}

void QGeoPositionInfoSourceGeoclueMaster::velocityUpdateSucceeded(double speed)
{
    // Store the velocity and mark it as fresh. Simple but hopefully adequate.
    m_lastVelocity = speed;
    m_lastVelocityIsFresh = true;
}

void QGeoPositionInfoSourceGeoclueMaster::singleUpdateFailed()
{
    if (m_requestTimer.isActive())
        m_requestTimer.stop();
    // Send timeout even if time wasn't up yet, because we are not trying again
    emit updateTimeout();
}

void QGeoPositionInfoSourceGeoclueMaster::singleUpdateSucceeded(QGeoPositionInfo info)
{
    if (m_requestTimer.isActive())
        m_requestTimer.stop();
    if (m_lastVelocityIsFresh) {
        info.setAttribute(QGeoPositionInfo::GroundSpeed, m_lastVelocity); // assume groundspeed
        emit positionUpdated(info);
    }
}

void QGeoPositionInfoSourceGeoclueMaster::regularUpdateFailed()
{
    // Emit timeout and keep on listening in case error condition clears.
    // Currently this is emitted each time an error occurs, and thereby it assumes
    // that there does not come many erroneous updates from position source.
    // This assumption may be invalid.
    m_lastVelocityIsFresh = false;
    m_lastPositionIsFresh = false;
    emit updateTimeout();
}

void QGeoPositionInfoSourceGeoclueMaster::regularUpdateSucceeded(QGeoPositionInfo info)
{
    m_lastPosition = info;
    m_lastPositionIsFresh = true;
    // If a non-intervalled startUpdates has been issued, send an update.
    if (!m_updateTimer.isActive()) {
        m_lastPositionIsFresh = false;
        if (m_lastVelocityIsFresh) {
            info.setAttribute(QGeoPositionInfo::GroundSpeed, m_lastVelocity); // assume groundspeed
            m_lastVelocityIsFresh = false;
        }
        emit positionUpdated(info);
    }
}

int QGeoPositionInfoSourceGeoclueMaster::init()
{
    g_type_init ();
    return configurePositionSource();
}

int QGeoPositionInfoSourceGeoclueMaster::configurePositionSource()
{
    GeoclueMaster *master(0);
    GError *error = 0;

    master = geoclue_master_get_default ();
    if (!master) {
        qCritical ("QGeoPositionInfoSourceGeoclueMaster error creating GeoclueMaster");
        return -1;
    }
    // Free potential previous sources, because new requirements can't be set for the client
    // (creating a position object after changing requirements seems to fail).
    if (m_client) {
        g_object_unref (m_client);
        m_client = 0;
    }
    if (m_pos) {
        g_object_unref(m_pos);
        m_client = 0;
    }
    if (m_vel) {
        g_object_unref(m_vel);
        m_vel = 0;
    }
    m_client = geoclue_master_create_client (master, NULL, &error);
    g_object_unref (master);

    if (!m_client) {
        qCritical ("QGeoPositionInfoSourceGeoclueMaster error creating GeoclueMasterClient.");
        if (error) {
            qCritical (error->message);
            g_error_free (error);
        }
        return -1;
    }

    if (!geoclue_master_client_set_requirements (m_client,
                                                 GEOCLUE_ACCURACY_LEVEL_NONE, // min_accuracy
                                                 0,                           // min_time
                                                 TRUE,                        // require_updates (signals)
                                                 m_preferredResources,
                                                 &error)){
        qCritical ("QGeoPositionInfoSourceGeoclueMaster geoclue set_requirements failed.");
        if (error) {
            qCritical (error->message);
            g_error_free (error);
        }
        g_object_unref (m_client);
        m_client = 0;
        return -1;
    }
    m_pos = geoclue_master_client_create_position (m_client, NULL);
    if (!m_pos) {
        qCritical("QGeoPositionInfoSourceGeoclueMaster failed to get a position object");
        g_object_unref (m_client);
        m_client = 0;
        return -1;
    }
    // Succeeding velocity is not mandatory. Master does not provide abstraction
    // for velocity provider, hence request Gypsy provider directly.
    m_vel = geoclue_velocity_new("org.freedesktop.Geoclue.Providers.Gypsy",
                                 "/org/freedesktop/Geoclue/Providers/Gypsy");
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    if (m_vel == NULL)
        qDebug("QGeoPositionInfoSourceGeoclueMaster velocity provider not available.");
#endif
    return 0;
}

void QGeoPositionInfoSourceGeoclueMaster::setUpdateInterval(int msec)
{
    msec = (((msec > 0) && (msec < minimumUpdateInterval())) || msec < 0)? minimumUpdateInterval() : msec;
    QGeoPositionInfoSource::setUpdateInterval(msec);
    m_updateInterval = msec;
    // If update timer is running, set the new interval
    if (m_updateTimer.isActive()) {
        m_updateTimer.setInterval(msec);
    }
}

void QGeoPositionInfoSourceGeoclueMaster::setPreferredPositioningMethods(PositioningMethods methods)
{
    switch (methods) {
    case SatellitePositioningMethods:
        qDebug() << "satellite requested";
        m_preferredResources = GEOCLUE_RESOURCE_GPS;
        qDebug() << "geoclue counterpart is:" << GEOCLUE_RESOURCE_GPS;
        break;
    case NonSatellitePositioningMethods:
        qDebug() << "non-satellite requested";
        m_preferredResources = (GeoclueResourceFlags)(GEOCLUE_RESOURCE_CELL | GEOCLUE_RESOURCE_NETWORK);
        qDebug() << "geoclue counterpart is:" << (GEOCLUE_RESOURCE_CELL | GEOCLUE_RESOURCE_NETWORK);
        break;
    case AllPositioningMethods:
        qDebug() << "all requested";
        m_preferredResources = GEOCLUE_RESOURCE_ALL;
        qDebug() << "geoclue counterpart is:" << (GEOCLUE_RESOURCE_ALL);
        break;
    default:
        qWarning("GeoPositionInfoSourceGeoClueMaster unknown preferred method.");
        break;
    }
    QGeoPositionInfoSource::setPreferredPositioningMethods(methods);
    qDebug() << "GC Master, requested to set methods to, and set them to:" << methods << m_preferredResources;
    configurePositionSource();
}

QGeoPositionInfo QGeoPositionInfoSourceGeoclueMaster::lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly*/) const
{
    return m_lastPosition;
}

QGeoPositionInfoSourceGeoclueMaster::PositioningMethods QGeoPositionInfoSourceGeoclueMaster::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

void QGeoPositionInfoSourceGeoclueMaster::startUpdates()
{
    if (m_updateTimer.isActive())
        return;
    if (m_updateInterval > 0) {
#ifdef Q_LOCATION_GEOCLUE_DEBUG
        qDebug() << "QGeoPositionInfoSourceGeoclueMaster startUpdates with interval: " << m_updateInterval;
#endif
        m_updateTimer.start(m_updateInterval);
    }
    g_signal_connect (G_OBJECT (m_pos), "position-changed",
                      G_CALLBACK (position_changed),this);
    if (m_vel) {
        g_signal_connect (G_OBJECT (m_vel), "velocity-changed",
                          G_CALLBACK (velocity_changed),this);
    }
}

int QGeoPositionInfoSourceGeoclueMaster::minimumUpdateInterval() const {
    return MINIMUM_UPDATE_INTERVAL;
}

void QGeoPositionInfoSourceGeoclueMaster::stopUpdates()
{
    g_signal_handlers_disconnect_by_func(G_OBJECT(m_pos), (void*)position_changed, this);
    g_signal_handlers_disconnect_by_func(G_OBJECT(m_vel), (void*)velocity_changed, this);
    if (m_updateTimer.isActive())
        m_updateTimer.stop();
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdate(int timeout)
{
    if (timeout < minimumUpdateInterval() && timeout != 0) {
        emit updateTimeout();
        return;
    }
    if (m_requestTimer.isActive()) {
        return;
    }
    // Create better logic for timeout value (specs leave it impl dependant).
    // Especially if there are active updates ongoing, there is no point of waiting
    // for whole cold start time.
    if (timeout == 0)
        m_requestTimer.start(UPDATE_TIMEOUT_COLD_START);
    else
        m_requestTimer.start(timeout);
    geoclue_position_get_position_async (m_pos, (GeocluePositionCallback)position_callback,this);
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdateTimeout()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster requestUpdate timeout occured.";
#endif
    // If we end up here, there has not been valid position update.
    emit updateTimeout();
}

void QGeoPositionInfoSourceGeoclueMaster::startUpdatesTimeout()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster startUpdates timeout occured.";
#endif
    // Check if there are position updates since last positionUpdated().
    // Do not however send timeout, that's reserved for signaling errors.
    if (m_lastPositionIsFresh) {
        emit positionUpdated(m_lastPosition);
        m_lastPositionIsFresh = false;
        m_lastVelocityIsFresh = false;
    }
}

#include "moc_qgeopositioninfosource_geocluemaster_p.cpp"
QTM_END_NAMESPACE
