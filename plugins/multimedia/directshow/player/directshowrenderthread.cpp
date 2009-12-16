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

#include "directshowglobal.h"
#include "directshowrenderthread.h"

DirectShowRenderThread::DirectShowRenderThread(QObject *parent)
    : QThread(parent)
    , m_pendingTasks(0)
    , m_executingTask(0)
    , m_executedTasks(0)
    , m_graph(0)
    , m_builder(0)
    , m_source(0)
    , m_audioOutput(0)
    , m_videoOutput(0)
    , m_rate(1.0)
    , m_position(0)
{
    start();
}

DirectShowRenderThread::~DirectShowRenderThread()
{
    {
        QMutexLocker locker(&m_mutex);
        m_pendingTasks = Return;

        if (m_builder)
            m_builder->Release();

        if (m_graph)
            m_graph->Release();

        if (m_source)
            m_source->Release();

        if (m_audioOutput)
            m_audioOutput->Release();

        if (m_videoOutput)
            m_videoOutput->Release();

        m_wait.wakeAll();
    }
    wait();
}

void DirectShowRenderThread::load(const QUrl &url, IGraphBuilder *graph)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        if (m_source) {
            m_source->Release();
            m_source = 0;
        }

        if (m_executingTask != 0) {
            m_graph->Abort();
            m_pendingTasks = Stop;

            m_wait.wait(&m_mutex);
        }

        m_graph->Release();
    }

    m_url = url;
    m_graph = graph;

    if (m_graph)
        m_graph->AddRef();

    m_pendingTasks = Load;

    m_wait.wakeAll();
}

void DirectShowRenderThread::setAudioOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_audioOutput) {
        if (m_executedTasks & SetAudioOutput) {
            m_graph->RemoveFilter(m_audioOutput);
        }
        m_audioOutput->Release();
    }

    m_audioOutput = filter;

    if (m_audioOutput) {
        m_audioOutput->AddRef();

        if (m_executedTasks & Load) {
            m_pendingTasks |= SetAudioOutput;

            m_wait.wakeAll();
        }
    }
}

void DirectShowRenderThread::setVideoOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_videoOutput) {
        if (m_executedTasks & SetVideoOutput) {
            m_graph->RemoveFilter(m_videoOutput);
        }
        m_videoOutput->Release();
    }

    m_videoOutput = filter;

    if (m_videoOutput) {
        m_videoOutput->AddRef();

        if (m_executedTasks & Load) {
            m_pendingTasks |= SetVideoOutput;

            m_wait.wakeAll();
        }
    }
}

void DirectShowRenderThread::setRate(qreal rate)
{
    QMutexLocker locker(&m_mutex);

    m_rate = rate;

    m_pendingTasks |= SetRate;

    if (m_executedTasks & Load)
        m_wait.wakeAll();
}

void DirectShowRenderThread::seek(qint64 position)
{
    QMutexLocker locker(&m_mutex);

    m_position = position;

    m_pendingTasks |= Seek;

    if (m_executedTasks & Load)
        m_wait.wakeAll();
}

void DirectShowRenderThread::play()
{
    QMutexLocker locker(&m_mutex);

    m_pendingTasks &= ~Pause;
    m_pendingTasks |= Play;

    if (m_executedTasks & Load) {
        if (m_executedTasks & Stop) {
            m_position = 0;
            m_pendingTasks |= Seek;
        }

        m_wait.wakeAll();
    }
}

void DirectShowRenderThread::pause()
{
    QMutexLocker locker(&m_mutex);

    m_pendingTasks &= ~Play;
    m_pendingTasks |= Pause;

    if (m_executedTasks & Load) {
        if (m_executedTasks & Stop) {
            m_position = 0;
            m_pendingTasks |= Seek;
        }

        m_wait.wakeAll();
    }
}

void DirectShowRenderThread::stop()
{
    QMutexLocker locker(&m_mutex);

    m_pendingTasks &= ~(Play | Pause | Seek);

    if (m_executedTasks & Load) {
        if (m_executingTask & (Play | Pause | Seek)) {
            //m_graph->Abort();
            m_pendingTasks |= Stop;

            m_wait.wait(&m_mutex);
        }

        if (m_executedTasks & (Play | Pause)) {
            if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
                control->Stop();
                control->Release();
            }
            m_executedTasks &= ~(Play | Pause);
            m_executedTasks |= Stop;
        }
    }
}

void DirectShowRenderThread::run()
{
    QMutexLocker locker(&m_mutex);

    while (!(m_pendingTasks & Return)) {
        if (m_pendingTasks & Load) {
            m_pendingTasks ^= Load;
            m_executingTask = Load;

            doLoad(&locker);
        } else if (m_pendingTasks & SetAudioOutput) {
            m_pendingTasks ^= SetAudioOutput;
            m_executingTask = SetAudioOutput;

            doSetAudioOutput(&locker);
        } else if (m_pendingTasks & SetVideoOutput) {
            m_pendingTasks ^= SetVideoOutput;
            m_executingTask = SetVideoOutput;

            doSetVideoOutput(&locker);
        } else if (m_pendingTasks & SetRate) {
            m_pendingTasks ^= SetRate;
            m_executingTask = SetRate;

            doSetRate(&locker);
        } else if (m_pendingTasks & Stop) {
            m_pendingTasks ^= Stop;

            m_wait.wakeAll();
        } else if (m_pendingTasks & Pause) {
            m_pendingTasks ^= Pause;
            m_executingTask = Pause;

            doPause(&locker);
        } else if (m_pendingTasks & Seek) {
            m_pendingTasks ^= Seek;
            m_executingTask = Seek;

            doSeek(&locker);
        } else if (m_pendingTasks & Play) {
            m_pendingTasks ^= Play;
            m_executingTask = Play;

            doPlay(&locker);
        }
        m_executingTask = 0;

        if (m_pendingTasks == 0)
            m_wait.wait(&m_mutex);
    }
}

void DirectShowRenderThread::doLoad(QMutexLocker *locker)
{
    m_executedTasks = 0;

    if (m_builder)
        m_builder->Release();

    m_builder = com_new<ICaptureGraphBuilder2>(CLSID_CaptureGraphBuilder2);

    if (!m_builder) {
        qWarning("failed to create capture graph builder object");
    } else if (!SUCCEEDED(m_builder->SetFiltergraph(m_graph))) {
        qWarning("failed to set filter graph on capture builder");

        m_builder->Release();
        m_builder = 0;
    } else {
        IBaseFilter *source = 0;
        QUrl url = m_url;

        // This is potentially time consuming so unlock the mutex, and give the
        // caller an opportunity to abort.
        locker->unlock();
        HRESULT hr = m_graph->AddSourceFilter(url.toString().utf16(), L"Source", &source);
        locker->relock();

        if (SUCCEEDED(hr)) {
            m_executedTasks = Load;

            m_source = source;

            if (m_audioOutput)
                m_pendingTasks |= SetAudioOutput;

            if (m_videoOutput)
                m_pendingTasks |= SetVideoOutput;

            if (m_rate != 1.0)
                m_pendingTasks |= SetRate;

            emit loaded();
        }
    }
}

void DirectShowRenderThread::doSetAudioOutput(QMutexLocker *locker)
{
    IGraphBuilder *graph = m_graph;
    graph->AddRef();

    IBaseFilter *source = m_source;
    source->AddRef();

    IBaseFilter *audioOutput = m_audioOutput;
    audioOutput->AddRef();

    if (!SUCCEEDED(graph->AddFilter(m_audioOutput, L"AudioOutput"))) {
        qWarning("Failed to add audio output to filter graph.");
    } else {
        locker->unlock();
        HRESULT hr = m_builder->RenderStream(0, &MEDIATYPE_Audio, source, 0, audioOutput);
        locker->relock();

        if (!SUCCEEDED(hr)) {
            m_graph->RemoveFilter(audioOutput);
        } else {
            m_executedTasks |= SetAudioOutput;
        }
    }

    audioOutput->Release();
    source->Release();
    graph->Release();
}

void DirectShowRenderThread::doSetVideoOutput(QMutexLocker *locker)
{
    IGraphBuilder *graph = m_graph;
    graph->AddRef();

    IBaseFilter *source = m_source;
    source->AddRef();

    IBaseFilter *videoOutput = m_videoOutput;
    videoOutput->AddRef();

    if (!SUCCEEDED(graph->AddFilter(m_videoOutput, L"VideoOutput"))) {
        qWarning("Failed to add video output to filter graph.");
    } else {
        locker->unlock();
        HRESULT hr = m_builder->RenderStream(0, &MEDIATYPE_Video, source, 0, videoOutput);
        locker->relock();

        if (!SUCCEEDED(hr)) {
            m_graph->RemoveFilter(videoOutput);
        } else {
            m_executedTasks |= SetVideoOutput;
        }
    }

    videoOutput->Release();
    source->Release();
    graph->Release();
}

void DirectShowRenderThread::doSetRate(QMutexLocker *locker)
{
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph)) {
        locker->unlock();
        seeking->SetRate(m_rate);
        locker->relock();

        seeking->Release();
    }
}

void DirectShowRenderThread::doSeek(QMutexLocker *locker)
{
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph)) {
        LONGLONG pos = LONGLONG(m_position) * 10;

        locker->unlock();
        seeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning);
        locker->relock();

        seeking->Release();
    }
}

void DirectShowRenderThread::doPlay(QMutexLocker *locker)
{
    if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
        locker->unlock();
        HRESULT hr = control->Run();
        locker->relock();

        control->Release();

        if (SUCCEEDED(hr))
            m_executedTasks |= Play;
    }
}

void DirectShowRenderThread::doPause(QMutexLocker *locker)
{
    if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
        locker->unlock();
        HRESULT hr = control->Pause();
        locker->relock();

        control->Release();

        if (SUCCEEDED(hr))
            m_executedTasks |= Pause;
    }
}
