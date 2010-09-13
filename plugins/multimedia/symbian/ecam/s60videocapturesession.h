/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOCAPTURESESSION_H
#define S60VIDEOCAPTURESESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qhash.h>

#include <qmediaencodersettings.h>
#include <qcamera.h>

#include "s60cameraengine.h"

#include <e32base.h>
#include <VideoRecorder.h> // CVideoRecorderUtility(Observer)
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
#include <mmf/devvideo/devvideobase.h>
#include <mmf/devvideo/devvideorecord.h>
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

QT_USE_NAMESPACE

class S60VideoCaptureSession : public QObject,
                               public MVideoRecorderUtilityObserver
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
                               ,public MMMFDevVideoRecordObserver
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
{
    Q_OBJECT
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_ENUMS(Error)
    Q_ENUMS(EcamErrors)
    Q_ENUMS(TVideoCaptureState)

public: // Enums

    enum Error {
        NoError = 0,
        OutOfMemoryError,
        InUseError,
        NotReadyError,
        UnknownError = -1
    };

    enum EcamErrors {
        KErrECamCameraDisabled =        -12100, // The camera has been disabled, hence calls do not succeed
        KErrECamSettingDisabled =       -12101, // This parameter or operation is supported, but presently is disabled.
        KErrECamParameterNotInRange =   -12102, // This value is out of range.
        KErrECamSettingNotSupported =   -12103, // This parameter or operation is not supported.
        KErrECamNotOptimalFocus =       -12104  // The optimum focus is lost
    };

    enum TVideoCaptureState
    {
        ENotInitialized = 0,
        EInitializing,
        EInitialized,
        EOpening,
        EOpenComplete,
        EPreparing,
        EPrepared,
        ERecording,
        EPaused
    };

public: // Constructor & Destructor

    S60VideoCaptureSession(QObject *parent = 0);
    ~S60VideoCaptureSession();

public: // MVideoRecorderUtilityObserver

    void MvruoOpenComplete(TInt aError);
    void MvruoPrepareComplete(TInt aError);
    void MvruoRecordComplete(TInt aError);
    void MvruoEvent(const TMMFEvent& aEvent);

#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
public: // MMMFDevVideoRecordObserver
    void MdvroReturnPicture(TVideoPicture *aPicture);
    void MdvroSupplementalInfoSent();
    void MdvroNewBuffers();
    void MdvroFatalError(TInt aError);
    void MdvroInitializeComplete(TInt aError);
    void MdvroStreamEnd();
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED


public: // Methods

    void setError(TInt aError);
    void setCameraHandle(CCameraEngine* cameraHandle);

    qint64 position();
    TVideoCaptureState state() const;
    bool isMuted() const;

    // Controls
    int initializeVideoRecording();
    void startRecording();
    void pauseRecording();
    void stopRecording();
    void setMuted(const bool muted);

    void applyAllSettings();

    // Output Location
    bool setOutputLocation(const QUrl &sink);
    QUrl outputLocation() const;

    // Resolution
    QSize videoResolution() const;
    void setVideoResolution(const QSize &resolution);
    QList<QSize> supportedVideoResolutions();
    QList<QSize> supportedVideoResolutions(const QVideoEncoderSettings &settings);

    // Framerate
    qreal framerate() const;
    void setFrameRate(const qreal rate);
    QList<qreal> supportedVideoFrameRates();
    QList<qreal> supportedVideoFrameRates(const QVideoEncoderSettings &settings);

    // Video Bitrate
    int bitrate();
    void setBitrate(const int bitrate);

    // Video Codecs
    QString videoCaptureCodec() const;
    void setVideoCaptureCodec(const QString &codecName);
    QStringList supportedVideoCaptureCodecs();
    bool isSupportedVideoCaptureCodec(const QString &codecName);
    QString videoCaptureCodecDescription(const QString &codecName);

    // Audio Codecs
    QString audioCaptureCodec() const;
    void setAudioCaptureCodec(const QString &codecName);
    QStringList supportedAudioCaptureCodecs();

    // Video Encoder Settings
    void videoEncoderSettings(QVideoEncoderSettings &videoSettings) const;
    void setVideoEncoderSettings(const QVideoEncoderSettings &videoSettings);

    // Audio Encoder Settings
    void audioEncoderSettings(QAudioEncoderSettings &audioSettings) const;
    void setAudioEncoderSettings(const QAudioEncoderSettings &audioSettings);

    // Video Capture Quality
    QtMultimediaKit::EncodingQuality videoCaptureQuality() const;
    void setVideoCaptureQuality(const QtMultimediaKit::EncodingQuality quality);

    // Video Containers
    QString videoContainer() const;
    void setVideoContainer(const QString &containerName);
    QStringList supportedVideoContainers();
    bool isSupportedVideoContainer(const QString &containerName);
    QString videoContainerDescription(const QString &containerName);

    // Audio SampleRate
    QList<int> supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous);

    // Video Options
    QSize pixelAspectRatio();
    void setPixelAspectRatio(const QSize par);
    int gain();
    void setGain(const int gain);
    int maxClipSizeInBytes() const;
    void setMaxClipSizeInBytes(const int size);

private: // Internal

    QCamera::Error fromSymbianErrorToQtMultimediaError(int aError);

    bool queryCurrentCameraInfo();
    void initializeVideoCaptureSettings();
    void doInitializeVideoRecorderL();
    void commitVideoEncoderSettings();
    void resetSession();

    void doSetVideoFrameRateFixed(bool fixed);
    void doSetBitrate(const int &bitrate);
    void doSetVideoResolution(const QSize &resolution);
    void doSetFrameRate(qreal rate);

    QMap<QString, int> formatMap();
    QMap<QString, int> formatDescMap();
    void updateVideoCaptureContainers();
    void updateVideoCaptureContainersL();
    void doPopulateVideoCodecsDataL();
    void doPopulateVideoCodecsL();
    void doPopulateAudioCodecsL();



Q_SIGNALS: // Notifications

    void stateChanged(S60VideoCaptureSession::TVideoCaptureState);
    void positionChanged(qint64 duration);
    void error(int error, const QString &errorString);

private: // Structs

    struct VideoControllerData {
        int controllerUid;
        int formatUid;
        QString formatDescription;
    };

    struct TSupportedFrameRatePictureSize {
        qreal frameRate;
        QSize frameSize;
        };

private: // Data

    CCameraEngine               *m_cameraEngine;
    CVideoRecorderUtility       *m_videoRecorder;
    int                         m_videoQuality;
    mutable int                 m_error;
    QUrl                        m_sink;
    mutable TCameraInfo         m_info;
    QVideoEncoderSettings       m_videoSettings;
    QAudioEncoderSettings       m_audioSettings;
    TVideoCaptureState          m_captureState;
    QString                     m_container;
    QHash<QString,VideoControllerData> m_videoControllerMap;
    QHash<QString,QList<TSupportedFrameRatePictureSize> >  m_videoCodecData;
    QStringList                 m_videoCodeclist;
    QHash<QString, TFourCC>     m_audioCodeclist;
    bool                        m_muted;
    bool                        m_startAfterPrepareComplete;
    bool                        m_prepareAfterOpenComplete;
    int                         m_maxClipSize;
};

#endif // S60VIDEOCAPTURESESSION_H
