#include "qgeosatelliteinfosource_meego_p.h"
#include <gypsy/gypsy-control.h>
#include <gypsy/gypsy-device.h>
#include <gconf/gconf-client.h>

QTM_BEGIN_NAMESPACE

#ifdef Q_LOCATION_GYPSY_DEBUG
#include <QDebug>
#endif

// Callback function for 'satellites-changed' -signal
static void satellites_changed (GypsySatellite *satellite,
                                GPtrArray *satellites,
                                gpointer userdata)
{
#ifdef Q_LOCATION_GYPSY_DEBUG
    qDebug() << "Gypsy satellites-changed -signal received.";
#endif
    ((QGeoSatelliteInfoSourceMeego*)userdata)->satellitesChanged(satellite, satellites);
}

QGeoSatelliteInfoSourceMeego::QGeoSatelliteInfoSourceMeego(QObject *parent) : QGeoSatelliteInfoSource(parent)
{
}

void QGeoSatelliteInfoSourceMeego::satellitesChanged(GypsySatellite* satellite,
                                                     GPtrArray* satellites)
{
    Q_UNUSED(satellite)
    QList<QtMobility::QGeoSatelliteInfo> satellites_in_view;
    QList<QtMobility::QGeoSatelliteInfo> satellites_in_use;
    unsigned int i;
    for (i = 0; i < satellites->len; i++) {
        GypsySatelliteDetails *details = (GypsySatelliteDetails*)satellites->pdata[i];
        QGeoSatelliteInfo info;
        info.setAttribute(QGeoSatelliteInfo::Elevation, details->elevation);
        info.setAttribute(QGeoSatelliteInfo::Azimuth, details->azimuth);
        info.setPrnNumber(details->satellite_id);
        info.setSignalStrength(details->snr);
        if (details->in_use)
            satellites_in_use.append(info);
        satellites_in_view.append(info);
    }
    emit satellitesInUseUpdated(satellites_in_use);
    emit satellitesInViewUpdated(satellites_in_view);
}

int QGeoSatelliteInfoSourceMeego::init()
{
    GypsyDevice *device;
    GError *error = NULL;
    char *path;

    GConfClient* client;
    gchar* device_name;

    g_type_init ();
    client = gconf_client_get_default();
    device_name = gconf_client_get_string(client, "/apps/geoclue/master/org.freedesktop.Geoclue.GPSDevice", NULL);

    GypsyControl *control = NULL;
    control = gypsy_control_get_default ();
    path = gypsy_control_create (control, device_name, &error);
    if (path == NULL) {
        g_warning ("Error creating client for %s: %s", device_name,
                   error->message);
        g_error_free (error);
        return -1;
    }

    device = gypsy_device_new (path);
    satellite = gypsy_satellite_new (path);

    gypsy_device_start (device, &error);
    if (error != NULL) {
        g_warning ("Error starting %s: %s", device_name,
                   error->message);
        g_error_free (error);
        return -1;
    }
    return 0;
}


void QGeoSatelliteInfoSourceMeego::startUpdates()
{
    g_signal_connect (satellite, "satellites-changed",
                      G_CALLBACK (satellites_changed), this);
}


void QGeoSatelliteInfoSourceMeego::stopUpdates()
{

    g_signal_handlers_disconnect_by_func(G_OBJECT(satellite), (void*)satellites_changed,
                                         this);

}

void QGeoSatelliteInfoSourceMeego::requestUpdate(int timeout)
{
    Q_UNUSED(timeout)
}

#include "moc_qgeosatelliteinfosource_meego_p.cpp"
QTM_END_NAMESPACE

