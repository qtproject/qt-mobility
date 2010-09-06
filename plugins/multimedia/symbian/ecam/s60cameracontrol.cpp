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

#include <QtCore/qstring.h>

#include "s60cameraservice.h"
#include "s60cameraengine.h"
#include "s60cameracontrol.h"
#include "s60imagecapturesession.h"
#include "s60videowidgetcontrol.h"
#include "s60cameraviewfinderengine.h"
#include "s60cameraconstants.h"

S60CameraControl::S60CameraControl(QObject *parent) :
    QCameraControl(parent),
    m_cameraEngine(NULL),
    m_imageSession(NULL),
    m_videoSession(NULL),
    m_videoOutput(NULL),
    m_captureMode(QCamera::CaptureStillImage),  // Default CaptureMode
    m_internalState(QCamera::UnloadedStatus),   // Default Status
    m_requestedState(QCamera::UnloadedState),   // Default State
    m_deviceIndex(NULL),
    m_error(NoError),
    m_startWhenLoaded(false)
{
}

S60CameraControl::S60CameraControl(S60VideoCaptureSession *videosession, 
                                   S60ImageCaptureSession *imagesession, 
                                   QObject *parent):
    QCameraControl(parent),
    m_cameraEngine(NULL),
    m_imageSession(NULL),
    m_videoSession(NULL),
    m_videoOutput(NULL),
    m_captureMode(QCamera::CaptureStillImage),  // Default CaptureMode
    m_internalState(QCamera::UnloadedStatus),   // Default Status
    m_requestedState(QCamera::UnloadedState),   // Default State
    m_deviceIndex(NULL),
    m_error(NoError),
    m_startWhenLoaded(false)
{
    m_videoSession = videosession;
    m_imageSession = imagesession;
        
    if (!videosession && !imagesession) {
        m_error = QCamera::CameraError;
    }
    // From now on it's safe to assume ImageSession and VideoSession exist
    
    connect(m_videoSession, SIGNAL(stateChanged(S60VideoCaptureSession::TVideoCaptureState)), 
        this, SLOT(videoStateChanged(S60VideoCaptureSession::TVideoCaptureState)));
    
    TRAPD(err, m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this));
    if (err) {
        m_error = fromSymbianErrorToQtMultimediaError(err);
    }
    m_viewfinderEngine = new CCameraViewfinderEngine(this, m_cameraEngine);
    if (m_viewfinderEngine == NULL) {
        m_error = QCamera::CameraError;
    }
    
    setCameraHandles();
    
    // Connect signal to session
    connect(this, SIGNAL(cameraReadyChanged(bool)), m_imageSession, SIGNAL(readyForCaptureChanged(bool)));
}

S60CameraControl::~S60CameraControl()
{
    if (m_cameraEngine) {
        stopCamera();
        unloadCamera();
        
        delete m_cameraEngine;
        m_cameraEngine = NULL;
    }
    
    if (m_viewfinderEngine) {
        delete m_viewfinderEngine;
        m_viewfinderEngine = NULL;
    }
}

void S60CameraControl::setState(QCamera::State state)
{
    if (m_error) { // Most probably failure in contructor
        emit error(QCamera::CameraError, tr("Unexpected error."));
        return;
    }
    
    if (m_requestedState == state) {
        return;
    }
    
    switch (state) {
        case QCamera::UnloadedState: // Release resources
            switch (m_internalState) {
                case QCamera::UnloadedStatus:
                    // Do nothing
                    break;
                case QCamera::LoadingStatus:
                case QCamera::StartingStatus:
                    // Not ready
                    return;
                case QCamera::LoadedStatus:
                case QCamera::StandbyStatus:
                    // Unload
                    unloadCamera();
                    break;
                case QCamera::ActiveStatus:
                    // Stop and Unload
                    stopCamera();
                    unloadCamera();
                    break;
                    
                default:
                    // Unregocnized internal state (Status)
                    break;
            }
            m_requestedState = QCamera::UnloadedState;
            break;
            
        case QCamera::LoadedState: // Reserve resources OR Stop ViewFinder and Cancel Capture
            switch (m_internalState) {
                case QCamera::UnloadedStatus:
                    // Load
                    loadCamera();
                    break;
                case QCamera::LoadingStatus:
                case QCamera::StartingStatus:
                    // Not ready
                    return;
                case QCamera::LoadedStatus:
                case QCamera::StandbyStatus:
                    // Do nothing
                    break;
                case QCamera::ActiveStatus:
                    // Stop
                    stopCamera();
                    break;
                    
                default:
                    // Unregocnized internal state (Status)
                    break;
            }
            m_requestedState = QCamera::LoadedState;
            break;
            
        case QCamera::ActiveState: // (Reserve Resources,) Prepare and Start ViewFinder
            switch (m_internalState) {
                case QCamera::UnloadedStatus:
                    // Load and Start
                    m_startWhenLoaded = true;
                    loadCamera();
                    break;
                case QCamera::LoadingStatus:
                case QCamera::StartingStatus:
                    // Not ready
                    return;
                case QCamera::LoadedStatus:
                case QCamera::StandbyStatus:
                    // Start
                    startCamera();
                    break;
                case QCamera::ActiveStatus:
                    // Do nothing
                    break;
                    
                default:
                    // Unregocnized internal state (Status)
                    break;
            }
            m_requestedState = QCamera::ActiveState;
            break;
            
        default:
            emit error(QCamera::NotSupportedFeatureError, tr("State not supported."));
            return;
    }
    
    emit stateChanged(m_requestedState);    
}

QCamera::State S60CameraControl::state() const
{
    return m_requestedState;
}

QCamera::Status S60CameraControl::status() const
{
    return m_internalState;
}

QCamera::CaptureMode S60CameraControl::captureMode() const
{
    return m_captureMode;
}

void S60CameraControl::setCaptureMode(QCamera::CaptureMode mode)
{    
    if (m_error) { // Most probably failure in contructor
        emit error(QCamera::CameraError, tr("Unexpected error."));
        return;
    }

    if (m_captureMode == mode) {
        return;
    }
    
    if (!isCaptureModeSupported(mode)) {
        emit error(QCamera::NotSupportedFeatureError, tr("CaptureMode not supported."));
        return;
    }
    
    switch (m_internalState) {
        case QCamera::UnloadedStatus:
        case QCamera::LoadedStatus:
        case QCamera::StandbyStatus:
            switch (mode) {
                case QCamera::CaptureStillImage:
                    m_captureMode = QCamera::CaptureStillImage;
                    break;
                case QCamera::CaptureVideo:
                    m_captureMode = QCamera::CaptureVideo;
                    break;
            }
            break;
        case QCamera::LoadingStatus:
        case QCamera::StartingStatus:
            // Not ready
            return;
        case QCamera::ActiveStatus:
            // Stop, Change Mode and Start again
            stopCamera();
            switch (mode) {
                case QCamera::CaptureStillImage:
                    m_captureMode = QCamera::CaptureStillImage;
                    break;
                case QCamera::CaptureVideo:
                    m_captureMode = QCamera::CaptureVideo;
                    break;
            }
            startCamera();
            break;

        default:
            // Unregocnized internal state (Status)
            emit error(QCamera::NotSupportedFeatureError, tr("CaptureMode not supported."));
            break;
    }

    emit captureModeChanged(mode);
}

bool S60CameraControl::isCaptureModeSupported(QCamera::CaptureMode mode) const
{
    switch (mode) {
        case QCamera::CaptureStillImage:
            return true;
        case QCamera::CaptureVideo:
            return true;

        default:
            return false;
    }
}

bool S60CameraControl::canChangeProperty(QCameraControl::PropertyChangeType changeType, QCamera::Status status) const
{
    return false;
}

void S60CameraControl::setVideoOutput(QObject *output, ViewfinderOutputType type)
{
    switch (type) {
        case VideoWidgetOutput:
            m_viewfinderEngine->setVideoWidgetControl(output);
            break;
        case VideoRendererOutput:
            m_viewfinderEngine->setVideoRendererControl(output);
            break;
        case VideoWindowOutput:
            m_viewfinderEngine->setVideoWindowControl(output);
            break;

        default:
            break;
    }
}

void S60CameraControl::loadCamera()
{
    m_internalState = QCamera::LoadingStatus;
    emit statusChanged(m_internalState);

    m_cameraEngine->ReserveAndPowerOn();

    // Completion notified in MceoCameraReady()
}

void S60CameraControl::unloadCamera()
{
    m_internalState = QCamera::LoadingStatus;
    emit statusChanged(m_internalState);

    m_cameraEngine->ReleaseAndPowerOff();

    m_internalState = QCamera::UnloadedStatus;
    emit statusChanged(m_internalState);
}

void S60CameraControl::startCamera()
{
    m_internalState = QCamera::StartingStatus;
    emit statusChanged(m_internalState);

    TRAPD(vfErr, m_viewfinderEngine->startViewfinderL());
    if (vfErr) {
        setError(vfErr);
        return;
    }

    if (m_captureMode == QCamera::CaptureStillImage) {
        int prepareSuccess = m_imageSession->prepareImageCapture();
        if (prepareSuccess) {
            setError(prepareSuccess);
            return;
        }
        emit cameraReadyChanged(true);
        m_internalState = QCamera::ActiveStatus;
        emit statusChanged(m_internalState);

    } else { // QCamera::CaptureVideo
        int prepareSuccess = m_videoSession->initializeVideoRecording();
        if (prepareSuccess)
            setError(prepareSuccess);
        // State change signalled when reservation is complete (in videoStateChanged())
        return;
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    MceoCameraReady(); // Signal that we are ready
#endif
}

void S60CameraControl::stopCamera()
{
    m_internalState = QCamera::StartingStatus;
    emit statusChanged(m_internalState);

    // Cancel ongoing operations if any
    m_imageSession->cancelCapture();
    m_videoSession->stopRecording();

    emit cameraReadyChanged(false);
    if (m_viewfinderEngine)
        m_viewfinderEngine->stopViewfinder();

    m_internalState = QCamera::LoadedStatus;
    emit statusChanged(m_internalState);
}

void S60CameraControl::videoStateChanged(const S60VideoCaptureSession::TVideoCaptureState state)
{
    if (state == S60VideoCaptureSession::EInitialized) {
        emit cameraReadyChanged(true);
        m_internalState = QCamera::ActiveStatus;
        emit statusChanged(m_internalState);
    }
}

void S60CameraControl::MceoCameraReady()
{
    if (m_internalState != QCamera::LoadedStatus) {
        m_internalState = QCamera::LoadedStatus;
        emit statusChanged(QCamera::LoadedStatus);

        if (m_startWhenLoaded) {
            startCamera();
            m_startWhenLoaded = false; // Reset
        }
    }
}

void S60CameraControl::MceoFocusComplete()
{
}

void S60CameraControl::MceoCapturedDataReady(TDesC8* aData)
{
    Q_UNUSED(aData);
}

void S60CameraControl::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    Q_UNUSED(aBitmap);
}

void S60CameraControl::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
    Q_UNUSED(aErrorType);
    setError(aError);
}

void S60CameraControl::setError(TInt aError)
{
    if (aError == KErrNone)
        return;

    m_error = aError;
    QCamera::Error cameraError = fromSymbianErrorToQtMultimediaError(m_error);

    QString symbianError = "Symbian ErrorCode:";
    symbianError.append(QString::number(m_error));
    emit error(cameraError, symbianError);
}

QCamera::Error S60CameraControl::fromSymbianErrorToQtMultimediaError(int aError)
{
    switch(aError) {
        case KErrNone:
            return QCamera::NoError; // No errors have occurred
        case KErrNotSupported:
        case KErrECamSettingNotSupported:
        case KErrECamParameterNotInRange:
            return QCamera::NotSupportedFeatureError; // The feature is not supported
        case KErrNoMemory:
        case KErrNotFound:
        case KErrBadHandle:
            return QCamera::ServiceMissingError; // No camera service available

        default:
            return QCamera::CameraError; // An error has occurred (i.e. General Error)
    }
}

// For S60CameraVideoDeviceControl
int S60CameraControl::deviceCount()
{
#ifdef Q_CC_NOKIAX86 // Emulator
    return 1;
#endif

    return CCameraEngine::CamerasAvailable();
}

int S60CameraControl::defaultDevice() const
{
    return KDefaultCameraDevice;
}

int S60CameraControl::selectedDevice() const
{
    return m_deviceIndex;
}

void S60CameraControl::setSelectedDevice(int index)
{
    if (m_deviceIndex != index) {
        m_deviceIndex = index;
        resetCamera();
    }
}

QString S60CameraControl::name(const int index)
{
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
    // Cancel ongoing activity
    m_imageSession->cancelCapture();
    m_videoSession->stopRecording();

    // Advanced settings must be destructed before the camera
    m_imageSession->deleteAdvancedSettings();

    // Release resources
    stopCamera();
    unloadCamera();

    if (m_viewfinderEngine) {
        delete m_viewfinderEngine;
        m_viewfinderEngine = NULL;
    }

    if (m_cameraEngine) {
        delete m_cameraEngine;
        m_cameraEngine = NULL;
    }

    TRAPD(err, m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this));
    setError(err);

    m_viewfinderEngine = new CCameraViewfinderEngine(this, m_cameraEngine);
    if (m_viewfinderEngine == NULL) {
        setError(KErrNoMemory);
    }

    setCameraHandles();

    // Reset state
    setState(QCamera::UnloadedState);
}

void S60CameraControl::setCameraHandles()
{
    m_imageSession->setCurrentDevice(m_deviceIndex);
    m_imageSession->setCameraHandle(m_cameraEngine);
    m_cameraEngine->SetImageCaptureObserver(m_imageSession);
    m_videoSession->setCameraHandle(m_cameraEngine);
}

// End of file
