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
    , m_pendingState(0)
    , m_executedState(0)
    , m_graph(0)
    , m_builder(0)
    , m_source(0)
    , m_audioOutput(0)
    , m_videoOutput(0)
{
    start();
}

DirectShowRenderThread::~DirectShowRenderThread()
{
    {
        QMutexLocker locker(&m_mutex);
        m_pendingState = Return;

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

void DirectShowRenderThread::render(const QUrl &url, IGraphBuilder *graph)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        if (m_source) {
            m_source->Release();
            m_source = 0;
        }

        m_graph->Abort();
        m_graph->Release();
    }

    m_url = url;
    m_graph = graph;

    if (m_graph)
        m_graph->AddRef();

    m_pendingState = Render;

    m_wait.wakeAll();
}

void DirectShowRenderThread::abort()
{
    QMutexLocker locker(&m_mutex);

    Q_ASSERT(!(m_pendingState & (Abort | Return)));

    if (m_pendingState != 0) {
        m_pendingState |= Abort;

        // There's a small race condition here where you could call Abort
        // before a task is started, so we'll abort in a loop until the wait
        // succeeds.
        do {
            if (m_graph)
                m_graph->Abort();
        } while (!m_wait.wait(&m_mutex, 300));
    }

    if (m_builder) {
        m_builder->Release();
        m_builder = 0;
    }
    if (m_graph) {
        m_graph->Release();
        m_graph = 0;
    }

    m_executedState = 0;
}

void DirectShowRenderThread::setAudioOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_audioOutput) {
        if (m_executedState & AudioOutput) {
            m_graph->RemoveFilter(m_audioOutput);
        }
        m_audioOutput->Release();
    }

    m_audioOutput = filter;

    if (m_audioOutput) {
        m_audioOutput->AddRef();

        if (m_executedState & Render) {
            m_pendingState |= AudioOutput;

            m_wait.wakeAll();
        }
    }
}

void DirectShowRenderThread::setVideoOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_videoOutput) {
        if (m_executedState & VideoOutput) {
            m_graph->RemoveFilter(m_videoOutput);
        }
        m_videoOutput->Release();
    }

    m_videoOutput = filter;

    if (m_videoOutput) {
        m_videoOutput->AddRef();

        if (m_executedState & Render) {
            m_pendingState |= VideoOutput;

            m_wait.wakeAll();
        }
    }
}

void DirectShowRenderThread::run()
{
    QMutexLocker locker(&m_mutex);

    while (!(m_pendingState & Return)) {
        if (m_pendingState & Abort) {
            m_pendingState = 0;

            m_wait.wakeAll();
        } else if (m_pendingState & Render) {
            m_pendingState ^= Render;

            doRender(&locker);
        } else if (m_pendingState & AudioOutput) {
            m_pendingState ^= AudioOutput;

            doSetAudioOutput(&locker);
        } else if (m_pendingState & VideoOutput) {
            m_pendingState ^= VideoOutput;

            doSetVideoOutput(&locker);
        }

        if (m_pendingState == 0)
            m_wait.wait(&m_mutex);
    }
}

void DirectShowRenderThread::doRender(QMutexLocker *locker)
{
    m_executedState = 0;

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
            m_executedState = Render;

            m_source = source;

            if (m_audioOutput)
                m_pendingState |= AudioOutput;

            if (m_videoOutput)
                m_pendingState |= VideoOutput;

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
            m_executedState |= AudioOutput;
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
            m_executedState |= VideoOutput;
        }
    }

    videoOutput->Release();
    source->Release();
    graph->Release();
}
