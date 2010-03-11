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
#include <QtCore/qdebug.h>
#include <QtGui/qwidget.h>
#include <QtCore/qlist.h>
#include "qvideowidget.h"

#include "S60cameraservice.h"
#include "S60cameracontrol.h"
#include "S60cameravideodevicecontrol.h"
#include "S60camerafocuscontrol.h"
#include "S60cameraexposurecontrol.h"
#include "S60cameraimageprocessingcontrol.h"
#include "s60cameraimagecapturecontrol.h"
#include "S60mediacontrol.h"
#include "S60videocapturesession.h"
#include "S60imagecapturesession.h"
#include "S60viewfinderwidget.h"
#include "S60videooutputcontrol.h"
#include "S60mediacontainercontrol.h"
#include "s60videoencoder.h"
#include "s60audioencoder.h"
#include "s60cameraimageencodercontrol.h"

S60CameraService::S60CameraService(QObject *parent)
    : QMediaService(parent)
{   
    // session classes for video and image capturing
    m_imagesession = new S60ImageCaptureSession(this);
    m_videosession = new S60VideoCaptureSession(this);    
    // different control classes which use session class to do the work
    m_control = new S60CameraControl(m_videosession, m_imagesession, this);    
    m_videoDeviceControl = new S60CameraVideoDeviceControl(m_control, this);    
    m_focusControl = new S60CameraFocusControl(m_imagesession, this);    
    m_exposureControl = new S60CameraExposureControl(m_imagesession, this);    
    m_imageProcessingControl = new S60CameraImageProcessingControl(m_imagesession, this);    
    m_imageCaptureControl = new S60CameraImageCaptureControl(m_imagesession, this);     
    m_media = new S60MediaControl(m_videosession, this);    
    m_mediaFormat = new S60MediaContainerControl(m_videosession, this);    
    m_videoEncoder = new S60VideoEncoder(m_videosession, this);
    m_audioEncoder = new S60AudioEncoder(m_videosession, this);
    m_viewFinderWidget = new S60ViewFinderWidgetControl(this);    
    m_videoOutput = new S60VideoOutputControl(this);    
    m_imageEncoderControl = new S60CameraImageEncoderControl(m_imagesession, this);    

    connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));
    
    m_videoOutput->setAvailableOutputs(QList<QVideoOutputControl::Output>() 
            << QVideoOutputControl::WidgetOutput);    
}

S60CameraService::~S60CameraService()
{
}

QMediaControl *S60CameraService::control(const char *name) const
{
    if (qstrcmp(name,QMediaRecorderControl_iid) == 0)
        return m_media;

    if(qstrcmp(name,QCameraControl_iid) == 0)
        return m_control;

    if(qstrcmp(name,QVideoEncoderControl_iid) == 0)
        return m_videoEncoder;
    
    if(qstrcmp(name,QAudioEncoderControl_iid) == 0)
        return m_audioEncoder;

    if(qstrcmp(name,QMediaContainerControl_iid) == 0)
        return m_mediaFormat;

    if(qstrcmp(name,QVideoOutputControl_iid) == 0)
        return m_videoOutput;
    
    if(qstrcmp(name,QCameraExposureControl_iid) == 0)
        return m_exposureControl;
    
    if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
        return m_viewFinderWidget;
    
    if(qstrcmp(name,QCameraFocusControl_iid) == 0)
        return m_focusControl;

    if(qstrcmp(name,QImageProcessingControl_iid) == 0)
        return m_imageProcessingControl;

    if(qstrcmp(name,QImageCaptureControl_iid) == 0)
        return m_imageCaptureControl;

    if(qstrcmp(name,QVideoDeviceControl_iid) == 0)
        return m_videoDeviceControl;

    if(qstrcmp(name,QImageEncoderControl_iid) == 0)
        return m_imageEncoderControl;

    return 0;
}
int S60CameraService::deviceCount()
{
    return S60CameraControl::deviceCount();
}

QString S60CameraService::deviceDescription(const int index)
{
    return S60CameraControl::description(index);
}
QString S60CameraService::deviceName(const int index)
{
    return S60CameraControl::name(index);
}

void S60CameraService::videoOutputChanged(QVideoOutputControl::Output output)
{
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_control->setVideoOutput(0);
        break;
    case QVideoOutputControl::WidgetOutput:
        m_control->setVideoOutput(m_viewFinderWidget);
        break;
    default:
        qWarning("Invalid video output selection");
        break;
    }
}


/*
bool S60CameraService::isEndpointSupported(QMediaService::MediaEndpoint endpointType)
{
    return false;
}

void S60CameraService::setInputStream(QIODevice* stream)
{
}

QIODevice* S60CameraService::inputStream() const
{
    return 0;
}

void S60CameraService::setOutputStream(QIODevice* stream)
{
}

QIODevice* S60CameraService::outputStream() const
{
    return 0;
}

QString S60CameraService::activeEndpoint(QMediaService::MediaEndpoint endpointType)
{
    return QByteArray();
}

bool S60CameraService::setActiveEndpoint(QMediaService::MediaEndpoint endpointType, const QString& endpoint)
{
    return true;
}

QList<QString> S60CameraService::supportedEndpoints(QMediaService::MediaEndpoint endpointType) const
{
    QList<QString> list;
    //TODO
    return list;
}
*/

