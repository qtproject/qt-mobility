#include "gypsymock.h"

#include <QTimerEvent>
#include <QDebug>
#include <QMetaObject>

#ifdef TST_GYPSY_MOCK_TRACE
#define TRACE0 qDebug() << "=GypsyMock=" << __FUNCTION__
#define TRACE1(STR1) TRACE0 << STR1;
#define TRACE2(STR1,STR2) TRACE0 << STR1 << STR2;
#define TRACE3(STR1, STR2, STR3) TRACE0 << STR1 << STR2 << STR3;
#else
#define TRACE0
#define TRACE1
#define TRACE2
#define TRACE3
#endif

#define TEST_DEVICE_NAME "/my/little/testdevice"
#define TEST_GYPSY_CONTROL "/my/little/gypsycontrol"
// TODO all calls to gypsymock should be done by calling slots to ensure
// synchronization (the mocked library functions call mockend directly even
// though its in different thread). now going a bit dangerously, but seems to work..
// TODO also currently only one concurrent user is allowed. Should be fairly
// straightforward to make support for multiple concurrents;
// TODO make currentMockend and GypsyDevice one and the same
static GypsyMock* lg_currentMockend = 0;
static QString lg_currentJournal = ":/data/gypsymock_basic_sat.journal";
static GypsyControl* lg_currentGypsyControl = 0; //zz
static GypsyDevice* lg_currentGypsyDevice = 0; // zzz
static GypsySatellite* lg_currentGypsySatellite = 0; // zzz
static GConfClient* lg_currentGConfClient = 0; // zzz

void g_type_init (void)
{
    TRACE0;
}

void g_object_unref(gpointer object) {
    TRACE0;
    // Check that unreferenced object exists
    if (object == lg_currentGypsyControl) {
        TRACE1(" unref for gypsy control. ");
        delete lg_currentGypsyControl; lg_currentGypsyControl = 0;
    } else if (object == lg_currentGypsyDevice) {
        TRACE1(" unref for gypsy device, deleting also mockend. ");
        delete lg_currentGypsyDevice; lg_currentGypsyDevice = 0;
        delete lg_currentMockend; lg_currentMockend = 0;
    } else if (object == lg_currentGypsySatellite) {
        TRACE1(" unref for gypsy satellite. ");
        delete lg_currentGypsySatellite; lg_currentGypsySatellite = 0;
    } else if (object == lg_currentGConfClient) {
        delete lg_currentGConfClient; lg_currentGConfClient = 0;
        TRACE1(" unref for gconf client ");
    } else {
        TRACE1("warning unref'd nonexistent!");
        Q_ASSERT(false);
    }
}

void     g_error_free          (GError *error)
{
    TRACE0;
    Q_ASSERT(error);
    // todo is this adequate
    delete error;
}

gulong	 g_signal_connect_data		      (gpointer		  instance,
                                               const gchar	 *detailed_signal,
                                               GCallback	  c_handler,
                                               gpointer		  data,
                                               GClosureNotify	  destroy_data,
                                               GConnectFlags	  connect_flags)
{
    TRACE0;
    Q_UNUSED(instance)
    Q_UNUSED(destroy_data)
    Q_UNUSED(connect_flags)
    Q_ASSERT(lg_currentMockend);
    if (QString::compare(QString::fromAscii(detailed_signal), "satellites-changed")) {
        TRACE1("erroneous callback requested.");
        return 0; // dummy
    }
    // lg_currentMockend->setSatelliteSource(static_cast<QObject*>(data));
    int retval;
    QMetaObject::invokeMethod(lg_currentMockend, "setSatelliteSource", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, retval),
                              Q_ARG(int, (int)data));

    //lg_currentMockend->setSatellitesChangedCallback(c_handler);
    QMetaObject::invokeMethod(lg_currentMockend, "setSatellitesChangedCallback", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, retval),
                              Q_ARG(int, (int)c_handler));
    return 1; // dummy
}

guint	 g_signal_handlers_disconnect_matched (gpointer		  instance,
                                               GSignalMatchType	  mask,
                                               guint		  signal_id,
                                               GQuark		  detail,
                                               GClosure		 *closure,
                                               gpointer		  func,
                                               gpointer		  data)
{
    TRACE0;
    Q_ASSERT(lg_currentMockend);
    Q_UNUSED(instance)
    Q_UNUSED(mask)
    Q_UNUSED(signal_id)
    Q_UNUSED(detail)
    Q_UNUSED(closure)
    Q_UNUSED(func)
    Q_UNUSED(data)

    // lg_currentMockend->disconnectSignal((int)func,(int)data);
    int retval;
    QMetaObject::invokeMethod(lg_currentMockend, "disconnectSignal", Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(int, retval),
                              Q_ARG(int, (int)func),
                              Q_ARG(int, (int)data));
    return 1; // dummy
}

GypsyControl *gypsy_control_get_default (void)
{
    TRACE0;
    if (lg_currentGypsyControl)
        TRACE1("warning, gypsy control already existed, memory will leak.");
    lg_currentGypsyControl = new GypsyControl;
    return lg_currentGypsyControl;
    // kept for future refernce
    // return (GypsyControl*)g_object_new (G_TYPE_OBJECT, NULL); // dummy
}
// gypsy_control_create
char *gypsy_control_create (GypsyControl *control,
                            const char   *device_name,
                            GError      **error)
{
    TRACE1(device_name);
    if (!control || QString::compare(QString::fromAscii(device_name),TEST_DEVICE_NAME)) {
        if (error) {
            *error = new GError;
            (*error)->message = (char*)"message"; // hmmm...
        }
        /* Kept for future example reference
        if (error) {
            *error = g_error_new ((GQuark)1, // dummy
                                  2,         // dummy
                                  "=mock= No control or wrong device_name.");
        }
        */
        return NULL;
    }
    return (char*)TEST_GYPSY_CONTROL;
}
// gypsy_device_new
GypsyDevice *gypsy_device_new (const char *object_path)
{
    TRACE0;
    if (!object_path || QString::compare(QString::fromAscii(object_path), TEST_GYPSY_CONTROL)) {
        TRACE1("No correctobject path given or mockend already existed.");
        return NULL;
    }
    if (lg_currentGypsyDevice)
        TRACE1("warning, gypsy device already existed, memory will leak.");
    lg_currentGypsyDevice = new GypsyDevice;
    lg_currentMockend = new GypsyMock;
    return lg_currentGypsyDevice;
    //return (GypsyDevice*)g_object_new (G_TYPE_OBJECT, NULL);
}
// gypsy_satellite_new
GypsySatellite *gypsy_satellite_new (const char *object_path)
{
    TRACE1(object_path);
    if (!object_path || QString::compare(QString::fromAscii(object_path), TEST_GYPSY_CONTROL)) {
        return NULL;
    }
    if (lg_currentGypsySatellite)
        TRACE1("warning, satellite already existed, memory will leak.");
    lg_currentGypsySatellite = new GypsySatellite;
    return lg_currentGypsySatellite;
    //return (GypsySatellite*)g_object_new (G_TYPE_OBJECT, NULL);
}
// gypsy_device_start
gboolean gypsy_device_start (GypsyDevice *device,
                             GError     **error)
{
    TRACE0;
    if (!device || !lg_currentMockend) {
        // todo zzz
        //if (error) {
        //    *error = g_error_new ((GQuark)1, // dummy
        //                          2,         // dummy
        //                          "=mock= No device provided or mockend not created at gypsy_device_start");
        //}
        return false;
    }
    // start the mockend
    Q_ASSERT(lg_currentMockend);
    //lg_currentMockend->start();
    QMetaObject::invokeMethod(lg_currentMockend, "start", Qt::BlockingQueuedConnection);
    return true;
}

gboolean gypsy_device_stop (GypsyDevice *device,
                             GError     **error)
{
    TRACE0;
    if (!device || !lg_currentMockend) {
        // todo zzz
        //if (error) {
        //    *error = g_error_new ((GQuark)1, // dummy
        //                          2,         // dummy
        //                          "=mock= No device provided or mockend not created at gypsy_device_stop");
        //}
        return false;
    }
    // stop current mockend.
    Q_ASSERT(lg_currentMockend);
    // lg_currentMockend->stop();
    QMetaObject::invokeMethod(lg_currentMockend, "stop", Qt::BlockingQueuedConnection);
    return true;
}

// gypsy_device_get_fix_status
GypsyDeviceFixStatus gypsy_device_get_fix_status (GypsyDevice *device,
                                                  GError      **error)
{
    TRACE0;
    if (!device) {
        // todo
        // if (error) {
        //    *error = g_error_new ((GQuark)1, // dummy
        //                          2,         // dummy
        //                          "=mock= No device provided for gypsy_device_get_fix_status");
        //}
        return GYPSY_DEVICE_FIX_STATUS_INVALID;
    }
    return GYPSY_DEVICE_FIX_STATUS_2D;
}

// gypsy_satellite_get_satellites
GPtrArray *gypsy_satellite_get_satellites (GypsySatellite *satellite,
                                           GError        **error)
{
    TRACE0;
    if (!satellite) {
        // todo
        // if (error) {
        //    *error = g_error_new ((GQuark)1, // dummy
        //                          2,         // dummy
        //                          "=mock= No satellite provided for gypsy_satellite_get_satellites");
        // }
        return NULL;
    }
    return (GPtrArray*)1; // todo
}

// gypsy_satellite_free_satellite_array
void gypsy_satellite_free_satellite_array (GPtrArray *satellites)
{
    TRACE0;
    Q_ASSERT(satellites);
    for (uint i = 0; i < satellites->len; i++) {
        delete (GypsySatelliteDetails*)satellites->pdata[i];
    }
    delete satellites->pdata;
    delete satellites;
    /* kept for future  reference
    for (uint i = 0; i < satellites->len; i++) {
        delete (GypsySatelliteDetails*)g_ptr_array_index(satellites, i);
    }
    g_ptr_array_free(satellites, true);
    */
}

GPtrArray* g_ptr_array_new()
{
    TRACE0;
    qDebug("1");
    // GPtrArray* array = new GPtrArray;
    GPtrArray* array = new GPtrArray;
    qDebug("2");
    array->len = 0;
    qDebug("3");
    *(array->pdata) = malloc(sizeof(void*) * 100); // enough
    qDebug("4");
    return array;
}

GConfClient* gconf_client_get_default(void)
{
    TRACE0;
    if (lg_currentGConfClient)
        TRACE1("warning, GConfClient already existed, memory will leak.");
    lg_currentGConfClient = new GConfClient;
    return lg_currentGConfClient;
    //return (GConfClient*)g_object_new (G_TYPE_OBJECT, NULL);
}

gchar* gconf_client_get_string(GConfClient* client,
                               const gchar* key,
                               GError** err)
{
    TRACE1(key);
    if (!client || QString::compare(QString::fromAscii(key),"/apps/geoclue/master/org.freedesktop.Geoclue.GPSDevice")) {
        // todo
        //if (err) {
        //    *err = g_error_new ((GQuark)1, // dummy
        //                          2,         // dummy
        //                          "=mock= No client or wrong key in gconf_client_get_string");
        //}
        return NULL;
    } else {
        return (gchar*)TEST_DEVICE_NAME;
    }
}

// These symbols override the symbols in the actual gypsy library;
// they are used to mock the backend.
GypsyMock::GypsyMock() :
    m_satelliteSource(0),
    m_satellitesChangedCallback(0)
{
    TRACE0;
    g_type_init();
    lg_currentMockend = this;
    this->moveToThread(&m_thread);
    m_thread.start();

    m_regular.satellites = 0;
}

GypsyMock::~GypsyMock()
{
    lg_currentMockend = 0;
    if (m_regular.satellites) {
        gypsy_satellite_free_satellite_array(m_regular.satellites);
    }
}

void GypsyMock::setSatelliteSource(int satelliteSource)
{
    TRACE0;
    m_satelliteSource = (QObject*)satelliteSource;
}

void GypsyMock::start()
{
    TRACE0;
    Q_ASSERT(QFile::exists(lg_currentJournal));
    // stop(); // just in case zzz re-enable this
    // If file is already open, continue reading it.
    if (!m_currentJournalFile.isOpen()) {
        m_currentJournalFile.setFileName(lg_currentJournal);
        if (!m_currentJournalFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "Journal opening failed:" << m_currentJournalFile.fileName();
            Q_ASSERT(false);
        }
    }
    TRACE1("calling read next etnry");
    UPDATE_TYPE update = readNextEntry();
    TRACE1("done calling read next etnry");
    if (update == REGULAR_UPDATE) {
        TRACE2("Starting timer id: ", m_regular.timerId);
        m_regular.timerId = startTimer(m_regular.timeout);
    } else {
        Q_ASSERT(false); // journal ran out
    }
}

void GypsyMock::stop()
{
    TRACE0;
    killTimer(m_regular.timerId);
    m_regular.timerId = 0;
    if (m_currentJournalFile.isOpen())
        m_currentJournalFile.close();
    m_satellitesChangedCallback = 0;
    m_satelliteSource = 0;
}

void GypsyMock::timerEvent(QTimerEvent *event)
{
    TRACE0;
    if (!m_satelliteSource) {
        TRACE1("No interested satellite sources, dropped.");
        return;
    }

    if (event->timerId() == m_regular.timerId) {
        // todo deliver the event

        if (m_regular.repeats > 0) {
            m_regular.repeats--;
            return;
        }
        killTimer(m_regular.timerId);
    } else {
        TRACE2("timer astray, killing and ignoring:", event->timerId());
        killTimer(event->timerId()); // Something has gone astray, kill and ignore
        return;
    }
    // no more repeats
    UPDATE_TYPE update = readNextEntry();
    if (update == REGULAR_UPDATE) {
        TRACE2("Starting timer id: ", m_regular.timerId);
        m_regular.timerId = startTimer(m_regular.timeout);
    } else {
        Q_ASSERT(false); // journal ran out
    }
}

GypsyMock::UPDATE_TYPE GypsyMock::readNextEntry()
{
    QString line;
    QTextStream stream;
    do {
        line = m_currentJournalFile.readLine();
        //line = line.trimmed();
        TRACE2("parsing:", line);
        if (line.at(0) == '#' || line.at(0) == '\n') {
            continue;
        }
        else if (line == "START_REGULAR_SAT_UPDATE\n") {
            line = m_currentJournalFile.readLine();
            TRACE2("parsing:", line);
            stream.setString(&line);
            // initialize datastructs (release any previous data)
            if (m_regular.satellites)
                gypsy_satellite_free_satellite_array(m_regular.satellites);
            m_regular.satellites = g_ptr_array_new();
            parseRegularUpdate(stream);
            continue;
        }
        else if (line == "SATELLITE\n") {
            // read single satellite data
            line = m_currentJournalFile.readLine();
            TRACE2("parsing:", line);
            stream.setString(&line);
            parseSatelliteData(stream);
            continue; // read next satellite or end of line
        }
        else if (line == "END_REGULAR_SAT_UPDATE\n") {
            TRACE1("data parsed, updates should start.");
            return REGULAR_UPDATE; // read data for one update round
        }
        else {
            qCritical("Invalid input file for gypsy mocking.");
            Q_ASSERT(false);
        }
    } while (line.length() > 0);
    qDebug("returning invalid_update");
    return INVALID_UPDATE;
}

void GypsyMock::parseRegularUpdate(QTextStream& stream)
{
   TRACE0;
   stream >> m_regular.repeats >> m_regular.timeout;
   qDebug() << "parsed values: " << m_regular.repeats << m_regular.timeout;
}

void GypsyMock::parseSatelliteData(QTextStream& stream)
{
    TRACE0;
    // create new satellite struct
    qDebug("1");
    GypsySatelliteDetails* details = new GypsySatelliteDetails;
    qDebug("2");
    int in_use;
    qDebug("3");
    stream >> details->elevation >> details->azimuth >> details->satellite_id >> details->snr >> in_use;
    qDebug("4");
    details->in_use = (bool)in_use; // stream doesnt support bools
    qDebug("5");
    qDebug() << "parsed values: " << details->elevation << details->azimuth << details->satellite_id << details->snr << details->in_use;
    qDebug("6");
    // add it to the struct
    m_regular.satellites->pdata[m_regular.satellites->len] = (gpointer)details;
    qDebug("7");
    m_regular.satellites->len++;
    qDebug("8");
    // kept for future reference
    //g_ptr_array_add(m_regular.satellites, (gpointer)details);
}

// void GypsyMock::setSatellitesChangedCallback(void (*callback)())
void GypsyMock::setSatellitesChangedCallback(int callback)
{
    TRACE0;
    m_satellitesChangedCallback = (SatellitesChangedCallback)callback;
}

// void GypsyMock::disconnectSignal(gpointer callback, gpointer satelliteSource)
void GypsyMock::disconnectSignal(int callback, int satelliteSource)
{
    TRACE0;
    if ((gpointer)satelliteSource != (gpointer)m_satelliteSource) {
        TRACE1("disconnectSignal requested without connecting.");
        return;
    }
    if ((gpointer)callback == (gpointer)m_satellitesChangedCallback) {
        m_satellitesChangedCallback = 0;
    }
    m_satelliteSource = 0;
}

// Misc functions to parametrize the behavior of the mockend
void gypsymock_setjournal(QString journal)
{
    lg_currentJournal = journal;
}
