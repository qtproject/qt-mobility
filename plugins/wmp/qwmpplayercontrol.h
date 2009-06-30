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

#ifndef QWMPPLAYERCONTROL_H
#define QWMPPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include "qwmpglobal.h"

#include <wmp.h>

class QUrl;

class Q_WMP_EXPORT QMediaPlayerControl : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        Stopped,
        Playing,
        Paused
    };

    QMediaPlayerControl(QObject *parent = 0) : QObject(parent) {}
    
    State state() const { return m_state; }

    qint64 duration() const { return m_duration; }
    virtual qint64 position() const = 0;

    bool isBuffering() const { return m_buffering; }

    virtual int bufferingProgress() const = 0;

    virtual int volume() const = 0;
    virtual bool isMuted() const = 0;

    virtual QUrl url() const = 0;
    virtual void setUrl(const QUrl &url) = 0;

public Q_SLOTS:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual void setVolume(int volume) = 0;
    virtual void setMuted(bool muted) = 0;

Q_SIGNALS:
    void stateChanged(QMediaPlayerControl::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void bufferingChanged(bool buffering);
    void currentMediaChanged();

protected:
    void setState(State state) { emit stateChanged(m_state = state); }
    void setDuration(qint64 duration) { emit durationChanged(m_duration = duration); }
    void setBuffering(bool buffering) { emit bufferingChanged(m_buffering = buffering); }

private:
    State m_state;
    qint64 m_duration;
    bool m_buffering;


};

class QWmpPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT
public:
    QWmpPlayerControl(IWMPCore3 *player, QObject *parent = 0);
    ~QWmpPlayerControl();

    qint64 position() const;

    int bufferingProgress() const;

    int volume() const;
    bool isMuted() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    using QMediaPlayerControl::setState;
    using QMediaPlayerControl::setDuration;
    using QMediaPlayerControl::setBuffering;
    using QMediaPlayerControl::currentMediaChanged;
    using QMediaPlayerControl::positionChanged;

public Q_SLOTS:
    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

private:
    IWMPCore3 *m_player;
    IWMPControls *m_controls;
    IWMPSettings *m_settings;
    IWMPNetwork *m_network;
};

#endif
