#include <QtCore>

#ifdef Q_LOCATION_GEOCLUE_DEBUG
#include <QDebug>
#endif

#include "qgeopositioninfosource_geocluemaster_p.h"

QTM_BEGIN_NAMESPACE

#define MINIMUM_UPDATE_INTERVAL 1000

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
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "Position update from GeoClue master, lat, lon, alt, time, fields: " <<
                latitude << longitude << altitude << timestamp << fields;
#endif
    ((QGeoPositionInfoSourceGeoclueMaster*)userdata)->positionChanged(
                position, fields, timestamp, latitude, longitude, altitude, accuracy);
}

void QGeoPositionInfoSourceGeoclueMaster::positionChanged(GeocluePosition      *position,
                                                  GeocluePositionFields fields,
                                                  int                   timestamp,
                                                  double                latitude,
                                                  double                longitude,
                                                  double                altitude,
                                                  GeoclueAccuracy      *accuracy)
{
    Q_UNUSED(accuracy);
    Q_UNUSED(position);
    if ((fields & GEOCLUE_POSITION_FIELDS_LATITUDE) &&
            (fields & GEOCLUE_POSITION_FIELDS_LONGITUDE)) {
        if (m_requestTimer.isActive())
            m_requestTimer.stop();
        QGeoCoordinate coordinate(latitude, longitude);
        QDateTime dateTime = QDateTime();
        dateTime.setTime_t(timestamp);
        if (fields & GEOCLUE_POSITION_FIELDS_ALTITUDE) {
            coordinate.setAltitude(altitude);
        }
        QGeoPositionInfo info(coordinate, dateTime);
        if (info.isValid()) {
            m_lastPosition = info;
            emit positionUpdated(info);
        }
    }
}

QGeoPositionInfoSourceGeoclueMaster::QGeoPositionInfoSourceGeoclueMaster(QObject *parent)
    : QGeoPositionInfoSource(parent), m_updateInterval(0), m_preferredResources(GEOCLUE_RESOURCE_ALL),
      m_client(0), m_pos(0)
{
    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(updateRequestTimeout()));
}

QGeoPositionInfoSourceGeoclueMaster::~QGeoPositionInfoSourceGeoclueMaster()
{
    if (m_pos)
        g_object_unref (m_pos);
    if (m_client)
        g_object_unref (m_client);
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
    m_client = geoclue_master_create_client (master, NULL, &error);
    g_object_unref (master);

    if (!m_client) {
        qCritical ("QGeoPositionInfoSourceGeoclueMaster error creating GeoclueMasterClient: %s\n", error->message);
        g_error_free (error);
        return -1;
    }

    if (!geoclue_master_client_set_requirements (m_client,
                                                 GEOCLUE_ACCURACY_LEVEL_NONE, // min_accuracy
                                                 0,                           // min_time
                                                 TRUE,                        // require_updates (signals)
                                                 m_preferredResources,
                                                 &error)){
        qCritical ("QGeoPositionInfoSourceGeoclueMaster geoclue set_requirements failed: %s", error->message);
        g_error_free (error);
        g_object_unref (m_client);
        return -1;
    }
    m_pos = geoclue_master_client_create_position (m_client, NULL);
    if (!m_pos) {
        qCritical("QGeoPositionInfoSourceGeoclueMaster failed to get a position object");
        g_object_unref (m_client);
        return -1;
    }
    return 0;
}

// TODO
void QGeoPositionInfoSourceGeoclueMaster::setUpdateInterval(int msec)
{
    msec = (((msec > 0) && (msec < minimumUpdateInterval())) || msec < 0)? minimumUpdateInterval() : msec;
    QGeoPositionInfoSource::setUpdateInterval(msec);
    m_updateInterval = msec;
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
    g_signal_connect (G_OBJECT (m_pos), "position-changed",
                      G_CALLBACK (position_changed),this);

}
int QGeoPositionInfoSourceGeoclueMaster::minimumUpdateInterval() const {
    return MINIMUM_UPDATE_INTERVAL;
}
void QGeoPositionInfoSourceGeoclueMaster::stopUpdates()
{
    g_signal_handlers_disconnect_by_func(G_OBJECT(m_pos), (void*)position_changed,
                                         NULL);
}

void QGeoPositionInfoSourceGeoclueMaster::requestUpdate(int timeout)
{
    if (timeout < minimumUpdateInterval()) {
        emit updateTimeout();
        return;
    }
    if (m_requestTimer.isActive()) {
        return;
    }
    m_requestTimer.start(timeout);
    geoclue_position_get_position_async (m_pos, (GeocluePositionCallback)position_changed,this);
}

void QGeoPositionInfoSourceGeoclueMaster::updateRequestTimeout()
{
#ifdef Q_LOCATION_GEOCLUE_DEBUG
    qDebug() << "QGeoPositionInfoSourceGeoclueMaster requestUpdate timeout occured.";
#endif
    emit updateTimeout();
}


#include "moc_qgeopositioninfosource_geocluemaster_p.cpp"
QTM_END_NAMESPACE
