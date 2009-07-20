/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
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


#include "qgstreamervideowidget.h"
#include "qgstreamervideorenderer.h"

#include "qmediaplaylistnavigator.h"
#include "qmediaplaylist.h"

QGstreamerPlayerService::QGstreamerPlayerService(QObject *parent)
    : QMediaPlayerService(parent)
{    
    m_session = new QGstreamerPlayerSession(this);
    m_control = new QGstreamerPlayerControl(m_session, this);
    m_metadata = new QGstreamerMetadataProvider(m_session, this);
}

QGstreamerPlayerService::~QGstreamerPlayerService()
{
}

QAbstractMediaControl *QGstreamerPlayerService::control(const char *name) const
{
    if (qstrcmp(name,"com.nokia.qt.MediaPlayerControl") == 0)
        return m_control;

    if (qstrcmp(name,"com.nokia.qt.MetadataControl") == 0)
        return m_metadata;

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
#ifndef QT_NO_VIDEOSURFACE
        res << QVideoRendererEndpoint_iid;
#endif
    }

    return res;
}

QObject *QGstreamerPlayerService::createEndpoint(const char *interface)
{
    //qDebug() << "request for endpoint" << interface;
    if (qstrcmp(interface,QMediaWidgetEndpoint_iid) == 0) {
        return new QGstreamerVideoWidget;
    }

#ifndef QT_NO_VIDEOSURFACE
    if (qstrcmp(interface,QVideoRendererEndpoint_iid) == 0) {
        return new QGstreamerVideoRenderer;
    }
#endif

    return 0;
}
