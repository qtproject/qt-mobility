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
}

RadioControl::~RadioControl()
{
    if(fd > 0)
        ::close(fd);
}

void RadioControl::setBand(int b)
{
    //???
}

int RadioControl::frequency() const
{
    return currentFreq;
}

void RadioControl::setFrequency(int frequency)
{
    v4l2_frequency freq;

    if(fd > 0) {
        memset( &freq, 0, sizeof( freq ) );
        freq.tuner = 0;
        if ( ioctl( fd, VIDIOC_G_FREQUENCY, &freq ) >= 0 ) {
            if(low)
                freq.frequency = (int)(frequency/62.5);
            else
                freq.frequency = (int)(frequency/62500);
            ioctl( fd, VIDIOC_S_FREQUENCY, &freq );
            currentFreq = frequency;
        }
        //changePropertyValue("frequency", frequency);
    }
}

void RadioControl::setStereo(bool stereo)
{
    //??? dont think we can set this
}

void RadioControl::setSignalStrength(int strength)
{
    // ??? this cannot be set, remove
}

void RadioControl::setDuration(qint64 duration)
{
    //???
}

void RadioControl::setVolume(int volume)
{
    //??? this would need to be impl in alsa or OSS!
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
        }
        changePropertyValue("muted", muted);
    }
}

void RadioControl::searchForward()
{
}

void RadioControl::searchBackward()
{
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
        //changePropertyValue("stereo", stereo);

        return true;
    }

    return false;
}



