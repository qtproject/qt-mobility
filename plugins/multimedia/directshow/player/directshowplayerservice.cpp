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

#include <directshowmetadatacontrol.h>
#include <directshowplayercontrol.h>
#include <directshowvideooutputcontrol.h>
#include <directshowvideorenderercontrol.h>

#include <qmediacontent.h>

#include <QtCore/qurl.h>

DirectShowPlayerService::DirectShowPlayerService(QObject *parent)
    : QMediaService(parent)
    , m_playerControl(new DirectShowPlayerControl(this))
    , m_metaDataControl(new DirectShowMetaDataControl(this))
    , m_videoOutputControl(new DirectShowVideoOutputControl)
    , m_videoRendererControl(new DirectShowVideoRendererControl)
    , m_graph(0)
    , m_builder(0)
    , m_source(0)
    , m_videoOutput(0)
{
    connect(m_videoOutputControl, SIGNAL(outputChanged()), this, SLOT(videoOutputChanged()));
    connect(m_videoRendererControl, SIGNAL(filterChanged()), this, SLOT(videoOutputChanged()));
}

DirectShowPlayerService::~DirectShowPlayerService()
{
    if (m_graph) {
        if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
            control->Stop();
            control->Release();
        }
        
        m_builder->Release();
        m_graph->Release();
        m_source->Release();

        m_graph = 0;
    }

    delete m_playerControl;
    delete m_metaDataControl;
    delete m_videoOutputControl;
    delete m_videoRendererControl;
}

QMediaControl *DirectShowPlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_playerControl;
    else if (qstrcmp(name, QMetaDataControl_iid) == 0)
        return m_metaDataControl;
    else if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutputControl;
    else if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRendererControl;
    else
        return 0;
}

void DirectShowPlayerService::load(const QMediaContent &media)
{
    if (m_graph) {
        if (IMediaControl *control = com_cast<IMediaControl>(m_graph)) {
            control->Stop();
            control->Release();
        }

        m_builder->Release();
        m_source->Release();
        m_graph->Release();

        m_builder = 0;
        m_source = 0;
        m_graph = 0;
    }

    if (!media.isNull()) {
        m_builder = com_new<ICaptureGraphBuilder2>(CLSID_CaptureGraphBuilder2);
        m_graph = com_new<IGraphBuilder>(CLSID_FilterGraph);

        QUrl uri = media.canonicalUri();

        if (!m_graph || !m_builder) {
            qWarning("failed to create com object");
        } else if (!SUCCEEDED(m_builder->SetFiltergraph(m_graph))) {
            qWarning("failed to set filter graph on capture builder");
        } else if (!SUCCEEDED(m_graph->AddSourceFilter(
                uri.toString().utf16(), L"Source", &m_source))) {
            qWarning("Failed to add source filter");
        } else {
            m_builder->RenderStream(0, &MEDIATYPE_Audio, m_source, 0, 0);

            if (m_videoOutput) {
                if (!SUCCEEDED(m_graph->AddFilter(m_videoOutput, L"VideoOutput"))) {
                    qWarning("Failed to add video output to filter graph.");
                } else {
                    m_builder->RenderStream(0, &MEDIATYPE_Video, m_source, 0, m_videoOutput);
                }
            }

            m_metaDataControl->metaDataChanged();

            return;
        }

        if (m_builder)
            m_builder->Release();

        if (m_source)
            m_source->Release();

        if (m_graph)
            m_graph->Release();

        m_builder = 0;
        m_source = 0;
        m_graph = 0;
    }

}

void DirectShowPlayerService::videoOutputChanged()
{
    if (m_videoOutput && m_graph) {
        m_graph->RemoveFilter(m_videoOutput);
    }

    switch (m_videoOutputControl->output()) {
    case QVideoOutputControl::RendererOutput:
        m_videoOutput = m_videoRendererControl->filter();
        break;
    default:
        m_videoOutput = 0;
        break;
    }

    if (m_videoOutput) {
        if (m_graph) {
            if (!SUCCEEDED(m_graph->AddFilter(m_videoOutput, L"VideoOutput"))) {
                qWarning("Failed to add video output to filter graph.");
            } else {
                m_builder->RenderStream(0, &MEDIATYPE_Video, m_source, 0, m_videoOutput);
            }
        }
    }
}
