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

#include "directshowplayerservice.h"

#include "directshowaudioendpointcontrol.h"
#include "directshowiosource.h"
#include "directshowmetadatacontrol.h"
#include "directshowplayercontrol.h"
#include "directshowvideooutputcontrol.h"
#include "directshowvideorenderercontrol.h"
#include "vmr9videowindowcontrol.h"

#include <qmediacontent.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qthread.h>
#include <QtCore/qvarlengtharray.h>

#include <uuids.h>

class DirectShowPlayerServiceThread : public QThread
{
public:
    DirectShowPlayerServiceThread(DirectShowPlayerService *service)
        : m_service(service)
    {
    }

protected:
    void run() { m_service->run(); }

private:
    DirectShowPlayerService *m_service;
};

DirectShowPlayerService::DirectShowPlayerService(QObject *parent)
    : QMediaService(parent)
    , m_playerControl(0)
    , m_audioEndpointControl(0)
    , m_metaDataControl(0)
    , m_videoOutputControl(0)
    , m_videoRendererControl(0)
    , m_videoWindowControl(0)
    , m_taskThread(0)
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
    m_playerControl = new DirectShowPlayerControl(this);
    m_metaDataControl = new DirectShowMetaDataControl(this);
    m_videoOutputControl = new DirectShowVideoOutputControl; 
    m_audioEndpointControl = new DirectShowAudioEndpointControl(this);
    m_videoRendererControl = new DirectShowVideoRendererControl(&m_loop);
    m_videoWindowControl = new Vmr9VideoWindowControl;

    m_taskThread = new DirectShowPlayerServiceThread(this);
    m_taskThread->start();

    connect(m_videoOutputControl, SIGNAL(outputChanged()), this, SLOT(videoOutputChanged()));
    connect(m_videoRendererControl, SIGNAL(filterChanged()), this, SLOT(videoOutputChanged()));

    connect(&m_graphEventNotifier, SIGNAL(activated(HANDLE)), this, SLOT(graphEvent(HANDLE)));
}

DirectShowPlayerService::~DirectShowPlayerService()
{
    {
        QMutexLocker locker(&m_mutex);

        m_pendingTasks = Shutdown;

        if (m_graph) {
            m_graphEventNotifier.setEnabled(false);

            if (IAMOpenProgress *progress = com_cast<IAMOpenProgress>(m_graph)) {
                progress->AbortOperation();
                progress->Release();
            }
            m_graph->Abort();
        }
        m_wait.wakeAll();

        m_loop.wait(&m_mutex);
    }

    m_taskThread->wait();
    delete m_taskThread;

    if (m_audioOutput) {
        m_audioOutput->Release();
        m_audioOutput = 0;
    }

    if (m_videoOutput) {
        m_videoOutput->Release();
        m_videoOutput = 0;
    }

    delete m_playerControl;
    delete m_audioEndpointControl;
    delete m_metaDataControl;
    delete m_videoOutputControl;
    delete m_videoRendererControl;
    delete m_videoWindowControl;
}

QMediaControl *DirectShowPlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_playerControl;
    else if (qstrcmp(name, QAudioEndpointSelector_iid) == 0)
        return m_audioEndpointControl;
    else if (qstrcmp(name, QMetaDataControl_iid) == 0)
        return m_metaDataControl;
    else if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutputControl;
    else if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRendererControl;
    else if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindowControl;
    else
        return 0;
}

void DirectShowPlayerService::load(const QMediaContent &media, QIODevice *stream)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        m_graphEventNotifier.setEnabled(false);

        if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
            control->Stop();
            control->Release();
        }

        if (m_source) {
            m_source->Release();
            m_source = 0;
        }

        if (m_executingTask != 0) {
            if (IAMOpenProgress *progress = com_cast<IAMOpenProgress>(m_graph)) {
                progress->AbortOperation();
                progress->Release();
            }
            m_pendingTasks = Stop;

            m_loop.wait(&m_mutex);
        }

        m_graph->Release();
        m_graph = 0;
    }

    m_stream = stream;

    if (media.isNull() && !stream) {
        m_url.clear();
    } else {
        m_url = media.canonicalUrl();

        m_graph = com_new<IFilterGraph2>(CLSID_FilterGraph);

        if (IMediaEventEx *event = com_cast<IMediaEventEx>(m_graph)) {
            HANDLE handle;
            if (event->GetEventHandle(reinterpret_cast<OAEVENT *>(&handle)) == S_OK) {
                event->CancelDefaultHandling(EC_BUFFERING_DATA);
                event->CancelDefaultHandling(EC_COMPLETE);
                event->CancelDefaultHandling(EC_LOADSTATUS);
                event->CancelDefaultHandling(EC_STATE_CHANGE);
                event->CancelDefaultHandling(EC_LENGTH_CHANGED);
                m_graphEventNotifier.setHandle(handle);
                m_graphEventNotifier.setEnabled(true);
            }
            event->Release();
        }

        m_pendingTasks = Load;

        m_wait.wakeAll();
    }
}

void DirectShowPlayerService::play()
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

void DirectShowPlayerService::pause()
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

void DirectShowPlayerService::stop()
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

void DirectShowPlayerService::setRate(qreal rate)
{
    QMutexLocker locker(&m_mutex);

    m_rate = rate;

    m_pendingTasks |= SetRate;

    if (m_executedTasks & Load)
        m_wait.wakeAll();
}

void DirectShowPlayerService::seek(qint64 position)
{
    QMutexLocker locker(&m_mutex);

    m_position = position;

    m_pendingTasks |= Seek;

    if (m_executedTasks & Load)
        m_wait.wakeAll();
}

void DirectShowPlayerService::setAudioOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        if (m_audioOutput && m_executedTasks & SetAudioOutput) {
            removeOutput(m_audioOutput);

            m_executedTasks &= ~SetAudioOutput;
        }

        m_audioOutput = filter;

        if (m_audioOutput) {
            m_audioOutput->AddRef();

            m_pendingTasks |= SetAudioOutput;
        } else {
            m_pendingTasks &= ~ SetAudioOutput;
        }

        if (m_executedTasks & Load) {
            m_pendingTasks |= Render;

            m_wait.wakeAll();
        }
    } else {
        if (m_audioOutput)
            m_audioOutput->Release();

        m_audioOutput = filter;

        if (m_audioOutput)
            m_audioOutput->AddRef();
    }
}

void DirectShowPlayerService::setVideoOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        if (m_videoOutput && m_executedTasks & SetVideoOutput) {
            removeOutput(m_videoOutput);

            m_executedTasks &= ~SetVideoOutput;
        }

        m_videoOutput = filter;

        if (m_videoOutput) {
            m_videoOutput->AddRef();

            m_pendingTasks |= SetVideoOutput;
        } else {
            m_pendingTasks &= ~ SetVideoOutput;
        }

        if (m_executedTasks & Load) {
            m_pendingTasks |= Render;

            m_wait.wakeAll();
        }
    } else {
        if (m_videoOutput)
            m_videoOutput->Release();

        m_videoOutput = filter;

        if (m_videoOutput)
            m_videoOutput->AddRef();
    }
}

void DirectShowPlayerService::videoOutputChanged()
{
    IBaseFilter *videoOutput = 0;

    switch (m_videoOutputControl->output()) {
    case QVideoOutputControl::RendererOutput:
        videoOutput = m_videoRendererControl->filter();
        break;
    case QVideoOutputControl::WindowOutput:
        videoOutput = m_videoWindowControl->filter();
        break;
    default:
        break;
    }

    setVideoOutput(videoOutput);
}

void DirectShowPlayerService::graphEvent(HANDLE handle)
{
    if (IMediaEvent *event = com_cast<IMediaEvent>(m_graph)) {
        long eventCode;
        LONG_PTR param1;
        LONG_PTR param2;

        while (event->GetEvent(&eventCode, &param1, &param2, 0) == S_OK) {
            qDebug("Graph Event %x", eventCode);
            switch (eventCode) {
            case EC_BUFFERING_DATA:
                m_playerControl->bufferingData(param1);
                break;
            case EC_COMPLETE:
                m_playerControl->complete(HRESULT(param1));
                break;
            case EC_LOADSTATUS:
                m_playerControl->loadStatus(param1);
                break;
            case EC_STATE_CHANGE:
                m_playerControl->stateChange(param1);
                break;
            case EC_LENGTH_CHANGED:
                m_playerControl->durationChanged(m_playerControl->duration());
                break;
            default:
                break;
            }

            event->FreeEventParams(eventCode, param1, param2);
        }
        event->Release();
    }
}

void DirectShowPlayerService::loaded()
{
    m_playerControl->durationChanged(m_playerControl->duration());
}

void DirectShowPlayerService::removeOutput(IBaseFilter *output)
{
    while (m_executingTask != 0) {
        if (m_executingTask == Render) {
            m_executingTask = 0;

            if (IAMOpenProgress *progress = com_cast<IAMOpenProgress>(m_graph)) {
                progress->AbortOperation();
                progress->Release();
            }
        }
        m_loop.wait(&m_mutex);
    }

    m_pendingTasks |= m_executedTasks & (Play | Pause);
    m_executedTasks &= ~(Play | Pause);

    if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
        control->Stop();
        control->Release();
    }

    if (IFilterChain *chain = com_cast<IFilterChain>(m_graph)) {
        IBaseFilter *start = findChainStart(output);

        chain->RemoveChain(start, output);

        start->Release();
        chain->Release();
    } else {
        m_graph->RemoveFilter(output);
    }

    output->Release();
}

IBaseFilter *DirectShowPlayerService::findChainStart(IBaseFilter *end) const
{
    // Note: This assumes none of the filters have multiple connected input pins.

    IBaseFilter *start = 0;
    end->AddRef();

    while (start != end) {
        start = end;

        IEnumPins *pins = 0;

        if (SUCCEEDED(end->EnumPins(&pins))) {
            for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
                PIN_DIRECTION direction;
                if (SUCCEEDED(pin->QueryDirection(&direction)) && direction == PINDIR_INPUT) {
                    IPin *peer = 0;
                    if (SUCCEEDED(pin->ConnectedTo(&peer))) {
                        PIN_INFO peerInfo;
                        if (SUCCEEDED(peer->QueryPinInfo(&peerInfo))) {
                            CLSID classId;
                            if (SUCCEEDED(peerInfo.pFilter->GetClassID(&classId))
                                    && classId == CLSID_Colour) {
                                end->Release();
                                end = peerInfo.pFilter;
                            } else {
                                peerInfo.pFilter->Release();
                            }
                        }
                        peer->Release();
                    }
                }
            }
            pins->Release();
        }
    }
    return start;
}

void DirectShowPlayerService::run()
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
        } else if (!(m_executedTasks & Render)) {
            m_pendingTasks &= ~(SetRate | Stop | Pause | Seek | Play);
        } else if (m_pendingTasks & SetRate) {
            m_pendingTasks ^= SetRate;
            m_executingTask = SetRate;

            doSetRate(&locker);
        } else if (m_pendingTasks & Stop) {
            m_pendingTasks ^= Stop;

            m_loop.wake();
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

    if (m_graph) {
        if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
            locker.unlock();
            m_graph->Abort();
            OAFilterState state;
            control->GetState(INFINITE, &state);
            locker.relock();
        }
        m_graph->Release();
        m_graph = 0;
    }

    if (m_source) {
        m_source->Release();
        m_source = 0;
    }

    m_loop.wake();
}

void DirectShowPlayerService::doLoad(QMutexLocker *locker)
{
    m_executedTasks = 0;

    IBaseFilter *source = 0;
    QUrl url = m_url;
    QIODevice *stream = m_stream;

    HRESULT hr = S_OK;

    if (stream) {
        source = new DirectShowIOSource(stream, &m_loop);

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
    }
}

void DirectShowPlayerService::doRender(QMutexLocker *locker)
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

    bool rendered = false;

    while (!filters.isEmpty()) {
        IEnumPins *pins = 0;
        IBaseFilter *filter = filters[filters.size() - 1];
        filters.removeLast();

        if (m_executingTask == Render && SUCCEEDED(filter->EnumPins(&pins))) {
            for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
                PIN_DIRECTION direction;
                if (pin->QueryDirection(&direction) == S_OK && direction == PINDIR_OUTPUT) {
                    IPin *peer = 0;
                    if (pin->ConnectedTo(&peer) == S_OK) {
                        PIN_INFO peerInfo;
                        if (peer->QueryPinInfo(&peerInfo) == S_OK)
                            filters.append(peerInfo.pFilter);
                        peer->Release();
                    } else {
                        locker->unlock();
                        if (SUCCEEDED(m_graph->RenderEx(
                                pin, AM_RENDEREX_RENDERTOEXISTINGRENDERERS, 0))) {
                            rendered = true;
                        }
                        locker->relock();
                    }
                }
            }
        }
        filter->Release();
    }
    
    if (m_executingTask == Render)
        m_executedTasks |= Render;

    m_loop.wake();
}

void DirectShowPlayerService::doSetRate(QMutexLocker *locker)
{
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph)) {
        locker->unlock();
        seeking->SetRate(m_rate);
        locker->relock();

        seeking->Release();
    }
}

void DirectShowPlayerService::doSeek(QMutexLocker *locker)
{
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph)) {
        LONGLONG pos = LONGLONG(m_position) * 10;

        locker->unlock();
        seeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning);
        locker->relock();

        seeking->Release();
    }
}

void DirectShowPlayerService::doPlay(QMutexLocker *locker)
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

void DirectShowPlayerService::doPause(QMutexLocker *locker)
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
