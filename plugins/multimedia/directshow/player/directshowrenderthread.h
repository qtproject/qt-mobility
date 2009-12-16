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

#ifndef DIRECTSHOWRENDERTHREAD_H
#define DIRECTSHOWRENDERTHREAD_H

#include <QtCore/qmutex.h>
#include <QtCore/qthread.h>
#include <QtCore/qurl.h>
#include <QtCore/qwaitcondition.h>

#include <dshow.h>

class QUrl;

class DirectShowRenderThread : public QThread
{
    Q_OBJECT
public:
    DirectShowRenderThread(QObject *parent = 0);
    ~DirectShowRenderThread();

    void shutdown();

    void load(const QUrl &url, IGraphBuilder *graph);

    void setAudioOutput(IBaseFilter *filter);
    void setVideoOutput(IBaseFilter *filter);

    void setRate(qreal rate);
    void seek(qint64 position);

    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void loaded();

protected:
    void run();

private:
    void doLoad(QMutexLocker *locker);
    void doSetAudioOutput(QMutexLocker *locker);
    void doSetVideoOutput(QMutexLocker *locker);
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
        SetRate        = 0x0010,
        Seek           = 0x0020,
        Play           = 0x0040,
        Pause          = 0x0080,
        Stop           = 0x0100
    };

    int m_pendingTasks;
    int m_executingTask;
    int m_executedTasks;
    IGraphBuilder *m_graph;
    ICaptureGraphBuilder2 *m_builder;
    IBaseFilter *m_source;
    IBaseFilter *m_audioOutput;
    IBaseFilter *m_videoOutput;
    qreal m_rate;
    qint64 m_position;
    QUrl m_url;
    QMutex m_mutex;
    QWaitCondition m_wait;
};

#endif
