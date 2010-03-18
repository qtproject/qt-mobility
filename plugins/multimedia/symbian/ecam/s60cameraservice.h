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

#ifndef S60CAMERASERVICE_H
#define S60CAMERASERVICE_H

#include <QtCore/qobject.h>

#include <QMediaService>

#include "qvideooutputcontrol.h"


QTM_USE_NAMESPACE

class S60MediaContainerControl;
class S60VideoEncoder;
class S60AudioEncoder;
class S60CameraControl;
class S60CameraVideoDeviceControl;
class S60MediaControl;
class S60ImageCaptureSession;
class S60VideoCaptureSession;
class S60VideoOutputControl;
class S60CameraFocusControl;
class S60CameraExposureControl;
class S60CameraImageProcessingControl;
class S60CameraImageCaptureControl;
class S60ViewFinderWidgetControl;
class S60CameraImageEncoderControl;

class S60CameraService : public QMediaService
{
    Q_OBJECT
public:
    S60CameraService(QObject *parent = 0);
    ~S60CameraService();

    QMediaControl *control(const char *name) const;
    
    static int deviceCount();
    static QString deviceName(const int index);
    static QString deviceDescription(const int index);

private Q_SLOTS:
    void videoOutputChanged(QVideoOutputControl::Output output);
/*
    virtual bool isEndpointSupported(QMediaService::MediaEndpoint endpointType);

    virtual void setInputStream(QIODevice* stream);
    virtual QIODevice* inputStream() const;

    virtual void setOutputStream(QIODevice* stream);
    virtual QIODevice* outputStream() const;

    virtual QList<QString> activeEndpoints(QMediaService::MediaEndpoint endpointType);
    virtual bool setActiveEndpoint(QMediaService::MediaEndpoint endpointType, const QString& endpoint);
    virtual QString endpointDescription(QMediaService::MediaEndpoint endpointType, const QString& endpoint);
    virtual QList<QString> supportedEndpoints(QMediaService::MediaEndpoint endpointType) const;
*/
    
private:

    S60ImageCaptureSession *m_imagesession;
    S60VideoCaptureSession *m_videosession;
    S60MediaContainerControl *m_mediaFormat;
    S60VideoEncoder *m_videoEncoder;
    S60AudioEncoder *m_audioEncoder; 
    S60CameraControl *m_control;
    S60CameraVideoDeviceControl *m_videoDeviceControl;
    S60CameraFocusControl *m_focusControl;
    S60CameraExposureControl *m_exposureControl;
    S60CameraImageProcessingControl *m_imageProcessingControl;
    S60CameraImageCaptureControl *m_imageCaptureControl;
    S60VideoOutputControl *m_videoOutput;
    S60MediaControl  *m_media;
    S60ViewFinderWidgetControl *m_viewFinderWidget;
    S60CameraImageEncoderControl *m_imageEncoderControl;
};

#endif
