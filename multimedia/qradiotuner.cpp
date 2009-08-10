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

#include "qradiotuner.h"
#include "qabstractmediacontrol_p.h"


/*!
    \class QRadioTuner
    \ingroup multimedia

    \preliminary
    \brief The abstract class for controling the tuning of radio devices.

    \sa QAbstractMediaService, QRadioPlayer
*/

/*!
    Construct a radio tuner class.
*/

QRadioTuner::QRadioTuner(QObject *parent):
    QAbstractMediaControl(*new QAbstractMediaControlPrivate, parent)
{
}

/*!
    Destruct the radio tuner class.
*/

QRadioTuner::~QRadioTuner()
{
}

/*!
    \fn int QRadioTuner::band() const

    Returns the current band being used.
*/

/*!
    \fn void QRadioTuner::bandChanged(QRadioPlayer::Band band)

    Signal emitted when \a band changed.
*/

/*!
    \fn void QRadioTuner::setBand(int band)

    Set current band to \a band.
*/

/*!
    \fn bool QRadioTuner::isSupportedBand(int band) const

    Return true if \a band is available.
*/

/*!
    \fn int QRadioTuner::frequency() const

    Returns the current frequency being used.
*/

/*!
    \fn void QRadioTuner::setFrequency(int frequency)

    Set the frequency to \a frequency.
*/

/*!
    \fn bool QRadioTuner::isStereo() const

    Return true if in stereo mode.
*/

/*!
    \fn void QRadioTuner::setStereo() const

    Set to stereo mode.
*/

/*!
    \fn int QRadioTuner::signalStrength() const

    Return the current signal strength as a percentage.
*/

/*!
    \fn qint64 QRadioTuner::duration() const

    Returns the time in milliseconds at this frequency.
*/

/*!
    \fn int QRadioTuner::volume() const

    Returns the volume as a percentage.
*/

/*!
    \fn void QRadioTuner::setVolume(int volume)

    Set the volume to \a volume (percentage)
*/

/*!
    \fn bool QRadioTuner::isMuted() const

    Returns true if muted.
*/

/*!
    \fn void QRadioTuner::setMuted(bool muted)

    Set mute state to \a muted.
*/

/*!
    \fn bool QRadioTuner::isSearching() const

    Returns true if currently scanning.
*/

/*!
    \fn  void QRadioTuner::searchForward()

    Start scanning, ascending freqeuncy.
*/

/*!
    \fn void QRadioTuner::searchBackward()

    Start scanning, descending frequency.
*/

/*!
    \fn void QRadioTuner::cancelSearch()

    Stop any scanning being done.
*/


/*!
    \fn void QRadioTuner::frequencyChanged(int frequency)

    Signal emitted when \a frequency changed.
*/

/*!
    \fn void QRadioTuner::stereoStatusChanged(bool stereo)

    Signal emitted when \a stereo mode changed.
*/

/*!
    \fn void QRadioTuner::searchingStatusChanged(bool searching)

    Signal emitted when \a searching status changed.
*/

/*!
    \fn void QRadioTuner::signalStrengthChanged(int signalStrength)

    Signal emitted when \a signalStrength changed (percentage).
*/

/*!
    \fn void QRadioTuner::durationChanged(qint64 duration)

    Signal emitted when \a duration changed.
*/

/*!
    \fn void QRadioTuner::volumeChanged(int volume)

    Signal emitted when \a volume changed (percentage).
*/

/*!
    \fn void QRadioTuner::mutingChanged(bool muted)

    Signal emitted when \a muted state changed.
*/


