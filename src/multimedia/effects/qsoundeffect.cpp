/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
*/

/*!
    \qmlproperty int SoundEffect::volume

    This property provides a way to control the volume for playback.
*/

/*!
    \qmlproperty bool SoundEffect::muted

    This property provides a way to control muting.
*/

/*!
    \qmlsignal SoundEffect::sourceChanged()

    This handler is called when the source has changed.
*/

/*!
    \qmlsignal SoundEffect::loopsChanged()

    This handler is called when the number of loops has changes.
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

    This handler is called when the audio source is loaded and ready for play.
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
    if (loopCount == 0)
        loopCount = 1;
    if (d->loopCount() == loopCount)
        return;

    d->setLoopCount(loopCount);
    emit loopCountChanged();
}

int QSoundEffect::volume() const
{
    return d->volume();
}

void QSoundEffect::setVolume(int volume)
{
    if (d->volume() == volume)
        return;

    d->setVolume(volume);
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
