#ifndef GYPSYMOCK_H
#define GYPSYMOCK_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <glib.h>
#include <glib-object.h>
#include <gypsy/gypsy-satellite.h>
#include <gypsy/gypsy-control.h>
#include <gypsy/gypsy-device.h>
#include <gconf/gconf-client.h>

#define TST_GYPSY_MOCK_TRACE 1

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

// ================  Misc functions
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
    void disconnectSignal(int callback, int positionSource);
    void connectSignal(int signal, int callback, int positionSource);

protected:
    void timerEvent(QTimerEvent *);

private:
    UPDATE_TYPE readNextEntry();
    void parseRegularUpdate(QTextStream& stream);
    void parseSatelliteData(QTextStream& stream);

private:
    QString m_currentJournal;
    QObject* m_satelliteSource;
    QFile m_currentJournalFile;
    UpdateData m_regular;
    QThread m_thread;
    SatellitesChangedCallback m_satellitesChangedCallback;
};

#endif // GYPSYMOCK_H
