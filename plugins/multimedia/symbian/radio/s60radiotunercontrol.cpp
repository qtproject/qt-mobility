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

#include "s60radiotunercontrol.h"
#include "s60radiotunerservice.h"

#include <QtCore/qdebug.h>

S60RadioTunerControl::S60RadioTunerControl(QObject *parent)
    :QRadioTunerControl(parent)
{
    initRadio();
    m_tunerState = QRadioTuner::StoppedState;
    m_muted = false;
    m_stereo = false;
    sig = 0;
    currentBand = QRadioTuner::FM;
    step = 100000;
    scanning = false;
    playTime.restart();
    timer = new QTimer(this);
    timer->setInterval(200);
    connect(timer,SIGNAL(timeout()),this,SLOT(search()));
    timer->start();
}

S60RadioTunerControl::~S60RadioTunerControl()
{
    timer->stop();
}
QRadioTuner::State S60RadioTunerControl::state() const
{
    return m_tunerState;
}

QRadioTuner::Band S60RadioTunerControl::band() const
{
    return currentBand;
}

bool S60RadioTunerControl::isBandSupported(QRadioTuner::Band b) const
{
    QRadioTuner::Band bnd = (QRadioTuner::Band)b;
    switch(bnd) {
        case QRadioTuner::FM:
            if(freqMin <= 87500000 && freqMax >= 108000000)
                return true;
            break;
        case QRadioTuner::LW:
            if(freqMin <= 148500 && freqMax >= 283500)
                return true;
        case QRadioTuner::AM:
            if(freqMin <= 520000 && freqMax >= 1610000)
                return true;
        default:
            if(freqMin <= 1711000 && freqMax >= 30000000)
                return true;
    }

    return false;
}

void S60RadioTunerControl::setBand(QRadioTuner::Band b)
{

    if(freqMin <= 87500000 && freqMax >= 108000000 && b == QRadioTuner::FM) {
        // FM 87.5 to 108.0 MHz, except Japan 76-90 MHz
        currentBand =  (QRadioTuner::Band)b;
        step = 100000; // 100kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 148500 && freqMax >= 283500 && b == QRadioTuner::LW) {
        // LW 148.5 to 283.5 kHz, 9kHz channel spacing (Europe, Africa, Asia)
        currentBand =  (QRadioTuner::Band)b;
        step = 1000; // 1kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 520000 && freqMax >= 1610000 && b == QRadioTuner::AM) {
        // AM 520 to 1610 kHz, 9 or 10kHz channel spacing, extended 1610 to 1710 kHz
        currentBand =  (QRadioTuner::Band)b;
        step = 1000; // 1kHz steps
        emit bandChanged(currentBand);

    } else if(freqMin <= 1711000 && freqMax >= 30000000 && b == QRadioTuner::SW) {
        // SW 1.711 to 30.0 MHz, divided into 15 bands. 5kHz channel spacing
        currentBand =  (QRadioTuner::Band)b;
        step = 500; // 500Hz steps
        emit bandChanged(currentBand);
    }
    playTime.restart();
}

int S60RadioTunerControl::frequency() const
{
    return currentFreq;
}

void S60RadioTunerControl::setFrequency(int frequency)
{
    Q_UNUSED(frequency);
}
int S60RadioTunerControl::frequencyStep(QRadioTuner::Band b) const
{
    //FIXME These should be in api?
    int step = 0;

    if(b == QRadioTuner::FM)
        step = 100000; // 100kHz steps
    else if(b == QRadioTuner::LW)
        step = 1000; // 1kHz steps
    else if(b == QRadioTuner::AM)
        step = 1000; // 1kHz steps
    else if(b == QRadioTuner::SW)
        step = 500; // 500Hz steps

    return step;
}

QPair<int,int> S60RadioTunerControl::frequencyRange(QRadioTuner::Band b) const
{
    //FIXME These should be in api?
    if(b == QRadioTuner::AM)
        return qMakePair<int,int>(520000,1710000);
    else if(b == QRadioTuner::FM)
        return qMakePair<int,int>(87500000,108000000);
    else if(b == QRadioTuner::SW)
        return qMakePair<int,int>(1711111,30000000);
    else if(b == QRadioTuner::LW)
        return qMakePair<int,int>(148500,283500);

    return qMakePair<int,int>(0,0);
}

bool S60RadioTunerControl::isStereo() const
{
    return m_stereo;
}

QRadioTuner::StereoMode S60RadioTunerControl::stereoMode() const
{
    return QRadioTuner::Auto;
}

void S60RadioTunerControl::setStereoMode(QRadioTuner::StereoMode mode)
{
    m_stereo = true;

    if(mode == QRadioTuner::ForceMono)
        m_stereo = false;

}

int S60RadioTunerControl::signalStrength() const
{
    return 0;
}

qint64 S60RadioTunerControl::duration() const
{
    return playTime.elapsed();
}

int S60RadioTunerControl::volume() const
{
    return 0;
}

void S60RadioTunerControl::setVolume(int volume)
{
    Q_UNUSED(volume);
}

bool S60RadioTunerControl::isMuted() const
{
    return m_muted;
}

void S60RadioTunerControl::setMuted(bool muted)
{
    m_muted = muted;
}

bool S60RadioTunerControl::isSearching() const
{
    //TODO
    return false;
}

void S60RadioTunerControl::cancelSearch()
{
    //TODO
}

void S60RadioTunerControl::searchForward()
{
    // Scan up
    if(scanning) {
        scanning = false;
        return;
    }
    scanning = true;
    forward  = true;
}

void S60RadioTunerControl::searchBackward()
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

void S60RadioTunerControl::search()
{
    int signal = signalStrength();
    if(sig != signal) {
        sig = signal;
        emit signalStrengthChanged(sig);
    }

    if(!scanning) return;

    if(forward) {
        setFrequency(currentFreq+step);
    } else {
        setFrequency(currentFreq-step);
    }
    emit signalStrengthChanged(signalStrength());

}

bool S60RadioTunerControl::isValid() const
{
    return available;
}

bool S60RadioTunerControl::initRadio()
{
    return true;
}

void S60RadioTunerControl::setVol(int v)
{
    Q_UNUSED(v);
}

int S60RadioTunerControl::getVol()
{
    return 0;
}
void S60RadioTunerControl::start()
{

}

void S60RadioTunerControl::stop()
{

}

QRadioTuner::Error S60RadioTunerControl::error() const
{
    return QRadioTuner::NoError;
}
QString S60RadioTunerControl::errorString() const
{
    return QString();
}
