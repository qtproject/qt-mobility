#ifndef QMEDIAPLAYERCONTROL_H
#define QMEDIAPLAYERCONTROL_H


#include "qabstractmediacontrol.h"

class QMediaPlayerControl : public QAbtractMediaControl
{
    Q_OBJECT

public:
    ~QMediaPlayerControl();

    int state() const;
    virtual void setState(int state);

    QMediaSource mediaSource() const;
    virtual void setMediaSource(QMediaSource mediaSource);

    qint64 duration() const;
    virtual void setDuration(qint64 duration);

    qint64 position() const;
    virtual void setPosition(qint64 position);

    int volume() const;
    virtual void setVolume(int volume);

    bool isMuted() const;
    virtual void setMuted(bool muted);

    bool isBuffering() const;
    virtual void setBuffering(bool buffering);

    int bufferStatus() const;
    virtual void setBufferState(int bufferStatus);

    bool isVideoAvailable() const;
    virtual void setVideoAvailable(bool videoAvailable);

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:
    QMediaPlayerControl(QObject* parent);
};

#endif  // QMEDIAPLAYERCONTROL_H

