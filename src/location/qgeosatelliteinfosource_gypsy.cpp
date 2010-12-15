#include "qgeosatelliteinfosource_gypsy_p.h"
#include <gypsy/gypsy-control.h>
#include <gypsy/gypsy-device.h>
#include <gconf/gconf-client.h>

#ifdef Q_LOCATION_GYPSY_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

#define UPDATE_TIMEOUT_COLD_START 120000

// Callback function for 'satellites-changed' -signal
static void satellites_changed (GypsySatellite *satellite,
                                GPtrArray *satellites,
                                gpointer userdata)
{
#ifdef Q_LOCATION_GYPSY_DEBUG
    qDebug() << "QGeoSatelliteInfoSourceGypsy Gypsy satellites-changed -signal received.";
#endif
    ((QGeoSatelliteInfoSourceGypsy*)userdata)->satellitesChanged(satellite, satellites);
}

QGeoSatelliteInfoSourceGypsy::QGeoSatelliteInfoSourceGypsy(QObject *parent) : QGeoSatelliteInfoSource(parent),
    m_satellite(0), m_device(0), m_updatesOngoing(false), m_requestOngoing(false)
{
    m_requestTimer.setSingleShot(true);
    QObject::connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
}

QGeoSatelliteInfoSourceGypsy::~QGeoSatelliteInfoSourceGypsy()
{
    GError* error = NULL;
    gypsy_device_stop (m_device, &error);
    if (error != NULL) {
        g_warning ("Error stopping the device: %s", error->message);
        g_object_unref(m_device);
        g_error_free (error);
    }
}

void QGeoSatelliteInfoSourceGypsy::satellitesChanged(GypsySatellite* satellite,
                                                     GPtrArray* satellites)
{
    if (!satellite || !satellites)
        return;
    // We have satellite data and assume it is valid.
    // If a single updateRequest was active, send signals right away.
    // If a periodic timer was running (meaning that the client wishes
    // to have updates at defined intervals), store the data for later sending.
    QList<QtMobility::QGeoSatelliteInfo> lastSatellitesInView;
    QList<QtMobility::QGeoSatelliteInfo> lastSatellitesInUse;

    unsigned int i;
    for (i = 0; i < satellites->len; i++) {
        GypsySatelliteDetails *details = (GypsySatelliteDetails*)satellites->pdata[i];
        QGeoSatelliteInfo info;
        info.setAttribute(QGeoSatelliteInfo::Elevation, details->elevation);
        info.setAttribute(QGeoSatelliteInfo::Azimuth, details->azimuth);
        info.setPrnNumber(details->satellite_id);
        info.setSignalStrength(details->snr);
        if (details->in_use)
            lastSatellitesInUse.append(info);
        lastSatellitesInView.append(info);
    }
    bool sendUpdates(false);
    // If a single updateRequest() has been issued:
    if (m_requestOngoing) {
        sendUpdates = true;
        m_requestTimer.stop();
        m_requestOngoing = false;
        // If there is no regular updates ongoing, disconnect now.
        if (!m_updatesOngoing)
            g_signal_handlers_disconnect_by_func(G_OBJECT(m_satellite), (void*)satellites_changed, this);
    }
    // If regular updates are to be delivered as they come:
    if (m_updatesOngoing)
        sendUpdates = true;

    if (sendUpdates) {
        emit satellitesInUseUpdated(lastSatellitesInUse);
        emit satellitesInViewUpdated(lastSatellitesInView);
    }
}

int QGeoSatelliteInfoSourceGypsy::init()
{
    GError *error = NULL;
    char *path;

    GConfClient* client;
    gchar* device_name;

    g_type_init ();
    client = gconf_client_get_default();
    device_name = gconf_client_get_string(client, "/apps/geoclue/master/org.freedesktop.Geoclue.GPSDevice", NULL);

    GypsyControl *control = NULL;
    control = gypsy_control_get_default ();
    // (path is the DBus path)
    path = gypsy_control_create (control, device_name, &error);
    if (path == NULL) {
        g_warning ("QGeoSatelliteInfoSourceGypsy error creating client for %s: %s", device_name,
                   error->message);
        g_error_free (error);
        g_object_unref(control);
        return -1;
    }

    m_device = gypsy_device_new (path);
    m_satellite = gypsy_satellite_new (path);

    gypsy_device_start (m_device, &error);
    if (error != NULL) {
        g_warning ("QGeoSatelliteInfoSourceGypsy error starting %s: %s", device_name,
                   error->message);
        g_object_unref(control);
        g_error_free (error);
        return -1;
    }
    g_object_unref(control);
    return 0;
}


void QGeoSatelliteInfoSourceGypsy::startUpdates()
{
    if (m_updatesOngoing)
        return;
    // If there is a request timer ongoing, we've connected to the signal already
    if (!m_requestTimer.isActive())
        g_signal_connect (m_satellite, "satellites-changed",
                          G_CALLBACK (satellites_changed), this);
    m_updatesOngoing = true;
}

void QGeoSatelliteInfoSourceGypsy::stopUpdates()
{
    if (!m_updatesOngoing)
        return;
    m_updatesOngoing = false;
    // Disconnect only if there is no single update request ongoing. Once single update request
    // is completed and it notices that there is no active update ongoing, it will disconnect
    // the signal.
    if (!m_requestTimer.isActive())
        g_signal_handlers_disconnect_by_func(G_OBJECT(m_satellite), (void*)satellites_changed, this);
}

void QGeoSatelliteInfoSourceGypsy::requestUpdate(int timeout)
{
    if (m_requestOngoing)
        return;
    if (timeout < 0) {
        emit requestTimeout();
        return;
    }
    m_requestOngoing = true;
    GError *error = 0;
    // If GPS has a fix a already, request current data.
    GypsyDeviceFixStatus fixStatus = gypsy_device_get_fix_status(m_device, &error);
    if (!error && (fixStatus != GYPSY_DEVICE_FIX_STATUS_INVALID &&
            fixStatus != GYPSY_DEVICE_FIX_STATUS_NONE)) {
#ifdef Q_LOCATION_GYPSY_DEBUG
        qDebug() << "QGeoSatelliteInfoSourceGypsy fix available, requesting current satellite data";
#endif
        GPtrArray* satelliteData = gypsy_satellite_get_satellites(m_satellite, &error);
        if (!error) {
            // The fix was available and we have satellite data to deliver right away.
            satellitesChanged(m_satellite, satelliteData);
            gypsy_satellite_free_satellite_array(satelliteData);
            return;
        }
    }
    // No fix is available. If updates are not ongoing already, start them.
    m_requestTimer.setInterval(timeout == 0? UPDATE_TIMEOUT_COLD_START: timeout);
    if (!m_updatesOngoing) {
        g_signal_connect (m_satellite, "satellites-changed",
                          G_CALLBACK (satellites_changed), this);
    }
    m_requestTimer.start();
    if (error) {
#ifdef Q_LOCATION_GYPSY_DEBUG
        qDebug() << "QGeoSatelliteInfoSourceGypsy error asking fix status or satellite data: " << error->message;
#endif
        g_error_free(error);
    }
}

void QGeoSatelliteInfoSourceGypsy::requestUpdateTimeout()
{
#ifdef Q_LOCATION_GYPSY_DEBUG
    qDebug("QGeoSatelliteInfoSourceGypsy request update timeout occured.");
#endif
    // If we end up here, there has not been valid satellite update.
    // Emit timeout and disconnect from signal if regular updates are not
    // ongoing (as we were listening just for one single requestUpdate).
    if (!m_updatesOngoing)
          g_signal_handlers_disconnect_by_func(G_OBJECT(m_satellite), (void*)satellites_changed, this);
    m_requestOngoing = false;
    emit requestTimeout();
}

#include "moc_qgeosatelliteinfosource_gypsy_p.cpp"
QTM_END_NAMESPACE
