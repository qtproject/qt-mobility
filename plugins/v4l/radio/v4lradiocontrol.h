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

#ifndef V4LRADIOCONTROL_H
#define V4LRADIOCONTROL_H

#include <QtCore/qobject.h>
#include <QtCore/qtimer.h>
#include <QTime>

#include "qradioplayercontrol.h"
#include "qradioplayer.h"

#include "linux/videodev2.h"

class V4LRadioService;

class V4LRadioControl : public QRadioPlayerControl
{
    Q_OBJECT
public:
    V4LRadioControl(QObject *parent = 0);
    ~V4LRadioControl();

    QRadioPlayer::Band band() const;
    void setBand(QRadioPlayer::Band b);
    bool isSupportedBand(QRadioPlayer::Band b) const;

    int frequency() const;
    void setFrequency(int frequency);

    bool isStereo() const;
    void setStereo(bool stereo);

    int signalStrength() const;

    qint64 duration() const;

    int volume() const;
    void setVolume(int volume);

    bool isMuted() const;
    void setMuted(bool muted);

    bool isSearching() const;
    void cancelSearch();

    bool isValid() const;

    void searchForward();
    void searchBackward();

private slots:
    void search();

private:
    bool initRadio();
    void setVol(int v);
    int  getVol();

    int fd;

    bool muted;
    bool stereo;
    bool low;
    bool available;
    int  tuners;
    int  step;
    int  vol;
    int  sig;
    bool scanning;
    bool forward;
    QTimer* timer;
    QRadioPlayer::Band   currentBand;
    qint64 freqMin;
    qint64 freqMax;
    qint64 currentFreq;
    QTime  playTime;
};

#endif
