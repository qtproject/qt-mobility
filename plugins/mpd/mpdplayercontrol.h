
#ifndef MPDPLAYERCONTROL_H
#define MPDPLAYERCONTROL_H

class MpdDaemon;

class MpdPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    MpdPlayerControl(MpdDaemon *mpd);
    ~MpdPlayerControl();

    void play();
    void pause();
    void stop();

private slots:
    void checkStatus();

private:
    MpdDaemon *daemon;
};

#endif  // MPDPLAYERCONTROL_H
