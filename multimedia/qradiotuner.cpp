
#include "qradiotuner.h"


QRadioTuner::QRadioTuner(QObject *parent):
    QAbstractMediaControl(parent)
{
    addPropertyWatch("duration");
}

QRadioTuner::~QRadioTuner()
{
}

int QRadioTuner::band() const
{
    return property("band").toInt();
}

void QRadioTuner::setBand(int b)
{
    changePropertyValue("band", b);
}

int QRadioTuner::frequency() const
{
    return property("frequency").toInt();
}

void QRadioTuner::setFrequency(int frequency)
{
    changePropertyValue("frequency", frequency);
}

bool QRadioTuner::isStereo() const
{
    return property("stereo").toBool();
}

void QRadioTuner::setStereo(bool stereo)
{
    changePropertyValue("stereo", stereo);
}

int QRadioTuner::signalStrength() const
{
    return property("signalStrength").toInt();
}

qint64 QRadioTuner::duration() const
{
    return property("duration");
}

void QRadioTuner::setDuration(qint64 duration)
{
    changePropertyValue("duration", duration);
}

int QRadioTuner::volume() const
{
    return property("volume");
}

void QRadioTuner::setVolume(int volume)
{
    changePropertyValue("volume", volume);
}

bool QRadioTuner::isMuted() const
{
    return property("muted").toBool();
}

void QRadioTuner::setMuted(bool muted)
{
    changePropertyValue("muted", muted);
}

