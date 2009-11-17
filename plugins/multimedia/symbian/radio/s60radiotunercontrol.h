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

#ifndef S60RADIOTUNERCONTROL_H
#define S60RADIOTUNERCONTROL_H

#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include <QTime>

#include "qradiotunercontrol.h"
#include "qradiotuner.h"

class S60RadioTunerService;

class S60RadioTunerControl : public QRadioTunerControl
{
    Q_OBJECT
public:
    S60RadioTunerControl(QObject *parent = 0);
    ~S60RadioTunerControl();

    QRadioTuner::Band band() const;
    void setBand(QRadioTuner::Band b);
    bool isBandSupported(QRadioTuner::Band b) const;

    int frequency() const;
    int frequencyStep(QRadioTuner::Band b) const;
    QPair<int,int> frequencyRange(QRadioTuner::Band b) const;
    void setFrequency(int frequency);

    bool isStereo() const;
    QRadioTuner::StereoMode stereoMode() const;
    void setStereoMode(QRadioTuner::StereoMode mode);


    int signalStrength() const;

    qint64 duration() const;

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    bool isSearching() const;
    void searchForward();
    void searchBackward();
    void cancelSearch();

    bool isValid() const;

    void start();
    void stop();

    QRadioTuner::Error error() const;
    QString errorString() const;

private slots:
    void search();

private:
    bool initRadio();
    void setVol(int v);
    int  getVol();

    bool m_muted;
    bool m_stereo;
    bool low;
    bool available;
    int  tuners;
    int  step;
    int  vol;
    int  sig;
    bool scanning;
    bool forward;
    QTimer* timer;
    QRadioTuner::Band   currentBand;
    qint64 freqMin;
    qint64 freqMax;
    qint64 currentFreq;
    QTime  playTime;
};

#endif
