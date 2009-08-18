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

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>

#include "qgstreamerplayerservice.h"
#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"
#include "qgstreamermetadataprovider.h"
#include "qgstreamervideooutputcontrol.h"

#include "qgstreamervideowidget.h"
#include "qgstreamervideooverlay.h"
#include "qgstreamervideorenderer.h"

#include "qmediaplaylistnavigator.h"
#include "qmediaplaylist.h"

QGstreamerPlayerService::QGstreamerPlayerService(QObject *parent)
    : QMediaPlayerService(parent)
{
    m_session = new QGstreamerPlayerSession(this);
    m_control = new QGstreamerPlayerControl(m_session, this);
    m_metadata = new QGstreamerMetadataProvider(m_session, this);
    m_videoOutput = new QGstreamerVideoOutputControl(this);
    connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));
#ifndef QT_NO_VIDEOSURFACE
    m_videoRenderer = new QGstreamerVideoRenderer(this);
    m_videoWindow = new QGstreamerVideoOverlay(this);
    m_videoOutput->setAvailableOutputs(QList<QVideoOutputControl::Output>()
            << QVideoOutputControl::RendererOutput
            << QVideoOutputControl::WindowOutput);
#endif
}

QGstreamerPlayerService::~QGstreamerPlayerService()
{
}

QAbstractMediaControl *QGstreamerPlayerService::control(const char *name) const
{
    if (qstrcmp(name,QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name,QMetadataProviderControl_iid) == 0)
        return m_metadata;

    if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutput;

#ifndef QT_NO_VIDEOSURFACE
    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRenderer;

    if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindow;
#endif

    return 0;
}

void QGstreamerPlayerService::setVideoOutput(QObject *output)
{
    m_control->setVideoOutput(output);
    QAbstractMediaService::setVideoOutput(output);
}

QList<QByteArray> QGstreamerPlayerService::supportedEndpointInterfaces(
            QMediaEndpointInterface::Direction direction) const
{
    QList<QByteArray> res;

    if (direction == QMediaEndpointInterface::Output) {
        res << QMediaWidgetEndpoint_iid;
    }

    return res;
}

QObject *QGstreamerPlayerService::createEndpoint(const char *interface)
{
    //qDebug() << "request for endpoint" << interface;
    if (qstrcmp(interface,QMediaWidgetEndpoint_iid) == 0) {
        return new QGstreamerVideoWidget;
    }

    return 0;
}

void QGstreamerPlayerService::videoOutputChanged(QVideoOutputControl::Output output)
{
#ifdef QT_NO_VIDEOSURFACE
    Q_UNUSED(output);
#else
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_control->setVideoOutput(videoOutput());
        break;
    case QVideoOutputControl::RendererOutput:
        m_control->setVideoOutput(m_videoRenderer);
        break;
    case QVideoOutputControl::WindowOutput:
        m_control->setVideoOutput(m_videoWindow);
        break;
    default:
        qWarning("Invalid video output selection");
        break;
    }
#endif
}

