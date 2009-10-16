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

#include "s60mediaplayerservice.h"
#include "s60videoplayercontrol.h"
#include "s60videoplayersession.h"
#include "s60videometadataprovider.h"
#include "s60videoplayeroutputcontrol.h"
#include "s60videowidget.h"

#ifndef QT_NO_MULTIMEDIA
#include "s60videooverlay.h"
#include "s60videorenderer.h"
#endif

#include <multimedia/qmediaplaylistnavigator.h>
#include <multimedia/qmediaplaylist.h>

S60MediaPlayerService::S60MediaPlayerService(QObject *parent):
     QMediaService(parent)
{

    m_session = new S60VideoPlayerSession(this);
    m_control = new S60VideoPlayerControl(m_session, this);
    m_metaData = new S60VideoMetaDataProvider(m_session, this);
    m_videoOutput = new S60VideoPlayerOutputControl(this);
    connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));
#ifndef QT_NO_MULTIMEDIA
    m_videoRenderer = new S60VideoRenderer(this);
    m_videoWindow = new S60VideoOverlay(this);
#endif
    m_videoWidget = new S60VideoWidgetControl(this);

    m_videoOutput->setAvailableOutputs(QList<QVideoOutputControl::Output>() << QVideoOutputControl::WidgetOutput);
/*#ifndef QT_NO_MULTIMEDIA
            << QVideoOutputControl::RendererOutput
            << QVideoOutputControl::WindowOutput
#endif
*/
           
}

S60MediaPlayerService::~S60MediaPlayerService()
{
}

QMediaControl *S60MediaPlayerService::control(const char *name) const
{
    if (qstrcmp(name,QMediaPlayerControl_iid) == 0)
        return m_control;

    if (qstrcmp(name,QMetaDataProviderControl_iid) == 0)
        return m_metaData;

    if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutput;

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
        return m_videoWidget;

#ifndef QT_NO_MULTIMEDIA
    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRenderer;

    if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindow;
#endif

    return 0;

}

void S60MediaPlayerService::videoOutputChanged(QVideoOutputControl::Output output)
{
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_control->setVideoOutput(0);
        break;

#ifndef QT_NO_MULTIMEDIA
    case QVideoOutputControl::RendererOutput:
        m_control->setVideoOutput(m_videoRenderer);
        break;
    case QVideoOutputControl::WindowOutput:
        m_control->setVideoOutput(m_videoWindow);
        break;
#endif

    case QVideoOutputControl::WidgetOutput:
        m_control->setVideoOutput(m_videoWidget);
        break;
    default:
        qWarning("Invalid video output selection");
        break;
    }
}

