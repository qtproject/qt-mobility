/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOCAPTURESESSION_H
#define S60VIDEOCAPTURESESSION_H

#include <QtCore/qurl.h>

#include <qmediaencodersettings.h>
#include <qcamera.h>
#include <qmediarecorder.h>

#include "s60cameraengine.h"

#include <e32base.h>
#include <videorecorder.h> // CVideoRecorderUtility

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60VideoCaptureSettings)
QT_FORWARD_DECLARE_CLASS(QTimer)

/*
 * VideoSession is the main class handling all video recording related
 * operations. It uses mainly CVideoRecorderUtility to do it's tasks, but if
 * DevVideoRecord is available it is used to provide more detailed
 * information of the supported video settings.
 */
class S60VideoCaptureSession : public QObject,
                               public MVideoRecorderUtilityObserver
{
    Q_OBJECT
    Q_ENUMS(Error)
    Q_ENUMS(EcamErrors)
    Q_ENUMS(TVideoCaptureState)

public: // Enums

    enum TVideoCaptureState
    {
        ENotInitialized = 0,    // 0 - VideoRecording is not initialized, instance may or may not be created
        EInitializing,          // 1 - Initialization is ongoing
        EInitialized,           // 2 - VideoRecording is initialized, OpenFile is called with dummy file
        EOpening,               // 3 - OpenFile called with actual output location, waiting completion
        EOpenComplete,          // 4 - OpenFile completed with the actual output location
        EPreparing,             // 5 - Preparing VideoRecording to use set video settings
        EPrepared,              // 6 - VideoRecording is prepared with the set settings, ready to record
        ERecording,             // 7 - Video recording is ongoing
        EPaused                 // 8 - Video recording has been started and paused
    };

public: // Constructor & Destructor

    S60VideoCaptureSession(QObject *parent = 0);
    ~S60VideoCaptureSession();

public: // MVideoRecorderUtilityObserver

    void MvruoOpenComplete(TInt aError);
    void MvruoPrepareComplete(TInt aError);
    void MvruoRecordComplete(TInt aError);
    void MvruoEvent(const TMMFEvent& aEvent);

public: // Methods

    void setError(const TInt error, const QString &description);
    void setCameraHandle(CCameraEngine* cameraHandle);

    qint64 position();
    TVideoCaptureState state() const;

    // Controls
    int initializeVideoRecording();
    void releaseVideoRecording();
    void applyAllSettings();

    void startRecording();
    void pauseRecording();
    void stopRecording(const bool reInitialize = true);

    // Output Location
    bool setOutputLocation(const QUrl &sink);
    QUrl outputLocation() const;

    // Encoder Settings
    S60VideoCaptureSettings *settings();
    bool isReadyToQueryVideoSettings() const;

private: // Internal

    QMediaRecorder::Error fromSymbianErrorToQtMultimediaError(int aError);

    void doInitializeVideoRecorderL();
    void resetSession(bool errorHandling = false);

signals: // Notification Signals

    void stateChanged(S60VideoCaptureSession::TVideoCaptureState);
    void positionChanged(qint64);
    void error(int, const QString&);

private slots: // Internal Slots

    void cameraStatusChanged(QCamera::Status);
    void durationTimerTriggered();

private: // Data

    friend class S60VideoCaptureSettings;

    CCameraEngine               *m_cameraEngine;
    CVideoRecorderUtility       *m_videoRecorder;
    S60VideoCaptureSettings     *m_videoSettings;
    QTimer                      *m_durationTimer;
    qint64                      m_position;
    // Symbian ErrorCode
    mutable int                 m_error;
    // This defines whether Camera is in ActiveStatus or not
    bool                        m_cameraStarted;
    // Internal state of the video recorder
    TVideoCaptureState          m_captureState;
    // Actual output file name/path
    QString                     m_sink;
    // Requested output file name/path, this may be different from m_sink if
    // asynchronous operation was ongoing in the CVideoRecorderUtility when new
    // outputLocation was set.
    QUrl                        m_requestedSink;
    // Set if OpenFileL should be executed when currently ongoing operation
    // is completed.
    bool                        m_openWhenReady;
    // Set if video capture should be prepared after OpenFileL has completed
    bool                        m_prepareAfterOpenComplete;
    // Set if video capture should be started when Prepare has completed
    bool                        m_startAfterPrepareComplete;
    // Tells if settings need to be applied after ongoing operation has finished
    bool                        m_commitSettingsWhenReady;
};

#endif // S60VIDEOCAPTURESESSION_H
