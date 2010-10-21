/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsoundeffect_p.h"

#if defined(QT_MULTIMEDIA_PULSEAUDIO)
#include "qsoundeffect_pulse_p.h"
#elif(QT_MULTIMEDIA_QMEDIAPLAYER)
#include "qsoundeffect_qmedia_p.h"
#else
#include "qsoundeffect_qsound_p.h"
#endif

QT_BEGIN_NAMESPACE

/*!
    \qmlclass SoundEffect QSoundEffect
    \brief The SoundEffect element provides a way to play sound effects in QML.

    This element is part of the \bold{QtMultimediaKit 1.1} module.

    The following example plays a wav file on mouse click.

    \qml
    import Qt 4.7
    import QtMultimediaKit 1.1

    Text {
        text: "Click Me!";
        font.pointSize: 24;
        width: 150; height: 50;

        SoundEffect {
            id: playSound
            source: "soundeffect.wav"
        }
        MouseArea {
            id: playArea
            anchors.fill: parent
            onPressed: { playSound.play() }
        }
    }
    \endqml
*/

/*!
    \qmlproperty url SoundEffect::source

    This property provides a way to control the sound to play.
*/

/*!
    \qmlproperty int SoundEffect::loops

    This property provides a way to control the number of times to repeat the sound on each play().

    Set to -1 (infinite) to enable infinite loop.
*/

/*!
    \qmlproperty int SoundEffect::volume

    This property holds the volume of the playback, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool SoundEffect::muted

    This property provides a way to control muting.
*/

/*!
    \qmlproperty bool SoundEffect::loaded

    This property indicates if the source has been loaded and ready for play
*/

/*!
    \qmlproperty bool SoundEffect::playing

    This property indicates if the soundeffect is playing or not.
*/

/*!
    \qmlproperty int SoundEffect::status

    This property indicates the following status of the soundeffect.

    Null: no source has been set or is null.
    Loading: the soundeffect is trying to load the source.
    Ready: the source is loaded and ready for play.
    Error: some error happened during operation, such as failure of loading the source.
*/

/*!
    \qmlsignal SoundEffect::sourceChanged()

    This handler is called when the source has changed.
*/

/*!
    \qmlsignal SoundEffect::loopsChanged()

    This handler is called when the number of loops has changed.
*/

/*!
    \qmlsignal SoundEffect::volumeChanged()

    This handler is called when the volume has changed.
*/

/*!
    \qmlsignal SoundEffect::mutedChanged()

    This handler is called when the mute state has changed.
*/

/*!
    \qmlsignal SoundEffect::loadedChanged()

    This handler is called when the loaded property has changed.
*/

/*!
    \qmlsignal SoundEffect::playingChanged()

    This handler is called when the playing property has changed.
*/

/*!
    \qmlsignal SoundEffect::statusChanged()

    This handler is called when the status property has changed.
*/


/*!
    \internal
*/

QSoundEffect::QSoundEffect(QObject *parent) :
    QObject(parent)
{
    d = new QSoundEffectPrivate(this);
    connect(d, SIGNAL(volumeChanged()), SIGNAL(volumeChanged()));
    connect(d, SIGNAL(mutedChanged()), SIGNAL(mutedChanged()));
    connect(d, SIGNAL(loadedChanged()), SIGNAL(loadedChanged()));
    connect(d, SIGNAL(playingChanged()), SIGNAL(playingChanged()));
    connect(d, SIGNAL(statusChanged()), SIGNAL(statusChanged()));
}

QSoundEffect::~QSoundEffect()
{
    d->deleteLater();
}

QStringList QSoundEffect::supportedMimeTypes()
{
    return QSoundEffectPrivate::supportedMimeTypes();
}

QUrl QSoundEffect::source() const
{
    return d->source();
}

void QSoundEffect::setSource(const QUrl &url)
{
    if (d->source() == url)
        return;

    d->setSource(url);

    emit sourceChanged();
}

int QSoundEffect::loopCount() const
{
    return d->loopCount();
}

void QSoundEffect::setLoopCount(int loopCount)
{
    if (loopCount < 0 && loopCount != Infinite) {
        qWarning("SoundEffect: loops should be SoundEffect.Infinite, 0 or positive integer");
        return;
    }
    if (loopCount == 0)
        loopCount = 1;
    if (d->loopCount() == loopCount)
        return;

    d->setLoopCount(loopCount);
    emit loopCountChanged();
}

qreal QSoundEffect::volume() const
{
    return qreal(d->volume()) / 100;
}

void QSoundEffect::setVolume(qreal volume)
{
    if (volume < 0 || volume > 1) {
        qWarning("SoundEffect: volume should be between 0.0 and 1.0");
        return;
    }
    int iVolume = qRound(volume * 100);
    if (d->volume() == iVolume)
        return;

    d->setVolume(iVolume);
    emit volumeChanged();
}

bool QSoundEffect::isMuted() const
{
    return d->isMuted();
}

void QSoundEffect::setMuted(bool muted)
{
    if (d->isMuted() == muted)
        return;

    d->setMuted(muted);
    emit mutedChanged();
}

bool QSoundEffect::isLoaded() const
{
    return d->isLoaded();
}

/*!
  \qmlmethod SoundEffect::play()

  Start playback of the sound effect, looping the effect for the number of
  times as specificed in the loops property.

  This is the default method for SoundEffect.

  \qml
  MouseArea {
      anchors.fill: parent
      onPressed: { SoundEffect { source:"soundeffect.wav" } }
  }
  \endqml
*/
void QSoundEffect::play()
{
    d->play();
}

bool QSoundEffect::isPlaying() const
{
    return d->isPlaying();
}

QSoundEffect::Status QSoundEffect::status() const
{
    return d->status();
}


/*!
  \qmlmethod SoundEffect::stop()

  Stop current playback.
  Note that if the backend is PulseAudio, due to the limitation of the underlying API,
  tis stop will only prevent next looping but will not be able to stop current playback immediately.

 */
void QSoundEffect::stop()
{
    d->stop();
}

QT_END_NAMESPACE

#include "moc_qsoundeffect_p.cpp"
