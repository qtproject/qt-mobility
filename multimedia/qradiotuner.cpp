/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <multimedia/qradiotuner.h>
#include <multimedia/qmediaservice.h>
#include <multimedia/qmediaobject_p.h>
#include <multimedia/qradiotunercontrol.h>

#include <QPair>


/*!
    \class QRadioTuner
    \brief The QRadioTuner class provides an interface to the systems analog radio device.

    You can control the systems analog radio device using this interface, for example:

    \code
        radio = new QRadioTuner;
        connect(radio,SIGNAL(frequencyChanged(int)),this,SLOT(freqChanged(int)));
        if(radio->isBandSupported(QRadioTuner::FM))
            radio->setBand(QRadioTuner::FM);
        radio->setFrequency(yourRadioStationFrequency);
        radio->setVolume(100);
        radio->start();
    \endcode

    The radio object will emit signals for any changes in state such as:
    bandChanged(), frequencyChanged(), stereoStatusChanged(), searchingStatusChanged(),
    signalStrengthChanged(), volumeChanged(), mutingChanged().

    You can change between the frequency bands using setBand() however it is recommended
    that you check to make sure the band is available first using isBandSupported().

*/


class QRadioTunerPrivate : public QMediaObjectPrivate
{
public:
    QRadioTunerPrivate():provider(0), control(0) {}
    QMediaServiceProvider *provider;
    QRadioTunerControl* control;
};



/*!
    Construct a QRadioTuner object with service \a provider and \a parent.

    If the service is not specified the system default will be used.
*/

QRadioTuner::QRadioTuner(QObject *parent, QMediaServiceProvider* provider):
    QMediaObject(*new QRadioTunerPrivate, parent, provider->requestService(Q_MEDIASERVICE_RADIO))
{
    Q_D(QRadioTuner);

    d->provider = provider;

    if (d->service != 0) {
        d->control = qobject_cast<QRadioTunerControl*>(d->service->control(QRadioTunerControl_iid));
        if (d->control != 0) {
            connect(d->control, SIGNAL(bandChanged(QRadioTuner::Band)), SIGNAL(bandChanged(QRadioTuner::Band)));
            connect(d->control, SIGNAL(frequencyChanged(int)), SIGNAL(frequencyChanged(int)));
            connect(d->control, SIGNAL(stereoStatusChanged(bool)), SIGNAL(stereoStatusChanged(bool)));
            connect(d->control, SIGNAL(searchingStatusChanged(bool)), SIGNAL(searchingStatusChanged(bool)));
            connect(d->control, SIGNAL(signalStrengthChanged(int)), SIGNAL(signalStrengthChanged(int)));
            connect(d->control, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged(int)));
            connect(d->control, SIGNAL(mutingChanged(bool)), SIGNAL(mutingChanged(bool)));
        }
    }
}

/*!
    Destruct the QRadioTuner object.
*/

QRadioTuner::~QRadioTuner()
{
    Q_D(QRadioTuner);

    d->provider->releaseService(d->service);
}

/*!
    \property QRadioTuner::band
    \brief The current frequency band being used.

    \sa QRadioTuner::Band
*/

/*!
    Returns the current QRadioTuner::Band.
*/

QRadioTuner::Band QRadioTuner::band() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->band();

    return QRadioTuner::FM;
}

/*!
    \property QRadioTuner::frequency
    \brief The current frequency being used in Hertz.
*/

/*!
    Returns the current frequency.
*/

int QRadioTuner::frequency() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->frequency();

    return 0;
}

/*!
    Returns the incremental step size in Hertz when changing frequency for \a band.
*/

int QRadioTuner::frequencyStep(QRadioTuner::Band band) const
{
    Q_D(const QRadioTuner);

    if(d->control != 0)
        return d->control->frequencyStep(band);

    return 0;
}

/*!
    Return the frequency range for the \a band.
*/

QPair<int,int> QRadioTuner::frequencyRange(QRadioTuner::Band band) const
{
    Q_D(const QRadioTuner);

    if(d->control != 0)
        return d->control->frequencyRange(band);

    return qMakePair<int,int>(0,0);
}

/*!
    \property QRadioTuner::stereo
    \brief In Stereo mode.
*/

/*!
    Returns true if in stereo.
*/

bool QRadioTuner::isStereo() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->isStereo();

    return false;
}

/*!
    Set the stereo mode to \a mode.
*/

void QRadioTuner::setStereoMode(QRadioTuner::StereoMode mode)
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        return d->control->setStereoMode(mode);
}

/*!
    Returns the stereo mode.
*/

QRadioTuner::StereoMode QRadioTuner::stereoMode() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->stereoMode();

    return QRadioTuner::Auto;
}

/*!
    Returns true if \a b is an available band.
*/

bool QRadioTuner::isBandSupported(QRadioTuner::Band b) const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->isBandSupported(b);

    return false;
}

/*!
    Activate the radio device.
*/

void QRadioTuner::start()
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        d->control->start();
}

/*!
    Deactivate the radio device.
*/

void QRadioTuner::stop()
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        d->control->stop();
}

/*!
    \property QRadioTuner::signalStrength
    \brief The strength of the current radio signal as a percentage.
*/

/*!
    Returns the signal strength as a percentage.
*/

int QRadioTuner::signalStrength() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->signalStrength();

    return 0;
}

/*!
    \property QRadioTuner::volume
    \brief The current volume as a percentage.
*/

/*!
    Returns the volume as a percentage.
*/

int QRadioTuner::volume() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->volume();

    return 0;
}

/*!
    \property QRadioTuner::muted
    \brief Returns true if muted.
*/

/*!
    Returns true if muted.
*/

bool QRadioTuner::isMuted() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->isMuted();

    return false;
}

/*!
    Sets the band being used to \a band.

    Note: On band change frequency is set to minimum frequency for new band.
*/

void QRadioTuner::setBand(QRadioTuner::Band band)
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->setBand(band);
}

/*!
    Sets the frequency being used to \a frequency.

    Note: If new frequency is not in range for current band, band is changed
          to band that supports new frequency.
*/

void QRadioTuner::setFrequency(int frequency)
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->setFrequency(frequency);
}

/*!
    Sets the volume to \a volume.
    volume is a percentage.
*/

void QRadioTuner::setVolume(int volume)
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->setVolume(volume);
}

/*!
    If muted is true sets to mute, otherwise unmute.
*/

void QRadioTuner::setMuted(bool muted)
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->setMuted(muted);
}

/*!
    \property QRadioTuner::searching
    \brief Returns true if currently scanning.
*/

/*!
    Returns true if currently scanning.
*/

bool QRadioTuner::isSearching() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->isSearching();

    return false;
}

/*!
    Start scanning through frequencies, ascending.
*/

void QRadioTuner::searchForward()
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->searchForward();
}

/*!
    Start scanning through frequencies, descending.
*/

void QRadioTuner::searchBackward()
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->searchBackward();
}

/*!
    Stop any scanning being done.
*/

void QRadioTuner::cancelSearch()
{
    Q_D(QRadioTuner);

    if (d->control != 0)
        d->control->cancelSearch();
}

/*!
    Returns the error state of the object.
*/

QRadioTuner::Error QRadioTuner::error() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->error();

    return QRadioTuner::ResourceError;
}

/*!
    Returns the current error string.
*/

QString QRadioTuner::errorString() const
{
    Q_D(const QRadioTuner);

    if (d->control != 0)
        return d->control->errorString();

    return QString();
}

/*!
    \fn void QRadioTuner::bandChanged(QRadioTuner::Band band)

    Signal the change in frequency range to \a band.
*/

/*!
    \fn void QRadioTuner::frequencyChanged(int frequency)

    Signal the change in current \a frequency.

    Note: When scanning frequencies the signal is only emitted on an interval
    set by setNotifyInterval().
*/

/*!
    \fn void QRadioTuner::mutingChanged(bool muted)

    Signal the change in mute condition to \a muted.
*/

/*!
    \fn void QRadioTuner::volumeChanged(int volume)

    Signal the change in volume percentage to \a volume.
*/

/*!
    \fn void QRadioTuner::searchingStatusChanged(bool searching)

    Signal the change in scanning mode to \a searching.
*/

/*!
    \fn void QRadioTuner::stereoStatusChanged(bool stereo)

    Signal the change in stereo mode to \a stereo.
*/

/*!
    \fn void QRadioTuner::signalStrengthChanged(int signalStrength)

    Signal emitted when signal strength changes to new value \a signalStrength.
*/

/*!
    \fn void QRadioTuner::error(QRadioTuner::Error err)

    Signal emitted when the error state changes to \a err.
*/

/*!
    \enum QRadioTuner::Band

    \value AM 520 to 1610 kHz, 9 or 10kHz channel spacing, extended 1610 to 1710 kHz
    \value FM 87.5 to 108.0 MHz, except Japan 76-90 MHz
    \value SW 1.711 to 30.0 MHz, divided into 15 bands. 5kHz channel spacing
    \value LW 148.5 to 283.5 kHz, 9kHz channel spacing (Europe, Africa, Asia)
*/

/*!
    \enum QRadioTuner::Error

    \value NoError         No errors has occurred.
    \value ResourceError   There is no radio service available.
    \value OpenError       Unable to open radio device.
    \value OutOfRangeError An attempt to set a frequency or band that is not supported by radio device.
*/

/*!
    \enum QRadioTuner::StereoMode

    \value Auto        Uses the stereo mode matching the station.
    \value ForceStereo Provide stereo mode, converting if required.
    \value ForceMono   Provide mono mode, converting if required.
*/


