#ifndef GYPSYMOCK_H
#define GYPSYMOCK_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QStringList>
#include <QString>
#include <QTextStream>

#define TST_GYPSY_MOCK_TRACE 1

// Mocked glib symbols
typedef void* gpointer;
typedef char gchar;
typedef void  (*GCallback) (void);
typedef void* GClosureNotify;
typedef uint GConnectFlags;
typedef uint GSignalMatchType;
typedef uint guint;
typedef void* GClosure;
typedef void* GQuark;
typedef ulong gulong;
typedef void* GObject;
typedef bool gboolean;
typedef int gint;

typedef struct _GError
{
  GQuark       domain;
  gint         code;
  gchar       *message;
} GError;

typedef struct _GPtrArray
{
  gpointer *pdata;
  guint	    len;
} GPtrArray;

extern "C" {

void g_type_init (void);
gulong	 g_signal_connect_data		      (gpointer		  instance,
                                               const gchar	 *detailed_signal,
                                               GCallback	  c_handler,
                                               gpointer		  data,
                                               GClosureNotify	  destroy_data = NULL,
                                               GConnectFlags	  connect_flags = (GConnectFlags)0);

guint	 g_signal_handlers_disconnect_matched (gpointer		  instance,
                                               GSignalMatchType	  mask,
                                               guint		  signal_id = guint(0),
                                               GQuark		  detail = GQuark(0),
                                               GClosure		 *closure = NULL,
                                               gpointer		  func = NULL,
                                               gpointer		  data = NULL);
void g_error_free (GError        *error);
void g_object_unref(gpointer object);
GPtrArray* g_ptr_array_new();
}

// Mocked Gypsy symbols
// GypsyDevice*
typedef struct _GypsyDevice {
        GObject parent_object;
} GypsyDevice;
// GypsyControl*
typedef struct _GypsyControl {
        GObject parent_object;
} GypsyControl;
// GypsySatellite*
typedef struct _GypsySatellite {
        GObject parent_object;
} GypsySatellite;
// GypsySatelliteDetails*
typedef struct _GypsySatelliteDetails {
        int satellite_id;
        gboolean in_use;
        guint elevation;
        guint azimuth;
        guint snr;
} GypsySatelliteDetails;
// GypsyDeviceFixStatus
typedef enum {
        GYPSY_DEVICE_FIX_STATUS_INVALID = 0,
        GYPSY_DEVICE_FIX_STATUS_NONE,
        GYPSY_DEVICE_FIX_STATUS_2D,
        GYPSY_DEVICE_FIX_STATUS_3D
} GypsyDeviceFixStatus;

// Gypsy is mangled as plain C
extern "C" {
GypsyControl *gypsy_control_get_default (void);
char *gypsy_control_create (GypsyControl *control,
                            const char   *device_name,
                            GError      **error);
GypsyDevice *gypsy_device_new (const char *object_path);
GypsySatellite *gypsy_satellite_new (const char *object_path);
gboolean gypsy_device_start (GypsyDevice *device,
                             GError     **error);
gboolean gypsy_device_stop (GypsyDevice *device,
                            GError     **error);
GypsyDeviceFixStatus gypsy_device_get_fix_status (GypsyDevice *device,
                                                  GError      **error);
GPtrArray *gypsy_satellite_get_satellites (GypsySatellite *satellite,
                                           GError        **error);
void gypsy_satellite_free_satellite_array (GPtrArray *satellites);
} // extern "C"

// Mocked GConf symbols
typedef struct _GConfClient
{
  GObject object;

} GConfClient;

// GConf is mangled as plain C
extern "C" {

GConfClient* gconf_client_get_default(void);
gchar*       gconf_client_get_string(GConfClient* client, const gchar* key,
                                     GError** err);
} // extern C

// misc
void gypsymock_setjournal(QString journal);
typedef void (*SatellitesChangedCallback) (GypsySatellite *satellite,
                                           GPtrArray *satellites,
                                           gpointer userdata);

class GypsyMock: public QObject
{
    Q_OBJECT
public:
    explicit GypsyMock();
    ~GypsyMock();
    void setSatelliteSource(QObject* positionSource);
    void disconnectSignal(gpointer callback, gpointer positionSource);
    void setSatellitesChangedCallback(void (*callback)());
    void singleUpdate();

    enum UPDATE_TYPE {
        INVALID_UPDATE = 0,
        REGULAR_UPDATE,
        SINGLE_UPDATE,
    };
    typedef struct {
        int timerId;
        int timeout;
        int repeats;
        GPtrArray* satellites;
        UPDATE_TYPE updateType;
    } UpdateData;

public slots:
    void start();
    void stop();
    void setSatelliteSource(int positionSource);
    void disconnectSignal(int callback, int positionSource);
    void setSatellitesChangedCallback(int);

protected:
    void timerEvent(QTimerEvent *);

private:
    UPDATE_TYPE readNextEntry();
    void parseRegularUpdate(QTextStream& stream);
    void parseSatelliteData(QTextStream& stream);

private:
    QFile m_currentJournalFile;
    QObject* m_satelliteSource;
    UpdateData m_regular;
    QThread m_thread;
    SatellitesChangedCallback m_satellitesChangedCallback;
};

#endif // GYPSYMOCK_H
