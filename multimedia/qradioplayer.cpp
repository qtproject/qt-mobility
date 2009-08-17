/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qradioplayer.h"

#include "qabstractmediaobject_p.h"
#include "qradiotuner.h"
#include "qradioservice.h"

/*!
    \class QRadioPlayer
    \brief The QRadioPlayer class provides an interface to the systems radio device.

    \ingroup multimedia

    You can control the systems radio device using this interface, for example:

    \code
        player = new QRadioPlayer;
        connect(player,SIGNAL(frequencyChanged(int)),this,SLOT(freqChanged(int)));
        if(player->isSupportedBand(QRadioPlayer::FM))
            player->setBand(QRadioPlayer::FM);
        player->setFrequency(yourRadioStationFrequency);
        player->setVolume(100);
    \endcode

    The player object will emit signals for any changes in state such as:
    bandChanged(), frequencyChanged(), stereoStatusChanged(), searchingStatusChanged(),
    signalStrengthChanged(), durationChanged(), volumeChanged(), mutingChanged().

    You can change between the frequency bands using setBand() however it is recommended
    that you check to make sure the band is available first using isSupportedBand().

*/

class QRadioPlayerPrivate : public QAbstractMediaObjectPrivate
{
public:
    QRadioService*  service;
    QRadioTuner*    control;
};

/*!
    Contruct a QRadioPlayer object with \a service and \a parent.
*/

QRadioPlayer::QRadioPlayer(QRadioService* service, QObject *parent):
    QAbstractMediaObject(*new QRadioPlayerPrivate, parent)
{
    Q_ASSERT(service != 0);

    Q_D(QRadioPlayer);

    d->service = service;
    d->control = qobject_cast<QRadioTuner *>(service->control("com.nokia.qt.RadioPlayerControl"));

    if(d->control) {
        connect(d->control,SIGNAL(bandChanged(QRadioPlayer::Band)),this,SIGNAL(bandChanged(QRadioPlayer::Band)));
        connect(d->control,SIGNAL(frequencyChanged(int)),this,SIGNAL(frequencyChanged(int)));
        connect(d->control,SIGNAL(stereoStatusChanged(bool)),this,SIGNAL(stereoStatusChanged(bool)));
        connect(d->control,SIGNAL(searchingStatusChanged(bool)),this,SIGNAL(searchingStatusChanged(bool)));
        connect(d->control,SIGNAL(signalStrengthChanged(int)),this,SIGNAL(signalStrengthChanged(int)));
        connect(d->control,SIGNAL(durationChanged(qint64)),this,SIGNAL(durationChanged(qint64)));
        connect(d->control,SIGNAL(volumeChanged(int)),this,SIGNAL(volumeChanged(int)));
        connect(d->control,SIGNAL(mutingChanged(bool)),this,SIGNAL(mutingChanged(bool)));
    }

    addPropertyWatch("duration");
}

/*!
    Destruct the QRadioPlayer object.
*/

QRadioPlayer::~QRadioPlayer()
{
    Q_D(QRadioPlayer);

    delete d->control;
    delete d->service;
}

/*!
    \property QRadioPlayer::band
    \brief The current frequency range being used.

    \sa QRadioPlayer::Band
*/

/*!
    Returns the current QRadioPlayer::Band.
*/

QRadioPlayer::Band QRadioPlayer::band() const
{
    return QRadioPlayer::Band(d_func()->control->band());
}

/*!
    \property QRadioPlayer::frequency
    \brief The current frequency being used in Hertz.
*/

/*!
    Returns the current frequency.
*/

int QRadioPlayer::frequency() const
{
    return d_func()->control->frequency();
}

/*!
    \property QRadioPlayer::stereo
    \brief In Stereo mode.
*/

/*!
    Returns true if in stereo.
*/

bool QRadioPlayer::isStereo() const
{
    return d_func()->control->isStereo();
}

/*!
    Returns true if \a b is an available band.
*/

bool QRadioPlayer::isSupportedBand(QRadioPlayer::Band b) const
{
    return d_func()->control->isSupportedBand(b);
}

/*!
    \property QRadioPlayer::signalStrength
    \brief The strength of the current radio signal as a percentage.
*/

/*!
    Returns the signal strength as a percentage.
*/

int QRadioPlayer::signalStrength() const
{
    return d_func()->control->signalStrength();
}

/*!
    \property QRadioPlayer::duration
    \brief The duration at current frequency in milliseconds.
*/

/*!
    Returns duration at current frequency in milliseconds.
*/

qint64 QRadioPlayer::duration() const
{
    return d_func()->control->duration();
}

/*!
    \property QRadioPlayer::volume
    \brief The current volume as a percentage.
*/

/*!
    Returns the volume as a percentage.
*/

int QRadioPlayer::volume() const
{
    return d_func()->control->volume();
}

/*!
    \property QRadioPlayer::muted
    \brief Returns true if muted.
*/

/*!
    Returns true if muted.
*/

bool QRadioPlayer::isMuted() const
{
    return d_func()->control->isMuted();
}

/*!
    Returns pointer to service class.
*/

QAbstractMediaService* QRadioPlayer::service() const
{
    return d_func()->service;
}

/*!
    Sets the band being used to \a band.
*/

void QRadioPlayer::setBand(QRadioPlayer::Band band)
{
    d_func()->control->setBand(band);
}

/*!
    Sets the frequency being used to \a frequency.
*/

void QRadioPlayer::setFrequency(int frequency)
{
    d_func()->control->setFrequency(frequency);
}

/*!
    If \a stereo is true, Sets to stereo mode, otherwise, mono mode.
*/

void QRadioPlayer::setStereo(bool stereo)
{
    d_func()->control->setStereo(stereo);
}

/*!
    Sets the volume to \a volume.
    volume is a percentage.
*/

void QRadioPlayer::setVolume(int volume)
{
    d_func()->control->setVolume(volume);
}

/*!
    If muted is true sets to mute, otherwise unmute.
*/

void QRadioPlayer::setMuted(bool muted)
{
    d_func()->control->setMuted(muted);
}

/*!
    Returns true if an available device.
*/

bool QRadioPlayer::isValid() const
{
    return d_func()->control != NULL;
}

/*!
    \property QRadioPlayer::searching
    \brief Returns true if currently scanning.
*/

/*!
    Returns true if currently scanning.
*/

bool QRadioPlayer::isSearching() const
{
    return d_func()->control->isSearching();
}

/*!
    Start scanning through frequencies, ascending.
*/

void QRadioPlayer::searchForward()
{
    d_func()->control->searchForward();
}

/*!
    Start scanning through frequencies, descending.
*/

void QRadioPlayer::searchBackward()
{
    d_func()->control->searchBackward();
}

/*!
    Stop any scanning being done.
*/

void QRadioPlayer::cancelSearch()
{
    d_func()->control->cancelSearch();
}

/*!
    Create a new radio service with parent \a provider.
*/

QRadioService* createRadioService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.RadioService/1.0") : 0;

    if (object) {
        QRadioService *service = qobject_cast<QRadioService *>(object);

        if (service)
            return service;

        delete object;
    }
    return 0;
}

/*!
    \fn void QRadioPlayer::bandChanged(QRadioPlayer::Band band)

    Signal the change in frequency range to \a band.
*/

/*!
    \fn void QRadioPlayer::frequencyChanged(int frequency)

    Signal the change in current \a frequency.
*/

/*!
    \fn void QRadioPlayer::mutingChanged(bool muted)

    Signal the change in mute condition to \a muted.
*/

/*!
    \fn void QRadioPlayer::volumeChanged(int volume)

    Signal the change in volume percentage to \a volume.
*/

/*!
    \fn void QRadioPlayer::searchingStatusChanged(bool searching)

    Signal the change in scanning mode to \a searching.
*/

/*!
    \fn void QRadioPlayer::stereoStatusChanged(bool stereo)

    Signal the change in stereo mode to \a stereo.
*/

/*!
    \fn void QRadioPlayer::durationChanged(qint64 duration)

    Signal the change in the \a duration of the current frequency playback in milliseconds.
*/

/*!
    \enum QRadioPlayer::Band

    \value AM 520 to 1610 kHz, 9 or 10kHz channel spacing, extended 1610 to 1710 kHz
    \value FM 87.5 to 108.0 MHz, except Japan 76-90 MHz
    \value SW 1.711 to 30.0 MHz, divided into 15 bands. 5kHz channel spacing
    \value LW 148.5 to 283.5 kHz, 9kHz channel spacing (Europe, Africa, Asia)
*/


