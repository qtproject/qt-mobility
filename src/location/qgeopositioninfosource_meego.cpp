#include <QtCore>

#ifdef Q_LOCATION_GEOCLUE_DEBUG
#include <QDebug>
#endif

#include "qgeopositioninfosource_meego_p.h"

QTM_BEGIN_NAMESPACE

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
    qDebug() << "Position update from GeoClue, lat, lon, alt, time: " << latitude << longitude << altitude << time;
#endif
    ((QGeoPositionInfoSourceMeego*)userdata)->positionChanged(
                position, fields, timestamp, latitude, longitude, altitude, accuracy);
}

void QGeoPositionInfoSourceMeego::positionChanged(GeocluePosition      *position,
                                                  GeocluePositionFields fields,
                                                  int                   timestamp,
                                                  double                latitude,
                                                  double                longitude,
                                                  double                altitude,
                                                  GeoclueAccuracy      *accuracy)
{
    QGeoCoordinate coordinate(latitude, longitude);
    QDateTime dateTime = QDateTime();
    dateTime.setTime_t(timestamp);
    QGeoPositionInfo info(coordinate, dateTime);
    if (info.isValid()) {
        lastPosition = info;
        emit positionUpdated(info);
    }
}

QGeoPositionInfoSourceMeego::QGeoPositionInfoSourceMeego(QObject *parent)
    : QGeoPositionInfoSource(parent), client(0), pos(0)
{
}

int QGeoPositionInfoSourceMeego::init()
{
    GeoclueMaster *master;
    GError *error = NULL;

    g_type_init ();

    master = geoclue_master_get_default ();
    client = geoclue_master_create_client (master, NULL, &error);
    g_object_unref (master);

    if (!client) {
        qCritical ("QGeoPositionInfoSourceMeego error creating GeoclueMasterClient: %s\n", error->message);
        g_error_free (error);
        return -1;
    }

    if (!geoclue_master_client_set_requirements (client,
                                                 GEOCLUE_ACCURACY_LEVEL_NONE,
                                                 0, TRUE,
                                                 GEOCLUE_RESOURCE_ALL,
                                                 &error)){
        qCritical ("QGeoPositionInfoSourceMeego geoclue set_requirements failed: %s", error->message);
        g_error_free (error);
        g_object_unref (client);
        return -1;

    }

    pos = geoclue_master_client_create_position (client, NULL);
    if (!pos) {
        qCritical("QGeoPositionInfoSourceMeego failed to get a position object");
        g_object_unref (client);
        return -1;
    }

    return 0;
}

QGeoPositionInfo QGeoPositionInfoSourceMeego::lastKnownPosition
(bool /*fromSatellitePositioningMethodsOnly*/) const
{
    return lastPosition;
}

QGeoPositionInfoSourceMeego::PositioningMethods QGeoPositionInfoSourceMeego::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

void QGeoPositionInfoSourceMeego::startUpdates()
{
    g_signal_connect (G_OBJECT (pos), "position-changed",
                      G_CALLBACK (position_changed),this);

}
int QGeoPositionInfoSourceMeego::minimumUpdateInterval() const{
    return 0;
}
void QGeoPositionInfoSourceMeego::stopUpdates()
{
    g_signal_handlers_disconnect_by_func(G_OBJECT(pos), (void*)position_changed,
                                         NULL);
}

void QGeoPositionInfoSourceMeego::requestUpdate(int /*timeout*/)
{
    GeocluePositionFields fields;
    int              timestamp;
    double           latitude;
    double           longitude;
    double           altitude;
    GeoclueAccuracy *accuracy;
    GError *error = NULL;
    fields = geoclue_position_get_position (pos, &timestamp,
                                            &latitude, &longitude, &altitude,
                                            &accuracy, &error);
    if (error) {
        qCritical ("QGeoPositionInfoSourceMeego Error in geoclue_position_get_position: %s.\n",
                   error->message);
        g_error_free (error);
        error = NULL;
        emit updateTimeout();
    } else {
        if (fields & GEOCLUE_POSITION_FIELDS_LATITUDE &&
                fields & GEOCLUE_POSITION_FIELDS_LONGITUDE) {
            qDebug ("We're at %.3f, %.3f.\n", latitude, longitude);
            positionChanged(pos, fields, timestamp, latitude, longitude, altitude,
                            accuracy);
            return;

        }
        qCritical ("QGeoPositionInfoSourceMeego Invalid Longitude and latitude");
        emit updateTimeout();
    }
}

QGeoPositionInfoSourceMeego::~QGeoPositionInfoSourceMeego()
{
    if (pos)
        g_object_unref (pos);
    if (client)
        g_object_unref (client);
}

#include "moc_qgeopositioninfosource_meego_p.cpp"
QTM_END_NAMESPACE
