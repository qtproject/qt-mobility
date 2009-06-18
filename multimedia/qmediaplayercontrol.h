#ifndef QMEDIAPLAYERCONTROL_H
#define QMEDIAPLAYERCONTROL_H


#include <QtCore/qobject.h>


class QMediaPlayerControl : public QAbtractMediaControl
{
    Q_OBJECT

public:
    virtual void setMediaSource(QMediaSource mediaSource) = 0;

    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

protected:
    QMediaPlayerControl(QObject* parent);
};

#endif  // QMEDIAPLAYERCONTROL_H

