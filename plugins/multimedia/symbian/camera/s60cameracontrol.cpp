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

#include "s60cameracontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60videocapturesession.h"
#include "s60viewfinderwidget.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>

const TInt defaultCameraDevice = 0;

S60CameraControl::S60CameraControl(QObject *parent)
    : QCameraControl(parent),
      m_cameraEngine(NULL),
      m_videoSession(NULL),
      m_imageSession(NULL),
      m_captureMode(QCamera::CaptureStillImage),
      m_state(QCamera::StoppedState),
      m_deviceIndex(NULL),
      m_VFWidgetSize(QSize()),
      m_error(NoError)
{
}

S60CameraControl::S60CameraControl(QObject *videosession,QObject *imagesession, QObject *parent)
   : QCameraControl(parent),
     m_cameraEngine(NULL),
     m_videoSession(NULL),
     m_imageSession(NULL),
     m_captureMode(QCamera::CaptureStillImage),
     m_state(QCamera::StoppedState),
     m_deviceIndex(NULL),
     m_VFWidgetSize(QSize()),
     m_error(NoError)
{
    // use cast if we want to change session class later on..
    m_videoSession = qobject_cast<S60VideoCaptureSession*>(videosession);
    m_imageSession = qobject_cast<S60ImageCaptureSession*>(imagesession);
    m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this);    
    setCameraHandles();
    // connect signal to session
    connect(this,SIGNAL(cameraReadyChanged(bool)),m_imageSession,SIGNAL(readyForCaptureChanged(bool)));
}

S60CameraControl::~S60CameraControl()
{
    if (m_cameraEngine) {
        m_cameraEngine->ReleaseAndPowerOff();
        delete m_cameraEngine;
        m_cameraEngine = NULL;
    }    
}

void S60CameraControl::start()
{   
    startCamera();    
}

void S60CameraControl::stop()
{    
    stopCamera();
}

QCamera::State S60CameraControl::state() const
{
    return m_state;
}

QCamera::CaptureMode S60CameraControl::captureMode() const
{
    return m_captureMode;
}

void S60CameraControl::setCaptureMode(QCamera::CaptureMode mode)
{    
    if (m_captureMode != mode) {
        m_captureMode = mode;        
        emit captureModeChanged(mode);
    }    
}

QCamera::CaptureModes S60CameraControl::supportedCaptureModes() const
{
    return QCamera::CaptureStillImage | QCamera::CaptureVideo;
}

void S60CameraControl::setVideoOutput(QObject *output)
{
    setVideoRenderer(output);
}

void S60CameraControl::setVideoRenderer(QObject *videoOutput)
{
    //qDebug() << "S60CameraControl::setVideoRenderer, videoOutput="<<videoOutput;
    S60ViewFinderWidgetControl* viewFinderWidgetControl =
            qobject_cast<S60ViewFinderWidgetControl*>(videoOutput);

    if (viewFinderWidgetControl) {
        QLabel *widget = qobject_cast<QLabel *>(viewFinderWidgetControl->videoWidget());
        disconnect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)), widget, SLOT(setPixmap(const QPixmap &)));
        disconnect(viewFinderWidgetControl, SIGNAL(widgetResized(QSize)), this, SLOT(resetVFSize(QSize)));
        connect(this, SIGNAL(viewFinderFrameReady(const QPixmap &)), widget, SLOT(setPixmap(const QPixmap &)));
        connect(viewFinderWidgetControl, SIGNAL(widgetResized(QSize)), this, SLOT(resetVFSize(QSize)));
        m_VFWidgetSize = widget->size();
    }
}

void S60CameraControl::startCamera()
{
    //qDebug() << "S60CameraControl::startCamera START";
    #ifdef Q_CC_NOKIAX86
    MceoCameraReady(); // signal that we are ready
    #endif

    if (!m_error ) {
        //qDebug() << "S60CameraControl::startCamera, ReserveAndPowerOn"<< m_error;
        m_cameraEngine->ReserveAndPowerOn();
    }
    //qDebug() << "S60CameraControl::startCamera END";
}

void S60CameraControl::stopCamera()
{
    //qDebug() <<  "Stopping camera";
    m_state = QCamera::StoppedState;

    if (m_cameraEngine) {
        m_cameraEngine->ReleaseAndPowerOff();
    }
    emit stateChanged(m_state);
}

void S60CameraControl::MceoCameraReady()
{
    //qDebug() << "S60CameraControl::MCeoCameraReady()";
    m_state = QCamera::ActiveState;
    emit stateChanged(m_state);
    if (m_cameraEngine) {
        TSize size = TSize(m_VFWidgetSize.width(), m_VFWidgetSize.height()); 
        TRAPD(err, m_cameraEngine->StartViewFinderL(size));
        //qDebug() << "S60VideoCaptureSession::MCeoCameraReady() error: "<< err;
        if (err == KErrNotReady || err == KErrNoMemory)             
            emit cameraReadyChanged(false);        
        else 
            emit cameraReadyChanged(true);
        setError(err);
    }
}

void S60CameraControl::MceoFocusComplete()
{
}

void S60CameraControl::MceoCapturedDataReady(TDesC8* aData)
{
}

void S60CameraControl::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
}

void S60CameraControl::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
    //qDebug() << "S60CameraControl::MceoHandleError, errorType"<<aErrorType;
    //qDebug() << "S60CameraControl::MceoHandleError, aError"<<aError;
    Q_UNUSED(aErrorType);
    setError(aError);
}

void S60CameraControl::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(&aFrame); 
    emit viewFinderFrameReady(pixmap);
    m_cameraEngine->ReleaseViewFinderBuffer();
}

void S60CameraControl::setError(TInt aError)
{
    if (aError == KErrNone)
        return;
        
    m_error = aError;
    QCamera::Error cameraError = fromSymbianErrorToMultimediaError(m_error);
    
    // TODO: fix to user friendly string at some point
    // These error string are only dev usable
    QString symbianError; 
    symbianError.append("Symbian:");
    symbianError.append(QString::number(m_error));
    emit error(cameraError, symbianError);
}

QCamera::Error S60CameraControl::fromSymbianErrorToMultimediaError(int aError)
{
    switch(aError) {
    case KErrNoMemory:
    case KErrNotFound:
    case KErrBadHandle:
        return QCamera::ServiceMissingError;
    case KErrNotSupported:
    case KErrECamSettingNotSupported:
    case KErrECamParameterNotInRange:
        return QCamera::NotSupportedFeatureError;
    case KErrCorrupt:
    case KErrECamSettingDisabled:
    case KErrECamNotOptimalFocus:
    case KErrAccessDenied:
    case KErrLocked:
    case KErrPermissionDenied:
        return QCamera::CameraError;
    case KErrNotReady:
    case KErrECamCameraDisabled:
    case KErrInUse:
        return QCamera::NotReadyToCaptureError;
    default:
        return QCamera::NoError;
}
}

void S60CameraControl::resetVFSize(QSize size)
{
    if(!m_cameraEngine->IsCameraReady())
        return;
    
    m_VFWidgetSize = size;
    TSize size_symbian = TSize(size.width(), size.height());
    m_cameraEngine->StopViewFinder();
    TRAP_IGNORE(m_cameraEngine->StartViewFinderL(size_symbian));
}

bool S60CameraControl::deviceReady()
{
    #ifdef Q_CC_NOKIAX86
    //qDebug() << "device ready";
    return true;
    #endif
    if ( m_cameraEngine )
        return m_cameraEngine->IsCameraReady();
    else
        return EFalse;
}

// For S60Cameravideodevicecontrol
int S60CameraControl::deviceCount()
{
    //qDebug() << "S60CameraControl::deviceCount(for emulator this is always 1)";
    #ifdef Q_CC_NOKIAX86
    return 1;
    #endif
    return CCameraEngine::CamerasAvailable();
}

int S60CameraControl::defaultDevice() const
{
    //First camera is the default    
    return defaultCameraDevice;
}

int S60CameraControl::selectedDevice() const
{
    //qDebug() << "S60CameraControl::selectedDevice returning="<<m_deviceIndex;
    return m_deviceIndex;
}

void S60CameraControl::setSelectedDevice(int index)
{
    //qDebug() << "S60CameraControl::setSelectedDevice,index="<<index;
    if (m_deviceIndex != index) {
        m_deviceIndex = index;
        resetCamera();        
    }    
}

QString S60CameraControl::name(const int index)
{
    //TODO: change these to use querycurrentcamerainfo
    // From where does the naming index start
    QString cameraName;
    switch (index) {
        case 0:
            cameraName = QLatin1String("Main camera");
        break;
        case 1:
            cameraName = QLatin1String("Secondary camera");
        break;
        case 2:
            cameraName = QLatin1String("Tertiary camera");
        break;
        default:
            cameraName = QLatin1String("Default camera");
        break;
    }
    return cameraName;
}

QString S60CameraControl::description(const int index)
{
    //TODO: change these to use querycurrentcamerainfo
    // what information is wanted throuhg this call?
    QString cameraDesc;
    switch (index) {
        case 0:
            cameraDesc = QLatin1String("Back camera");
        break;
        case 1:
            cameraDesc = QLatin1String("Front camera");
        break;
        case 2:
            cameraDesc = QLatin1String("Tertiary camera description");
        break;
        default:
            cameraDesc = QLatin1String("Default camera description");
        break;
    }
    return cameraDesc;
}

void S60CameraControl::resetCamera()
{
    //qDebug() << "S60CameraControl::resetCamera START";
    stopCamera();
    m_imageSession->deleteAdvancedSettings();
    delete m_cameraEngine;
    m_cameraEngine = NULL;   
    //qDebug() << "S60ImageCaptureSession::resetCamera. Creating new camera with index=" << m_deviceIndex;
    TRAPD(err, m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this));       
    setError(err);
    setCameraHandles();
    //qDebug() << "S60CameraControl::resetCamera END";
}

void S60CameraControl::setCameraHandles()
{   
    //qDebug() << "S60CameraControl::setCameraHandles START";
    m_imageSession->setCurrentDevice(m_deviceIndex);    
    m_imageSession->setCameraHandle(m_cameraEngine);    
    m_cameraEngine->SetImageCaptureObserver(m_imageSession);    
    m_videoSession->setCameraHandle(m_cameraEngine);
    //qDebug() << "S60CameraControl::setCameraHandles START";
}
