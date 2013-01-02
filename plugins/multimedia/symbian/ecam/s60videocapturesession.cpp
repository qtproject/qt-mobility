/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include <QtCore/qdir.h>
#include <QtCore/qtimer.h>

#include "s60videocapturesession.h"
#include "s60videocapturesettings.h"
#include "s60filenamegenerator.h"
#include "s60cameraconstants.h"

#include <utf.h>
#include <bautils.h>

using namespace S60CameraConstants;

S60VideoCaptureSession::S60VideoCaptureSession(QObject *parent) :
    QObject(parent),
    m_cameraEngine(0),
    m_videoRecorder(0),
    m_videoSettings(new S60VideoCaptureSettings(this, this)),
    m_durationTimer(new QTimer),
    m_position(0),
    m_error(KErrNone),
    m_cameraStarted(false),
    m_captureState(ENotInitialized),    // Default state
    m_sink(QString()),
    m_requestedSink(QUrl()),
    m_openWhenReady(false),
    m_prepareAfterOpenComplete(false),
    m_startAfterPrepareComplete(false),
    m_commitSettingsWhenReady(false)
{
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
    // Populate info of supported codecs, and their resolution, etc.
    TRAPD(err, m_videoSettings->doPopulateVideoCodecsDataL());
    setError(err, tr("Failed to gather video codec information."));
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

    m_videoSettings->initializeVideoCaptureSettings();

    m_durationTimer->setInterval(KDurationChangedInterval);
    connect(m_durationTimer, SIGNAL(timeout()), this, SLOT(durationTimerTriggered()));
}

S60VideoCaptureSession::~S60VideoCaptureSession()
{
    if (m_captureState >= ERecording)
        m_videoRecorder->Stop();

    if (m_captureState >= EInitialized)
        m_videoRecorder->Close();

    if (m_videoRecorder) {
        delete m_videoRecorder;
        m_videoRecorder = 0;
    }

    if (m_durationTimer) {
        delete m_durationTimer;
        m_durationTimer = 0;
    }
}

/*
 * This function can be used both internally and from Control classes using this session.
 * The error notification will go to client application through QMediaRecorder error signal.
 */
void S60VideoCaptureSession::setError(const TInt error, const QString &description)
{
    if (error == KErrNone)
        return;

    m_error = error;
    QMediaRecorder::Error recError = fromSymbianErrorToQtMultimediaError(m_error);

    // Stop/Close/Reset only of other than "not supported" or "not found" error
    if (m_error != KErrNotSupported && m_error != KErrNotFound) {
        if (m_captureState >= ERecording)
            m_videoRecorder->Stop();

        if (m_captureState >= EInitialized)
            m_videoRecorder->Close();

        // Reset state
        if (m_captureState != ENotInitialized) {
            if (m_durationTimer->isActive())
                m_durationTimer->stop();
            m_captureState = ENotInitialized;
            emit stateChanged(m_captureState);
        }
    }

    emit this->error(recError, description);

    // Reset only of other than "not supported" error
    if (m_error != KErrNotSupported && m_error != KErrNotFound) {
        qWarning("Resetting video capture session to recover from error.");
        resetSession(true);
    } else {
        m_error = KErrNone; // Reset error
    }
}

QMediaRecorder::Error S60VideoCaptureSession::fromSymbianErrorToQtMultimediaError(int aError)
{
    switch(aError) {
    case KErrNone:
        return QMediaRecorder::NoError;         // No errors have occurred
    case KErrArgument:
    case KErrNotSupported:
        return QMediaRecorder::FormatError;     // The feature/format is not supported
    case KErrNoMemory:
    case KErrNotFound:
    case KErrBadHandle:
        return QMediaRecorder::ResourceError;   // Not able to use camera/recorder resources
    default:
        return QMediaRecorder::ResourceError;   // Other error has occurred
    }
}

/*
 * This function applies all recording settings to make latency during the
 * start of the recording as short as possible
 */
void S60VideoCaptureSession::applyAllSettings()
{
    switch (m_captureState) {
    case ENotInitialized:
    case EInitializing:
        m_commitSettingsWhenReady = true;
        return;
    case EInitialized:
        setOutputLocation(QUrl());
        m_prepareAfterOpenComplete = true;
        return;
    case EOpening:
        m_prepareAfterOpenComplete = true;
        return;
    case EOpenComplete:
        // Do nothing, ready to commit
        break;
    case EPreparing:
        m_commitSettingsWhenReady = true;
        return;
    case EPrepared:
        // Revert state internally, since logically applying settings means going
        // from OpenComplete ==> Preparing ==> Prepared.
        m_captureState = EOpenComplete;
        break;
    case ERecording:
    case EPaused:
        setError(KErrNotReady, tr("Cannot apply settings while recording."));
        return;
    default:
        setError(KErrGeneral, tr("Unexpected camera error."));
        return;
    }

    // Commit settings - State is now OpenComplete (possibly reverted from Prepared)
    m_videoSettings->commitVideoEncoderSettings();

    // If capture state has been changed to:
    // * Opening: A different media container has been requested
    // * Other: Failure during the setting committing
    // ==> Return
    if (m_captureState != EOpenComplete)
        return;

    // Start preparing
    m_captureState = EPreparing;
    emit stateChanged(m_captureState);

    if (m_cameraEngine->IsCameraReady())
        m_videoRecorder->Prepare();
}

void S60VideoCaptureSession::setCameraHandle(CCameraEngine* cameraHandle)
{
    m_cameraEngine = cameraHandle;
    m_videoSettings->setCameraHandle(m_cameraEngine);

    // Initialize settings for the new camera
    m_videoSettings->initializeVideoCaptureSettings();

    resetSession();
}

void S60VideoCaptureSession::doInitializeVideoRecorderL()
{
    if (m_captureState > ENotInitialized)
        resetSession();

    m_captureState = EInitializing;
    emit stateChanged(m_captureState);

    // OpenFile using generated default file name
    if (m_videoSettings->m_requestedContainer == QLatin1String("video/mp4"))
        m_sink = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::VideoFileName,
                                                       QLatin1String(".mp4"));
    else if (m_videoSettings->m_requestedContainer == QLatin1String("video/3gp"))
        m_sink = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::VideoFileName,
                                                       QLatin1String(".3gp"));
    else
        m_sink = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::VideoFileName,
                                                       QLatin1String(".3gp2"));
    QString symbianFileName = QDir::toNativeSeparators(m_sink);
    TPtrC16 fileSink(reinterpret_cast<const TUint16*>(symbianFileName.utf16()));

    int cameraHandle = m_cameraEngine->Camera() ? m_cameraEngine->Camera()->Handle() : 0;

    TUid controllerUid;
    TUid formatUid;
    m_videoSettings->selectController(controllerUid, formatUid);

    if (m_videoRecorder) {
        // File open completes in MvruoOpenComplete
        TRAPD(err, m_videoRecorder->OpenFileL(fileSink, cameraHandle, controllerUid, formatUid));
        setError(err, tr("Failed to initialize video recorder."));
        m_videoSettings->m_container = m_videoSettings->m_requestedContainer;
    } else {
        setError(KErrNotReady, tr("Unexpected camera error."));
    }
}

void S60VideoCaptureSession::resetSession(bool errorHandling)
{
    if (m_videoRecorder) {
        delete m_videoRecorder;
        m_videoRecorder = 0;
    }

    if (m_captureState != ENotInitialized) {
        if (m_durationTimer->isActive())
            m_durationTimer->stop();
        m_captureState = ENotInitialized;
        emit stateChanged(m_captureState);
    }

    // Reset error to be able to recover
    m_error = KErrNone;

    // Reset flags
    m_openWhenReady = false;
    m_prepareAfterOpenComplete = false;
    m_startAfterPrepareComplete = false;
    m_commitSettingsWhenReady = false;

    TRAPD(err, m_videoRecorder = CVideoRecorderUtility::NewL(*this));
    if (err) {
        qWarning("Failed to create video recorder.");
        if (errorHandling)
            emit error(QMediaRecorder::ResourceError, tr("Failed to recover from video error."));
        else
            setError(err, tr("Failure in creation of video recorder device."));
        return;
    }
    m_videoSettings->setRecorderHandle(m_videoRecorder);
    m_videoSettings->updateVideoCaptureContainers();
}

bool S60VideoCaptureSession::setOutputLocation(const QUrl &sink)
{
    m_requestedSink = sink;

    if (m_error)
        return false;

    switch (m_captureState) {
    case ENotInitialized:
    case EInitializing:
    case EOpening:
    case EPreparing:
        m_openWhenReady = true;
        return true;
    case EInitialized:
    case EOpenComplete:
    case EPrepared:
        // Continue
        break;
    case ERecording:
    case EPaused:
        setError(KErrNotReady, tr("Cannot set file name while recording."));
        return false;
    default:
        setError(KErrGeneral, tr("Unexpected camera error."));
        return false;
    }

    // Empty URL - Use default file name and path
    if (sink.isEmpty()) {
        if (m_captureState == EInitialized) {
            // File is already opened with generated default name

            m_captureState = EOpenComplete;
            emit stateChanged(m_captureState);

            // Prepare right away if needed
            if (m_startAfterPrepareComplete || m_prepareAfterOpenComplete) {
                m_prepareAfterOpenComplete = false; // Reset

                // Commit settings and prepare with them
                applyAllSettings();
            }
            return true;
        } else {
            // Video not yet initialized
            if (m_videoSettings->m_requestedContainer == QLatin1String("video/mp4"))
                m_sink = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::VideoFileName,
                                                               QLatin1String(".mp4"));
            else if (m_videoSettings->m_requestedContainer == QLatin1String("video/3gp"))
                m_sink = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::VideoFileName,
                                                               QLatin1String(".3gp"));
            else
                m_sink = S60FileNameGenerator::defaultFileName(S60FileNameGenerator::VideoFileName,
                                                               QLatin1String(".3gp2"));
        }

    } else { // Non-empty URL

        QString postfix;
        if (m_videoSettings->m_requestedContainer == QLatin1String("video/mp4"))
            postfix = QLatin1String(".mp4");
        else if (m_videoSettings->m_requestedContainer == QLatin1String("video/3gp"))
            postfix = QLatin1String(".3gp");
        else
            postfix = QLatin1String(".3gp2");

        if (sink.scheme().contains(QLatin1String("file"), Qt::CaseInsensitive)) {
            m_sink = S60FileNameGenerator::generateFileNameFromUrl(S60FileNameGenerator::VideoFileName,
                                                                   sink,
                                                                   postfix);
        } else {
            setError(KErrNotSupported, tr("Network URL is not supported as video sink."));
        }
    }

    // State is either Initialized, OpenComplete or Prepared, Close previously opened file
    if (m_videoRecorder)
        m_videoRecorder->Close();
    else
        setError(KErrNotReady, tr("Unexpected camera error."));

    // Open file

    QString fileName(m_sink);
    TPtrC16 fileSink(reinterpret_cast<const TUint16*>(fileName.utf16()));

    int cameraHandle = m_cameraEngine->Camera() ? m_cameraEngine->Camera()->Handle() : 0;

    TUid controllerUid;
    TUid formatUid;
    m_videoSettings->selectController(controllerUid, formatUid);

    if (m_videoRecorder) {
        // File open completes in MvruoOpenComplete
        TRAPD(err, m_videoRecorder->OpenFileL(fileSink, cameraHandle, controllerUid, formatUid));
        setError(err, tr("Failed to initialize video recorder."));
        m_videoSettings->m_container = m_videoSettings->m_requestedContainer;
        m_captureState = EOpening;
        emit stateChanged(m_captureState);
    } else {
        setError(KErrNotReady, tr("Unexpected camera error."));
    }

    m_videoSettings->m_uncommittedSettings = true;
    return true;
}

QUrl S60VideoCaptureSession::outputLocation() const
{
    QString qtLocation(m_sink);
    qtLocation.replace(QLatin1Char('\\'), QLatin1Char('/'));
    return QUrl::fromLocalFile(qtLocation);
}

qint64 S60VideoCaptureSession::position()
{
    // Update position only if recording is ongoing
    if ((m_captureState == ERecording) && m_videoRecorder) {
        // Signal will be automatically emitted of position changes
        TRAPD(err, m_position = m_videoRecorder->DurationL().Int64() / 1000);
        setError(err, tr("Cannot retrieve video position."));
    }

    return m_position;
}

S60VideoCaptureSession::TVideoCaptureState S60VideoCaptureSession::state() const
{
    return m_captureState;
}

S60VideoCaptureSettings *S60VideoCaptureSession::settings()
{
    return m_videoSettings;
}

bool S60VideoCaptureSession::isReadyToQueryVideoSettings() const
{
    if (!m_videoRecorder)
        return false;

    switch (m_captureState) {
    case EInitialized:
    case EOpenComplete:
    case EPrepared:
    case ERecording:
    case EPaused:
        // Possible to query settings from CVideoRecorderUtility
        return true;
    default:
        // All others - use requested settings
        return false;
    }
}

int S60VideoCaptureSession::initializeVideoRecording()
{
    if (m_error)
        return m_error;

    TRAPD(symbianError, doInitializeVideoRecorderL());
    setError(symbianError, tr("Failed to initialize video recorder."));

    return symbianError;
}

void S60VideoCaptureSession::releaseVideoRecording()
{
    if (m_captureState >= ERecording) {
        m_videoRecorder->Stop();
        if (m_durationTimer->isActive())
            m_durationTimer->stop();
    }

    if (m_captureState >= EInitialized)
        m_videoRecorder->Close();

    // Reset state
    m_captureState = ENotInitialized;

    // Reset error to be able to recover from error
    m_error = KErrNone;

    // Reset flags
    m_openWhenReady = false;
    m_prepareAfterOpenComplete = false;
    m_startAfterPrepareComplete = false;
    m_commitSettingsWhenReady = false;
    m_videoSettings->m_uncommittedSettings = false;
}

void S60VideoCaptureSession::startRecording()
{
    if (m_error) {
        setError(m_error, tr("Unexpected recording error."));
        return;
    }

    switch (m_captureState) {
    case ENotInitialized:
    case EInitializing:
    case EInitialized:
        if (m_captureState == EInitialized)
            setOutputLocation(m_requestedSink);
        m_startAfterPrepareComplete = true;
        return;
    case EOpening:
    case EPreparing:
        // Execute FileOpenL() and Prepare() asap and then start recording
        m_startAfterPrepareComplete = true;
        return;
    case EOpenComplete:
    case EPrepared:
        if (m_captureState == EPrepared && !m_videoSettings->m_uncommittedSettings)
            break;
        // Revert state internally, since logically applying settings means going
        // from OpenComplete ==> Preparing ==> Prepared.
        m_captureState = EOpenComplete;
        m_startAfterPrepareComplete = true;

        // Commit settings and prepare with them
        applyAllSettings();
        return;
    case ERecording:
        // Discard
        return;
    case EPaused:
        // Continue
        break;
    default:
        setError(KErrGeneral, tr("Unexpected camera error."));
        return;
    }

    // State should now be either Prepared with no Uncommitted Settings or Paused

    if (!m_cameraStarted) {
        m_startAfterPrepareComplete = true;
        return;
    }

    if (m_cameraEngine && !m_cameraEngine->IsCameraReady()) {
        setError(KErrNotReady, tr("Camera not ready to start video recording."));
        return;
    }

    if (m_videoRecorder) {
        m_videoRecorder->Record();
        m_captureState = ERecording;
        emit stateChanged(m_captureState);
        m_durationTimer->start();

        // Reset all flags
        m_openWhenReady = false;
        m_prepareAfterOpenComplete = false;
        m_startAfterPrepareComplete = false;
    } else {
        setError(KErrNotReady, tr("Unexpected camera error."));
    }
}

void S60VideoCaptureSession::pauseRecording()
{
    if (m_captureState == ERecording) {
        if (m_videoRecorder) {
            TRAPD(err, m_videoRecorder->PauseL());
            setError(err, tr("Pausing video recording failed."));
            m_captureState = EPaused;
            emit stateChanged(m_captureState);
            if (m_durationTimer->isActive())
                m_durationTimer->stop();

            // Notify last duration
            TRAP(err, m_position = m_videoRecorder->DurationL().Int64() / 1000);
            setError(err, tr("Cannot retrieve video position."));
            emit positionChanged(m_position);
        } else {
            setError(KErrNotReady, tr("Unexpected camera error."));
        }
    }
}

void S60VideoCaptureSession::stopRecording(const bool reInitialize)
{
    if (m_captureState != ERecording && m_captureState != EPaused)
        return; // Ignore

    if (m_videoRecorder) {
        m_videoRecorder->Stop();
        m_videoRecorder->Close();

        m_captureState = ENotInitialized;
        emit stateChanged(m_captureState);

        if (m_durationTimer->isActive())
            m_durationTimer->stop();

        // VideoRecording will be re-initialized unless explicitly requested not to do so
        if (reInitialize) {
            if (m_cameraEngine->IsCameraReady())
                initializeVideoRecording();
        }
    } else {
        setError(KErrNotReady, tr("Unexpected camera error."));
    }
}

void S60VideoCaptureSession::MvruoOpenComplete(TInt aError)
{
    if (m_error)
        return;

    if (aError == KErrNone && m_videoRecorder) {
        if (m_captureState == EInitializing) {
            // Dummy file open completed, initialize settings
            TRAPD(err, m_videoSettings->doPopulateAudioCodecsL());
            setError(err, tr("Failed to gather information of supported audio codecs."));

            // For DevVideoRecord codecs are populated during
            // doPopulateVideoCodecsDataL()
            TRAP(err, m_videoSettings->doPopulateVideoCodecsL());
            setError(err, tr("Failed to gather information of supported video codecs."));
#ifndef S60_DEVVIDEO_RECORDING_SUPPORTED
            // Max parameters needed to be populated, if not using DevVideoRecord
            // Otherwise done already in constructor
            m_videoSettings->doPopulateMaxVideoParameters();
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

            m_captureState = EInitialized;
            emit stateChanged(m_captureState);

            // Initialize settings if not already done
            m_videoSettings->initializeVideoCaptureSettings();

            // Validate codecs to be used
            m_videoSettings->validateRequestedCodecs();

            if (m_openWhenReady || m_prepareAfterOpenComplete || m_startAfterPrepareComplete) {
                setOutputLocation(m_requestedSink);
                m_openWhenReady = false; // Reset
            }
            if (m_commitSettingsWhenReady) {
                applyAllSettings();
                m_commitSettingsWhenReady = false; // Reset
            }
            return;

        } else if (m_captureState == EOpening) {
            // Actual file open completed
            m_captureState = EOpenComplete;
            emit stateChanged(m_captureState);

            // Prepare right away
            if (m_startAfterPrepareComplete || m_prepareAfterOpenComplete) {
                m_prepareAfterOpenComplete = false; // Reset

                // Commit settings and prepare with them
                applyAllSettings();
            }
            return;

        } else if (m_captureState == ENotInitialized) {
            // Resources released while waiting OpenFileL to complete
            m_videoRecorder->Close();
            return;

        } else {
            setError(KErrGeneral, tr("Unexpected camera error."));
            return;
        }
    }

    if (m_videoRecorder)
        m_videoRecorder->Close();
    if (aError == KErrNotFound || aError == KErrNotSupported || aError == KErrArgument)
        setError(KErrGeneral, tr("Requested video container or controller is not supported."));
    else
        setError(KErrGeneral, tr("Failure during video recorder initialization."));
}

void S60VideoCaptureSession::MvruoPrepareComplete(TInt aError)
{
    if (m_error)
        return;

    if(aError == KErrNone) {
        if (m_captureState == ENotInitialized) {
            // Resources released while waiting for Prepare to complete
            m_videoRecorder->Close();
            return;
        }

        // The changed resolution will not be signalled outside the backend.
        // It is needed for the video output to adjust for the changed video
        // aspect ratio.
        m_videoSettings->notifyAppliedResolution();

        m_captureState = EPrepared;
        emit stateChanged(EPrepared);

        // Check the actual active settings, but only if new settings have not
        // been set (otherwise the requested settings would be overwritten)
        if (!m_videoSettings->m_uncommittedSettings) {
            m_videoSettings->queryAudioEncoderSettings();
            m_videoSettings->queryVideoEncoderSettings();
        }

        if (m_openWhenReady == true) {
            setOutputLocation(m_requestedSink);
            m_openWhenReady = false; // Reset
        }

        if (m_commitSettingsWhenReady) {
            applyAllSettings();
            m_commitSettingsWhenReady = false; // Reset
        }

        if (m_startAfterPrepareComplete) {
            m_startAfterPrepareComplete = false; // Reset
            startRecording();
        }
    } else {
        m_videoRecorder->Close();
        if (aError == KErrNotSupported)
            setError(aError, tr("Camera preparation for video recording failed because of unsupported setting."));
        else
            setError(aError, tr("Failed to prepare camera for video recording."));
    }
}

void S60VideoCaptureSession::MvruoRecordComplete(TInt aError)
{
    if (!m_videoRecorder) {
        setError(KErrNotReady, tr("Unexpected camera error."));
        return;
    }

    if((aError == KErrNone || aError == KErrCompletion)) {
        m_videoRecorder->Stop();

        // Reset state
        if (m_captureState != ENotInitialized) {
            m_captureState = ENotInitialized;
            emit stateChanged(m_captureState);
            if (m_durationTimer->isActive())
                m_durationTimer->stop();
        }

        if (m_cameraEngine->IsCameraReady())
            initializeVideoRecording();
    }
    m_videoRecorder->Close();

    if (aError == KErrDiskFull)
        setError(aError, tr("Not enough space for video, recording stopped."));
    else
        setError(aError, tr("Recording stopped due to unexpected error."));
}

void S60VideoCaptureSession::MvruoEvent(const TMMFEvent& aEvent)
{
    Q_UNUSED(aEvent);
}

void S60VideoCaptureSession::cameraStatusChanged(QCamera::Status status)
{
    if (status == QCamera::ActiveStatus) {
        m_cameraStarted = true;

        // Continue preparation or start recording if previously requested
        if (m_captureState == EInitialized
            && (m_openWhenReady || m_prepareAfterOpenComplete || m_startAfterPrepareComplete)) {
            setOutputLocation(m_requestedSink);
            m_openWhenReady = false; // Reset
        } else if (m_captureState == EOpenComplete && m_prepareAfterOpenComplete) {
            applyAllSettings();
            m_prepareAfterOpenComplete = false; // Reset
        } else if (m_captureState == EPrepared && m_startAfterPrepareComplete) {
            startRecording();
            m_startAfterPrepareComplete = false; // Reset
        }

    } else if (status == QCamera::UnloadedStatus) {
        m_cameraStarted = false;
        releaseVideoRecording();
    } else {
        m_cameraStarted = false;
    }
}

void S60VideoCaptureSession::durationTimerTriggered()
{
    // Update position only if recording is ongoing
    if (m_captureState == ERecording && m_videoRecorder) {
        // Signal will be automatically emitted of position changes
        TRAPD(err, m_position = m_videoRecorder->DurationL().Int64() / 1000);
        setError(err, tr("Cannot retrieve video position."));

        emit positionChanged(m_position);
    }
}

// End of file
