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

#include "radiocontrol.h"
#include "radioservice.h"
#include "qmediasource.h"

#include <QtCore/qdebug.h>

#include <fcntl.h>

#include <sys/ioctl.h>
#include "linux/videodev2.h"

RadioControl::RadioControl(QObject *parent)
    :QRadioTuner(parent)
{
    fd = -1;
    initRadio();
    muted = false;
    stereo = false;
    currentBand = QRadioPlayer::FM;
    step = 100000;
    scanning = false;
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(search()));
}

RadioControl::~RadioControl()
{
    if(fd > 0)
        ::close(fd);
}

int RadioControl::band() const
{
    return currentBand;
}

bool RadioControl::isSupportedBand(int b) const
{
    qWarning()<<b;
    QRadioPlayer::Band bnd = (QRadioPlayer::Band)b;
    switch(bnd) {
        case QRadioPlayer::FM:
            if(freqMin <= 87500000 && freqMax >= 108000000)
                return true;
            break;
        case QRadioPlayer::LW:
            if(freqMin <= 148500 && freqMax >= 283500)
                return true;
        case QRadioPlayer::AM:
            if(freqMin <= 520000 && freqMax >= 1610000)
                return true;
        default:
            if(freqMin <= 1711000 && freqMax >= 30000000)
                return true;
    }

    return false;
}

void RadioControl::setBand(int b)
{
    if(freqMin <= 87500000 && freqMax >= 108000000 && b == QRadioPlayer::FM) {
        // FM 87.5 to 108.0 MHz, except Japan 76-90 MHz
        currentBand =  (QRadioPlayer::Band)b;
        step = 100000; // 100kHz steps

    } else if(freqMin <= 148500 && freqMax >= 283500 && b == QRadioPlayer::LW) {
        // LW 148.5 to 283.5 kHz, 9kHz channel spacing (Europe, Africa, Asia)
        currentBand =  (QRadioPlayer::Band)b;
        step = 1000; // 1kHz steps

    } else if(freqMin <= 520000 && freqMax >= 1610000 && b == QRadioPlayer::AM) {
        // AM 520 to 1610 kHz, 9 or 10kHz channel spacing, extended 1610 to 1710 kHz
        currentBand =  (QRadioPlayer::Band)b;
        step = 1000; // 1kHz steps

    } else if(freqMin <= 1711000 && freqMax >= 30000000 && b == QRadioPlayer::SW) {
        // SW 1.711 to 30.0 MHz, divided into 15 bands. 5kHz channel spacing
        currentBand =  (QRadioPlayer::Band)b;
        step = 500; // 500Hz steps
    }
}

int RadioControl::frequency() const
{
    return currentFreq;
}

void RadioControl::setFrequency(int frequency)
{
    qint64 f = frequency;

    v4l2_frequency freq;

    //qWarning()<<"f="<<frequency<<", c="<<currentFreq<<", min="<<freqMin<<", max="<<freqMax;

    if(frequency < freqMin)
        f = freqMax;
    if(frequency > freqMax)
        f = freqMin;

    if(fd > 0) {
        memset( &freq, 0, sizeof( freq ) );
        // Use the first tuner
        freq.tuner = 0;
        if ( ioctl( fd, VIDIOC_G_FREQUENCY, &freq ) >= 0 ) {
            if(low) {
                // For low, freq in units of 62.5Hz, so convert from Hz to units.
                freq.frequency = (int)(f/62.5);
            } else {
                // For high, freq in units of 62.5kHz, so convert from Hz to units.
                freq.frequency = (int)(f/62500);
            }
            ioctl( fd, VIDIOC_S_FREQUENCY, &freq );
            currentFreq = f;
            emit frequencyChanged(currentFreq);
        }
    }
}

bool RadioControl::isStereo() const
{
    return stereo;
}

void RadioControl::setStereo(bool stereo)
{
    //??? dont think we can set this, TODO
}

int RadioControl::signalStrength() const
{
    v4l2_tuner tuner;

    // Return the first tuner founds signal strength.
    for ( int index = 0; index < tuners; ++index ) {
        memset( &tuner, 0, sizeof( tuner ) );
        tuner.index = index;
        if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) < 0 )
            continue;
        if ( tuner.type != V4L2_TUNER_RADIO )
            continue;
        // percentage signal strength
        return tuner.signal*100/65535;
    }

    return 0;
}

qint64 RadioControl::duration() const
{
    // ??? what is this supposed to do?
    return 0;
}

void RadioControl::setDuration(qint64 duration)
{
    //???
}

int RadioControl::volume() const
{
    v4l2_queryctrl queryctrl;

    if(fd > 0) {
        memset( &queryctrl, 0, sizeof( queryctrl ) );
        queryctrl.id = V4L2_CID_AUDIO_VOLUME;
        if ( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl ) >= 0 ) {
            v4l2_control control;
            if(queryctrl.maximum == 0) {
                qWarning()<<"volume must be done using alsa/OSS, TODO";
            } else {
                // percentage volume returned
                return queryctrl.default_value*100/queryctrl.maximum;
            }
        }
    }
    return 0;
}

void RadioControl::setVolume(int volume)
{
    v4l2_queryctrl queryctrl;

    if(fd > 0) {
        memset( &queryctrl, 0, sizeof( queryctrl ) );
        queryctrl.id = V4L2_CID_AUDIO_VOLUME;
        if ( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl ) >= 0 ) {
            v4l2_control control;
            qWarning()<<"min="<<queryctrl.minimum<<", max="<<queryctrl.maximum<<", vol="<<queryctrl.default_value<<", step="<<queryctrl.step;
            if(queryctrl.maximum > 0) {
                memset( &control, 0, sizeof( control ) );
                control.id = V4L2_CID_AUDIO_VOLUME;
                control.value = volume*queryctrl.maximum/100;
                ioctl( fd, VIDIOC_S_CTRL, &control );
            } else {
                qWarning()<<"volume adjustment needs to be impl with alsa/OSS code?";
            }
        }
    }
}

bool RadioControl::isMuted() const
{
    return muted;
}

void RadioControl::setMuted(bool muted)
{
    v4l2_queryctrl queryctrl;

    if(fd > 0) {
        memset( &queryctrl, 0, sizeof( queryctrl ) );
        queryctrl.id = V4L2_CID_AUDIO_MUTE;
        if ( ioctl( fd, VIDIOC_QUERYCTRL, &queryctrl ) >= 0 ) {
            v4l2_control control;
            memset( &control, 0, sizeof( control ) );
            control.id = V4L2_CID_AUDIO_MUTE;
            control.value = (muted ? queryctrl.maximum : queryctrl.minimum );
            ioctl( fd, VIDIOC_S_CTRL, &control );
            this->muted = muted;
        }
    }
}

void RadioControl::searchForward()
{
    // Scan up
    if(scanning) {
        timer->stop();
        scanning = false;
        return;
    }
    scanning = true;
    forward  = true;
    timer->start();
}

void RadioControl::searchBackward()
{
    // Scan down
    if(scanning) {
        timer->stop();
        scanning = false;
        return;
    }
    scanning = true;
    forward  = false;
    timer->start();
}

void RadioControl::search()
{
    if(!scanning) return;

    if(forward) {
        setFrequency(currentFreq+step);
    } else {
        setFrequency(currentFreq-step);
    }
    emit signalStrengthChanged(signalStrength());
}

bool RadioControl::initRadio()
{
    v4l2_tuner tuner;
    v4l2_input input;
    v4l2_audio audio;
    v4l2_frequency freq;
    v4l2_capability cap;

    low = false;
    available = false;
    freqMin = freqMax = currentFreq = 0;

    fd = ::open("/dev/radio", O_RDWR);

    if(fd != -1) {
        // Capabilites
        memset( &cap, 0, sizeof( cap ) );
        if(::ioctl(fd, VIDIOC_QUERYCAP, &cap ) >= 0) {
            if(((cap.capabilities & V4L2_CAP_RADIO) == 0) && ((cap.capabilities & V4L2_CAP_AUDIO) == 0))
                available = true;
        }

        // Tuners
        memset( &input, 0, sizeof( input ) );
        tuners = 0;
        for ( ;; ) {
            memset( &input, 0, sizeof( input ) );
            input.index = tuners;
            if ( ioctl( fd, VIDIOC_ENUMINPUT, &input ) < 0 )
                break;
            ++tuners;
        }

        // Freq bands
        for ( int index = 0; index < tuners; ++index ) {
            memset( &tuner, 0, sizeof( tuner ) );
            tuner.index = index;
            if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) < 0 )
                continue;
            if ( tuner.type != V4L2_TUNER_RADIO )
                continue;
            if ( ( tuner.capability & V4L2_TUNER_CAP_LOW ) != 0 ) {
                // Units are 1/16th of a kHz.
                low = true;
            }
            if(low) {
                freqMin = tuner.rangelow * 62.5;
                freqMax = tuner.rangehigh * 62.5;
            } else {
                freqMin = tuner.rangelow * 62500;
                freqMax = tuner.rangehigh * 62500;
            }
        }

        // frequency
        memset( &freq, 0, sizeof( freq ) );
        if(::ioctl(fd, VIDIOC_G_FREQUENCY, &freq ) >= 0) {
            if ( ((int)freq.frequency) != -1 ) {    // -1 means not set.
                if(low)
                    currentFreq = freq.frequency * 62.5;
                else
                    currentFreq = freq.frequency * 62500;
            }
        }

        qWarning()<<"min="<<freqMin<<", max="<<freqMax<<", current="<<currentFreq;

        // stereo
        bool stereo = false;
        memset( &tuner, 0, sizeof( tuner ) );
        if ( ioctl( fd, VIDIOC_G_TUNER, &tuner ) >= 0 ) {
            if((tuner.rxsubchans & V4L2_TUNER_SUB_STEREO) != 0)
                stereo = true;
        }

        return true;
    }

    return false;
}



