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
#include "directshowiosource.h"
#include "directshowrenderthread.h"

#include <QtCore/qvarlengtharray.h>

DirectShowRenderThread::DirectShowRenderThread(QObject *parent)
    : QThread(parent)
    , m_pendingTasks(0)
    , m_executingTask(0)
    , m_executedTasks(0)
    , m_stream(0)
    , m_graph(0)
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
    if (m_audioOutput)
        m_audioOutput->Release();

    if (m_videoOutput)
        m_videoOutput->Release();
}

void DirectShowRenderThread::shutdown()
{
    {
        QMutexLocker locker(&m_mutex);
        m_pendingTasks = Shutdown;

        if (m_graph)
            m_graph->Abort();
   
        m_wait.wakeAll();
    }

    wait();

    if (m_graph) {
        m_graph->Release();
        m_graph = 0;
    }

    if (m_source) {
        m_source->Release();
        m_source = 0;
    }
}

void DirectShowRenderThread::load(const QUrl &url, QIODevice *stream, IFilterGraph2 *graph)
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
    m_stream = stream;
    m_graph = graph;

    if (m_graph) {
        m_graph->AddRef();

        m_pendingTasks = Load;

        m_wait.wakeAll();
    }
}

void DirectShowRenderThread::setAudioOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        // Don't mess with the graph while it's being generated.
        // But should be smarter than this and interrupt; see IAMOpenProgress.
        while (m_executingTask == Render) {
            m_pendingTasks |= SetAudioOutput;

            if (IAMOpenProgress *progress = com_cast<IAMOpenProgress>(m_graph)) {
                progress->AbortOperation();
                progress->Release();
            }

            m_wait.wait(&m_mutex);
        }

        if (m_audioOutput) {
            if (m_executedTasks & SetAudioOutput) {
                m_graph->RemoveFilter(m_audioOutput);
            }
            m_audioOutput->Release();
        }

        m_audioOutput = filter;

        if (m_executedTasks & Load) {
            m_pendingTasks |= Render | SetAudioOutput;

            m_wait.wakeAll();
        }

        if (m_audioOutput) {
            m_audioOutput->AddRef();
        } else {
            m_pendingTasks &= ~SetAudioOutput;
        }
    } else {
        if (m_audioOutput)
            m_audioOutput->Release();

        m_audioOutput = filter;

        if (m_audioOutput)
            m_audioOutput->AddRef();
    }
}

void DirectShowRenderThread::setVideoOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    // Don't mess with the graph while it's being generated.
    // But should be smarter than this and interrupt; see IAMOpenProgress.

    if (m_graph) {
        while (m_executingTask == Render) {
            m_pendingTasks |= SetVideoOutput;

                if (IAMOpenProgress *progress = com_cast<IAMOpenProgress>(m_graph)) {
                    progress->AbortOperation();
                    progress->Release();
                }

            m_wait.wait(&m_mutex);
        }

        if (m_videoOutput) {
            if (m_executedTasks & SetVideoOutput && m_graph) {
                m_graph->RemoveFilter(m_videoOutput);
            }
            m_videoOutput->Release();
        }

        m_videoOutput = filter;

        if (m_executedTasks & Load && m_graph) {
            m_pendingTasks |= Render | SetVideoOutput;

            m_wait.wakeAll();
        }

        if (m_videoOutput) {
            m_videoOutput->AddRef();
        } else {
            m_pendingTasks &= ~ SetVideoOutput;
        }
    } else {
        if (m_videoOutput)
            m_videoOutput->Release();
        
        m_videoOutput = filter;

        if (m_videoOutput)
            m_videoOutput->AddRef();
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

    while (!(m_pendingTasks & Shutdown)) {
        if (m_pendingTasks & Load) {
            m_pendingTasks ^= Load;
            m_executingTask = Load;

            doLoad(&locker);
        } else if (m_pendingTasks & Render) {
            m_pendingTasks ^= Render;
            m_executingTask = Render;

            doRender(&locker);
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
    qDebug(Q_FUNC_INFO);
    m_executedTasks = 0;

    IBaseFilter *source = 0;
    QUrl url = m_url;
    QIODevice *stream = m_stream;

    HRESULT hr = S_OK;

    if (stream) {
        source = new DirectShowIOSource(stream);

        if (!SUCCEEDED(hr = m_graph->AddFilter(source, L"Source"))) {
            source->Release();
            source = 0;
        }
    } else {
        locker->unlock();
        hr = m_graph->AddSourceFilter(url.toString().utf16(), L"Source", &source);
        locker->relock();
    }

    if (SUCCEEDED(hr)) {
        m_executedTasks = Load;
        m_pendingTasks |= Render;

        if (m_audioOutput)
            m_pendingTasks |= SetAudioOutput;
        if (m_videoOutput)
            m_pendingTasks |= SetVideoOutput;

        m_source = source;

        if (m_rate != 1.0)
            m_pendingTasks |= SetRate;

        emit loaded();
    }
}

void DirectShowRenderThread::doRender(QMutexLocker *locker)
{
    if (m_pendingTasks & SetAudioOutput) {
        m_graph->AddFilter(m_audioOutput, L"AudioOutput");

        m_pendingTasks ^= SetAudioOutput;
        m_executedTasks |= SetAudioOutput;
    }
    if (m_pendingTasks & SetVideoOutput) {
        m_graph->AddFilter(m_videoOutput, L"VideoOutput");

        m_pendingTasks ^= SetVideoOutput;
        m_executedTasks |= SetVideoOutput;
    }

    QVarLengthArray<IBaseFilter *, 16> filters;
    m_source->AddRef();
    filters.append(m_source);

    while (!filters.isEmpty()) {
        IEnumPins *pins = 0;
        IBaseFilter *filter = filters[filters.size() - 1];
        filters.removeLast();

        if (!(m_pendingTasks & (SetAudioOutput | SetVideoOutput))
                && SUCCEEDED(filter->EnumPins(&pins))) {
            for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
                PIN_DIRECTION direction;
                if (pin->QueryDirection(&direction) == S_OK && direction == PINDIR_OUTPUT) {
                    IPin *peer = 0;
                    if (pin->ConnectedTo(&peer) == S_OK) {
                        PIN_INFO peerInfo;
                        if (peer->QueryPinInfo(&peerInfo) == S_OK) {
                            filters.append(peerInfo.pFilter);
                        } else {
                            peerInfo.pFilter->Release();
                        }
                        peer->Release();
                    } else {
                        locker->unlock();
                        m_graph->RenderEx(pin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, 0);
                        locker->relock();
                    }
                }
            }
        }
        filter->Release();
    }
}

void DirectShowRenderThread::doSetRate(QMutexLocker *locker)
{
    qDebug(Q_FUNC_INFO);
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph)) {
        locker->unlock();
        seeking->SetRate(m_rate);
        locker->relock();

        seeking->Release();
    }
}

void DirectShowRenderThread::doSeek(QMutexLocker *locker)
{
    qDebug(Q_FUNC_INFO);
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
    qDebug(Q_FUNC_INFO);
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
    qDebug(Q_FUNC_INFO);
    if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
        locker->unlock();
        HRESULT hr = control->Pause();
        locker->relock();

        control->Release();

        if (SUCCEEDED(hr))
            m_executedTasks |= Pause;
    }
}
