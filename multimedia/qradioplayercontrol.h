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

#ifndef QRADIOPLAYERCONTROL_H
#define QRADIOPLAYERCONTROL_H

#include "qabstractmediacontrol.h"

#include "qradioplayer.h"

class Q_MEDIA_EXPORT QRadioPlayerControl : public QAbstractMediaControl
{
    Q_OBJECT

public:
    ~QRadioPlayerControl();

    virtual QRadioPlayer::Band band() const = 0;
    virtual void setBand(QRadioPlayer::Band b) = 0;
    virtual bool isSupportedBand(QRadioPlayer::Band b) const = 0;

    virtual int frequency() const = 0;
    virtual void setFrequency(int frequency) = 0;

    virtual bool isStereo() const = 0;
    virtual void setStereo(bool stereo) = 0;

    virtual int signalStrength() const = 0;

    virtual qint64 duration() const = 0;

    virtual int volume() const = 0;
    virtual void setVolume(int volume) = 0;

    virtual bool isMuted() const = 0;
    virtual void setMuted(bool muted) = 0;

    virtual bool isSearching() const = 0;

    virtual void searchForward() = 0;
    virtual void searchBackward() = 0;
    virtual void cancelSearch() = 0;

Q_SIGNALS:
    void bandChanged(QRadioPlayer::Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void searchingStatusChanged(bool stereo);
    void signalStrengthChanged(int signalStrength);
    void durationChanged(qint64 durattion);
    void volumeChanged(int volume);
    void mutingChanged(bool muted);

protected:
    QRadioPlayerControl(QObject *parent = 0);
};

#define QRadioPlayerControl_iid "com.nokia.qt.RadioPlayerControl"
Q_MEDIA_DECLARE_CONTROL(QRadioPlayerControl, QRadioPlayerControl_iid)

#endif  // QRADIOPLAYERCONTROL_H
