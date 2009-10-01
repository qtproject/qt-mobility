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
#include <QtCore/qlist.h>
#include "qvideowidget.h"

#include "S60cameraservice.h"
#include "S60cameracontrol.h"
#include "S60camerafocuscontrol.h"
#include "S60cameraexposurecontrol.h"
#include "S60mediacontrol.h"
#include "S60camerasession.h"
//#include "S60videowidget.h"
#include "S60videooutputcontrol.h"
#include "S60mediaformatcontrol.h"
#include "s60videoencoder.h"

S60CameraService::S60CameraService(QObject *parent)
    : QMediaService(parent)
{
    m_session = new S60CameraSession(this);
    m_control = new S60CameraControl(this,m_session);
    m_focusControl = new S60CameraFocusControl(this, m_session);
    m_exposureControl = new S60CameraExposureControl(this, m_session);
    m_media = new S60MediaControl(m_session);
    m_mediaFormat = new S60MediaFormatControl(m_session);
    m_videoEncoder = new S60VideoEncoder(m_session);
    m_videoOutput = new S60VideoOutputControl(m_session);

}

S60CameraService::~S60CameraService()
{
    delete m_media;
    delete m_control;
    delete m_focusControl;
    delete m_exposureControl;
    delete m_session;
    delete m_videoEncoder;
    delete m_mediaFormat;
    delete m_videoOutput;
}


/*
void S60CameraService::setVideoOutput(QObject *output)
{
    S60VideoWidget *videoWidget = qobject_cast<S60VideoWidget*>(output);
    if (videoWidget) {
        m_session->setVideoOutput(videoWidget);
    }

    QAbstractMediaService::setVideoOutput(output);
}
*/

QMediaControl *S60CameraService::control(const char *name) const
{
    if (qstrcmp(name,QMediaRecorderControl_iid) == 0)
        return m_media;

    if(qstrcmp(name,QCameraControl_iid) == 0)
        return m_control;

    if(qstrcmp(name,QVideoEncoderControl_iid) == 0)
        return m_videoEncoder;

    if(qstrcmp(name,QMediaFormatControl_iid) == 0)
        return m_mediaFormat;

    if(qstrcmp(name,QVideoOutputControl_iid) == 0)
        return m_videoOutput;
    
    if(qstrcmp(name,QCameraExposureControl_iid) == 0)
        return m_exposureControl;
    
    if(qstrcmp(name,QCameraFocusControl_iid) == 0)
        return m_focusControl;

    return 0;
}
/*
QObject *S60CameraService::createEndpoint(const char *interface)
{
    if (qstrcmp(interface, QMediaWidgetEndpoint_iid) == 0) {
        return new VideoWidget;
    }

    return 0;
}
*/
