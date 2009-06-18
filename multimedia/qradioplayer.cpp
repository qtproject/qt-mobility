#include <private/qobject_p.h>

#include "qradioplayer.h"

/*!
    \class QRadioPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QRadioPlayerPrivate : public QAbstractMediaObjectPrivate
{
public:
    QRadioService*  service;
    QRadioTuner*    tuner;
};


QRadioPlayer::QRadioPlayer(QRadioService* service, QObject *parent):
    QAbstractMediaObject(*new QRadioPlayerPrivate, parent)
{
    Q_ASSERT(service != 0);

    Q_D(QRadioPlayer)
    d->service = service;
    d->tuner = service->control();
}

QRadioPlayer::~QRadioPlayer()
{
    Q_D(QRadioPlayer)

    delete d->tuner;
    delete d->service;
}

QRadioPlayer::Band QRadioPlayer::band() const
{
    return d_func()->tuner->band();
}

int QRadioPlayer::frequency() const
{
    return d_func()->tuner->frequency();
}

bool QRadioPlayer::isStereo() const
{
    return d_func()->tuner->isStereo();
}

int QRadioPlayer::signalStrength() const
{
    return d_func()->tuner->signalStrength();
}

qint64 QRadioPlayer::duration() const
{
    return d_func()->tuner->duration();
}

int QRadioPlayer::volume() const
{
    return d_func()->tuner->volume();
}

bool QRadioPlayer::isMuted() const
{
    return d_func()->tuner->isMuted();
}

QRadioService* QRadioPlayer::service() const
{
    return d_func()->service;
}

void QRadioPlayer::setBand(QRadioPlayer::Band band)
{
    d_func()->tuner->setBand(band):
}

void QRadioPlayer::setFrequency(int frequency)
{
    d_func()->tuner->setFrequency(frequency);
}

void QRadioPlayer::setStereo(bool stereo)
{
    d_func()->tuner->setStereo(stereo);
}

void QRadioPlayer::setVolume(int volume)
{
    d_func()->tuner->setVolume(volume);
}

void QRadioPlayer::setMuted(bool muted)
{
    d_func()->tuner->setMuted(muted);
}

void QRadioPlayer::searchForward()
{
    d_func()->tuner->searchForward();
}

void QRadioPlayer::searchBackward()
{
    d_func()->tuner->searchBackward();
}


QRadioService* createRadioService(QMediaServiceProvider *provider)
{
    return qobject_cast<QRadioService*>(provider->createObject("com.nokia.Qt.RadioSerice/1.0"));
}
