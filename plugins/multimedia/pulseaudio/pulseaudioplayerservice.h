#ifndef PULSEAUDIOPLAYERSERVICE_H
#define PULSEAUDIOPLAYERSERVICE_H

#include <qmediaservice.h>

QTM_USE_NAMESPACE

class PulseAudioPlayerControl;

class PulseAudioPlayerService : public QMediaService
{
Q_OBJECT
public:
    explicit PulseAudioPlayerService(QObject *parent = 0);
    ~PulseAudioPlayerService();

    QMediaControl* control(const char *name) const;

private:
    PulseAudioPlayerControl *m_control;
};

#endif // PULSEAUDIOPLAYERSERVICE_H
