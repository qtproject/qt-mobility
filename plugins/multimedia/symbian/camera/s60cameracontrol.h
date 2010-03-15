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

#ifndef S60CAMERACONTROL_H
#define S60CAMERACONTROL_H

#include <QtCore/qobject.h>
#include "qcameracontrol.h"
#include <s60cameraengineobserver.h>
#include "s60cameraservice.h"
#include <s60cameraengine.h>
#include <e32base.h>
#include <fbs.h>

QTM_USE_NAMESPACE

class S60CameraService;
class S60ImageCaptureSession;
class S60VideoCaptureSession;

class S60CameraControl : public QCameraControl, public MCameraEngineObserver
{
    Q_OBJECT
public:
    
    enum Error {
        NoError = 0,
        OutOfMemoryError,
        InUseError,
        NotReadyError,
        UnknownError = -1
    };
    
    enum EcamErrors {
        KErrECamCameraDisabled = -12100, // The camera has been disabled, hence calls do not succeed 
        KErrECamSettingDisabled = -12101, //  This parameter or operation is supported, but presently is disabled. 
        KErrECamParameterNotInRange = -12102, //  This value is out of range. 
        KErrECamSettingNotSupported = -12103, //  This parameter or operation is not supported. 
        KErrECamNotOptimalFocus = -12104 // The optimum focus is lost  
    };
    
    S60CameraControl(QObject *parent = 0);
    S60CameraControl(QObject *videosession, QObject *imagesession, QObject *parent = 0);
    ~S60CameraControl();
    
    void setError(TInt aError);
    QCamera::Error fromSymbianErrorToMultimediaError(int aError);
    
    bool deviceReady();
    
    //videodevicecontrol
    static int deviceCount();
    static QString name(const int index);
    static QString description(const int index);
    int defaultDevice() const;
    int selectedDevice() const;
    void setSelectedDevice(int index);

    void start();
    void stop();
    QCamera::State state() const;
    QCamera::CaptureMode captureMode() const;
    void setCaptureMode(QCamera::CaptureMode);
    QCamera::CaptureModes supportedCaptureModes() const;
    
    void setVideoOutput(QObject *output);
    
protected: // From MCameraEngineObserver
    void MceoCameraReady();
    void MceoFocusComplete();
    void MceoCapturedDataReady(TDesC8* aData);
    void MceoCapturedBitmapReady(CFbsBitmap* aBitmap);
    void MceoViewFinderFrameReady(CFbsBitmap& aFrame);
    void MceoHandleError(TCameraEngineError aErrorType, TInt aError);
    
private:
    void startCamera();
    void stopCamera();
    void resetCamera();
    void setVideoRenderer(QObject *videoOutput);
    void setCameraHandles();

private slots: 
    void resetVFSize(QSize size);
    
Q_SIGNALS:
    void viewFinderFrameReady(const QPixmap &pixmap);
    void stateChanged(QCamera::State);
    void cameraReadyChanged(bool);

private:    
    CCameraEngine *m_cameraEngine;
    S60ImageCaptureSession *m_imageSession;
    S60VideoCaptureSession *m_videoSession;
    QCamera::CaptureMode m_captureMode;
    QCamera::State m_state;
    int m_deviceIndex;
    QSize m_VFWidgetSize;    
    mutable int m_error;
};

#endif
