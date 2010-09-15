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


#include <qglobal.h>
#include <QtCore/qstring.h>
#include <QtCore/qdir.h>

#include "s60videocapturesession.h"
#include "s60cameraconstants.h"

#include <utf.h>
#include <bautils.h>
#include <coemain.h>

#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
#include <mmf/devvideo/devvideorecord.h>
#include <mmf/devvideo/devvideobase.h>
#endif

#ifdef USE_SYMBIAN_VIDEOENUMS
#include <mmf/common/mmfvideoenums.h>
#endif

S60VideoCaptureSession::S60VideoCaptureSession(QObject *parent) :
    QObject(parent),
    m_cameraEngine(NULL),
    m_videoRecorder(NULL),
    m_videoQuality(QtMultimediaKit::NormalQuality * KSymbianImageQualityCoefficient),   // Default video quality
    m_error(NoError),
    m_captureState(ENotInitialized),    // Default state
    m_videoControllerMap(QHash<QString, VideoControllerData>()),
    m_videoCodecData(QHash<QString, QList<TSupportedFrameRatePictureSize> >()),
    m_muted(false),
    m_startAfterPrepareComplete(false),
    m_prepareAfterOpenComplete(false),
    m_maxClipSize(-1)
{
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
    TRAPD(err, doPopulateVideoCodecsDataL());
    setError(err);
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED
}

S60VideoCaptureSession::~S60VideoCaptureSession()
{
    if (m_videoRecorder) {
        delete m_videoRecorder;
        m_videoRecorder = NULL;
    }
}

/*
 * This function can be used both internally and from Control classes using this session.
 * The error notification will go to client application through QMediaRecorder error signal.
 */
void S60VideoCaptureSession::setError(TInt aError)
{
    if (aError == KErrNone)
        return;

    // Reset state
    m_captureState = ENotInitialized;
    emit stateChanged(m_captureState);

    m_error = aError;
    QCamera::Error cameraError = fromSymbianErrorToQtMultimediaError(m_error);

    QString errorDescription;
    switch (cameraError) {
        case QCamera::ServiceMissingError:
            errorDescription = "Camera service is missing";
            break;
        case QCamera::NotSupportedFeatureError:
            errorDescription = "The requested camera feature is not supported";
            break;
        case QCamera::CameraError:
            errorDescription = "General camera error occurred";
            break;

        default:
            errorDescription = "Unexpected camera error occurred";
            break;
    }

    emit error(cameraError, errorDescription);
}

QCamera::Error S60VideoCaptureSession::fromSymbianErrorToQtMultimediaError(int aError)
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

/*
 * This function applies all recording settings to make latency during the
 * start of the recording as short as possible. After this it is not possible to
 * set settings (inc. output location) after stopping the recording.
 */
void S60VideoCaptureSession::applyAllSettings()
{
    if (m_captureState == EInitialized) {
        setOutputLocation(QUrl());
        m_prepareAfterOpenComplete = true;
        return;
    }

    if (m_captureState == EOpening) {
        m_prepareAfterOpenComplete = true;
        return;
    }

    // Start preparing
    m_captureState = EPreparing;
    emit stateChanged(m_captureState);

    commitVideoEncoderSettings();
    m_videoRecorder->Prepare();
}

QList<QSize> S60VideoCaptureSession::supportedVideoResolutions()
{
    QList<QSize> list;

    // If we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {

        foreach (QString codecName, formatMap().keys()) {
            int codecIndex = formatMap().value(codecName);
            CCamera::TFormat format = static_cast<CCamera::TFormat>(codecIndex);
            TUint32 videoFormatsSupported = m_info.iVideoFrameFormatsSupported;

            if (videoFormatsSupported&format) {
                CCamera *camera = m_cameraEngine->Camera();
                for (int i = 0; i < m_info.iNumVideoFrameSizesSupported; i++) {
                    TSize size;
                    camera->EnumerateVideoFrameSizes(size, i, format);
                    QSize qSize(size.iWidth, size.iHeight);
                    if (!list.contains(qSize))
                        list << QSize(size.iWidth, size.iHeight);
                }
            }
        }
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    list << QSize(50, 50);
    list << QSize(100, 100);
    list << QSize(800,600);
#endif

    return list;
}

QList<QSize> S60VideoCaptureSession::supportedVideoResolutions(const QVideoEncoderSettings &settings)
{
    QList<QSize> supportedFrameSizes;

    if (settings.codec().isEmpty())
        return supportedFrameSizes;
    if (!m_videoCodeclist.contains(settings.codec()))
        return supportedFrameSizes;
    if (m_videoCodecData.isEmpty())
        return supportedVideoResolutions();

    QList<TSupportedFrameRatePictureSize> list = m_videoCodecData[settings.codec()];

    if (settings.frameRate() == 0.0) {
        foreach(TSupportedFrameRatePictureSize size, list) {
            if(!supportedFrameSizes.contains(size.frameSize))
                supportedFrameSizes << size.frameSize;
        }
    } else {
        foreach(TSupportedFrameRatePictureSize size, list) {
            if (qFuzzyCompare(size.frameRate,settings.frameRate()))
                if(!supportedFrameSizes.contains(size.frameSize))
                    supportedFrameSizes << size.frameSize;
        }
    }

    QList<QSize> cameraSupportedFrameSizes = supportedVideoResolutions();
    foreach(QSize frameSize, supportedFrameSizes) {
        if (!cameraSupportedFrameSizes.contains(frameSize))
            supportedFrameSizes.removeAll(frameSize);
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    supportedFrameSizes << QSize(50, 50);
    supportedFrameSizes << QSize(100, 100);
    supportedFrameSizes << QSize(800,600);
#endif

    return supportedFrameSizes;
}

void S60VideoCaptureSession::setCameraHandle(CCameraEngine* cameraHandle)
{
    m_cameraEngine = cameraHandle;

    resetSession();
}

void S60VideoCaptureSession::resetSession()
{
    if (m_videoRecorder) {
        delete m_videoRecorder;
        m_videoRecorder = NULL;
    }

    m_error = KErrNone;
    TRAPD(err, m_videoRecorder = CVideoRecorderUtility::NewL(*this));
    setError(err);

    updateVideoCaptureContainers();
}

QList<qreal> S60VideoCaptureSession::supportedVideoFrameRates()
{
    QList<qreal> list = QList<qreal>();

    // If we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {

        foreach (QString codecName, formatMap().keys()) {
            int codecIndex = formatMap().value(codecName);
            CCamera::TFormat format = static_cast<CCamera::TFormat>( codecIndex );
            TUint32 videoFormatsSupported = m_info.iVideoFrameFormatsSupported;

            if (videoFormatsSupported & format) {
                CCamera *camera = m_cameraEngine->Camera();
                for (int i=0; i < m_info.iNumVideoFrameRatesSupported; i++) {
                    TReal32 rate;
                    TInt maxSizeIndex=0;
                    camera->EnumerateVideoFrameRates(rate, i,format, maxSizeIndex);
                    if (!list.contains(rate))
                        if (rate > 0.0)
                            list << rate;
                }
            }
        }
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    list << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
#endif

    return list;
}

QList<qreal> S60VideoCaptureSession::supportedVideoFrameRates(const QVideoEncoderSettings &settings)
{
    QList<qreal> supportedFrameRates;

    if (settings.codec().isEmpty())
        return supportedFrameRates;
    if (!m_videoCodeclist.contains(settings.codec()))
        return supportedFrameRates;
    if (m_videoCodecData.isEmpty())
        return supportedVideoFrameRates();

    QList<TSupportedFrameRatePictureSize> list = m_videoCodecData[settings.codec()];

    if (settings.resolution().isEmpty()) {
        foreach(TSupportedFrameRatePictureSize size, list)
            if(!supportedFrameRates.contains(size.frameRate))
                supportedFrameRates << size.frameRate;
    }else {
        foreach(TSupportedFrameRatePictureSize size, list) {
            if(!supportedFrameRates.contains(size.frameRate))
                supportedFrameRates << size.frameRate;
        }
    }

    QList<qreal> cameraSupportedFrameRates = supportedVideoFrameRates();
    foreach(qreal frameRate, supportedFrameRates) {
        if (!cameraSupportedFrameRates.contains(frameRate))
            supportedFrameRates.removeAll(frameRate);
    }

#ifdef Q_CC_NOKIAX86 // Emulator
    supportedFrameRates << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
#endif
    return supportedFrameRates;
}

bool S60VideoCaptureSession::setOutputLocation(const QUrl &sink)
{
    if (m_captureState < EInitialized || m_captureState >= EOpenComplete) {
        setError(KErrNotReady);
        return false;
    }

    // Empty URL
    if (sink.isEmpty()) {
        // Use default file name and path (C:\Data\Videos\video.mp4)
        m_sink.setUrl("C:\\Data\\Videos\\video.mp4");

    // Relative URL
    } else if (sink.isRelative()) {
        QUrl base(QDir::rootPath());
        QUrl resolvedUrl = base.resolved(sink);
        QString fileName = QDir::toNativeSeparators(resolvedUrl.toString());
        TPtrC16 path(reinterpret_cast<const TUint16*>(fileName.utf16()));
        TRAPD(err, BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(), path));
        if (!err) {
            m_sink = resolvedUrl;
        } else {
            return false;
        }

    // Absolute URL
    } else {
        QString fileName = QDir::toNativeSeparators(sink.toString());
        TPtrC16 path(reinterpret_cast<const TUint16*>(fileName.utf16()));
        TRAPD(err, BaflUtils::EnsurePathExistsL(CCoeEnv::Static()->FsSession(), path));
        if (!err) {
            m_sink = sink;
        } else {
            return false;
        }
    }

    if (m_captureState == EInitialized) {

        // Open file
        QString fileName = QDir::toNativeSeparators(m_sink.toString());
        TPtrC16 sink(reinterpret_cast<const TUint16*>(fileName.utf16()));

        int cameraHandle = m_cameraEngine->Camera()->Handle();

        TUid controllerUid(TUid::Uid(m_videoControllerMap[m_container].controllerUid));
        TUid formatUid(TUid::Uid(m_videoControllerMap[m_container].formatUid));

        TPtrC16 str(reinterpret_cast<const TUint16*>(m_videoSettings.codec().utf16()));
        HBufC8* videocodec(NULL);
        TRAPD(error, videocodec = CnvUtfConverter::ConvertFromUnicodeToUtf8L(str));
        setError(error);
        CleanupStack::PushL(videocodec);
        TFourCC audioCodec = m_audioCodeclist[m_audioSettings.codec()];

        if (m_videoRecorder) {
            // File open completes in MvruoOpenComplete
            TRAPD(err, m_videoRecorder->OpenFileL(sink, cameraHandle, controllerUid, formatUid, *videocodec, audioCodec));
            setError(err);
            CleanupStack::PopAndDestroy(videocodec);
            m_captureState = EOpening;
            emit stateChanged(m_captureState);
        }
        else {
            CleanupStack::PopAndDestroy(videocodec);
            setError(KErrNotReady);
        }
    } else {
        setError(KErrNotReady);
    }

    return true;
}

QUrl S60VideoCaptureSession::outputLocation() const
{
    return m_sink;
}

qint64 S60VideoCaptureSession::position()
{
    qint64 position = 0;
    // Update position only if recording is ongoing
    if ((m_captureState == ERecording) && m_videoRecorder) {
        TRAPD(err, position = m_videoRecorder->DurationL().Int64() / 1000);
        setError(err);
    }

    return position;
}

S60VideoCaptureSession::TVideoCaptureState S60VideoCaptureSession::state() const
{
    return m_captureState;
}

bool S60VideoCaptureSession::isMuted() const
{
    return m_muted;
}

void S60VideoCaptureSession::setMuted(const bool muted)
{
    // Muting audio from recording is not supported in Symbian
    Q_UNUSED(muted);
}

void S60VideoCaptureSession::commitVideoEncoderSettings()
{
    if (m_captureState >= EInitialized && m_captureState <= EOpenComplete) {
/*
        doSetVideoResolution(m_videoSettings.resolution());
        doSetFrameRate(m_videoSettings.frameRate());
        doSetBitrate(m_videoSettings.bitRate());
*/
    }
}

void S60VideoCaptureSession::doSetVideoFrameRateFixed(bool fixed)
{
#ifndef PRE_S60_50_PLATFORM // S60 5.0 or later
    if (m_videoRecorder) {
        TRAPD(err, m_videoRecorder->SetVideoFrameRateFixedL(fixed));
        setError(err);
    }
    else
        setError(KErrNotReady);
#else // PRE_S60_50_PLATFORM - S60 3.1 or 3.2
    Q_UNUSED(fixed);
#endif // PRE_S60_50_PLATFORM
}

void S60VideoCaptureSession::setVideoEncoderSettings(const QVideoEncoderSettings &videoSettings)
{
    m_videoSettings = videoSettings;
}

void S60VideoCaptureSession::videoEncoderSettings(QVideoEncoderSettings &videoSettings) const
{
    videoSettings = m_videoSettings;
}

void S60VideoCaptureSession::setAudioEncoderSettings(const QAudioEncoderSettings &audioSettings)
{
    m_audioSettings = audioSettings;
}

void S60VideoCaptureSession::audioEncoderSettings(QAudioEncoderSettings &audioSettings) const
{
    audioSettings = m_audioSettings;
}

QtMultimediaKit::EncodingQuality S60VideoCaptureSession::videoCaptureQuality() const
{
#ifndef PRE_S60_50_PLATFORM
    if (m_videoQuality == EVideoQualityLow)
        return QtMultimediaKit::LowQuality;
    else if (m_videoQuality == EVideoQualityNormal)
        return QtMultimediaKit::NormalQuality;
    else if (m_videoQuality == EVideoQualityHigh)
        return QtMultimediaKit::HighQuality;
    else if (m_videoQuality == EVideoQualityLossless)
        return QtMultimediaKit::VeryHighQuality;
#endif // PRE_S60_50_PLATFORM

    return QtMultimediaKit::NormalQuality;
}

void S60VideoCaptureSession::setVideoCaptureQuality(QtMultimediaKit::EncodingQuality quality)
{
#ifndef PRE_S60_50_PLATFORM
    m_videoQuality = quality * KSymbianImageQualityCoefficient;
    if (m_videoRecorder) {
        TRAPD(err, m_videoRecorder->SetVideoQualityL(m_videoQuality));
        setError(err);
    }
    else
        setError(KErrNotReady);
#endif // PRE_S60_50_PLATFORM

    // Otherwise ignore
}

int S60VideoCaptureSession::initializeVideoRecording()
{
    TInt symbianError = KErrNone;
    TRAP(symbianError, doInitializeVideoRecorderL());
    setError(symbianError);

    return symbianError;
}

void S60VideoCaptureSession::startRecording()
{
    if (m_captureState == EInitialized) {
        setOutputLocation(QUrl());
        m_startAfterPrepareComplete = true;
        return;
    }

    if (m_captureState == EOpening || m_captureState == EPreparing) {
        // Execute FileOpenL() and Prepare() asap and then start recording
        m_startAfterPrepareComplete = true;
        return;
    }

    if (m_captureState == EOpenComplete) {
        commitVideoEncoderSettings();

        // Start preparing
        m_captureState = EPreparing;
        emit stateChanged(m_captureState);

        m_videoRecorder->Prepare();
    }

    if (m_captureState == EPaused || m_captureState == EPrepared) {

        if (m_cameraEngine && !m_cameraEngine->IsCameraReady()) {
            setError(KErrNotReady);
            return;
        }

        if (m_videoRecorder) {
            m_videoRecorder->Record();
            m_captureState = ERecording;
            emit stateChanged(m_captureState);
        }
        else
            setError(KErrNotReady);
    }
}

void S60VideoCaptureSession::pauseRecording()
{
    if (m_captureState == ERecording) {
        if (m_videoRecorder) {
            TRAPD(err, m_videoRecorder->PauseL());
            setError(err);
            m_captureState = EPaused;
            emit stateChanged(m_captureState);
        }
        else
            setError(KErrNotReady);

    } else if (m_captureState == EPaused) {
        if (m_videoRecorder) {
            TRAPD(err, m_videoRecorder->Record());
            setError(err);
            m_captureState = ERecording;
            emit stateChanged(m_captureState);
        }
        else
            setError(KErrNotReady);
    }
}

void S60VideoCaptureSession::stopRecording()
{
    if (m_captureState != ERecording && m_captureState != EPaused)
        return; // Ignore

    if (m_videoRecorder) {
        m_videoRecorder->Stop();
        m_videoRecorder->Close();

        m_captureState = EInitialized;
        emit stateChanged(m_captureState);
    }
    else
        setError(KErrNotReady);
}

bool S60VideoCaptureSession::queryCurrentCameraInfo()
{
    bool returnValue = false;

    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera)
            camera->CameraInfo(m_info);
        returnValue = true;
    }
    return returnValue;
}

QMap<QString, int> S60VideoCaptureSession::formatMap()
{
    QMap<QString, int> formats;
    // Format list copied from CCamera::TFormat (ecam.h)

    formats.insert("Monochrome", 0x0001);
    formats.insert("16bitRGB444", 0x0002);
    formats.insert("16BitRGB565", 0x0004);
    formats.insert("32BitRGB888", 0x0008);
    formats.insert("Jpeg", 0x0010);
    formats.insert("Exif", 0x0020);
    formats.insert("FbsBitmapColor4K", 0x0040);
    formats.insert("FbsBitmapColor64K", 0x0080);
    formats.insert("FbsBitmapColor16M", 0x0100);
    formats.insert("UserDefined", 0x0200);
    formats.insert("FbsBitmapColor16MU", 0x00010000);
    formats.insert("YUV420Interleaved", 0x0400);
    formats.insert("YUV420Planar", 0x0800);
    formats.insert("YUV422", 0x1000);
    formats.insert("YUV422Reversed", 0x2000);
    formats.insert("YUV444", 0x4000);
    formats.insert("YUV420SemiPlanar", 0x8000);
    formats.insert("MJPEG", 0x00020000);
    formats.insert("EncodedH264", 0x00040000);

    return formats;
}

void S60VideoCaptureSession::updateVideoCaptureContainers()
{
    TRAPD(err, updateVideoCaptureContainersL());
    setError(err);
}

void S60VideoCaptureSession::updateVideoCaptureContainersL()
{
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

    // Set the media ids
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
    for (TInt index=0; index<controllers.Count(); index++) {
        const RMMFFormatImplInfoArray& recordFormats =
            controllers[index]->RecordFormats();
        for (TInt j=0; j<recordFormats.Count(); j++) {
            const CDesC8Array& mimeTypes = recordFormats[j]->SupportedMimeTypes();
            TInt count = mimeTypes.Count();
            if (count > 0) {
                TPtrC8 mimeType = mimeTypes[0];
                QString type = QString::fromUtf8((char *)mimeType.Ptr(),
                        mimeType.Length());
                VideoControllerData data;
                data.controllerUid = controllers[index]->Uid().iUid;
                data.formatUid = recordFormats[j]->Uid().iUid;
                data.formatDescription = QString::fromUtf16(
                        recordFormats[j]->DisplayName().Ptr(),
                        recordFormats[j]->DisplayName().Length());
                m_videoControllerMap[type] = data;
            }
        }
    }
    CleanupStack::PopAndDestroy(&controllers);
    CleanupStack::PopAndDestroy(&mediaIds);
    CleanupStack::PopAndDestroy(format);
    CleanupStack::PopAndDestroy(pluginParameters);
}

QStringList S60VideoCaptureSession::supportedVideoCaptureCodecs()
{
    return m_videoCodeclist;
}

QStringList S60VideoCaptureSession::supportedAudioCaptureCodecs()
{
    QStringList keys = m_audioCodeclist.keys();
    keys.sort();
    return keys;
}

QList<int> S60VideoCaptureSession::supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous)
{
    Q_UNUSED(settings);
    Q_UNUSED(continuous);

    return QList<int>();
}

bool S60VideoCaptureSession::isSupportedVideoCaptureCodec(const QString &codecName)
{
    return m_videoCodeclist.contains(codecName);
}

QString S60VideoCaptureSession::audioCaptureCodec() const
{
    return m_audioSettings.codec();
}

QString S60VideoCaptureSession::videoCaptureCodec() const
{
    return m_videoSettings.codec();
}

void S60VideoCaptureSession::setVideoCaptureCodec(const QString &codecName)
{
    if (codecName.isEmpty() || codecName == m_videoSettings.codec())
        return;

    m_videoSettings.setCodec(codecName);
}

void S60VideoCaptureSession::setAudioCaptureCodec(const QString &codecName)
{
    if (codecName.isEmpty() || codecName == m_audioSettings.codec())
        return;

    m_audioSettings.setCodec(codecName);
}

QString S60VideoCaptureSession::videoCaptureCodecDescription(const QString &codecName)
{
    QString codecDescription;
    codecDescription = codecName;

    return codecDescription;
}

int S60VideoCaptureSession::bitrate()
{
    return m_videoSettings.bitRate();
}

void S60VideoCaptureSession::setBitrate(const int bitrate)
{
    m_videoSettings.setBitRate(bitrate);
}

void S60VideoCaptureSession::doSetBitrate(const int &bitrate)
{
    if (m_videoRecorder) {
        TRAPD(err, m_videoRecorder->SetVideoBitRateL(bitrate));
        setError(err);
    } else {
        setError(KErrNotReady);
    }
}

QSize S60VideoCaptureSession::videoResolution() const
{
    return m_videoSettings.resolution();
}

void S60VideoCaptureSession::setVideoResolution(const QSize &resolution)
{
    m_videoSettings.setResolution(resolution);
}

void S60VideoCaptureSession::doSetVideoResolution(const QSize &resolution)
{
    if (m_videoRecorder) {
        TSize size(resolution.width(), resolution.height());
        TRAPD(err, m_videoRecorder->SetVideoFrameSizeL(size));
        setError(err);
    } else {
        setError(KErrNotReady);
    }
}

qreal S60VideoCaptureSession::framerate() const
{
    return m_videoSettings.frameRate();
}

void S60VideoCaptureSession::setFrameRate(qreal rate)
{
    m_videoSettings.setFrameRate(rate);
}

void S60VideoCaptureSession::doSetFrameRate(qreal rate)
{
    if (m_videoRecorder) {
        QList<qreal> list = supportedVideoFrameRates();
        if (list.contains(rate)) {
            TRAPD(err, m_videoRecorder->SetVideoFrameRateL(rate));
            setError(err);
        } else {
            setError(KErrNotSupported);
        }
    } else {
        setError(KErrNotReady);
    }
}

void S60VideoCaptureSession::initializeVideoCaptureSettings()
{
    if (m_videoCodeclist.count() > 0)
        m_videoSettings.setCodec(m_videoCodeclist[0]); // Setting the first video codec found as default value
    if (m_audioCodeclist.keys().count() > 0)
        m_audioSettings.setCodec(m_audioCodeclist.keys()[0]); // Setting the first audio codec found as default value

     // Use maximum resolution and framerate

    QList<QSize> sizes = supportedVideoResolutions();
    QSize maxSize = QSize();
    foreach (QSize size, sizes) {
        if ((size.width() * size.height()) > (maxSize.width() * maxSize.height()))
            maxSize = size;
    }
    m_videoSettings.setResolution(maxSize);

    QList<qreal> rates = supportedVideoFrameRates();
    qreal maxRate = 0.0;
    foreach (qreal rate, rates)
        maxRate = qMax(maxRate, rate);
    m_videoSettings.setFrameRate(maxRate);

    // Use VBR as default value
    m_videoSettings.setBitRate(KMMFVariableVideoBitRate);

    m_videoSettings.setQuality(QtMultimediaKit::HighQuality);
}

QSize S60VideoCaptureSession::pixelAspectRatio()
{
#ifndef S60_31_PLATFORM
    TVideoAspectRatio par;
    TRAPD(err, m_videoRecorder->GetPixelAspectRatioL(par));
    if (err) {
        setError(err);
    }
    return QSize(par.iNumerator, par.iDenominator);
#else // S60_31_PLATFORM
    return QSize();
#endif // !S60_31_PLATFORM
}

void S60VideoCaptureSession::setPixelAspectRatio(const QSize par)
{
#ifndef S60_31_PLATFORM
    const TVideoAspectRatio videoPar(par.width(), par.height());
    TRAPD(err, m_videoRecorder->SetPixelAspectRatioL(videoPar));
    if (err) {
        setError(err);
    }
#else // S60_31_PLATFORM
    Q_UNUSED(par);
#endif // !S60_31_PLATFORM
}

int S60VideoCaptureSession::gain()
{
    TInt gain = 0;
    TRAPD(err, gain = m_videoRecorder->GainL());
    if (err) {
        setError(err);
    }
    return (int)gain;
}

void S60VideoCaptureSession::setGain(const int gain)
{
    TRAPD(err, m_videoRecorder->SetGainL(gain));
    if (err) {
        setError(err);
    }
}

int S60VideoCaptureSession::maxClipSizeInBytes() const
{
    return m_maxClipSize;
}

void S60VideoCaptureSession::setMaxClipSizeInBytes(const int size)
{
    TRAPD(err, m_videoRecorder->SetMaxClipSizeL(size));
    if (err) {
        setError(err);
    } else
        m_maxClipSize = size;
}

void S60VideoCaptureSession::MvruoOpenComplete(TInt aError)
{
    if (aError == KErrNone && m_videoRecorder) {
        m_captureState = EOpenComplete;
        emit stateChanged(m_captureState);

        // Prepare right away
        if (m_startAfterPrepareComplete || m_prepareAfterOpenComplete) {
            m_prepareAfterOpenComplete = false; // Reset
            commitVideoEncoderSettings();
            m_videoRecorder->Prepare();
        }
        return;
    }

    setError(aError);
}

void S60VideoCaptureSession::MvruoPrepareComplete(TInt aError)
{
    if(aError == KErrNone) {
        m_captureState = EPrepared;
        emit stateChanged(EPrepared);
    }
    else
        setError(aError);

    if (m_startAfterPrepareComplete) {
        m_startAfterPrepareComplete = false; // Reset
        startRecording();
    }
}

void S60VideoCaptureSession::MvruoRecordComplete(TInt aError)
{
    if (!m_videoRecorder) {
        setError(KErrNotReady);
        return;
    }

    if((aError == KErrNone || aError == KErrCompletion)) {
        m_videoRecorder->Stop();
        m_videoRecorder->Close();

        // Reset state
        m_captureState = EInitialized;
        emit stateChanged(m_captureState);
    }

    setError(aError);
}

void S60VideoCaptureSession::MvruoEvent(const TMMFEvent& aEvent)
{
    Q_UNUSED(aEvent);
}

#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
void S60VideoCaptureSession::MdvroReturnPicture(TVideoPicture *aPicture)
{
    Q_UNUSED(aPicture);
}

void S60VideoCaptureSession::MdvroSupplementalInfoSent()
{
}

void S60VideoCaptureSession::MdvroNewBuffers()
{
}

void S60VideoCaptureSession::MdvroFatalError(TInt aError)
{
    Q_UNUSED(aError);
}

void S60VideoCaptureSession::MdvroInitializeComplete(TInt aError)
{
    Q_UNUSED(aError);
}

void S60VideoCaptureSession::MdvroStreamEnd()
{
}

void S60VideoCaptureSession::doPopulateVideoCodecsDataL()
{
    RArray<TUid> encs;
    CMMFDevVideoRecord *mDvr;
    mDvr = CMMFDevVideoRecord::NewL(*this);
    CleanupStack::PushL(mDvr);
    mDvr->GetEncoderListL(encs);
    CleanupClosePushL(encs);
    //qDebug()<< "MDF DevVideoRecord encoders count ="<<encs.Count();
    for (int i = 0; i < encs.Count(); i++ ) {
        //qDebug()<<"************************************************";
        //qDebug()<<"Get info for encoder 0x%08x"<<encs[i].iUid;
        CVideoEncoderInfo *einfo = mDvr->VideoEncoderInfoLC(encs[i]);
        const RPointerArray<CCompressedVideoFormat> &vformts=einfo->SupportedOutputFormats();
        for(int x=0; x<vformts.Count(); x++) {
            QString codecMimeType = QString::fromUtf8((char *)vformts[x]->MimeType().Ptr(),vformts[x]->MimeType().Length());
            //qDebug()<<"Supported Mimetype: "<<codecMimeType;
            if (codecMimeType.contains("video/h263", Qt::CaseSensitive) || codecMimeType.contains("video/MP4", Qt::CaseSensitive) )
                continue;

            const RArray<TPictureRateAndSize> &rsz=einfo->MaxPictureRates();
            TSupportedFrameRatePictureSize data;
            for(int j=0; j<rsz.Count(); j++) {
                data.frameRate = rsz[j].iPictureRate;
                data.frameSize = QSize(rsz[j].iPictureSize.iWidth,rsz[j].iPictureSize.iHeight);
                m_videoCodecData[codecMimeType] << data;
                /*qDebug()<<"Codec mimetype: "<<codecMimeType;
                qDebug()<<"Max framerate: "<<rsz[j].iPictureRate;
                qDebug()<<"Max framesize: "<<rsz[j].iPictureSize.iWidth<<"x"<<rsz[j].iPictureSize.iHeight;*/
            }
        }
        CleanupStack::PopAndDestroy(einfo);
    }
    CleanupStack::PopAndDestroy(&encs);
    CleanupStack::PopAndDestroy(mDvr);
}
#endif  // S60_DEVVIDEO_RECORDING_SUPPORTED

QStringList S60VideoCaptureSession::supportedVideoContainers()
{
    return m_videoControllerMap.keys();
}

bool S60VideoCaptureSession::isSupportedVideoContainer(const QString &containerName)
{
    return m_videoControllerMap.keys().contains(containerName);
}

QString S60VideoCaptureSession::videoContainer() const
{
    return m_container;
}

void S60VideoCaptureSession::setVideoContainer(const QString &containerName)
{
    if (containerName == m_container)
        return;

    m_container = containerName;
}

QString S60VideoCaptureSession::videoContainerDescription(const QString &containerName)
{
    return m_videoControllerMap.keys().contains(containerName) ?
        m_videoControllerMap[containerName].formatDescription : QString();
}

void S60VideoCaptureSession::doPopulateAudioCodecsL()
{
    if (m_captureState == EInitializing) {
        m_audioCodeclist.clear();

        RArray<TFourCC> audioTypes;
        CleanupClosePushL(audioTypes);
        if (m_videoRecorder)
            m_videoRecorder->GetSupportedAudioTypesL(audioTypes);
        else
            setError(KErrNotReady);
        for (TInt i = 0; i < audioTypes.Count(); i++) {
            TUint32 codec = audioTypes[i].FourCC();
        }
        if (audioTypes.Find(TFourCC(KMMFFourCCCodeAMR)) != KErrNotFound) {
            m_audioCodeclist[QString("audio/amr")] = KMMFFourCCCodeAMR;
        }

        if (audioTypes.Find(TFourCC(KMMFFourCCCodeAAC)) != KErrNotFound) {
            m_audioCodeclist[QString("audio/aac")] = KMMFFourCCCodeAAC;
        }
        CleanupStack::PopAndDestroy(&audioTypes);
    }
}

void S60VideoCaptureSession::doPopulateVideoCodecsL()
{
    if (m_captureState == EInitializing) {
        m_videoCodeclist.clear();

        CDesC8ArrayFlat* videoTypes = new (ELeave) CDesC8ArrayFlat(10);
        CleanupStack::PushL(videoTypes);
        if (m_videoRecorder)
            m_videoRecorder->GetSupportedVideoTypesL(*videoTypes);
        else
            setError(KErrNotReady);
        for (TInt i = 0; i < videoTypes->Count(); i++) {
            TPtrC8 videoType = videoTypes->MdcaPoint(i);
            m_videoCodeclist << QString::fromUtf8((char *)videoType.Ptr(), videoType.Length());
        }
        CleanupStack::PopAndDestroy(videoTypes);
    }
}

void S60VideoCaptureSession::doInitializeVideoRecorderL()
{
    m_captureState = EInitializing;
    emit stateChanged(m_captureState);

    if (m_videoRecorder) {
/*
        TRAPD(err, doPopulateVideoCodecsL());
        setError(err);
        TRAP(err, doPopulateAudioCodecsL());
        setError(err);
*/
        initializeVideoCaptureSettings();

        m_captureState = EInitialized;
        emit stateChanged(m_captureState);

    } else {
        setError(KErrNotReady);
    }
}

// End of file
