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

    // TODO: Defninition
    enum AudioQualityDefinition
    {
        ENoAudioQuality = 0,
        EOnlyAudioQuality,
        EAudioQualityAndBitRate,
        EAudioQualityAndSampleRate,
    };

    // TODO: Defninition
    enum VideoQualityDefinition
    {
        ENoVideoQuality = 0,
        EOnlyVideoQuality,
        EVideoQualityAndResolution,
        EVideoQualityAndFrameRate,
        EVideoQualityAndBitRate,
        EVideoQualityAndResolutionAndBitRate,
        EVideoQualityAndResolutionAndFrameRate,
        EVideoQualityAndFrameRateAndBitRate
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
    void setVideoResolution(const QSize &resolution);
    QList<QSize> supportedVideoResolutions(bool &continuous);
    QList<QSize> supportedVideoResolutions(const QVideoEncoderSettings &settings, bool &continuous);

    // Framerate
    void setFrameRate(const qreal rate);
    QList<qreal> supportedVideoFrameRates(bool &continuous);
    QList<qreal> supportedVideoFrameRates(const QVideoEncoderSettings &settings, bool &continuous);

    // Other Video Settings
    void setBitrate(const int bitrate);
    void setVideoEncodingMode(const QtMultimediaKit::EncodingMode mode);

    // Video Codecs
    void setVideoCaptureCodec(const QString &codecName);
    QStringList supportedVideoCaptureCodecs();
    QString videoCaptureCodecDescription(const QString &codecName);

    // Audio Codecs
    void setAudioCaptureCodec(const QString &codecName);
    QStringList supportedAudioCaptureCodecs();

    // Video Encoder Settings
    void videoEncoderSettings(QVideoEncoderSettings &videoSettings) const;

    // Audio Encoder Settings
    void audioEncoderSettings(QAudioEncoderSettings &audioSettings) const;

    // Video Capture Quality
    void setVideoCaptureQuality(const QtMultimediaKit::EncodingQuality quality,
                                const VideoQualityDefinition mode);
    // Audio Capture Quality
    void setAudioCaptureQuality(const QtMultimediaKit::EncodingQuality quality,
                                const AudioQualityDefinition mode);

    // Video Containers
    QString videoContainer() const;
    void setVideoContainer(const QString &containerName);
    QStringList supportedVideoContainers();
    bool isSupportedVideoContainer(const QString &containerName);
    QString videoContainerDescription(const QString &containerName);

    // Audio Settings
    QList<int> supportedSampleRates(const QAudioEncoderSettings &settings, bool &continuous);
    void setAudioSampleRate(const int sampleRate);
    void setAudioBitRate(const int bitRate);
    void setAudioChannelCount(const int channelCount);
    void setAudioEncodingMode(const QtMultimediaKit::EncodingMode mode);

    // Video Options
    QSize pixelAspectRatio();
    void setPixelAspectRatio(const QSize par);
    int gain();
    void setGain(const int gain);
    int maxClipSizeInBytes() const;
    void setMaxClipSizeInBytes(const int size);

private: // Internal

    QCamera::Error fromSymbianErrorToQtMultimediaError(int aError);

    void initializeVideoCaptureSettings();
    void doInitializeVideoRecorderL();
    void commitVideoEncoderSettings();
    void resetSession();

    void doSetCodecs(const QString &aCodec, const QString &vCodec);
    void doSetVideoResolution(const QSize &resolution);
    void doSetFrameRate(qreal rate);
    void doSetBitrate(const int &bitrate);

//    QMap<QString, int> formatMap();
//    QMap<QString, int> formatDescMap();
    void updateVideoCaptureContainers();
    void doUpdateVideoCaptureContainersL();
    void selectController(const QString &format,
                          TUid &controllerUid,
                          TUid &formatUid);
    void doPopulateVideoCodecsDataL();
    void doPopulateVideoCodecsL();
#ifndef S60_DEVVIDEO_RECORDING_SUPPORTED
	void doPopulateMaxVideoParameters();
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
	void doPopulateAudioCodecsL();
	QSize maximumResolutionForMimeType(const QString &mimeType) const;
	qreal maximumFrameRateForMimeType(const QString &mimeType) const;
	int maximumBitRateForMimeType(const QString &mimeType) const;

Q_SIGNALS: // Notifications

    void stateChanged(S60VideoCaptureSession::TVideoCaptureState);
    void positionChanged(qint64 duration);
    void error(int error, const QString &errorString);

private: // Structs

    struct VideoFormatData {
        QString     description;
        QStringList supportedMimeTypes;
    };

    /*
     * This structure is used to define supported resolutions and framerate
     * (depending on each other) for each supported encodec mime type (encoder
     * profile and level)
     */
    struct SupportedFrameRatePictureSize {
        SupportedFrameRatePictureSize() {}
        SupportedFrameRatePictureSize(qreal rate, QSize size):
            frameRate(rate),
            frameSize(size) {}
        qreal frameRate;
        QSize frameSize;
        };

    /*
     * This structure defines supported resolution/framerate pairs and bitrate
     * for the codec mime type (certain codec profile and level). It also
     * holds the controller Uid (default = 0).
     */
    struct MaxResolutionRatesAndTypes {
        MaxResolutionRatesAndTypes() {}
        MaxResolutionRatesAndTypes(QSize size, qreal fRate, int bRate):
            bitRate(bRate)
        {
            frameRatePictureSizePair.append(SupportedFrameRatePictureSize(fRate,size));
        }
        QList<SupportedFrameRatePictureSize> frameRatePictureSizePair;
        QStringList                          mimeTypes;
        int                                  bitRate;
    };

private: // Data

    CCameraEngine               *m_cameraEngine;
    CVideoRecorderUtility       *m_videoRecorder;

    mutable int                 m_error;
    TVideoCaptureState          m_captureState;

    QUrl                        m_sink;
    QUrl                        m_requestedSink;
    QVideoEncoderSettings       m_videoSettings;
    QAudioEncoderSettings       m_audioSettings;
    int                         m_videoQuality;
    QString                     m_container;
    QString                     m_requestedContainer;
    bool                        m_muted;
    int                         m_maxClipSize;

    mutable TCameraInfo         m_info;
    QStringList                 m_videoCodeclist;
    QHash<QString, TFourCC>     m_audioCodeclist;
    QHash<TInt, QHash<TInt,VideoFormatData> > m_videoControllerMap;
    QList<MaxResolutionRatesAndTypes> m_videoParametersForEncoder;

    bool                        m_openWhenInitialized;
    bool                        m_prepareAfterOpenComplete;
    bool                        m_startAfterPrepareComplete;
    bool                        m_uncommittedSettings;
};

#endif // S60VIDEOCAPTURESESSION_H
