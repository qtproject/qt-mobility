
#ifndef MPDPLAYERCONTROL_H
#define MPDPLAYERCONTROL_H

#include <qmediaplayercontrol.h>


class MpdDaemon;

class MpdPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    MpdPlayerControl(MpdDaemon *mpd, QObject *parent);
    ~MpdPlayerControl();

    void play();
    void pause();
    void stop();

private slots:
    void notify();
    void playerChanged();
    void mixerChanged();

private:
    MpdDaemon *daemon;
};

#endif  // MPDPLAYERCONTROL_H
