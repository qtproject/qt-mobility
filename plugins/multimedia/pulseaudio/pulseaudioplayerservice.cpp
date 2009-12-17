#include "pulseaudioplayercontrol.h"
#include "pulseaudioplayerservice.h"


PulseAudioPlayerService::PulseAudioPlayerService(QObject *parent) :
    QMediaService(parent),
    m_control(0)
{
    m_control = new PulseAudioPlayerControl;
}

PulseAudioPlayerService::~PulseAudioPlayerService()
{
    delete m_control;
}

QMediaControl* PulseAudioPlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_control;

    return 0;
}
