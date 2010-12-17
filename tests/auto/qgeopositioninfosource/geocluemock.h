#ifndef GEOCLUEMOCK_H
#define GEOCLUEMOCK_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <qgeopositioninfosource_geocluemaster_p.h>
#include <geoclue/geoclue-master.h>
#include <geoclue/geoclue-master-client.h>
#include <geoclue/geoclue-velocity.h>
#include <geoclue/geoclue-position.h>

//#define TST_GEOCLUE_MOCK_TRACE 1

void geocluemock_setjournal(QString journal);

typedef void (*PositionChangedCallback) (GeocluePosition*,
                   GeocluePositionFields,
                   int,
                   double,
                   double,
                   double,
                   GeoclueAccuracy*,
                   gpointer);

typedef void (*VelocityChangedCallback) (GeoclueVelocity*,
                              GeoclueVelocityFields,
                              int,
                              double,
                              double,
                              double,
                              gpointer);

typedef void (*PositionCallback) (GeocluePosition*,
                   GeocluePositionFields,
                   int,
                   double,
                   double,
                   double,
                   GeoclueAccuracy*,
                   GError*,
                   gpointer);

class GeoclueMock: public QObject
{
    Q_OBJECT
public:
    explicit GeoclueMock();
    ~GeoclueMock();
    void setPositionSource(QObject* positionSource);
    void setPositionChangedCallback(void (*callback)());
    void setVelocityChangedCallback(void (*callback)());
    void setPositionCallback(GeocluePositionCallback callback);
    void disconnectSignal(gpointer callback, gpointer positionSource);
    void singleUpdate();

    enum UPDATE_TYPE {
        INVALID_UPDATE = 0,
        REGULAR_UPDATE,
        SINGLE_UPDATE,
        VELOCITY_UPDATE
    };

    typedef struct {
        int timerId;
        int timeout;
        int repeats;
        int timestamp;
        int fields;
        double latitude;
        double longitude;
        double altitude;
        double speed;
        double direction;
        double climb;
        UPDATE_TYPE updateType;
    } UpdateData;

public slots:
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent *);

private:
    UPDATE_TYPE readNextEntry();
    void parseRegularUpdate(QTextStream& stream);
    void parseSingleUpdate(QTextStream& stream);
    void parseVelocityUpdate(QTextStream& stream);

private:
    QFile m_currentJournalFile;
    QObject* m_positionSource;
    UpdateData m_regular;
    UpdateData m_single;
    PositionChangedCallback m_positionChangedCallback;
    VelocityChangedCallback m_velocityChangedCallback;
    PositionCallback m_positionCallback;
};

#endif // GEOCLUEMOCK_H
