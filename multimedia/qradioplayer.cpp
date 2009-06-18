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
    QRadioTuner*    control;
};


QRadioPlayer::QRadioPlayer(QRadioService* service, QObject *parent):
    QAbstractMediaObject(*new QRadioPlayerPrivate, parent)
{
    Q_ASSERT(service != 0);

    Q_D(QRadioPlayer)
    d->service = service;
    d->control = service->control();
}

QRadioPlayer::~QRadioPlayer()
{
    Q_D(QRadioPlayer)

    delete d->control;
    delete d->service;
}

QRadioPlayer::Band QRadioPlayer::band() const
{
    return d_func()->control->band();
}

int QRadioPlayer::frequency() const
{
    return d_func()->control->frequency();
}

bool QRadioPlayer::isStereo() const
{
    return d_func()->control->isStereo();
}

int QRadioPlayer::signalStrength() const
{
    return d_func()->control->signalStrength();
}

qint64 QRadioPlayer::duration() const
{
    return d_func()->control->duration();
}

int QRadioPlayer::volume() const
{
    return d_func()->control->volume();
}

bool QRadioPlayer::isMuted() const
{
    return d_func()->control->isMuted();
}

QRadioService* QRadioPlayer::service() const
{
    return d_func()->service;
}

void QRadioPlayer::setBand(QRadioPlayer::Band band)
{
    d_func()->control->setBand(band):
}

void QRadioPlayer::setFrequency(int frequency)
{
    d_func()->control->setFrequency(frequency);
}

void QRadioPlayer::setStereo(bool stereo)
{
    d_func()->control->setStereo(stereo);
}

void QRadioPlayer::setVolume(int volume)
{
    d_func()->control->setVolume(volume);
}

void QRadioPlayer::setMuted(bool muted)
{
    d_func()->control->setMuted(muted);
}

void QRadioPlayer::searchForward()
{
    d_func()->control->searchForward();
}

void QRadioPlayer::searchBackward()
{
    d_func()->control->searchBackward();
}


QRadioService* createRadioService(QMediaServiceProvider *provider)
{
    return qobject_cast<QRadioService*>(provider->createObject("com.nokia.qt.RadioService/1.0"));
}
