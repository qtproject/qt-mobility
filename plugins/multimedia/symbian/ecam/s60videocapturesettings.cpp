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

#include <QtCore/qdir.h>
#include <QtCore/qtimer.h>

#include "s60videocapturesession.h"
#include "s60videocapturesettings.h"
#include "s60filenamegenerator.h"
#include "s60cameraconstants.h"

#include <utf.h>
#include <bautils.h>
#include <videorecorder.h> // CVideoRecorderUtility

using namespace S60CameraConstants;

S60VideoCaptureSettings::S60VideoCaptureSettings(S60VideoCaptureSession *session,
                                                 QObject *parent) :
    QObject(parent),
    m_session(session),
    m_cameraEngine(0),
    m_videoRecorder(0),
    m_position(0),
    m_captureSettingsSet(false),
    m_container(QString()),
    m_requestedContainer(QString()),
    m_muted(false),
    m_maxClipSize(-1),
    m_videoControllerMap(QHash<int, QHash<int,VideoFormatData> >()),
    m_videoParametersForEncoder(QList<MaxResolutionRatesAndTypes>()),
    m_uncommittedSettings(false)
{
}

S60VideoCaptureSettings::~S60VideoCaptureSettings()
{
    // Clear all data structures
    foreach (MaxResolutionRatesAndTypes structure, m_videoParametersForEncoder) {
        structure.frameRatePictureSizePair.clear();
        structure.mimeTypes.clear();
    }
    m_videoParametersForEncoder.clear();

    QList<TInt> controllers = m_videoControllerMap.keys();
    for (int i = 0; i < controllers.size(); ++i) {
        foreach (VideoFormatData data, m_videoControllerMap[controllers[i]])
            data.supportedMimeTypes.clear();
        m_videoControllerMap[controllers[i]].clear();
    }
    m_videoControllerMap.clear();
}

void S60VideoCaptureSettings::setCameraHandle(CCameraEngine* cameraHandle)
{
    m_cameraEngine = cameraHandle;
}

void S60VideoCaptureSettings::setRecorderHandle(CVideoRecorderUtility *recorder)
{
    m_videoRecorder = recorder;
    m_uncommittedSettings = false; // Reset
}

void S60VideoCaptureSettings::notifySettingsSet()
{
    m_captureSettingsSet = true;
}

void S60VideoCaptureSettings::notifyAppliedResolution()
{
    emit captureSizeChanged(m_videoSettings.resolution());
}

QList<QSize> S60VideoCaptureSettings::sortResolutions(QList<QSize> resolutions)
{
    if (resolutions.count() < 2)
        return resolutions;

    // Sort resolutions
    QList<QSize> smaller;
    QList<QSize> larger;

    // Biased midpoint selection (works well on most platforms)
    QSize mid = resolutions.first();
    int midMultiplied = mid.width() * mid.height();

    foreach (QSize resolution, resolutions) {
        if (resolution.width() * resolution.height() <= midMultiplied) {
            if (resolution != mid)
                smaller << resolution;
        } else {
            larger << resolution;
        }
    }

    // Sort to descending order
    return (sortResolutions(larger) << mid) + sortResolutions(smaller);
}

QList<QSize> S60VideoCaptureSettings::supportedVideoResolutions(bool *continuous)
{
    QList<QSize> resolutions;

    // Secondary Camera
    if (m_cameraEngine->CurrentCameraIndex() != 0) {
        TCameraInfo *info = m_cameraEngine->CameraInfo();
        if (info) {
            TInt videoResolutionCount = info->iNumVideoFrameSizesSupported;
            CCamera *camera = m_cameraEngine->Camera();
            if (camera) {
                for (TInt i = 0; i < videoResolutionCount; ++i) {
                    TSize checkedResolution;
                    camera->EnumerateVideoFrameSizes(checkedResolution, i, CCamera::EFormatYUV420Planar);
                    QSize qtResolution(checkedResolution.iWidth, checkedResolution.iHeight);
                    if (!resolutions.contains(qtResolution))
                        resolutions.append(qtResolution);
                }
            } else {
                m_session->setError(KErrGeneral, tr("Could not query supported video resolutions."));
            }
        } else {
            m_session->setError(KErrGeneral, tr("Could not query supported video resolutions."));
        }

    // Primary Camera
    } else {
        if (m_videoParametersForEncoder.count() > 0) {
            // Append all supported resolutions to the list
            foreach (MaxResolutionRatesAndTypes parameters, m_videoParametersForEncoder)
                for (int i = 0; i < parameters.frameRatePictureSizePair.count(); ++i)
                    if (!resolutions.contains(parameters.frameRatePictureSizePair[i].frameSize))
                        resolutions.append(parameters.frameRatePictureSizePair[i].frameSize);
        }
    }

    // Also arbitrary resolutions are supported
    if (continuous)
        *continuous = true;

#ifdef Q_CC_NOKIAX86 // Emulator
    resolutions << QSize(160, 120);
    resolutions << QSize(352, 288);
    resolutions << QSize(640,480);
#endif // Q_CC_NOKIAX86

    // Sort list (from large to small resolution)
    if (resolutions.count() > 1) {
        // First check if resolutions already are in correct order
        bool isDescending = true;
        for (int i = 0; i < resolutions.count() - 1; ++i) {
            if (resolutions.at(i).width() * resolutions.at(i).height() <
                resolutions.at(i+1).width() * resolutions.at(i+1).height())
                isDescending = false;
        }
        if (!isDescending)
            resolutions = sortResolutions(resolutions);
    }

    return resolutions;
}

QList<QSize> S60VideoCaptureSettings::supportedVideoResolutions(const QVideoEncoderSettings &settings, bool *continuous)
{
    QList<QSize> supportedFrameSizes;

    if (settings.codec().isEmpty())
        return supportedFrameSizes;

    if (!m_videoCodecList.contains(settings.codec(), Qt::CaseInsensitive))
        return supportedFrameSizes;

    // Take generic mime types into account
    QString codec = settings.codec();
    codec = codec.toLower();
    if (codec == QLatin1String("video/h264"))
#ifdef SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/h264; profile-level-id=42801f");
#else // SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/h264; profile-level-id=42800d");
#endif // SYMBIAN_3_PLATFORM
    else if (codec == QLatin1String("video/mp4v-es"))
#ifdef SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/mp4v-es; profile-level-id=6");
#else // SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/mp4v-es; profile-level-id=4");
#endif // SYMBIAN_3_PLATFORM
    else if (codec == QLatin1String("video/h263-2000") ||
             codec == QLatin1String("video/h263-2000; profile=0"))
        codec = QLatin1String("video/h263-2000; profile=0; level=40");

    // Secondary Camera
    if (m_cameraEngine->CurrentCameraIndex() != 0) {
        TCameraInfo *info = m_cameraEngine->CameraInfo();
        if (info) {
            TInt videoResolutionCount = info->iNumVideoFrameSizesSupported;
            CCamera *camera = m_cameraEngine->Camera();
            if (camera) {
                for (TInt i = 0; i < videoResolutionCount; ++i) {
                    TSize checkedResolution;
                    camera->EnumerateVideoFrameSizes(checkedResolution, i, CCamera::EFormatYUV420Planar);
                    QSize qtResolution(checkedResolution.iWidth, checkedResolution.iHeight);
                    QSize maxForCodec = maximumResolutionForMimeType(codec);
                    if (!supportedFrameSizes.contains(qtResolution) &&
                        (qtResolution.width() * qtResolution.height()) <= (maxForCodec.width() * maxForCodec.height()))
                        supportedFrameSizes << qtResolution;
                }
            } else {
                m_session->setError(KErrGeneral, tr("Could not query supported video resolutions."));
            }
        } else {
            m_session->setError(KErrGeneral, tr("Could not query supported video resolutions."));
        }

    // Primary Camera
    } else {
        // Find maximum resolution (using defined framerate if set)
        for (int i = 0; i < m_videoParametersForEncoder.count(); ++i) {
            // Check if encoder supports the requested codec
            if (!m_videoParametersForEncoder[i].mimeTypes.contains(codec, Qt::CaseInsensitive))
                continue;

            foreach (SupportedFrameRatePictureSize pair, m_videoParametersForEncoder[i].frameRatePictureSizePair) {
                if (!supportedFrameSizes.contains(pair.frameSize)) {
                    QSize maxForMime = maximumResolutionForMimeType(codec);
                    if (settings.frameRate() != 0) {
                        if (settings.frameRate() <= pair.frameRate) {
                            if ((pair.frameSize.width() * pair.frameSize.height()) <= (maxForMime.width() * maxForMime.height()))
                                supportedFrameSizes << pair.frameSize;
                        }
                    } else {
                        if ((pair.frameSize.width() * pair.frameSize.height()) <= (maxForMime.width() * maxForMime.height()))
                            supportedFrameSizes << pair.frameSize;
                    }
                }
            }
        }
    }

    // Also arbitrary resolutions are supported
    if (continuous)
        *continuous = true;

#ifdef Q_CC_NOKIAX86 // Emulator
    supportedFrameSizes << QSize(160, 120);
    supportedFrameSizes << QSize(352, 288);
    supportedFrameSizes << QSize(640,480);
#endif

    // Sort list (from large to small resolution)
    if (supportedFrameSizes.count() > 1) {
        // First check if resolutions already are in correct order
        bool isDescending = true;
        for (int i = 0; i < supportedFrameSizes.count() - 1; ++i) {
            if (supportedFrameSizes.at(i).width() * supportedFrameSizes.at(i).height() <
                supportedFrameSizes.at(i+1).width() * supportedFrameSizes.at(i+1).height())
                isDescending = false;
        }
        if (!isDescending)
            supportedFrameSizes = sortResolutions(supportedFrameSizes);
    }

    return supportedFrameSizes;
}

QList<qreal> S60VideoCaptureSettings::supportedVideoFrameRates(bool *continuous)
{
    QList<qreal> supportedRatesList;

    if (m_videoParametersForEncoder.count() > 0) {
        // Insert min and max to the list
        supportedRatesList.append(10.0); // Use 10fps as sensible minimum
        qreal foundMaxFrameRate(0.0);

        // Find max framerate
        foreach (MaxResolutionRatesAndTypes parameters, m_videoParametersForEncoder) {
            for (int i = 0; i < parameters.frameRatePictureSizePair.count(); ++i) {
                qreal maxFrameRate = parameters.frameRatePictureSizePair[i].frameRate;
                if (maxFrameRate > foundMaxFrameRate)
                    foundMaxFrameRate = maxFrameRate;
            }
        }

        supportedRatesList.append(foundMaxFrameRate);
    }

    // Add also other standard framerates to the list
    if (!supportedRatesList.isEmpty()) {
        if (supportedRatesList.last() > 30.0) {
            if (!supportedRatesList.contains(30.0))
                supportedRatesList.insert(1, 30.0);
        }
        if (supportedRatesList.last() > 25.0) {
            if (!supportedRatesList.contains(25.0))
                supportedRatesList.insert(1, 25.0);
        }
        if (supportedRatesList.last() > 15.0) {
            if (!supportedRatesList.contains(15.0))
                supportedRatesList.insert(1, 15.0);
        }
        if (supportedRatesList.last() > 10.0) {
            if (!supportedRatesList.contains(10))
                supportedRatesList.insert(1, 10.0);
        }
    }

    // Also arbitrary framerates are supported
    if (continuous)
        *continuous = true;

#ifdef Q_CC_NOKIAX86 // Emulator
    supportedRatesList << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
#endif

    return supportedRatesList;
}

QList<qreal> S60VideoCaptureSettings::supportedVideoFrameRates(const QVideoEncoderSettings &settings, bool *continuous)
{
    QList<qreal> supportedFrameRates;

    if (settings.codec().isEmpty())
        return supportedFrameRates;
    if (!m_videoCodecList.contains(settings.codec(), Qt::CaseInsensitive))
        return supportedFrameRates;

    // Take generic mime types into account
    QString codec = settings.codec();
    codec = codec.toLower();
    if (codec == QLatin1String("video/h264"))
#ifdef SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/h264; profile-level-id=42801f");
#else // SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/h264; profile-level-id=42800d");
#endif // SYMBIAN_3_PLATFORM
    else if (codec == QLatin1String("video/mp4v-es"))
#ifdef SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/mp4v-es; profile-level-id=6");
#else // SYMBIAN_3_PLATFORM
        codec = QLatin1String("video/mp4v-es; profile-level-id=4");
#endif // SYMBIAN_3_PLATFORM
    else if (codec == QLatin1String("video/h263-2000") ||
             codec == QLatin1String("video/h263-2000; profile=0"))
        codec = QLatin1String("video/h263-2000; profile=0; level=40");

    // Find maximum framerate (using defined resolution if set)
    for (int i = 0; i < m_videoParametersForEncoder.count(); ++i) {
        // Check if encoder supports the requested codec
        if (!m_videoParametersForEncoder[i].mimeTypes.contains(codec, Qt::CaseInsensitive))
            continue;

        foreach (SupportedFrameRatePictureSize pair, m_videoParametersForEncoder[i].frameRatePictureSizePair) {
            if (!supportedFrameRates.contains(pair.frameRate)) {
                qreal maxRateForMime = maximumFrameRateForMimeType(codec);
                if (settings.resolution().width() != 0 && settings.resolution().height() != 0) {
                    if((settings.resolution().width() * settings.resolution().height()) <= (pair.frameSize.width() * pair.frameSize.height())) {
                        if (pair.frameRate <= maxRateForMime)
                            supportedFrameRates << pair.frameRate;
                    }
                } else {
                    if (pair.frameRate <= maxRateForMime)
                        supportedFrameRates << pair.frameRate;
                }
            }
        }
        if (supportedFrameRates.isEmpty() && maximumFrameRateForMimeType(codec) > 0)
            supportedFrameRates << maximumFrameRateForMimeType(codec);
    }

    // Add also other standard framerates to the list
    if (!supportedFrameRates.isEmpty()) {
        if (supportedFrameRates.last() > 30.0) {
            if (!supportedFrameRates.contains(30.0))
                supportedFrameRates.insert(1, 30.0);
        }
        if (supportedFrameRates.last() > 25.0) {
            if (!supportedFrameRates.contains(25.0))
                supportedFrameRates.insert(1, 25.0);
        }
        if (supportedFrameRates.last() > 15.0) {
            if (!supportedFrameRates.contains(15.0))
                supportedFrameRates.insert(1, 15.0);
        }
        if (supportedFrameRates.last() > 10.0) {
            if (!supportedFrameRates.contains(10))
                supportedFrameRates.insert(1, 10.0);
        }
    }

    // Also arbitrary framerates are supported
    if (continuous)
        *continuous = true;

#ifdef Q_CC_NOKIAX86 // Emulator
    supportedFrameRates << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
#endif

    return supportedFrameRates;
}

bool S60VideoCaptureSettings::isMuted() const
{
    return m_muted;
}

void S60VideoCaptureSettings::setMuted(const bool muted)
{
    // Check if request is already active
    if (muted == isMuted())
        return;

    // CVideoRecorderUtility can mute/unmute only if not recording
    if (m_session->m_captureState > S60VideoCaptureSession::EPrepared) {
        // KErrNotFound converts to QMediaRecorder::ResourceError which is sensible here.
        // KErrNotFound error does not reset the device.
        if (muted)
            m_session->setError(KErrNotFound, tr("Muting audio is not supported during recording."));
        else
            m_session->setError(KErrNotFound, tr("Unmuting audio is not supported during recording."));
        return;
    }

    m_muted = muted;
    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::commitVideoEncoderSettings()
{
    if (m_session->m_captureState == S60VideoCaptureSession::EOpenComplete) {

        if (m_container != m_requestedContainer) {
            m_session->setOutputLocation(m_session->m_requestedSink);
            return;
        }

        TRAPD(err, doSetCodecsL());
        if (err) {
            m_session->setError(err, tr("Failed to set audio or video codec."));
            m_audioSettings.setCodec(KMimeTypeDefaultAudioCodec);
            m_videoSettings.setCodec(KMimeTypeDefaultVideoCodec);
        }

        doSetVideoResolution(m_videoSettings.resolution());
        doSetFrameRate(m_videoSettings.frameRate());
        doSetBitrate(m_videoSettings.bitRate());

        // Audio/Video EncodingMode are not supported in Symbian

#ifndef S60_31_PLATFORM
        if (m_audioSettings.sampleRate() != -1 && m_audioSettings.sampleRate() != 0) {
            TRAP(err, m_videoRecorder->SetAudioSampleRateL((TInt)m_audioSettings.sampleRate()));
            if (err != KErrNotSupported) {
                m_session->setError(err, tr("Setting audio sample rate failed."));
            } else {
                m_session->setError(err, tr("Setting audio sample rate is not supported."));
                m_audioSettings.setSampleRate(KDefaultSampleRate); // Reset
            }
        }
#endif // S60_31_PLATFORM

        TRAP(err, m_videoRecorder->SetAudioBitRateL((TInt)m_audioSettings.bitRate()));
        if (err != KErrNotSupported) {
            if (err == KErrArgument) {
                m_session->setError(KErrNotSupported, tr("Requested audio bitrate is not supported or previously set codec is not supported with requested bitrate."));
                int fallback = 16000;
                TRAP(err, m_videoRecorder->SetAudioBitRateL(TInt(fallback)));
                if (err == KErrNone)
                    m_audioSettings.setBitRate(fallback);
            } else {
                m_session->setError(err, tr("Setting audio bitrate failed."));
            }
        }

#ifndef S60_31_PLATFORM
        if (m_audioSettings.channelCount() != -1) {
            TRAP(err, m_videoRecorder->SetAudioChannelsL(TUint(m_audioSettings.channelCount())));
            if (err != KErrNotSupported) {
                m_session->setError(err, tr("Setting audio channel count failed."));
            } else {
                m_session->setError(err, tr("Setting audio channel count is not supported."));
                m_audioSettings.setChannelCount(KDefaultChannelCount); // Reset
            }
        }
#endif // S60_31_PLATFORM

        TBool isAudioMuted = EFalse;
        TRAP(err, isAudioMuted = !m_videoRecorder->AudioEnabledL());
        if (err != KErrNotSupported && err != KErrNone)
            m_session->setError(err, tr("Failure when checking if audio is enabled."));

        if (m_muted != (bool)isAudioMuted) {
            TRAP(err, m_videoRecorder->SetAudioEnabledL(TBool(!m_muted)));
            if (err) {
                if (err != KErrNotSupported)
                    m_session->setError(err, tr("Failed to mute/unmute audio."));
                else
                    m_session->setError(err, tr("Muting/unmuting audio is not supported."));
            } else {
                emit mutedChanged(m_muted);
            }
        }

        m_uncommittedSettings = false; // Reset
    }
}

void S60VideoCaptureSettings::queryAudioEncoderSettings()
{
    if (!m_videoRecorder || !m_session->isReadyToQueryVideoSettings())
        return;

    TInt err = KErrNone;

    // Codec
    TFourCC audioCodec;
    TRAP(err, audioCodec = m_videoRecorder->AudioTypeL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying audio codec failed."));
    }
    QString codec = "";
    foreach (TFourCC aCodec, m_audioCodecList) {
        if (audioCodec == aCodec)
            codec = m_audioCodecList.key(aCodec);
    }
    m_audioSettings.setCodec(codec);

#ifndef S60_31_PLATFORM
    // Samplerate
    TInt sampleRate = -1;
    TRAP(err, sampleRate = m_videoRecorder->AudioSampleRateL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying audio sample rate failed."));
    }
    m_audioSettings.setSampleRate(int(sampleRate));
#endif // S60_31_PLATFORM

    // BitRate
    TInt bitRate = -1;
    TRAP(err, bitRate = m_videoRecorder->AudioBitRateL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying audio bitrate failed."));
    }
    m_audioSettings.setBitRate(int(bitRate));

#ifndef S60_31_PLATFORM
    // ChannelCount
    TUint channelCount = 0;
    TRAP(err, channelCount = m_videoRecorder->AudioChannelsL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying audio channel count failed."));
    }
    if (channelCount != 0)
        m_audioSettings.setChannelCount(int(channelCount));
    else
        m_audioSettings.setChannelCount(-1);
#endif // S60_31_PLATFORM

    // EncodingMode
    m_audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);

    // IsMuted
    TBool isEnabled = ETrue;
    TRAP(err, isEnabled = m_videoRecorder->AudioEnabledL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying whether audio is muted failed."));
    }
    m_muted = bool(!isEnabled);
}

void S60VideoCaptureSettings::queryVideoEncoderSettings()
{
    if (!m_videoRecorder || !m_session->isReadyToQueryVideoSettings())
        return;

    TInt err = KErrNone;

    // Codec
    const TDesC8 &videoMimeType = m_videoRecorder->VideoFormatMimeType();
    QString videoMimeTypeString;
    if (videoMimeType.Length() > 0) {
        // First convert the 8-bit descriptor to Unicode
        HBufC16* videoCodec = 0;
        TRAP(err, videoCodec = CnvUtfConverter::ConvertToUnicodeFromUtf8L(videoMimeType));
        if (err) {
            m_session->setError(err, tr("Failed to convert video codec to Qt format."));
        } else {
            // Successful - Deep copy QString from that
            videoMimeTypeString = QString::fromUtf16(videoCodec->Ptr(), videoCodec->Length());
            m_videoSettings.setCodec(videoMimeTypeString);
        }
    }

    // Resolution
    TSize symbianResolution;
    TRAP(err, m_videoRecorder->GetVideoFrameSizeL(symbianResolution));
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying video resolution failed."));
    }
    QSize resolution = QSize(symbianResolution.iWidth, symbianResolution.iHeight);
    m_videoSettings.setResolution(resolution);

    // FrameRate
    TReal32 frameRate = 0;
    TRAP(err, frameRate = m_videoRecorder->VideoFrameRateL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying video framerate failed."));
    }
    m_videoSettings.setFrameRate(qreal(frameRate));

    // BitRate
    TInt bitRate = -1;
    TRAP(err, bitRate = m_videoRecorder->VideoBitRateL());
    if (err) {
        if (err != KErrNotSupported)
            m_session->setError(err, tr("Querying video bitrate failed."));
    }
    m_videoSettings.setBitRate(int(bitRate));

    // EncodingMode
    m_audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
}

QVideoEncoderSettings &S60VideoCaptureSettings::videoEncoderSettings()
{
    queryVideoEncoderSettings();
    return m_videoSettings;
}

QAudioEncoderSettings &S60VideoCaptureSettings::audioEncoderSettings()
{
    queryAudioEncoderSettings();
    return m_audioSettings;
}

void S60VideoCaptureSettings::validateRequestedCodecs()
{
    if (!m_audioCodecList.contains(m_audioSettings.codec())) {
        m_audioSettings.setCodec(KMimeTypeDefaultAudioCodec);
        m_session->setError(KErrNotSupported, tr("Currently selected audio codec is not supported by the platform."));
    }
    if (!m_videoCodecList.contains(m_videoSettings.codec(), Qt::CaseInsensitive)) {
        m_videoSettings.setCodec(KMimeTypeDefaultVideoCodec);
        m_session->setError(KErrNotSupported, tr("Currently selected video codec is not supported by the platform."));
    }
}

void S60VideoCaptureSettings::setVideoQuality(const QtMultimediaKit::EncodingQuality quality)
{
    m_videoSettings.setQuality(quality);
    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::setAudioQuality(const QtMultimediaKit::EncodingQuality quality)
{
    m_audioSettings.setQuality(quality);
    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::updateVideoCaptureContainers()
{
    TRAPD(err, doUpdateVideoCaptureContainersL());
    m_session->setError(err, tr("Failed to gather video container information."));
}

void S60VideoCaptureSettings::doUpdateVideoCaptureContainersL()
{
    // Clear container data structure
    QList<TInt> mapControllers = m_videoControllerMap.keys();
    for (int i = 0; i < mapControllers.size(); ++i) {
        foreach (VideoFormatData data, m_videoControllerMap[mapControllers[i]])
            data.supportedMimeTypes.clear();
        m_videoControllerMap[mapControllers[i]].clear();
    }
    m_videoControllerMap.clear();

    // Resolve the supported video format and retrieve a list of controllers
    CMMFControllerPluginSelectionParameters* pluginParameters =
        CMMFControllerPluginSelectionParameters::NewLC();
    CMMFFormatSelectionParameters* format =
        CMMFFormatSelectionParameters::NewLC();

    // Set the play and record format selection parameters to be blank.
    // Format support is only retrieved if requested.
    pluginParameters->SetRequiredPlayFormatSupportL(*format);
    pluginParameters->SetRequiredRecordFormatSupportL(*format);

    // Set the media IDs
    RArray<TUid> mediaIds;
    CleanupClosePushL(mediaIds);

    User::LeaveIfError(mediaIds.Append(KUidMediaTypeVideo));

    // Get plugins that support at least video
    pluginParameters->SetMediaIdsL(mediaIds,
        CMMFPluginSelectionParameters::EAllowOtherMediaIds);
    pluginParameters->SetPreferredSupplierL(KNullDesC,
        CMMFPluginSelectionParameters::EPreferredSupplierPluginsFirstInList);

    // Array to hold all the controllers support the match data
    RMMFControllerImplInfoArray controllers;
    CleanupResetAndDestroyPushL(controllers);
    pluginParameters->ListImplementationsL(controllers);

    // Find the first controller with at least one record format available
    for (TInt index = 0; index < controllers.Count(); ++index) {
        m_videoControllerMap.insert(controllers[index]->Uid().iUid, QHash<TInt,VideoFormatData>());

        const RMMFFormatImplInfoArray& recordFormats = controllers[index]->RecordFormats();
        for (TInt j = 0; j < recordFormats.Count(); ++j) {
            VideoFormatData formatData;
            formatData.description = QString::fromUtf16(
                                    recordFormats[j]->DisplayName().Ptr(),
                                    recordFormats[j]->DisplayName().Length());

            const CDesC8Array& mimeTypes = recordFormats[j]->SupportedMimeTypes();
            for (int k = 0; k < mimeTypes.Count(); ++k) {
                TPtrC8 mimeType = mimeTypes[k];
                QString type = QString::fromUtf8((char *)mimeType.Ptr(),
                        mimeType.Length());
                formatData.supportedMimeTypes.append(type);
            }

            m_videoControllerMap[controllers[index]->Uid().iUid].insert(recordFormats[j]->Uid().iUid, formatData);
        }
    }

    CleanupStack::PopAndDestroy(&controllers);
    CleanupStack::PopAndDestroy(&mediaIds);
    CleanupStack::PopAndDestroy(format);
    CleanupStack::PopAndDestroy(pluginParameters);
}

/*
 * This goes through the available controllers and selects proper one based
 * on the format. Function sets proper UIDs to be used for controller and format.
 */
void S60VideoCaptureSettings::selectController(TUid &controllerUid,
                                               TUid &formatUid)
{
    QList<TInt> controllers = m_videoControllerMap.keys();
    QList<TInt> formats;

    for (int i = 0; i < controllers.count(); ++i) {
        formats = m_videoControllerMap[controllers[i]].keys();
        for (int j = 0; j < formats.count(); ++j) {
            VideoFormatData formatData = m_videoControllerMap[controllers[i]][formats[j]];
            if (formatData.supportedMimeTypes.contains(m_requestedContainer, Qt::CaseInsensitive)) {
                controllerUid = TUid::Uid(controllers[i]);
                formatUid = TUid::Uid(formats[j]);
            }
        }
    }
}

QStringList S60VideoCaptureSettings::supportedVideoCodecs()
{
    QStringList videoCodecs;

    // Secondary Camera
    if (m_cameraEngine->CurrentCameraIndex() != 0) {
        QSize maxForCamera(0,0);
        TCameraInfo *info = m_cameraEngine->CameraInfo();
        if (info) {
            TInt videoResolutionCount = info->iNumVideoFrameSizesSupported;
            CCamera *camera = m_cameraEngine->Camera();
            if (camera) {
                for (TInt i = 0; i < videoResolutionCount; ++i) {
                    TSize checkedResolution;
                    camera->EnumerateVideoFrameSizes(checkedResolution, i, CCamera::EFormatYUV420Planar);
                    if ((checkedResolution.iWidth * checkedResolution.iHeight) >
                        (maxForCamera.width() * maxForCamera.height()))
                        maxForCamera = QSize(checkedResolution.iWidth, checkedResolution.iHeight);
                }
            }
        }

        // Add only codecs for which the maximum resolution is smaller than the
        // secondary camera maximum resolution
        foreach (QString codec, m_videoCodecList) {
            QSize maxForCodec = maximumResolutionForMimeType(codec);
            if ((maxForCodec.width() * maxForCodec.height()) <
                (maxForCamera.width() * maxForCamera.height()))
                videoCodecs << codec;
        }

    // Primary Camera
    } else {
        videoCodecs = m_videoCodecList;
    }

    return videoCodecs;
}

QStringList S60VideoCaptureSettings::supportedAudioCodecs()
{
    QStringList keys = m_audioCodecList.keys();
    keys.sort();
    return keys;
}

QList<int> S60VideoCaptureSettings::supportedAudioSampleRates(const QAudioEncoderSettings &settings, bool *continuous)
{
    QList<int> rates;

    TRAPD(err, rates = doGetSupportedSampleRatesL(settings, continuous));
    if (err != KErrNotSupported)
        m_session->setError(err, tr("Failed to query information of supported sample rates."));

    return rates;
}

QList<int> S60VideoCaptureSettings::doGetSupportedSampleRatesL(const QAudioEncoderSettings &settings, bool *continuous)
{
    QList<int> sampleRates;

    if (m_session->m_captureState < S60VideoCaptureSession::EOpenComplete)
        return sampleRates;

#ifndef S60_31_PLATFORM
    RArray<TUint> supportedSampleRates;
    CleanupClosePushL(supportedSampleRates);

    if (!settings.codec().isEmpty()) {

        TFourCC currentAudioCodec;
        currentAudioCodec = m_videoRecorder->AudioTypeL();

        TFourCC requestedAudioCodec;
        if (qstrcmp(settings.codec().toLocal8Bit().constData(), "audio/aac") == 0)
            requestedAudioCodec.Set(KMMFFourCCCodeAAC);
        else if (qstrcmp(settings.codec().toLocal8Bit().constData(), "audio/amr") == 0)
            requestedAudioCodec.Set(KMMFFourCCCodeAMR);
        m_videoRecorder->SetAudioTypeL(requestedAudioCodec);

        m_videoRecorder->GetSupportedAudioSampleRatesL(supportedSampleRates);

        m_videoRecorder->SetAudioTypeL(currentAudioCodec);
    } else {
        m_videoRecorder->GetSupportedAudioSampleRatesL(supportedSampleRates);
    }

    for (int i = 0; i < supportedSampleRates.Count(); ++i)
        sampleRates.append(int(supportedSampleRates[i]));

    CleanupStack::PopAndDestroy(); // RArray<TUint> supportedSampleRates
#else // S60 3.1 Platform
    Q_UNUSED(settings);
#endif // S60_31_PLATFORM

    if (continuous)
        *continuous = false;

    return sampleRates;
}

void S60VideoCaptureSettings::setAudioSampleRate(const int sampleRate)
{
    if (sampleRate != -1) {
        m_audioSettings.setSampleRate(sampleRate);
        m_uncommittedSettings = true;
    }

    // Quality has no effect here as sample rate is not supported on Symbian
}

void S60VideoCaptureSettings::setAudioBitRate(const int bitRate)
{
    if (bitRate != -1) {
        m_audioSettings.setBitRate(bitRate);
    } else {
        // Use quality to set BitRate
        switch (m_audioSettings.quality()) {
        case QtMultimediaKit::VeryLowQuality:
            m_audioSettings.setBitRate(16000);
            break;
        case QtMultimediaKit::LowQuality:
            m_audioSettings.setBitRate(16000);
            break;
        case QtMultimediaKit::NormalQuality:
            m_audioSettings.setBitRate(32000);
            break;
        case QtMultimediaKit::HighQuality:
            m_audioSettings.setBitRate(64000);
            break;
        case QtMultimediaKit::VeryHighQuality:
            m_audioSettings.setBitRate(64000);
            break;
        default:
            m_audioSettings.setBitRate(64000);
            break;
        }
    }

    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::setAudioChannelCount(const int channelCount)
{
    if (channelCount != -1)
        m_audioSettings.setChannelCount(channelCount);

    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::setVideoCodec(const QString &codecName)
{
    if (codecName == m_videoSettings.codec())
        return;

    if (codecName.isEmpty())
        m_videoSettings.setCodec(KMimeTypeDefaultVideoCodec); // Use default
    else
        m_videoSettings.setCodec(codecName);

    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::setAudioCodec(const QString &codecName)
{
    if (codecName == m_audioSettings.codec())
        return;

    if (codecName.isEmpty()) {
        m_audioSettings.setCodec(KMimeTypeDefaultAudioCodec); // Use default
    } else {
        // If information of supported codecs is already available check that
        // given codec is supported
        if (m_session->m_captureState >= S60VideoCaptureSession::EOpenComplete) {
            if (m_audioCodecList.contains(codecName)) {
                m_audioSettings.setCodec(codecName);
                m_uncommittedSettings = true;
            } else {
                m_session->setError(KErrNotSupported, tr("Requested audio codec is not supported"));
            }
        } else {
            m_audioSettings.setCodec(codecName);
            m_uncommittedSettings = true;
        }
    }
}

QString S60VideoCaptureSettings::videoCodecDescription(const QString &codecName)
{
    QString codecDescription;
    if (codecName.contains("video/H263-2000", Qt::CaseInsensitive))
        codecDescription.append("H.263 Video Codec");
    else if (codecName.contains("video/mp4v-es", Qt::CaseInsensitive))
        codecDescription.append("MPEG-4 Part 2 Video Codec");
    else if (codecName.contains("video/H264", Qt::CaseInsensitive))
        codecDescription.append("H.264 AVC (MPEG-4 Part 10) Video Codec");
    else
        codecDescription.append("Video Codec");

    return codecDescription;
}

void S60VideoCaptureSettings::doSetCodecsL()
{
    // Determine Profile and Level for the video codec if needed
    QString setVideoCodec(m_videoSettings.codec());
    setVideoCodec = setVideoCodec.toLower();
    if (setVideoCodec == QLatin1String("video/h264") ||
        setVideoCodec == QLatin1String("video/mp4v-es") ||
        setVideoCodec == QLatin1String("video/h263-2000") ||
        setVideoCodec == QLatin1String("video/h263-2000; profile=0")) {
        m_videoSettings.setCodec(determineProfileAndLevel());
        setVideoCodec = m_videoSettings.codec();
    }

    if (m_videoRecorder) {
        // CVideoReorderUtility is CaseSensitive, make sure right format is used
        QStringList foundList = m_videoCodecList.filter(setVideoCodec, Qt::CaseInsensitive);
        if (foundList.count() > 0)
            setVideoCodec = foundList.first();
        else
            User::Leave(KErrNotSupported);

        TPtrC16 str(reinterpret_cast<const TUint16*>(setVideoCodec.utf16()));
        HBufC8* videoCodec(0);
        videoCodec = CnvUtfConverter::ConvertFromUnicodeToUtf8L(str);
        CleanupStack::PushL(videoCodec);

        TFourCC audioCodec = m_audioCodecList[m_audioSettings.codec()];

        TInt vErr = KErrNone;
        TInt aErr = KErrNone;
        TRAP(vErr, m_videoRecorder->SetVideoTypeL(*videoCodec));
        TRAP(aErr, m_videoRecorder->SetAudioTypeL(audioCodec));

        User::LeaveIfError(vErr);
        User::LeaveIfError(aErr);

        CleanupStack::PopAndDestroy(videoCodec);
    } else {
        m_session->setError(KErrNotReady, tr("Unexpected camera error."));
    }
}

QString S60VideoCaptureSettings::determineProfileAndLevel()
{
    QString determinedMimeType = m_videoSettings.codec();

    // H.263
    if (determinedMimeType.contains(QLatin1String("video/H263-2000"), Qt::CaseInsensitive)) {
        if (!determinedMimeType.contains(QLatin1String("profile=0"), Qt::CaseInsensitive))
            determinedMimeType.append(QLatin1String("; profile=0"));
        if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (176*144)) {
            if (m_videoSettings.frameRate() > 15.0)
                determinedMimeType.append(QLatin1String("; level=20"));
            else
                determinedMimeType.append(QLatin1String("; level=40"));
        } else {
            if (m_videoSettings.bitRate() > 64000)
                determinedMimeType.append(QLatin1String("; level=45"));
            else
                determinedMimeType.append(QLatin1String("; level=10"));
        }

    // MPEG-4
    } else if (determinedMimeType.contains(QLatin1String("video/mp4v-es"), Qt::CaseInsensitive)) {
        if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (720*480)) {
            determinedMimeType.append(QLatin1String("; profile-level-id=6"));
        } else if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (640*480)) {
            determinedMimeType.append(QLatin1String("; profile-level-id=5"));
        } else if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (352*288)) {
            determinedMimeType.append(QLatin1String("; profile-level-id=4"));
        } else if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (176*144)) {
            if (m_videoSettings.frameRate() > 15.0)
                determinedMimeType.append(QLatin1String("; profile-level-id=3"));
            else
                determinedMimeType.append(QLatin1String("; profile-level-id=2"));
        } else {
            if (m_videoSettings.bitRate() > 64000)
                determinedMimeType.append(QLatin1String("; profile-level-id=9"));
            else
                determinedMimeType.append(QLatin1String("; profile-level-id=1"));
        }

    // H.264
    } else if (determinedMimeType.contains(QLatin1String("video/H264"), Qt::CaseInsensitive)) {
        if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (640*480)) {
            determinedMimeType.append(QLatin1String("; profile-level-id=42801F"));
        } else if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (352*288)) {
            determinedMimeType.append(QLatin1String("; profile-level-id=42801E"));
        } else if ((m_videoSettings.resolution().width() * m_videoSettings.resolution().height()) > (176*144)) {
            if (m_videoSettings.frameRate() > 15.0)
                determinedMimeType.append(QLatin1String("; profile-level-id=428015"));
            else
                determinedMimeType.append(QLatin1String("; profile-level-id=42800C"));
        } else {
            determinedMimeType.append(QLatin1String("; profile-level-id=42900B"));
        }
    }

    return determinedMimeType;
}

void S60VideoCaptureSettings::setVideoBitrate(const int bitrate)
{
    if (bitrate != -1) {
        m_videoSettings.setBitRate(bitrate);
    } else {
        // Use one of pre-defined bitrates based on quality requested
        switch (m_videoSettings.quality()) {
        case QtMultimediaKit::VeryLowQuality:
            m_videoSettings.setBitRate(64000);
            break;
        case QtMultimediaKit::LowQuality:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setBitRate(128000);
#else // S60 5.0 or older
            m_videoSettings.setBitRate(64000);
#endif // SYMBIAN_3_PLATFORM
            break;
        case QtMultimediaKit::NormalQuality:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setBitRate(2000000);
#else // S60 5.0 or older
            m_videoSettings.setBitRate(128000);
#endif // SYMBIAN_3_PLATFORM
            break;
        case QtMultimediaKit::HighQuality:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setBitRate(4000000);
#else // S60 5.0 or older
            m_videoSettings.setBitRate(384000);
#endif // SYMBIAN_3_PLATFORM
            break;
        case QtMultimediaKit::VeryHighQuality:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setBitRate(14000000);
#else // S60 5.0 or older
            m_videoSettings.setBitRate(2000000);
#endif // SYMBIAN_3_PLATFORM
            break;
        default:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setBitRate(2000000);
#else // S60 5.0 or older
            m_videoSettings.setBitRate(128000);
#endif // SYMBIAN_3_PLATFORM
            break;
        }
    }

    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::doSetBitrate(const int &bitrate)
{
    if (bitrate != -1) {
        if (m_videoRecorder) {
            TRAPD(err, m_videoRecorder->SetVideoBitRateL(bitrate));
            if (err) {
                if (err == KErrNotSupported || err == KErrArgument) {
                    m_session->setError(KErrNotSupported, tr("Requested video bitrate is not supported."));
                    m_videoSettings.setBitRate(64000); // Reset
                } else {
                    m_session->setError(err, tr("Failed to set video bitrate."));
                }
            }
        } else {
            m_session->setError(KErrNotReady, tr("Unexpected camera error."));
        }
    }
}

void S60VideoCaptureSettings::setVideoResolution(const QSize &resolution)
{
    if (!resolution.isEmpty()) {
        m_videoSettings.setResolution(resolution);
    } else {
        QList<QSize> supportedResolutions = supportedVideoResolutions(m_videoSettings, 0);
        switch (m_videoSettings.quality()) {
        case QtMultimediaKit::VeryLowQuality:
            m_videoSettings.setResolution(supportedResolutions.last());
            break;
        case QtMultimediaKit::LowQuality:
            if (supportedResolutions.count() > 3)
                m_videoSettings.setResolution(supportedResolutions.at(supportedResolutions.count() - 2));
            else
                m_videoSettings.setResolution(supportedResolutions.last());
            break;
        case QtMultimediaKit::NormalQuality:
            m_videoSettings.setResolution(supportedResolutions.at(supportedResolutions.count() / 2));
            break;
        case QtMultimediaKit::HighQuality:
            if (supportedResolutions.count() > 3)
                m_videoSettings.setResolution(supportedResolutions.at(1));
            else
                m_videoSettings.setResolution(supportedResolutions.first());
            break;
        case QtMultimediaKit::VeryHighQuality:
            m_videoSettings.setResolution(supportedResolutions.first());
            break;
        default:
            m_videoSettings.setResolution(supportedResolutions.first());
            break;
        }
    }

    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::doSetVideoResolution(const QSize &resolution)
{
    TSize size((TInt)resolution.width(), (TInt)resolution.height());

    // Make sure resolution is not too big if main camera is not used
    if (m_cameraEngine->CurrentCameraIndex() != 0) {
        TCameraInfo *info = m_cameraEngine->CameraInfo();
        if (info) {
            TInt videoResolutionCount = info->iNumVideoFrameSizesSupported;
            TSize maxCameraVideoResolution = TSize(0,0);
            CCamera *camera = m_cameraEngine->Camera();
            if (camera) {
                for (TInt i = 0; i < videoResolutionCount; ++i) {
                    TSize checkedResolution;
                    // Use YUV video max frame size in the check (Through
                    // CVideoRecorderUtility/DevVideoRecord it is possible to
                    // query only encoder maximums)
                    camera->EnumerateVideoFrameSizes(checkedResolution, i, CCamera::EFormatYUV420Planar);
                    if ((checkedResolution.iWidth * checkedResolution.iHeight) >
                        (maxCameraVideoResolution.iWidth * maxCameraVideoResolution.iHeight))
                        maxCameraVideoResolution = checkedResolution;
                }
                if ((maxCameraVideoResolution.iWidth * maxCameraVideoResolution.iHeight) <
                    (size.iWidth * size.iHeight)) {
                    size = maxCameraVideoResolution;
                    m_session->setError(KErrNotSupported, tr("Requested resolution is not supported for this camera."));
                }
            } else {
                m_session->setError(KErrGeneral, tr("Could not query supported video resolutions."));
            }
        } else {
            m_session->setError(KErrGeneral, tr("Could not query supported video resolutions."));
        }
    }

    if (resolution.width() != -1 && resolution.height() != -1) {
        if (m_videoRecorder) {
            TRAPD(err, m_videoRecorder->SetVideoFrameSizeL((TSize)size));
            if (err == KErrNotSupported || err == KErrArgument) {
                m_session->setError(KErrNotSupported, tr("Requested video resolution is not supported."));
                TSize fallBack(640,480);
                TRAPD(err, m_videoRecorder->SetVideoFrameSizeL(fallBack));
                if (err == KErrNone) {
                    m_videoSettings.setResolution(QSize(fallBack.iWidth,fallBack.iHeight));
                } else {
                    fallBack = TSize(176,144);
                    TRAPD(err, m_videoRecorder->SetVideoFrameSizeL(fallBack));
                    if (err == KErrNone)
                        m_videoSettings.setResolution(QSize(fallBack.iWidth,fallBack.iHeight));
                }
            } else {
                m_session->setError(err, tr("Failed to set video resolution."));
            }
        } else {
            m_session->setError(KErrNotReady, tr("Unexpected camera error."));
        }
    }
}

void S60VideoCaptureSettings::setVideoFrameRate(qreal rate)
{
    if (!qFuzzyCompare(rate, qreal(0.0))) {
        m_videoSettings.setFrameRate(rate);
    } else {
        // Use one of pre-defined framerates based on the quality requested
        switch (m_videoSettings.quality()) {
        case QtMultimediaKit::VeryLowQuality:
            m_videoSettings.setFrameRate(qreal(10.0));
            break;
        case QtMultimediaKit::LowQuality:
            m_videoSettings.setFrameRate(qreal(10.0));
            break;
        case QtMultimediaKit::NormalQuality:
            m_videoSettings.setFrameRate(qreal(15.0));
            break;
        case QtMultimediaKit::HighQuality:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setFrameRate(qreal(30.0));
#else // S60 5.0 or older
            m_videoSettings.setFrameRate(qreal(15.0));
#endif // SYMBIAN_3_PLATFORM
            break;
        case QtMultimediaKit::VeryHighQuality:
#ifdef SYMBIAN_3_PLATFORM
            m_videoSettings.setFrameRate(qreal(30.0));
#else // S60 5.0 or older
            m_videoSettings.setFrameRate(qreal(15.0));
#endif // SYMBIAN_3_PLATFORM
            break;
        default:
            m_videoSettings.setFrameRate(qreal(15.0));
            break;
        }
    }

    m_uncommittedSettings = true;
}

void S60VideoCaptureSettings::doSetFrameRate(qreal rate)
{
    if (rate != 0) {
        if (m_videoRecorder) {
            bool continuous = false;
            QList<qreal> list = supportedVideoFrameRates(&continuous);
            qreal maxRate = 0.0;
            foreach (qreal fRate, list)
                if (fRate > maxRate)
                    maxRate = fRate;
            if (maxRate >= rate && rate > 0) {
                TRAPD(err, m_videoRecorder->SetVideoFrameRateL((TReal32)rate));
                if (err == KErrNotSupported) {
                    m_session->setError(KErrNotSupported, tr("Requested framerate is not supported."));
                    TReal32 fallBack = 15.0;
                    TRAPD(err, m_videoRecorder->SetVideoFrameRateL(fallBack));
                    if (err == KErrNone)
                        m_videoSettings.setFrameRate((qreal)fallBack);
                } else {
                    if (err == KErrArgument) {
                        m_session->setError(KErrNotSupported, tr("Requested framerate is not supported."));
                        m_videoSettings.setFrameRate(15.0); // Reset
                    } else {
                        m_session->setError(err, tr("Failed to set video framerate."));
                    }
                }
            } else {
                m_session->setError(KErrNotSupported, tr("Requested framerate is not supported."));
                m_videoSettings.setFrameRate(15.0); // Reset
            }
        } else {
            m_session->setError(KErrNotReady, tr("Unexpected camera error."));
        }
    }
}

void S60VideoCaptureSettings::setVideoEncodingMode(const QtMultimediaKit::EncodingMode mode)
{
    // This has no effect as it has no support in Symbian

    if (mode == QtMultimediaKit::ConstantQualityEncoding) {
        m_videoSettings.setEncodingMode(mode);
        return;
    }
    m_session->setError(KErrNotSupported, tr("Requested video encoding mode is not supported"));
}

void S60VideoCaptureSettings::setAudioEncodingMode(const QtMultimediaKit::EncodingMode mode)
{
    // This has no effect as it has no support in Symbian

    if (mode == QtMultimediaKit::ConstantQualityEncoding) {
        m_audioSettings.setEncodingMode(mode);
        return;
    }
    m_session->setError(KErrNotSupported, tr("Requested audio encoding mode is not supported"));
}

void S60VideoCaptureSettings::initializeVideoCaptureSettings()
{
    // Check if user has already requested some settings
    if (m_captureSettingsSet)
        return;

    QSize resolution(-1, -1);
    qreal frameRate(0);
    int bitRate(-1);

    if (m_cameraEngine) {

        if (m_videoRecorder && m_session->m_captureState >= S60VideoCaptureSession::EInitialized) {

            // Resolution
            QList<QSize> resos = supportedVideoResolutions(0);
            foreach (QSize reso, resos) {
                if ((reso.width() * reso.height()) > (resolution.width() * resolution.height()))
                    resolution = reso;
            }

            // Needed to query supported framerates for this codec/resolution pair
            m_videoSettings.setCodec(KMimeTypeDefaultVideoCodec);
            m_videoSettings.setResolution(resolution);

            // FrameRate
            QList<qreal> fRates = supportedVideoFrameRates(m_videoSettings, 0);
            foreach (qreal rate, fRates) {
                if (rate > frameRate)
                    frameRate = rate;
            }

            // BitRate
#ifdef SYMBIAN_3_PLATFORM
            if (m_cameraEngine->CurrentCameraIndex() == 0)
                bitRate = KBiR_H264_PLID_42801F;    // 14Mbps
            else
                bitRate = KBiR_H264_PLID_428016;    // 4Mbps
#else // Other platforms
            if (m_cameraEngine->CurrentCameraIndex() == 0)
                bitRate = KBiR_MPEG4_PLID_4;        // 2/4Mbps
            else
                bitRate = KBiR_MPEG4_PLID_3;        // 384kbps
#endif // SYMBIAN_3_PLATFORM

        } else {
#ifdef SYMBIAN_3_PLATFORM
            if (m_cameraEngine->CurrentCameraIndex() == 0) {
                // Primary camera
                resolution = KResH264_PLID_42801F;  // 1280x720
                frameRate = KFrR_H264_PLID_42801F;  // 30fps
                bitRate = KBiR_H264_PLID_42801F;    // 14Mbps
            } else {
                // Other cameras
                resolution = KResH264_PLID_42801E;  // 640x480
                frameRate = KFrR_H264_PLID_428014;  // 30fps
                bitRate = KBiR_H264_PLID_428016;    // 4Mbps
            }
#else // Other platforms
            if (m_cameraEngine->CurrentCameraIndex() == 0) {
                // Primary camera
                resolution = KResMPEG4_PLID_4;      // 640x480
                frameRate = KFrR_MPEG4_PLID_4;      // 15/30fps
                bitRate = KBiR_MPEG4_PLID_4;        // 2/4Mbps
            } else {
                // Other cameras
                resolution = KResMPEG4_PLID_3;      // 352x288
                frameRate = KFrR_MPEG4;             // 15fps
                bitRate = KBiR_MPEG4_PLID_3;        // 384kbps
            }
#endif // SYMBIAN_3_PLATFORM
        }
    } else {
#ifdef SYMBIAN_3_PLATFORM
        resolution = KResH264_PLID_42801F;
        frameRate = KFrR_H264_PLID_42801F;
        bitRate = KBiR_H264_PLID_42801F;
#else // Pre-Symbian3 Platforms
        resolution = KResMPEG4_PLID_4;
        frameRate = KFrR_MPEG4_PLID_4;
        bitRate = KBiR_MPEG4_PLID_4;
#endif // SYMBIAN_3_PLATFORM
    }

    // Set specified settings (Resolution, FrameRate and BitRate)
    m_videoSettings.setResolution(resolution);
    m_videoSettings.setFrameRate(frameRate);
    m_videoSettings.setBitRate(bitRate);

    // Video Settings: Codec, EncodingMode and Quality
    m_videoSettings.setCodec(KMimeTypeDefaultVideoCodec);
    m_videoSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    m_videoSettings.setQuality(QtMultimediaKit::VeryHighQuality);

    // Audio Settings
    m_audioSettings.setCodec(KMimeTypeDefaultAudioCodec);
    m_audioSettings.setBitRate(KDefaultBitRate);
    m_audioSettings.setSampleRate(KDefaultSampleRate);
    m_audioSettings.setChannelCount(KDefaultChannelCount);
    m_audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    m_audioSettings.setQuality(QtMultimediaKit::VeryHighQuality);
}

QSize S60VideoCaptureSettings::pixelAspectRatio()
{
#ifndef S60_31_PLATFORM
    TVideoAspectRatio par;
    TRAPD(err, m_videoRecorder->GetPixelAspectRatioL(par));
    if (err)
        m_session->setError(err, tr("Failed to query current pixel aspect ratio."));
    return QSize(par.iNumerator, par.iDenominator);
#else // S60_31_PLATFORM
    return QSize();
#endif // !S60_31_PLATFORM
}

void S60VideoCaptureSettings::setPixelAspectRatio(const QSize par)
{
#ifndef S60_31_PLATFORM

    const TVideoAspectRatio videoPar(par.width(), par.height());
    TRAPD(err, m_videoRecorder->SetPixelAspectRatioL(videoPar));
    if (err)
        m_session->setError(err, tr("Failed to set pixel aspect ratio."));
#else // S60_31_PLATFORM
    Q_UNUSED(par);
#endif // !S60_31_PLATFORM

    m_uncommittedSettings = true;
}

int S60VideoCaptureSettings::audioGain()
{
    TInt gain = 0;
    TRAPD(err, gain = m_videoRecorder->GainL());
    if (err)
        m_session->setError(err, tr("Failed to query audio gain."));
    return (int)gain;
}

void S60VideoCaptureSettings::setAudioGain(const int gain)
{
    TRAPD(err, m_videoRecorder->SetGainL(gain));
    if (err)
        m_session->setError(err, tr("Failed to set audio gain."));

    m_uncommittedSettings = true;
}

int S60VideoCaptureSettings::maxClipSizeInBytes() const
{
    return m_maxClipSize;
}

void S60VideoCaptureSettings::setMaxClipSizeInBytes(const int size)
{
    TRAPD(err, m_videoRecorder->SetMaxClipSizeL(size));
    if (err) {
        m_session->setError(err, tr("Failed to set maximum video size."));
    } else
        m_maxClipSize = size;

    m_uncommittedSettings = true;
}

#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
void S60VideoCaptureSettings::MdvroReturnPicture(TVideoPicture *aPicture)
{
    // Not used
    Q_UNUSED(aPicture);
}

void S60VideoCaptureSettings::MdvroSupplementalInfoSent()
{
    // Not used
}

void S60VideoCaptureSettings::MdvroNewBuffers()
{
    // Not used
}

void S60VideoCaptureSettings::MdvroFatalError(TInt aError)
{
    m_session->setError(aError, tr("Unexpected camera error."));
}

void S60VideoCaptureSettings::MdvroInitializeComplete(TInt aError)
{
    // Not used
    Q_UNUSED(aError);
}

void S60VideoCaptureSettings::MdvroStreamEnd()
{
    // Not used
}

/*
 * This populates video codec information (supported codecs, resolutions,
 * framerates, etc.) using DevVideoRecord API.
 */
void S60VideoCaptureSettings::doPopulateVideoCodecsDataL()
{
    RArray<TUid> encoders;
    CleanupClosePushL(encoders);

    CMMFDevVideoRecord *mDevVideoRecord = CMMFDevVideoRecord::NewL(*this);
    CleanupStack::PushL(mDevVideoRecord);

    // Retrieve list of all encoders provided by the platform
    mDevVideoRecord->GetEncoderListL(encoders);

    for (int i = 0; i < encoders.Count(); ++i ) {
        CVideoEncoderInfo *encoderInfo = mDevVideoRecord->VideoEncoderInfoLC(encoders[i]);

        // Discard encoders that are not HW accelerated and do not support direct capture
        if (encoderInfo->Accelerated() == false || encoderInfo->SupportsDirectCapture() == false) {
            CleanupStack::Check(encoderInfo);
            CleanupStack::PopAndDestroy(encoderInfo);
            continue;
        }

        m_videoParametersForEncoder.append(MaxResolutionRatesAndTypes());
        int newIndex = m_videoParametersForEncoder.count() - 1;

        m_videoParametersForEncoder[newIndex].bitRate = (int)encoderInfo->MaxBitrate();

        // Get supported MIME Types
        const RPointerArray<CCompressedVideoFormat> &videoFormats = encoderInfo->SupportedOutputFormats();
        for(int x = 0; x < videoFormats.Count(); ++x) {
            QString codecMimeType =
                QString::fromUtf8((char*)videoFormats[x]->MimeType().Ptr(),
                                  videoFormats[x]->MimeType().Length());
            m_videoParametersForEncoder[newIndex].mimeTypes << codecMimeType;
        }

        // Get supported maximum Resolution/Framerate pairs
        const RArray<TPictureRateAndSize> &ratesAndSizes = encoderInfo->MaxPictureRates();
        SupportedFrameRatePictureSize data;
        for(int j = 0; j < ratesAndSizes.Count(); ++j) {
            data.frameRate = ratesAndSizes[j].iPictureRate;
            data.frameSize = QSize(ratesAndSizes[j].iPictureSize.iWidth, ratesAndSizes[j].iPictureSize.iHeight);

            // Save data to the hash
            m_videoParametersForEncoder[newIndex].frameRatePictureSizePair.append(data);
        }

        CleanupStack::Check(encoderInfo);
        CleanupStack::PopAndDestroy(encoderInfo);
    }

    CleanupStack::Check(mDevVideoRecord);
    CleanupStack::PopAndDestroy(mDevVideoRecord);
    CleanupStack::PopAndDestroy(); // RArray<TUid> encoders
}
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

QStringList S60VideoCaptureSettings::supportedVideoContainers()
{
    QStringList containers;
    QList<TInt> controllers = m_videoControllerMap.keys();
    for (int i = 0; i < controllers.count(); ++i)
        foreach (VideoFormatData formatData, m_videoControllerMap[controllers[i]])
            for (int j = 0; j < formatData.supportedMimeTypes.count(); ++j)
                if (containers.contains(formatData.supportedMimeTypes[j], Qt::CaseInsensitive) == false)
                    containers.append(formatData.supportedMimeTypes[j]);

    return containers;
}

bool S60VideoCaptureSettings::isSupportedVideoContainer(const QString &containerName)
{
    return supportedVideoContainers().contains(containerName, Qt::CaseInsensitive);
}

QString S60VideoCaptureSettings::videoContainer() const
{
    return m_container;
}

void S60VideoCaptureSettings::setVideoContainer(const QString &containerName)
{
    if (containerName.compare(m_requestedContainer, Qt::CaseInsensitive) == 0)
        return;

    if (containerName.isEmpty()) {
        m_requestedContainer = KMimeTypeDefaultContainer; // Use default
    } else {
        if (supportedVideoContainers().contains(containerName, Qt::CaseInsensitive)) {
            m_requestedContainer = containerName.toLower();
        } else {
            m_session->setError(KErrNotSupported, tr("Requested video container is not supported."));
            m_requestedContainer = KMimeTypeDefaultContainer; // Reset to default
        }
    }

    m_uncommittedSettings = true;
}

QString S60VideoCaptureSettings::videoContainerDescription(const QString &containerName)
{
    QList<TInt> formats;
    QList<TInt> encoders = m_videoControllerMap.keys();
    for (int i = 0; i < encoders.count(); ++i) {
        formats = m_videoControllerMap[encoders[i]].keys();
        for (int j = 0; j < formats.count(); ++j)
            if (m_videoControllerMap[encoders[i]][formats[j]].supportedMimeTypes.contains(containerName, Qt::CaseInsensitive))
                return m_videoControllerMap[encoders[i]][formats[j]].description;
    }

    return QString();
}

void S60VideoCaptureSettings::doPopulateAudioCodecsL()
{
    if (m_session->m_captureState == S60VideoCaptureSession::EInitializing) {
        m_audioCodecList.clear();

        RArray<TFourCC> audioTypes;
        CleanupClosePushL(audioTypes);

        if (m_videoRecorder)
            m_videoRecorder->GetSupportedAudioTypesL(audioTypes);
        else
            m_session->setError(KErrNotReady, tr("Unexpected camera error."));

        for (TInt i = 0; i < audioTypes.Count(); i++) {
            TUint32 codec = audioTypes[i].FourCC();

            if (codec == KMMFFourCCCodeAMR)
                m_audioCodecList.insert(QString("audio/amr"), KMMFFourCCCodeAMR);
            if (codec == KMMFFourCCCodeAAC)
                m_audioCodecList.insert(QString("audio/aac"), KMMFFourCCCodeAAC);
        }
        CleanupStack::PopAndDestroy(&audioTypes);
    }
}

void S60VideoCaptureSettings::doPopulateVideoCodecsL()
{
    if (m_session->m_captureState == S60VideoCaptureSession::EInitializing) {
        m_videoCodecList.clear();

        CDesC8ArrayFlat* videoTypes = new (ELeave) CDesC8ArrayFlat(10);
        CleanupStack::PushL(videoTypes);

        if (m_videoRecorder)
            m_videoRecorder->GetSupportedVideoTypesL(*videoTypes);
        else
            m_session->setError(KErrNotReady, tr("Unexpected camera error."));

        for (TInt i = 0; i < videoTypes->Count(); i++) {
            TPtrC8 videoType = videoTypes->MdcaPoint(i);
            QString codecMimeType = QString::fromUtf8((char *)videoType.Ptr(), videoType.Length());
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
            for (int j = 0; j < m_videoParametersForEncoder.size(); ++j) {
                if (m_videoParametersForEncoder[j].mimeTypes.contains(codecMimeType, Qt::CaseInsensitive)) {
                    m_videoCodecList << codecMimeType;
                    break;
                }
            }
#else // CVideoRecorderUtility
            m_videoCodecList << codecMimeType;
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
        }
        CleanupStack::PopAndDestroy(videoTypes);
    }
}

#ifndef S60_DEVVIDEO_RECORDING_SUPPORTED
/*
 * Maximum resolution, framerate and bitrate can not be queried via MMF or
 * ECam, but needs to be set according to the definitions of the video
 * standard in question. In video standards, the values often depend on each
 * other, but the below function defines constant maximums.
 */
void S60VideoCaptureSettings::doPopulateMaxVideoParameters()
{
    m_videoParametersForEncoder.append(MaxResolutionRatesAndTypes()); // For H.263
    m_videoParametersForEncoder.append(MaxResolutionRatesAndTypes()); // For MPEG-4
    m_videoParametersForEncoder.append(MaxResolutionRatesAndTypes()); // For H.264

    for (int i = 0; i < m_videoCodecList.count(); ++i) {

        // Use all lower case for comparisons
        QString codec = m_videoCodecList[i].toLower();

        if (codec.contains("video/h263-2000", Qt::CaseInsensitive)) {
            // H.263
            if (codec == "video/h263-2000" ||
                codec == "video/h263-2000; profile=0" ||
                codec == "video/h263-2000; profile=0; level=10" ||
                codec == "video/h263-2000; profile=3") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(176,144)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 64000)
                    m_videoParametersForEncoder[0].bitRate = 64000;
                continue;
            } else if (codec == "video/h263-2000; profile=0; level=20") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 128000)
                    m_videoParametersForEncoder[0].bitRate = 128000;
                continue;
            } else if (codec == "video/h263-2000; profile=0; level=30") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 384000)
                    m_videoParametersForEncoder[0].bitRate = 384000;
                continue;
            } else if (codec == "video/h263-2000; profile=0; level=40") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 2048000)
                    m_videoParametersForEncoder[0].bitRate = 2048000;
                continue;
            } else if (codec == "video/h263-2000; profile=0; level=45") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(176,144)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 128000)
                    m_videoParametersForEncoder[0].bitRate = 128000;
                continue;
            } else if (codec == "video/h263-2000; profile=0; level=50") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 4096000)
                    m_videoParametersForEncoder[0].bitRate = 4096000;
                continue;
            }

        } else if (codec.contains("video/mp4v-es", Qt::CaseInsensitive)) {
            // Mpeg-4
            if (codec == "video/mp4v-es" ||
                codec == "video/mp4v-es; profile-level-id=1" ||
                codec == "video/mp4v-es; profile-level-id=8") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(176,144)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 64000)
                    m_videoParametersForEncoder[0].bitRate = 64000;
                continue;
            } else if (codec == "video/mp4v-es; profile-level-id=2" ||
                       codec == "video/mp4v-es; profile-level-id=9") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 128000)
                    m_videoParametersForEncoder[0].bitRate = 128000;
                continue;
            } else if (codec == "video/mp4v-es; profile-level-id=3") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 384000)
                    m_videoParametersForEncoder[0].bitRate = 384000;
                continue;
            } else if (codec == "video/mp4v-es; profile-level-id=4") {
#if (defined(S60_31_PLATFORM) | defined(S60_32_PLATFORM))
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(640,480)));
#else // S60 5.0 and later platforms
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(640,480)));
#endif
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 4000000)
                    m_videoParametersForEncoder[0].bitRate = 4000000;
                continue;
            } else if (codec == "video/mp4v-es; profile-level-id=5") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(25.0, QSize(720,576)));
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(720,480)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 8000000)
                    m_videoParametersForEncoder[0].bitRate = 8000000;
                continue;
            } else if (codec == "video/mp4v-es; profile-level-id=6") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(1280,720)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 12000000)
                    m_videoParametersForEncoder[0].bitRate = 12000000;
                continue;
            }

        } else if (codec.contains("video/h264", Qt::CaseInsensitive)) {
            // H.264
            if (codec == "video/h264" ||
                codec == "video/h264; profile-level-id=42800a") {
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(176,144)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 64000)
                    m_videoParametersForEncoder[0].bitRate = 64000;
                continue;
            } else if (codec == "video/h264; profile-level-id=42900b") { // BP, L1b
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(176,144)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 128000)
                    m_videoParametersForEncoder[0].bitRate = 128000;
                continue;
            } else if (codec == "video/h264; profile-level-id=42800b") { // BP, L1.1
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(7.5, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 192000)
                    m_videoParametersForEncoder[0].bitRate = 192000;
                continue;
            } else if (codec == "video/h264; profile-level-id=42800c") { // BP, L1.2
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(15.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 384000)
                    m_videoParametersForEncoder[0].bitRate = 384000;
                continue;
            } else if (codec == "video/h264; profile-level-id=42800d") { // BP, L1.3
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 768000)
                    m_videoParametersForEncoder[0].bitRate = 768000;
                continue;
            } else if (codec == "video/h264; profile-level-id=428014") { // BP, L2
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 2000000)
                    m_videoParametersForEncoder[0].bitRate = 2000000;
                continue;
            } else if (codec == "video/h264; profile-level-id=428015") { // BP, L2.1
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(50.0, QSize(352,288)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 4000000)
                    m_videoParametersForEncoder[0].bitRate = 4000000;
                continue;
            } else if (codec == "video/h264; profile-level-id=428016") { // BP, L2.2
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(16.9, QSize(640,480)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 4000000)
                    m_videoParametersForEncoder[0].bitRate = 4000000;
                continue;
            } else if (codec == "video/h264; profile-level-id=42801e") { // BP, L3
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(33.8, QSize(640,480)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 10000000)
                    m_videoParametersForEncoder[0].bitRate = 10000000;
                continue;
            } else if (codec == "video/h264; profile-level-id=42801f") { // BP, L3.1
                m_videoParametersForEncoder[0].frameRatePictureSizePair.append(SupportedFrameRatePictureSize(30.0, QSize(1280,720)));
                m_videoParametersForEncoder[0].mimeTypes.append(codec);
                if (m_videoParametersForEncoder[0].bitRate < 14000000)
                    m_videoParametersForEncoder[0].bitRate = 14000000;
                continue;
            }
        }
    }
}
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED

/*
 * This function returns the maximum resolution defined by the video standards
 * for different MIME Types.
 */
QSize S60VideoCaptureSettings::maximumResolutionForMimeType(const QString &mimeType) const
{
    QSize maxSize(-1,-1);
    // Use all lower case for comparisons
    QString lowerMimeType = mimeType.toLower();

    if (lowerMimeType == "video/h263-2000") {
        maxSize = KResH263;
    } else if (lowerMimeType == "video/h263-2000; profile=0") {
        maxSize = KResH263_Profile0;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=10") {
        maxSize = KResH263_Profile0_Level10;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=20") {
        maxSize = KResH263_Profile0_Level20;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=30") {
        maxSize = KResH263_Profile0_Level30;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=40") {
        maxSize = KResH263_Profile0_Level40;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=45") {
        maxSize = KResH263_Profile0_Level45;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=50") {
        maxSize = KResH263_Profile0_Level50;
    } else if (lowerMimeType == "video/h263-2000; profile=3") {
        maxSize = KResH263_Profile3;
    } else if (lowerMimeType == "video/mp4v-es") {
        maxSize = KResMPEG4;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=1") {
        maxSize = KResMPEG4_PLID_1;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=2") {
        maxSize = KResMPEG4_PLID_2;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=3") {
        maxSize = KResMPEG4_PLID_3;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=4") {
        maxSize = KResMPEG4_PLID_4;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=5") {
        maxSize = KResMPEG4_PLID_5;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=6") {
        maxSize = KResMPEG4_PLID_6;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=8") {
        maxSize = KResMPEG4_PLID_8;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=9") {
        maxSize = KResMPEG4_PLID_9;
    } else if (lowerMimeType == "video/h264") {
        maxSize = KResH264;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800a" ||
               lowerMimeType == "video/h264; profile-level-id=4d400a" ||
               lowerMimeType == "video/h264; profile-level-id=64400a") { // L1
        maxSize = KResH264_PLID_42800A;
    } else if (lowerMimeType == "video/h264; profile-level-id=42900b" ||
               lowerMimeType == "video/h264; profile-level-id=4d500b" ||
               lowerMimeType == "video/h264; profile-level-id=644009") { // L1.b
        maxSize = KResH264_PLID_42900B;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800b" ||
               lowerMimeType == "video/h264; profile-level-id=4d400b" ||
               lowerMimeType == "video/h264; profile-level-id=64400b") { // L1.1
        maxSize = KResH264_PLID_42800B;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800c" ||
               lowerMimeType == "video/h264; profile-level-id=4d400c" ||
               lowerMimeType == "video/h264; profile-level-id=64400c") { // L1.2
        maxSize = KResH264_PLID_42800C;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800d" ||
               lowerMimeType == "video/h264; profile-level-id=4d400d" ||
               lowerMimeType == "video/h264; profile-level-id=64400d") { // L1.3
        maxSize = KResH264_PLID_42800D;
    } else if (lowerMimeType == "video/h264; profile-level-id=428014" ||
               lowerMimeType == "video/h264; profile-level-id=4d4014" ||
               lowerMimeType == "video/h264; profile-level-id=644014") { // L2
        maxSize = KResH264_PLID_428014;
    } else if (lowerMimeType == "video/h264; profile-level-id=428015" ||
               lowerMimeType == "video/h264; profile-level-id=4d4015" ||
               lowerMimeType == "video/h264; profile-level-id=644015") { // L2.1
        maxSize = KResH264_PLID_428015;
    } else if (lowerMimeType == "video/h264; profile-level-id=428016" ||
               lowerMimeType == "video/h264; profile-level-id=4d4016" ||
               lowerMimeType == "video/h264; profile-level-id=644016") { // L2.2
        maxSize = KResH264_PLID_428016;
    } else if (lowerMimeType == "video/h264; profile-level-id=42801e" ||
               lowerMimeType == "video/h264; profile-level-id=4d401e" ||
               lowerMimeType == "video/h264; profile-level-id=64401e") { // L3
        maxSize = KResH264_PLID_42801E;
    } else if (lowerMimeType == "video/h264; profile-level-id=42801f" ||
               lowerMimeType == "video/h264; profile-level-id=4d401f" ||
               lowerMimeType == "video/h264; profile-level-id=64401f") { // L3.1
        maxSize = KResH264_PLID_42801F;
    } else if (lowerMimeType == "video/h264; profile-level-id=428020" ||
               lowerMimeType == "video/h264; profile-level-id=4d4020" ||
               lowerMimeType == "video/h264; profile-level-id=644020") { // L3.2
        maxSize = KResH264_PLID_428020;
    } else if (lowerMimeType == "video/h264; profile-level-id=428028" ||
               lowerMimeType == "video/h264; profile-level-id=4d4028" ||
               lowerMimeType == "video/h264; profile-level-id=644028") { // L4
        maxSize = KResH264_PLID_428028;
    }

    return maxSize;
}

/*
 * This function returns the maximum framerate defined by the video standards
 * for different MIME Types.
 */

qreal S60VideoCaptureSettings::maximumFrameRateForMimeType(const QString &mimeType) const
{
    qreal maxRate(-1.0);
    // Use all lower case for comparisons
    QString lowerMimeType = mimeType.toLower();

    if (lowerMimeType == "video/h263-2000") {
        maxRate = KFrR_H263;
    } else if (lowerMimeType == "video/h263-2000; profile=0") {
        maxRate = KFrR_H263_Profile0;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=10") {
        maxRate = KFrR_H263_Profile0_Level10;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=20") {
        maxRate = KFrR_H263_Profile0_Level20;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=30") {
        maxRate = KFrR_H263_Profile0_Level30;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=40") {
        maxRate = KFrR_H263_Profile0_Level40;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=45") {
        maxRate = KFrR_H263_Profile0_Level45;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=50") {
        maxRate = KFrR_H263_Profile0_Level50;
    } else if (lowerMimeType == "video/h263-2000; profile=3") {
        maxRate = KFrR_H263_Profile3;
    } else if (lowerMimeType == "video/mp4v-es") {
        maxRate = KFrR_MPEG4;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=1") {
        maxRate = KFrR_MPEG4_PLID_1;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=2") {
        maxRate = KFrR_MPEG4_PLID_2;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=3") {
        maxRate = KFrR_MPEG4_PLID_3;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=4") {
        maxRate = KFrR_MPEG4_PLID_4;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=5") {
        maxRate = KFrR_MPEG4_PLID_5;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=6") {
        maxRate = KFrR_MPEG4_PLID_6;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=8") {
        maxRate = KFrR_MPEG4_PLID_8;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=9") {
        maxRate = KFrR_MPEG4_PLID_9;
    } else if (lowerMimeType == "video/h264") {
        maxRate = KFrR_H264;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800a" ||
               lowerMimeType == "video/h264; profile-level-id=4d400a" ||
               lowerMimeType == "video/h264; profile-level-id=64400a") { // L1
        maxRate = KFrR_H264_PLID_42800A;
    } else if (lowerMimeType == "video/h264; profile-level-id=42900b" ||
               lowerMimeType == "video/h264; profile-level-id=4d500b" ||
               lowerMimeType == "video/h264; profile-level-id=644009") { // L1.b
        maxRate = KFrR_H264_PLID_42900B;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800b" ||
               lowerMimeType == "video/h264; profile-level-id=4d400b" ||
               lowerMimeType == "video/h264; profile-level-id=64400b") { // L1.1
        maxRate = KFrR_H264_PLID_42800B;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800c" ||
               lowerMimeType == "video/h264; profile-level-id=4d400c" ||
               lowerMimeType == "video/h264; profile-level-id=64400c") { // L1.2
        maxRate = KFrR_H264_PLID_42800C;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800d" ||
               lowerMimeType == "video/h264; profile-level-id=4d400d" ||
               lowerMimeType == "video/h264; profile-level-id=64400d") { // L1.3
        maxRate = KFrR_H264_PLID_42800D;
    } else if (lowerMimeType == "video/h264; profile-level-id=428014" ||
               lowerMimeType == "video/h264; profile-level-id=4d4014" ||
               lowerMimeType == "video/h264; profile-level-id=644014") { // L2
        maxRate = KFrR_H264_PLID_428014;
    } else if (lowerMimeType == "video/h264; profile-level-id=428015" ||
               lowerMimeType == "video/h264; profile-level-id=4d4015" ||
               lowerMimeType == "video/h264; profile-level-id=644015") { // L2.1
        maxRate = KFrR_H264_PLID_428015;
    } else if (lowerMimeType == "video/h264; profile-level-id=428016" ||
               lowerMimeType == "video/h264; profile-level-id=4d4016" ||
               lowerMimeType == "video/h264; profile-level-id=644016") { // L2.2
        maxRate = KFrR_H264_PLID_428016;
    } else if (lowerMimeType == "video/h264; profile-level-id=42801e" ||
               lowerMimeType == "video/h264; profile-level-id=4d401e" ||
               lowerMimeType == "video/h264; profile-level-id=64401e") { // L3
        maxRate = KFrR_H264_PLID_42801E;
    } else if (lowerMimeType == "video/h264; profile-level-id=42801f" ||
               lowerMimeType == "video/h264; profile-level-id=4d401f" ||
               lowerMimeType == "video/h264; profile-level-id=64401f") { // L3.1
        maxRate = KFrR_H264_PLID_42801F;
    } else if (lowerMimeType == "video/h264; profile-level-id=428020" ||
               lowerMimeType == "video/h264; profile-level-id=4d4020" ||
               lowerMimeType == "video/h264; profile-level-id=644020") { // L3.2
        maxRate = KFrR_H264_PLID_428020;
    } else if (lowerMimeType == "video/h264; profile-level-id=428028" ||
               lowerMimeType == "video/h264; profile-level-id=4d4028" ||
               lowerMimeType == "video/h264; profile-level-id=644028") { // L4
        maxRate = KFrR_H264_PLID_428028;
    }

    return maxRate;
}

/*
 * This function returns the maximum bitrate defined by the video standards
 * for different MIME Types.
 */
int S60VideoCaptureSettings::maximumBitRateForMimeType(const QString &mimeType) const
{
    int maxRate(-1.0);
    // Use all lower case for comparisons
    QString lowerMimeType = mimeType.toLower();

    if (lowerMimeType == "video/h263-2000") {
        maxRate = KBiR_H263;
    } else if (lowerMimeType == "video/h263-2000; profile=0") {
        maxRate = KBiR_H263_Profile0;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=10") {
        maxRate = KBiR_H263_Profile0_Level10;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=20") {
        maxRate = KBiR_H263_Profile0_Level20;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=30") {
        maxRate = KBiR_H263_Profile0_Level30;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=40") {
        maxRate = KBiR_H263_Profile0_Level40;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=45") {
        maxRate = KBiR_H263_Profile0_Level45;
    } else if (lowerMimeType == "video/h263-2000; profile=0; level=50") {
        maxRate = KBiR_H263_Profile0_Level50;
    } else if (lowerMimeType == "video/h263-2000; profile=3") {
        maxRate = KBiR_H263_Profile3;
    } else if (lowerMimeType == "video/mp4v-es") {
        maxRate = KBiR_MPEG4;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=1") {
        maxRate = KBiR_MPEG4_PLID_1;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=2") {
        maxRate = KBiR_MPEG4_PLID_2;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=3") {
        maxRate = KBiR_MPEG4_PLID_3;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=4") {
        maxRate = KBiR_MPEG4_PLID_4;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=5") {
        maxRate = KBiR_MPEG4_PLID_5;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=6") {
        maxRate = KBiR_MPEG4_PLID_6;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=8") {
        maxRate = KBiR_MPEG4_PLID_8;
    } else if (lowerMimeType == "video/mp4v-es; profile-level-id=9") {
        maxRate = KBiR_MPEG4_PLID_9;
    } else if (lowerMimeType == "video/h264") {
        maxRate = KBiR_H264;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800a" ||
               lowerMimeType == "video/h264; profile-level-id=4d400a" ||
               lowerMimeType == "video/h264; profile-level-id=64400a") { // L1
        maxRate = KBiR_H264_PLID_42800A;
    } else if (lowerMimeType == "video/h264; profile-level-id=42900b" ||
               lowerMimeType == "video/h264; profile-level-id=4d500b" ||
               lowerMimeType == "video/h264; profile-level-id=644009") { // L1.b
        maxRate = KBiR_H264_PLID_42900B;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800b" ||
               lowerMimeType == "video/h264; profile-level-id=4d400b" ||
               lowerMimeType == "video/h264; profile-level-id=64400b") { // L1.1
        maxRate = KBiR_H264_PLID_42800B;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800c" ||
               lowerMimeType == "video/h264; profile-level-id=4d400c" ||
               lowerMimeType == "video/h264; profile-level-id=64400c") { // L1.2
        maxRate = KBiR_H264_PLID_42800C;
    } else if (lowerMimeType == "video/h264; profile-level-id=42800d" ||
               lowerMimeType == "video/h264; profile-level-id=4d400d" ||
               lowerMimeType == "video/h264; profile-level-id=64400d") { // L1.3
        maxRate = KBiR_H264_PLID_42800D;
    } else if (lowerMimeType == "video/h264; profile-level-id=428014" ||
               lowerMimeType == "video/h264; profile-level-id=4d4014" ||
               lowerMimeType == "video/h264; profile-level-id=644014") { // L2
        maxRate = KBiR_H264_PLID_428014;
    } else if (lowerMimeType == "video/h264; profile-level-id=428015" ||
               lowerMimeType == "video/h264; profile-level-id=4d4015" ||
               lowerMimeType == "video/h264; profile-level-id=644015") { // L2.1
        maxRate = KBiR_H264_PLID_428015;
    } else if (lowerMimeType == "video/h264; profile-level-id=428016" ||
               lowerMimeType == "video/h264; profile-level-id=4d4016" ||
               lowerMimeType == "video/h264; profile-level-id=644016") { // L2.2
        maxRate = KBiR_H264_PLID_428016;
    } else if (lowerMimeType == "video/h264; profile-level-id=42801e" ||
               lowerMimeType == "video/h264; profile-level-id=4d401e" ||
               lowerMimeType == "video/h264; profile-level-id=64401e") { // L3
        maxRate = KBiR_H264_PLID_42801E;
    } else if (lowerMimeType == "video/h264; profile-level-id=42801f" ||
               lowerMimeType == "video/h264; profile-level-id=4d401f" ||
               lowerMimeType == "video/h264; profile-level-id=64401f") { // L3.1
        maxRate = KBiR_H264_PLID_42801F;
    } else if (lowerMimeType == "video/h264; profile-level-id=428020" ||
               lowerMimeType == "video/h264; profile-level-id=4d4020" ||
               lowerMimeType == "video/h264; profile-level-id=644020") { // L3.2
        maxRate = KBiR_H264_PLID_428020;
    } else if (lowerMimeType == "video/h264; profile-level-id=428028" ||
               lowerMimeType == "video/h264; profile-level-id=4d4028" ||
               lowerMimeType == "video/h264; profile-level-id=644028") { // L4
        maxRate = KBiR_H264_PLID_428028;
    }

    return maxRate;
}

// End of file
