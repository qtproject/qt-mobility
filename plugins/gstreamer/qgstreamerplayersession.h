#ifndef QGSTREAMERPLAYERSESSION_H
#define QGSTREAMERPLAYERSESSION_H

#include <QObject>
#include <QUrl>
#include "qgstreamerplayercontrol.h"

#include <gst/gst.h>

class QGstreamerBusHelper;
class QGstreamerMessage;

class QGstreamerPlayerSession : public QObject
{
Q_OBJECT
public:
    QGstreamerPlayerSession(QObject *parent);
    virtual ~QGstreamerPlayerSession();

    QUrl url() const;

    QGstreamerPlayerControl::State state() const { return m_state; }

    qint64 duration() const;
    qint64 position() const;

    bool isBuffering() const;

    int bufferingProgress() const;

    double volume() const;
    bool isMuted() const;

    QObject *videoOutput() const;
    bool setVideoOutput(QObject *videoOutput);
    bool isVideoAvailable() const;

public slots:
    void load(const QUrl &url);

    void play();
    void pause();
    void stop();

    void seek(qint64 pos);

    void setVolume(double volume);
    void setMuted(bool muted);

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayerControl::State state);
    void volumeChanged(double volume);
    void mutedStateChaned(bool muted);
    void videoAvailabilityChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferingProgressChanged(int percentFilled);
    void playbackFinished();

private slots:
    void busMessage(const QGstreamerMessage &message);
    void getStreamInfo();

private:
    QUrl m_url;
    QGstreamerPlayerControl::State m_state;
    QGstreamerBusHelper* m_busHelper;
    GstElement* m_playbin;
    GstBus* m_bus;
    QObject *m_videoOutput;

    double m_volume;
    bool m_muted;

    qint64 m_lastPosition;
    qint64 m_duration;
};

#endif // QGSTREAMERPLAYERSESSION_H
