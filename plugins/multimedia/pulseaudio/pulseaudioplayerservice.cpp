#include "pulseaudioplayercontrol.h"
#include "pulseaudioplayerservice.h"


PulseAudioPlayerService::PulseAudioPlayerService(QObject *parent) :
    QMediaService(parent)
{
    m_control = new PulseAudioPlayerControl;
}

QMediaControl* PulseAudioPlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_control;

    return 0;
}
