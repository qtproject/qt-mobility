/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60radiocontrol.h"
#include "s60radioservice.h"

#include <QtCore/qdebug.h>

S60RadioControl::S60RadioControl(QObject *parent)
    :QRadioPlayerControl(parent)
{
    initRadio();
    muted = false;
    stereo = false;
    sig = 0;
    currentBand = QRadioPlayer::FM;
    step = 100000;
    scanning = false;
    playTime.restart();
    timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer,SIGNAL(timeout()),this,SLOT(search()));
    timer->start();
}

S60RadioControl::~S60RadioControl()
{
    timer->stop();
}

QRadioPlayer::Band S60RadioControl::band() const
{
    return currentBand;
}

bool S60RadioControl::isSupportedBand(QRadioPlayer::Band b) const
{
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

void S60RadioControl::setBand(QRadioPlayer::Band b)
{
	//TODO COMMON CODE, NOT SYMBIAN SPESIFIC?

    if(freqMin <= 87500000 && freqMax >= 108000000 && b == QRadioPlayer::FM) {
        // FM 87.5 to 108.0 MHz, except Japan 76-90 MHz
        currentBand =  (QRadioPlayer::Band)b;
        step = 100000; // 100kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 148500 && freqMax >= 283500 && b == QRadioPlayer::LW) {
        // LW 148.5 to 283.5 kHz, 9kHz channel spacing (Europe, Africa, Asia)
        currentBand =  (QRadioPlayer::Band)b;
        step = 1000; // 1kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 520000 && freqMax >= 1610000 && b == QRadioPlayer::AM) {
        // AM 520 to 1610 kHz, 9 or 10kHz channel spacing, extended 1610 to 1710 kHz
        currentBand =  (QRadioPlayer::Band)b;
        step = 1000; // 1kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 1711000 && freqMax >= 30000000 && b == QRadioPlayer::SW) {
        // SW 1.711 to 30.0 MHz, divided into 15 bands. 5kHz channel spacing
        currentBand =  (QRadioPlayer::Band)b;
        step = 500; // 500Hz steps
        emit bandChanged(currentBand);
    }
    playTime.restart();
}

int S60RadioControl::frequency() const
{
    return currentFreq;
}

void S60RadioControl::setFrequency(int frequency)
{
}

bool S60RadioControl::isStereo() const
{
    return stereo;
}

void S60RadioControl::setStereo(bool stereo)
{

}

int S60RadioControl::signalStrength() const
{
	return 0;
}

qint64 S60RadioControl::duration() const
{
    return playTime.elapsed();
}

int S60RadioControl::volume() const
{
    return 0;
}

void S60RadioControl::setVolume(int volume)
{
}

bool S60RadioControl::isMuted() const
{
    return muted;
}

void S60RadioControl::setMuted(bool muted)
{

}

bool S60RadioControl::isSearching() const
{
    //TODO
    return false;
}

void S60RadioControl::cancelSearch()
{
    //TODO
}

void S60RadioControl::searchForward()
{
    // Scan up
    if(scanning) {
        scanning = false;
        return;
    }
    scanning = true;
    forward  = true;
}

void S60RadioControl::searchBackward()
{
    // Scan down
    if(scanning) {
        scanning = false;
        return;
    }
    scanning = true;
    forward  = false;
    timer->start();
}

void S60RadioControl::search()
{
    int signal = signalStrength();
    if(sig != signal) {
        sig = signal;
        emit signalStrengthChanged(sig);
    }
    emit durationChanged(playTime.elapsed());

    if(!scanning) return;

    if(forward) {
        setFrequency(currentFreq+step);
    } else {
        setFrequency(currentFreq-step);
    }
    emit signalStrengthChanged(signalStrength());
}

bool S60RadioControl::isValid() const
{
    return available;
}

bool S60RadioControl::initRadio()
{
    return true;
}

void S60RadioControl::setVol(int v)
{
}

int S60RadioControl::getVol()
{

    return 0;
}

