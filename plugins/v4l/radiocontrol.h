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

#ifndef RADIOCONTROL_H
#define RADIOCONTROL_H

#include <QtCore/qobject.h>

#include "qradiotuner.h"
#include "qmediasource.h"

#include "linux/videodev2.h"

class RadioService;

class RadioControl : public QRadioTuner
{
    Q_OBJECT
public:
    RadioControl(QObject *parent = 0);
    ~RadioControl();

    void setBand(int b);
    int frequency() const;
    void setFrequency(int frequency);
    void setStereo(bool stereo);
    void setSignalStrength(int strength);
    void setDuration(qint64 duration);
    void setVolume(int volume);
    void setMuted(bool muted);
    void searchForward();
    void searchBackward();

private:
    bool initRadio();

    int fd;

    bool low;
    bool available;
    int  tuners;
    qint64 freqMin;
    qint64 freqMax;
    qint64 currentFreq;
};

#endif
