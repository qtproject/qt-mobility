
#include "qradioplayer.h"
#include "qradiotuner.h"

/*!
    \class QRadioPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QRadioPlayerPrivate
{
public:
    QRadioService*  service;
    QRadioTuner*    control;
};


QRadioPlayer::QRadioPlayer(QRadioService* service, QObject *parent):
    QAbstractMediaObject(parent),
    d(new QRadioPlayerPrivate)
{
    Q_ASSERT(service != 0);

    d->service = service;
    d->control = qobject_cast<QRadioTuner *>(service->control());
    d->control->setNotifyObject(this);
}

QRadioPlayer::~QRadioPlayer()
{
    delete d->control;
    delete d->service;
}

QRadioPlayer::Band QRadioPlayer::band() const
{
    return QRadioPlayer::Band(d->control->band());
}

int QRadioPlayer::frequency() const
{
    return d->control->frequency();
}

bool QRadioPlayer::isStereo() const
{
    return d->control->isStereo();
}

int QRadioPlayer::signalStrength() const
{
    return d->control->signalStrength();
}

qint64 QRadioPlayer::duration() const
{
    return d->control->duration();
}

int QRadioPlayer::volume() const
{
    return d->control->volume();
}

bool QRadioPlayer::isMuted() const
{
    return d->control->isMuted();
}

QAbstractMediaService* QRadioPlayer::service() const
{
    return d->service;
}

void QRadioPlayer::setBand(QRadioPlayer::Band band)
{
    d->control->setBand(band);
}

void QRadioPlayer::setFrequency(int frequency)
{
    d->control->setFrequency(frequency);
}

void QRadioPlayer::setStereo(bool stereo)
{
    d->control->setStereo(stereo);
}

void QRadioPlayer::setVolume(int volume)
{
    d->control->setVolume(volume);
}

void QRadioPlayer::setMuted(bool muted)
{
    d->control->setMuted(muted);
}

void QRadioPlayer::searchForward()
{
    d->control->searchForward();
}

void QRadioPlayer::searchBackward()
{
    d->control->searchBackward();
}


QRadioService* createRadioService(QMediaServiceProvider *provider)
{
    return qobject_cast<QRadioService*>(provider->createObject("com.nokia.qt.RadioService/1.0"));
}
