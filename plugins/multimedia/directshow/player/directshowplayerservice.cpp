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

#include <directshowplayerservice.h>

#include <directshowaudioendpointcontrol.h>
#include <directshowmetadatacontrol.h>
#include <directshowplayercontrol.h>
#include <directshowvideooutputcontrol.h>
#include <directshowvideorenderercontrol.h>

#include <qmediacontent.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qurl.h>

DirectShowPlayerService::DirectShowPlayerService(QObject *parent)
    : QMediaService(parent)
    , m_playerControl(0)
    , m_audioEndpointControl(0)
    , m_metaDataControl(0)
    , m_videoOutputControl(0)
    , m_videoRendererControl(0)
    , m_graph(0)
    , m_renderThread(&m_loop)
{
    m_playerControl = new DirectShowPlayerControl(this);
    m_metaDataControl = new DirectShowMetaDataControl(this);
    m_videoOutputControl = new DirectShowVideoOutputControl; 
    m_audioEndpointControl = new DirectShowAudioEndpointControl(&m_renderThread);
    m_videoRendererControl = new DirectShowVideoRendererControl(&m_loop);


    connect(m_videoOutputControl, SIGNAL(outputChanged()), this, SLOT(videoOutputChanged()));
    connect(m_videoRendererControl, SIGNAL(filterChanged()), this, SLOT(videoOutputChanged()));

    connect(&m_graphEventNotifier, SIGNAL(activated(HANDLE)), this, SLOT(graphEvent(HANDLE)));

    connect(&m_renderThread, SIGNAL(loaded()), this, SLOT(loaded()), Qt::QueuedConnection);
}

DirectShowPlayerService::~DirectShowPlayerService()
{
    if (m_graph) {
        m_graphEventNotifier.setEnabled(false);

        m_graph->Release();
        m_graph = 0;
    }

    m_renderThread.shutdown();

    delete m_playerControl;
    delete m_audioEndpointControl;
    delete m_metaDataControl;
    delete m_videoOutputControl;
    delete m_videoRendererControl;
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
    else
        return 0;
}

void DirectShowPlayerService::load(const QMediaContent &media, QIODevice *stream)
{
    if (m_graph) {
        m_graphEventNotifier.setEnabled(false);

        if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
            control->Stop();
            control->Release();
        }

        m_graph->Release();
        m_graph = 0;
    }

    if (media.isNull() && !stream) {
        m_renderThread.load(QUrl(), 0, 0);
    } else {
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

        m_renderThread.load(media.canonicalUrl(), stream, m_graph);
    }
}

void DirectShowPlayerService::videoOutputChanged()
{
    IBaseFilter *videoOutput = 0;

    switch (m_videoOutputControl->output()) {
    case QVideoOutputControl::RendererOutput:
        videoOutput = m_videoRendererControl->filter();
        break;
    default:
        break;
    }

    m_renderThread.setVideoOutput(videoOutput);
}

void DirectShowPlayerService::graphEvent(HANDLE handle)
{
    qDebug(Q_FUNC_INFO);

    if (IMediaEvent *event = com_cast<IMediaEvent>(m_graph)) {
        long eventCode;
        LONG_PTR param1;
        LONG_PTR param2;

        while (event->GetEvent(&eventCode, &param1, &param2, 0) == S_OK) {
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
