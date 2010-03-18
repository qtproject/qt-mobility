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

#include <BADESCA.H>
#include <BAUTILS.H>

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>
#include "S60VideoCaptureSession.h"
#include "s60viewfinderwidget.h"
#include "s60videoencoder.h"

#include <fbs.h>
#include <qglobal.h>
#include <QDir>
#include <pathinfo.h> 
#include <utf.h>
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED
#include <mmf/devvideo/devvideorecord.h>
#include <mmf/devvideo/devvideobase.h>
#endif
#ifdef USE_SYMBIAN_VIDEOENUMS
#include <mmf/common/mmfvideoenums.h>
#endif

const int KSymbianImageQualityCoefficient = 25;
_LIT(KVideoDummyMpg, "c:\\data\\temp");
_LIT8(KMimeTypeMPEG4VSPL4, "video/mp4v-es; profile-level-id=4");   

S60VideoCaptureSession::S60VideoCaptureSession(QObject *parent)
    : QObject(parent)
    , m_cameraEngine(NULL)
    , m_videoQuality(QtMedia::LowQuality*KSymbianImageQualityCoefficient)     
    , m_error(NoError)
    , m_videoRecorder(NULL)    
    , m_captureState(ENotInitialized)    
    , m_videoControllerMap(QHash<QString, VideoControllerData>())    
    , m_videoCodecData(QHash<QString, QList<TSupportedFrameRatePictureSize> >())    
{           
#ifdef S60_DEVVIDEO_RECORDING_SUPPORTED    
    TRAPD(err, doPopulateVideoCodecsDataL());   
    setError(err);
#endif // S60_DEVVIDEO_RECORDING_SUPPORTED   
}

S60VideoCaptureSession::~S60VideoCaptureSession()
{
    delete m_videoRecorder;
    m_videoRecorder = NULL;    
}

void S60VideoCaptureSession::setError(TInt aError)
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

QCamera::Error S60VideoCaptureSession::fromSymbianErrorToMultimediaError(int aError)
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

QList<QSize> S60VideoCaptureSession::supportedVideoResolutions()
{
    //qDebug() << "S60VideoCaptureSession::supportedVideoResolutions";
    QList<QSize> list;
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        
        foreach (QString codecName, formatMap().keys()) {
            int codecIndex = formatMap().value(codecName);
            CCamera::TFormat format = static_cast<CCamera::TFormat>( codecIndex );   
            TUint32 videoFormatsSupported = m_info.iVideoFrameFormatsSupported;
    
            if (videoFormatsSupported&format) {
                CCamera *camera = m_cameraEngine->Camera();
                for (int i=0; i < m_info.iNumVideoFrameSizesSupported; i++) {
                    TSize size;
                    camera->EnumerateVideoFrameSizes(size,i, format);
                    QSize qSize(size.iWidth, size.iHeight);
                    if (!list.contains(qSize))
                        list << QSize(size.iWidth, size.iHeight);
                }
            }
        }
    }
    #ifdef Q_CC_NOKIAX86
    list << QSize(50, 50);
    list << QSize(100, 100);
    list << QSize(800,600);
    #endif
    return list;    
}

QList<QSize> S60VideoCaptureSession::supportedVideoResolutions(const QVideoEncoderSettings &settings)
{
    //qDebug() << "S60VideoCaptureSession::supportedVideoResolutions(const QVideoEncoderSettings &settings) START";     
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
    }else {
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

    #ifdef Q_CC_NOKIAX86
    supportedFrameSizes << QSize(50, 50);
    supportedFrameSizes << QSize(100, 100);
    supportedFrameSizes << QSize(800,600);
    #endif
    return supportedFrameSizes;
}

void S60VideoCaptureSession::setCameraHandle(CCameraEngine* camerahandle)
{
    m_cameraEngine = NULL;
    m_cameraEngine = camerahandle;
    resetSession();
}

void S60VideoCaptureSession::resetSession()
{
    //qDebug() << "S60VideoCaptureSession::resetCamera START";
    delete m_videoRecorder;
    m_videoRecorder = NULL;   
    m_error = KErrNone;    
    TRAPD(err, m_videoRecorder = CVideoRecorderUtility::NewL(*this));    
    setError(err);    
    updateVideoCaptureContainers();  
    TRAP(err, doInitializeVideoRecorderL());
    setError(err);    
    //qDebug() << "S60VideoCaptureSession::resetCamera END";
}

QList<qreal> S60VideoCaptureSession::supportedVideoFrameRates()
{
    //qDebug() << "S60VideoCaptureSession::supportedVideoResolutions";
    QList<qreal> list = QList<qreal>();
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        
        foreach (QString codecName, formatMap().keys()) {
            int codecIndex = formatMap().value(codecName);
            CCamera::TFormat format = static_cast<CCamera::TFormat>( codecIndex );    
            TUint32 videoFormatsSupported = m_info.iVideoFrameFormatsSupported;
    
            if (videoFormatsSupported&format) {
                CCamera *camera = m_cameraEngine->Camera();
                for (int i=0; i < m_info.iNumVideoFrameRatesSupported; i++) {
                    TReal32 rate;
                    TInt maxSizeIndex=0;
                    camera->EnumerateVideoFrameRates(rate, i,format, maxSizeIndex);                    
                    if (!list.contains(rate)) {
                        if (rate > 0.0)
                            list << rate;
                    }
                }
            }
        }
    }
    #ifdef Q_CC_NOKIAX86
    list << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
    #endif    
    return list;
}

QList<qreal> S60VideoCaptureSession::supportedVideoFrameRates(const QVideoEncoderSettings &settings)
{
    //qDebug() << "S60VideoCaptureSession::supportedVideoFrameRates(const QVideoEncoderSettings &settings) START";    
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
       
    #ifdef Q_CC_NOKIAX86
    supportedFrameRates << 30.0 << 25.0 << 15.0 << 10.0 << 5.0;
    #endif
    return supportedFrameRates;    
}

bool S60VideoCaptureSession::setOutputLocation(const QUrl &sink)
{
    //qDebug() << "S60VideoCaptureSession::setOutputlocation";
    m_sink = sink;
    return true;
}

QUrl S60VideoCaptureSession::outputLocation() const
{
    //qDebug() << "S60VideoCaptureSession::outputLocation";
    return m_sink;
}

qint64 S60VideoCaptureSession::position()
{
    //qDebug() << "S60VideoCaptureSession::position";
    qint64 position = 0;    
    if ( (m_captureState == ERecording) && m_videoRecorder) {
        TRAPD(err, position = m_videoRecorder->DurationL().Int64() / 1000);
        setError(err);
    }
    return position;
}

void S60VideoCaptureSession::commitVideoEncoderSettings()
{          
    doSetVideoResolution(m_videoSettings.resolution());    
    doSetFrameRate(m_videoSettings.frameRate());    
    doSetBitrate(m_videoSettings.bitRate());    
}

void S60VideoCaptureSession::setVideoFrameRateFixed(bool fixed)
{
#ifndef PRE_S60_50_PLATFORM 
    TRAPD(err, m_videoRecorder->SetVideoFrameRateFixedL(fixed));
    setError(err);    
#endif //PRE_S60_50_PLATFORM
}

void S60VideoCaptureSession::saveVideoEncoderSettings(QVideoEncoderSettings &videoSettings)
{   
    m_videoSettings = videoSettings;    
}

void S60VideoCaptureSession::getCurrentVideoEncoderSettings(QVideoEncoderSettings &videoSettings)
{
    videoSettings = m_videoSettings;
}

void S60VideoCaptureSession::saveAudioEncoderSettings(QAudioEncoderSettings &audioSettings)
{   
    m_audioSettings = audioSettings;    
}

void S60VideoCaptureSession::getCurrentAudioEncoderSettings(QAudioEncoderSettings &audioSettings)
{
    audioSettings = m_audioSettings;
}

QtMedia::EncodingQuality S60VideoCaptureSession::videoCaptureQuality() const
{  
#ifndef PRE_S60_50_PLATFORM
    if (m_videoQuality == EVideoQualityLow) 
        return QtMedia::LowQuality;
    else if (m_videoQuality == EVideoQualityNormal)
        return QtMedia::NormalQuality;
    else if (m_videoQuality == EVideoQualityHigh)
        return QtMedia::HighQuality;
    else if (m_videoQuality == EVideoQualityLossless)
        return QtMedia::VeryHighQuality;        
    else
#endif //PRE_S60_50_PLATFORM        
    return QtMedia::VeryLowQuality;       
}

void S60VideoCaptureSession::setVideoCaptureQuality(QtMedia::EncodingQuality quality)
{            
    m_videoQuality = quality*KSymbianImageQualityCoefficient;
#ifndef PRE_S60_50_PLATFORM        
    TRAPD(err, m_videoRecorder->SetVideoQualityL(m_videoQuality));
    setError(err);
#endif //PRE_S60_50_PLATFORM
}

void S60VideoCaptureSession::startRecording()
{
    //qDebug()<< "S60VideoCaptureSession::startRecording START, m_captureState ="<<m_captureState;            
    if (m_captureState == EInitialized || m_captureState == ERecordComplete) {
        QString filename = QDir::toNativeSeparators(m_sink.toString());
        TPtrC16 sink(reinterpret_cast<const TUint16*>(filename.utf16()));
        
        int cameraHandle = m_cameraEngine->Camera()->Handle();
    
        TUid controllerUid(TUid::Uid(m_videoControllerMap[m_container].controllerUid));
        TUid formatUid(TUid::Uid(m_videoControllerMap[m_container].formatUid));        
        
        TPtrC16 str(reinterpret_cast<const TUint16*>(m_videoSettings.codec().utf16()));        
        HBufC8* videocodec(NULL);        
        TRAPD(error, videocodec = CnvUtfConverter::ConvertFromUnicodeToUtf8L(str));
        setError(error);
        CleanupStack::PushL(videocodec);        
        TFourCC audioCodec = m_audioCodeclist[m_audioSettings.codec()];        
        TRAPD(err, m_videoRecorder->OpenFileL(sink, cameraHandle, controllerUid, formatUid, *videocodec, audioCodec));        
        setError(err);
        CleanupStack::PopAndDestroy(videocodec); 
        m_captureState = EInitialized;
        emit stateChanged(m_captureState);
    } else if (m_captureState == EPaused) {
        m_videoRecorder->Record();
        m_captureState = ERecording;        
        emit stateChanged(m_captureState);
    }        
    //qDebug()<< "S60VideoCaptureSession::startRecording END";
}

void S60VideoCaptureSession::pauseRecording()
{
 //qDebug()<< "S60VideoCaptureSession::pauseRecording";
    if (m_captureState == ERecording) {
        TRAPD(err, m_videoRecorder->PauseL());
        setError(err);
        m_captureState = EPaused;
        emit stateChanged(m_captureState);
    } else if (m_captureState == EPaused) {  
        TRAPD(err, m_videoRecorder->Record());
        setError(err);
        m_captureState = ERecording;
        emit stateChanged(m_captureState);
    }
  //  qDebug()<< "S60VideoCaptureSession::pauseRecording - no state changed, current state: "<< m_captureState;
}

void S60VideoCaptureSession::stopRecording()
{
    m_videoRecorder->Stop();
    m_videoRecorder->Close();
    m_captureState = ERecordComplete;
    emit stateChanged(m_captureState);
}

bool S60VideoCaptureSession::queryCurrentCameraInfo()
{
    //qDebug() << "S60VideoCaptureSession::queryCameraInfo";    
    bool returnValue = false;

    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {
            camera->CameraInfo(m_info);
        }
        returnValue = true;
    }
    return returnValue;
}

QMap<QString, int> S60VideoCaptureSession::formatMap()
{
    QMap<QString, int> formats;
    // format list copied from ecam.h CCamera::TFormat
  
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

QStringList S60VideoCaptureSession::supportedVideoCaptureCodecs(){   
      
    return m_videoCodeclist;  
}

QStringList S60VideoCaptureSession::supportedAudioCaptureCodecs()
{      
    QStringList keys = m_audioCodeclist.keys();
    keys.sort();      
    return keys;
}

bool S60VideoCaptureSession::isSupportedVideoCaptureCodec(const QString &codecName)
{
    return m_videoCodeclist.contains(codecName);
}

QString S60VideoCaptureSession::audioCaptureCodec()
{
    return m_audioSettings.codec();
}

QString S60VideoCaptureSession::videoCaptureCodec()
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
    return QString();
}

int S60VideoCaptureSession::bitrate()
{
    return m_videoSettings.bitRate();
}

void S60VideoCaptureSession::setBitrate(const int &bitrate)
{
    m_videoSettings.setBitRate(bitrate);
}

void S60VideoCaptureSession::doSetBitrate(const int &bitrate)
{
    if (m_videoRecorder) {           
        TRAPD(err, m_videoRecorder->SetVideoBitRateL(bitrate));
        qDebug() << "S60VideoCaptureSession::doSetBitrate, error: " << err;
        setError(err);
    }
}

QSize S60VideoCaptureSession::videoResolution()
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
        qDebug() << "S60VideoCaptureSession::doSetVideoResolution, error: " << err;
        setError(err);
    }
}

qreal S60VideoCaptureSession::framerate()
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
            qDebug() << "S60VideoCaptureSession::doSetFrameRate, error: " << err;
            setError(err);
        }
    }
}

void S60VideoCaptureSession::initializeVideoCaptureSettings()
{
    if (m_videoCodeclist.count() > 0)
        m_videoSettings.setCodec(m_videoCodeclist[0]); // Setting the first codec found as initial value    
    if (m_audioCodeclist.keys().count() > 0)
        m_audioSettings.setCodec(m_audioCodeclist.keys()[0]); // Setting the first codec found as initial value    
     // set lowest value found as initial value for frame rate
    QList<qreal> rates = supportedVideoFrameRates();
    qreal minRate = 30.0;
    foreach (qreal rate, rates)
        minRate = qMin(minRate, rate);
    m_videoSettings.setFrameRate(minRate);    
    
    QSize minResolution(176, 144);
    m_videoSettings.setResolution(minResolution);
    
    // use variable bit rate as initial value
    m_videoSettings.setBitRate(KMMFVariableVideoBitRate);
    
    m_videoSettings.setQuality(QtMedia::LowQuality);
}

void S60VideoCaptureSession::MvruoOpenComplete(TInt aError)
{
    //qDebug() << "S60VideoCaptureSession::MvruoOpenComplete, error: " << aError;
    if (aError==KErrNone) {
        if (m_captureState == EInitializing) {
            TRAPD(err, doPopulateVideoCodecsL());
            TRAP(err, doPopulateAudioCodecsL());              
            setError(err);
            m_videoRecorder->Close();
            initializeVideoCaptureSettings();
            m_captureState = EInitialized;
        }else {
            commitVideoEncoderSettings();
            m_captureState = EOpenCompelete;
            m_videoRecorder->Prepare();
            emit stateChanged(m_captureState);
        }
    }else {
        m_captureState = EInitialized;
        emit stateChanged(m_captureState);
    }
    setError(aError);
}

void S60VideoCaptureSession::MvruoPrepareComplete(TInt aError)
{
    //qDebug() << "S60VideoCaptureSession::MvruoPrepareComplete, error: " << aError;
    if(aError==KErrNone) {
        m_videoRecorder->Record();
        m_captureState = ERecording;
        emit stateChanged(m_captureState);
    //    qDebug() << "S60VideoCaptureSession::MvruoPrepareComplete: Record called";
    } else {
        m_captureState = ENotInitialized;
        emit stateChanged(m_captureState);
    }
    setError(aError);
}

void S60VideoCaptureSession::MvruoRecordComplete(TInt aError)
{
    //qDebug() << "S60VideoCaptureSession::MvruoRecordComplete";
    if((aError==KErrNone) || (aError==KErrCompletion)) {
        m_videoRecorder->Stop();
        m_videoRecorder->Close();
    }
    m_captureState = ERecordComplete;
    emit stateChanged(m_captureState);
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

QString S60VideoCaptureSession::videoContainer()
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
    return m_videoControllerMap.keys().contains(containerName)?
        m_videoControllerMap[containerName].formatDescription:QString();
}

void S60VideoCaptureSession::doPopulateAudioCodecsL()
{    
    if (m_captureState == EInitializing) {
        m_audioCodeclist.clear();
        
        RArray<TFourCC> audioTypes;
        CleanupClosePushL(audioTypes);
        m_videoRecorder->GetSupportedAudioTypesL(audioTypes);
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
        m_videoRecorder->GetSupportedVideoTypesL(*videoTypes);
        for (TInt i = 0; i < videoTypes->Count(); i++) {
            TPtrC8 videoType = videoTypes->MdcaPoint(i);
            m_videoCodeclist << QString::fromUtf8((char *)videoType.Ptr(), videoType.Length());            
        }
        CleanupStack::PopAndDestroy(videoTypes);
    }
}

void S60VideoCaptureSession::doInitializeVideoRecorderL()
{
    if (m_videoRecorder) {
        int cameraHandle = m_cameraEngine->Camera()->Handle();
        
        TUid controllerUid(TUid::Uid(m_videoControllerMap[m_videoControllerMap.keys()[0]].controllerUid));
        TUid formatUid(TUid::Uid(m_videoControllerMap[m_videoControllerMap.keys()[0]].formatUid));

        m_captureState = EInitializing;
        m_videoRecorder->OpenFileL(KVideoDummyMpg, cameraHandle, controllerUid, formatUid, KMimeTypeMPEG4VSPL4, KMMFFourCCCodeAMR);
    }
}
