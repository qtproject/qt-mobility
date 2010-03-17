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

#include <QtCore/qvariant.h>
#include <QtGui/qwidget.h>

#include "v4lcameraservice.h"
#include "v4lcameracontrol.h"
#include "v4lcamerasession.h"
#include "v4lvideorenderer.h"
#include "v4lvideooutputcontrol.h"
#include "v4lvideodevicecontrol.h"
#include "v4limagecapturecontrol.h"
#include "v4lmediacontainercontrol.h"
#include "v4lvideoencode.h"
#include "v4lrecordercontrol.h"

V4LCameraService::V4LCameraService(QObject *parent):
    QMediaService(parent)
{
    m_session = new V4LCameraSession(this);
    m_control = new V4LCameraControl(m_session);

    m_videoOutput = new V4LVideoOutputControl(this);
    m_videoOutput->setOutput(QVideoOutputControl::RendererOutput);

    m_videoDevice = new V4LVideoDeviceControl(m_session);

    m_videoRenderer = new V4LVideoRendererControl(m_session, this);

    m_imageCapture = new V4LImageCaptureControl(m_session);

    m_formatControl = new V4LMediaContainerControl(m_session);

    m_videoEncode = new V4LVideoEncode(m_session);

    m_recorderControl = new V4LRecorderControl(m_session);

    m_device = QByteArray("/dev/video1");
}

V4LCameraService::~V4LCameraService()
{
    delete m_control;
    delete m_session;
    delete m_videoOutput;
    delete m_videoDevice;
    delete m_videoRenderer;
    delete m_imageCapture;
    delete m_formatControl;
    delete m_videoEncode;
    delete m_recorderControl;
}

QMediaControl *V4LCameraService::control(const char *name) const
{
    if(qstrcmp(name,QCameraControl_iid) == 0)
        return m_control;

    if(qstrcmp(name,QVideoOutputControl_iid) == 0)
        return m_videoOutput;

    if(qstrcmp(name,QVideoRendererControl_iid) == 0)
        return m_videoRenderer;

    if(qstrcmp(name,QVideoDeviceControl_iid) == 0)
        return m_videoDevice;

    if (qstrcmp(name, QImageCaptureControl_iid) == 0)
        return m_imageCapture;

    if (qstrcmp(name, QMediaContainerControl_iid) == 0)
        return m_formatControl;

    if (qstrcmp(name, QVideoEncoderControl_iid) == 0)
        return m_videoEncode;

    if (qstrcmp(name, QMediaRecorderControl_iid) == 0)
        return m_recorderControl;

    return 0;
}
