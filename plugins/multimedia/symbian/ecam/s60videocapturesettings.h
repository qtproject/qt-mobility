/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOCAPTURESETTINGS_H
#define S60VIDEOCAPTURESETTINGS_H

//#include <QtCore/QUrl>
#include <QtCore/QHash>

#include <qmediaencodersettings.h>
#include <qcamera.h>
#include <qmediarecorder.h>

#include "s60cameraengine.h"

#include <e32base.h>
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
#include <mmf/devvideo/devvideorecord.h>
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
#include <mmf/common/mmfutilities.h>    // TFourCC

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60VideoCaptureSession)
QT_FORWARD_DECLARE_CLASS(CVideoRecorderUtility)

/*
 * VideoSession is the main class handling all video recording related
 * operations. It uses mainly CVideoRecorderUtility to do it's tasks, but if
 * DevVideoRecord is available it is used to provide more detailed
 * information of the supported video settings.
 */
class S60VideoCaptureSettings : public QObject
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
                               ,public MMMFDevVideoRecordObserver
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
{
    Q_OBJECT

public: // Constructor & Destructor

    S60VideoCaptureSettings(S60VideoCaptureSession *session,
                            QObject *parent = 0);
    ~S60VideoCaptureSettings();

public: // Methods

    void setCameraHandle(CCameraEngine* cameraHandle);
    void setRecorderHandle(CVideoRecorderUtility *recorder);
    void notifySettingsSet();
    void notifyAppliedResolution();
    void queryAudioEncoderSettings();
    void queryVideoEncoderSettings();

    // Muting
    void setMuted(const bool muted);
    bool isMuted() const;

    // Resolution
    void setVideoResolution(const QSize &resolution);
    QList<QSize> supportedVideoResolutions(bool *continuous);
    QList<QSize> supportedVideoResolutions(const QVideoEncoderSettings &settings, bool *continuous);

    // Framerate
    void setVideoFrameRate(const qreal rate);
    QList<qreal> supportedVideoFrameRates(bool *continuous);
    QList<qreal> supportedVideoFrameRates(const QVideoEncoderSettings &settings, bool *continuous);

    // Other Video Settings
    void setVideoBitrate(const int bitrate);
    void setVideoEncodingMode(const QtMultimediaKit::EncodingMode mode);

    // Video Codecs
    void setVideoCodec(const QString &codecName);
    QStringList supportedVideoCodecs();
    QString videoCodecDescription(const QString &codecName);

    // Audio Codecs
    void setAudioCodec(const QString &codecName);
    QStringList supportedAudioCodecs();

    // Encoder Settings
    QVideoEncoderSettings &videoEncoderSettings();
    QAudioEncoderSettings &audioEncoderSettings();

    // Quality
    void setVideoQuality(const QtMultimediaKit::EncodingQuality quality);
    void setAudioQuality(const QtMultimediaKit::EncodingQuality quality);

    // Video Containers
    QString videoContainer() const;
    void setVideoContainer(const QString &containerName);
    QStringList supportedVideoContainers();
    bool isSupportedVideoContainer(const QString &containerName);
    QString videoContainerDescription(const QString &containerName);

    // Audio Settings
    QList<int> supportedAudioSampleRates(const QAudioEncoderSettings &settings, bool *continuous);
    void setAudioSampleRate(const int sampleRate);
    void setAudioBitRate(const int bitRate);
    void setAudioChannelCount(const int channelCount);
    void setAudioEncodingMode(const QtMultimediaKit::EncodingMode mode);

    // Video Options
    QSize pixelAspectRatio();
    void setPixelAspectRatio(const QSize par);
    int audioGain();
    void setAudioGain(const int gain);
    int maxClipSizeInBytes() const;
    void setMaxClipSizeInBytes(const int size);

#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
public: // MMMFDevVideoRecordObserver
    void MdvroReturnPicture(TVideoPicture *aPicture);
    void MdvroSupplementalInfoSent();
    void MdvroNewBuffers();
    void MdvroFatalError(TInt aError);
    void MdvroInitializeComplete(TInt aError);
    void MdvroStreamEnd();
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

private: // Internal

    void initializeVideoCaptureSettings();
    void commitVideoEncoderSettings();
    void validateRequestedCodecs();

    void doSetCodecsL();
    QString determineProfileAndLevel();
    QList<QSize> sortResolutions(QList<QSize> resolutions);
    void doSetVideoResolution(const QSize &resolution);
    void doSetFrameRate(qreal rate);
    void doSetBitrate(const int &bitrate);

    void updateVideoCaptureContainers();
    void doUpdateVideoCaptureContainersL();
    void selectController(TUid &controllerUid,
                          TUid &formatUid);

    void doPopulateVideoCodecsDataL();
    void doPopulateVideoCodecsL();
#ifndef S60_DEVVIDEO_RECORDING_SUPPORTED
    void doPopulateMaxVideoParameters();
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
    void doPopulateAudioCodecsL();

    QList<int> doGetSupportedSampleRatesL(const QAudioEncoderSettings &settings,
                                          bool *continuous);
    QSize maximumResolutionForMimeType(const QString &mimeType) const;
    qreal maximumFrameRateForMimeType(const QString &mimeType) const;
    int maximumBitRateForMimeType(const QString &mimeType) const;

signals: // Notification Signals

    void mutedChanged(bool);
    void captureSizeChanged(const QSize&);

private: // Structs

    /*
     * This structure holds the information of supported video mime types for
     * the format and also description for it.
     */
    struct VideoFormatData {
        QString     description;
        QStringList supportedMimeTypes;
    };

    /*
     * This structure is used to define supported resolutions and framerate
     * (depending on each other) for each supported encoder mime type (defining
     * encoder, profile and level)
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
     * This structure defines supported resolution/framerate pairs and maximum
     * bitrate for a single encodec device. It also the supported mime types
     * (codec, profile and level) of the encoder device.
     *
     * Structure defines 2 contructors:
     *    - First with no attributes
     *    - Second, which will construct the sructure appending one
     *      resolution/framerate pair to the list of
     *      SupportedFrameRatePictureSizes and setting the given bitrate as
     *      maximum. This second constructor is for convenience.
     *
     * This struct is used in m_videoParametersForEncoder (QList).
     *
     * Here's a visualization of an example strcuture:
     * STRUCT:
     *    |-- Resolution/FrameRate Pairs:
     *    |      |- VGA / 30fps
     *    |      |- 720p / 25fps
     *    |      |- Etc.
     *    |
     *    |-- MimeTypes:
     *    |      |- video/mp4v-es; profile-level-id=1
     *    |      |- video/mp4v-es; profile-level-id=2
     *    |      |- Etc.
     *    |
     *    |-- Max BitRate: 1Mbps
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

    friend class S60VideoCaptureSession;

    S60VideoCaptureSession      *m_session;
    CCameraEngine               *m_cameraEngine;
    CVideoRecorderUtility       *m_videoRecorder;
    qint64                      m_position;
    // Requested videoSettings. The may not be active settings before those are
    // committed (with commitVideoEncoderSettings())
    QVideoEncoderSettings       m_videoSettings;
    // Requested audioSettings. The may not be active settings before those are
    // committed (with commitVideoEncoderSettings())
    QAudioEncoderSettings       m_audioSettings;
    // Tells whether settings should be initialized when changing the camera
    bool                        m_captureSettingsSet;
    // Active container
    QString                     m_container;
    // Requested container, this may be different from m_container if
    // asynchronous operation was ongoing in the CVideoRecorderUtility when new
    // container was set.
    QString                     m_requestedContainer;
    // Requested muted value. This may not be active value before settings are
    // committed (with commitVideoEncoderSettings())
    bool                        m_muted;
    // Maximum ClipSize in Bytes
    int                         m_maxClipSize;
    // List of supported video codec mime types
    QStringList                 m_videoCodecList;
    // Hash of supported video codec mime types and corresponding FourCC codes
    QHash<QString, TFourCC>     m_audioCodecList;
    // Map of video capture controllers information. It is populated during
    // doUpdateVideoCaptureContainersL().
    //
    // Here's a visualization of an example strcuture:
    // m_videoControllerMap(HASH):
    //   |
    //   |-- Controller 1 : HASH
    //   |                   |- Container 1 (UID) : FormatData
    //   |                   |                          |- Description
    //   |                   |                          |- List of supported MimeTypes
    //   |                   |- Container 2 (UID) : FormatData
    //   |                   |                          |- Description
    //   |                   |                          |- List of supported MimeTypes
    //   |                   |- Etc.
    //   |
    //   |-- Controller 2: HASH
    //   |                   |- Container 1 (UID) : FormatData
    //   |                   |                          |- Description
    //   |                   |                          |- List of supported MimeTypes
    //   |                   |- Etc.
    //
    QHash<TInt, QHash<TInt,VideoFormatData> > m_videoControllerMap;
    // List of Encoder information. If DevVideoRecord is available info is
    // gathered during doPopulateVideoCodecsDataL() for each encoder (hw
    // accelerated and supporting camera input) found. If DevVideoRecord is not
    // available, the info is set in doPopulateMaxVideoParameters() based on
    // supported codec list received from CVideoRecorderUtility.
    QList<MaxResolutionRatesAndTypes> m_videoParametersForEncoder;
    // Tells if settings have been set after last Prepare()
    bool                        m_uncommittedSettings;
};

#endif // S60VIDEOCAPTURESETTINGS_H
