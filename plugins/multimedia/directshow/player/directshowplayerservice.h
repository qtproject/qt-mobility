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

#ifndef DIRECTSHOWPLAYERSERVICE_H
#define DIRECTSHOWPLAYERSERVICE_H

#include <qmediaservice.h>

#include "directshoweventloop.h"
#include "directshowglobal.h"

#include <QtCore/qmutex.h>
#include <QtCore/qurl.h>
#include <QtCore/qwaitcondition.h>

#include <QtCore/private/qwineventnotifier_p.h>

class DirectShowAudioEndpointControl;
class DirectShowMetaDataControl;
class DirectShowPlayerControl;
class DirectShowVideoOutputControl;
class DirectShowVideoRendererControl;
class Vmr9VideoWindowControl;

QTM_BEGIN_NAMESPACE
class QMediaContent;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class DirectShowPlayerService : public QMediaService
{
    Q_OBJECT
public:
    enum GraphStatus
    {
        NoMedia,
        Loading,
        Loaded,
        InvalidMedia
    };

    enum StreamType
    {
        AudioStream = 0x01,
        VideoStream = 0x02
    };

    DirectShowPlayerService(QObject *parent = 0);
    ~DirectShowPlayerService();

    QMediaControl* control(const char *name) const;

    GraphStatus graphStatus() const { return m_graphStatus; }

    IFilterGraph2 *graph() { return m_graph; }
    IBaseFilter *source() { return 0; }

    void load(const QMediaContent &media, QIODevice *stream);
    void play();
    void pause();
    void stop();

    qint64 position() const;
    qint64 duration() const { return m_duration; }

    void seek(qint64 position);
    void setRate(qreal rate);

    void setAudioOutput(IBaseFilter *filter);
    void setVideoOutput(IBaseFilter *filter);

private Q_SLOTS:
    void videoOutputChanged();
    void graphEvent(HANDLE handle);

private:
    void removeOutput(IBaseFilter *output);
    IBaseFilter *findChainStart(IBaseFilter *end) const;
    bool isConnected(IBaseFilter *filter, PIN_DIRECTION direction) const;

    void run();

    void doLoad(QMutexLocker *locker);
    void doRender(QMutexLocker *locker);
    void doSetRate(QMutexLocker *locker);
    void doSeek(QMutexLocker *locker);
    void doPlay(QMutexLocker *locker);
    void doPause(QMutexLocker *locker);

    enum Task
    {
        Shutdown       = 0x0001,
        Load           = 0x0002,
        SetAudioOutput = 0x0004,
        SetVideoOutput = 0x0008,
        Render         = 0x0010,
        SetRate        = 0x0020,
        Seek           = 0x0040,
        Play           = 0x0080,
        Pause          = 0x0100,
        Stop           = 0x0200
    };

    DirectShowPlayerControl *m_playerControl;
    DirectShowMetaDataControl *m_metaDataControl;
    DirectShowVideoOutputControl *m_videoOutputControl;
    DirectShowVideoRendererControl *m_videoRendererControl;
    Vmr9VideoWindowControl *m_videoWindowControl;
    DirectShowAudioEndpointControl *m_audioEndpointControl;

    QThread *m_taskThread;
    int m_pendingTasks;
    int m_executingTask;
    int m_executedTasks;
    GraphStatus m_graphStatus;
    QIODevice *m_stream;
    IFilterGraph2 *m_graph;
    IBaseFilter *m_source;
    IBaseFilter *m_audioOutput;
    IBaseFilter *m_videoOutput;
    int m_streamTypes;
    qreal m_rate;
    qint64 m_position;
    qint64 m_duration;

    QUrl m_url;
    QMutex m_mutex;
    QWaitCondition m_wait;
    QWinEventNotifier m_graphEventNotifier;
    DirectShowEventLoop m_loop;

    friend class DirectShowPlayerServiceThread;
};


#endif
