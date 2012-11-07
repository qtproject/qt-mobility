/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60CAMERACONTROL_H
#define S60CAMERACONTROL_H

#include <qcameracontrol.h>

#include "s60cameraengineobserver.h"    // MCameraEngineObserver
#include "s60videocapturesession.h"     // TVideoCaptureState
#include "s60cameraviewfinderengine.h"  // ViewfinderOutputType

#include <e32base.h>
#include <fbs.h>

QT_USE_NAMESPACE

using namespace S60CameraEngineError;

QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)
QT_FORWARD_DECLARE_CLASS(S60VideoCaptureSession)
QT_FORWARD_DECLARE_CLASS(CCameraEngine)
QT_FORWARD_DECLARE_CLASS(S60CameraViewfinderEngine)
QT_FORWARD_DECLARE_CLASS(QTimer)

/*
 * Control for controlling camera base operations (e.g. start/stop and capture
 * mode).
 */
class S60CameraControl : public QCameraControl, public MCameraEngineObserver
{
    Q_OBJECT

public: // Constructors & Destructor

    S60CameraControl(QObject *parent = 0);
    S60CameraControl(S60VideoCaptureSession *videosession,
                     S60ImageCaptureSession *imagesession,
                     QObject *parent = 0);
    ~S60CameraControl();

public: // QCameraControl

    // State
    QCamera::State state() const;
    void setState(QCamera::State state);

    // Status
    QCamera::Status status() const;

    // Capture Mode
    QCamera::CaptureMode captureMode() const;
    void setCaptureMode(QCamera::CaptureMode);
    bool isCaptureModeSupported(QCamera::CaptureMode mode) const;

    // Property Setting
    bool canChangeProperty(QCameraControl::PropertyChangeType changeType,
                           QCamera::Status status) const;

/*
Q_SIGNALS:
    void stateChanged(QCamera::State);
    void statusChanged(QCamera::Status);
    void error(int error, const QString &errorString);
    void captureModeChanged(QCamera::CaptureMode);
*/

public: // Internal

    void setError(const TInt error, const QString &description);
    void detectNewUiOrientation();

    // To provide QVideoDeviceControl info
    static int deviceCount();
    static QString name(const int index);
    static QString description(const int index);
    int defaultDevice() const;
    int selectedDevice() const;
    void setSelectedDevice(const int index);

    void setVideoOutput(QObject *output,
                        const S60CameraViewfinderEngine::ViewfinderOutputType type);
    void releaseVideoOutput(const S60CameraViewfinderEngine::ViewfinderOutputType type);

private slots: // Internal Slots

    void videoStateChanged(const S60VideoCaptureSession::TVideoCaptureState state);
    // Needed to detect image capture completion when trying to rotate the camera
    void readyToRotateChanged(bool isReady);
    /*
     * This method moves the camera to the StandBy status:
     *    - If camera access was lost
     *    - If camera has been inactive in LoadedStatus for a long time
     */
    void toStandByStatus();

protected: // MCameraEngineObserver

    void MceoCameraReady();
    void MceoHandleError(TCameraEngineError aErrorType, TInt aError);

private: // Internal

    QCamera::Error fromSymbianErrorToQtMultimediaError(int aError);

    void loadCamera();
    void unloadCamera();
    void startCamera();
    void stopCamera();

    void resetCameraOrientation();
    void resetCamera(bool errorHandling = false);
    void setCameraHandles();

signals: // Internal Signals

    void cameraReadyChanged(bool);
    void devicesChanged();

private: // Enums

    /*
     * Defines the orientation of the camera; Converts easily to boolean
     * (answering question "Is camera on landscape?")
     */
    enum CameraOrientation {
        CameraOrientationPortrait = 0,
        CameraOrientationLandscape = 1,
        CameraOrientationNotSet
    };

private: // Data

    CCameraEngine               *m_cameraEngine;
    S60CameraViewfinderEngine   *m_viewfinderEngine;
    S60ImageCaptureSession      *m_imageSession;
    S60VideoCaptureSession      *m_videoSession;
    QObject                     *m_videoOutput;
    QTimer                      *m_inactivityTimer;
    QCamera::CaptureMode        m_captureMode;
    QCamera::CaptureMode        m_requestedCaptureMode;
    bool                        m_settingCaptureModeInternally;
    QCamera::Status             m_internalState;
    QCamera::State              m_requestedState;
    int                         m_deviceIndex;
    mutable int                 m_error;
    bool                        m_changeCaptureModeWhenReady;
    CameraOrientation           m_cameraOrientation;
    CameraOrientation           m_requestedCameraOrientation;
    S60VideoCaptureSession::TVideoCaptureState m_videoCaptureState;
};

#endif // S60CAMERACONTROL_H
