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

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>
#include "s60camerasession.h"
#include "s60viewfinderwidget.h"
#include "s60videoencoder.h"
#include "s60cameraservice.h"

#include <fbs.h>
#include <qglobal.h>
#include <QDir>

const int KSymbianImageQualityCoefficient = 25;

CCamera::TFormat KSymbianDefaultImageCodec = CCamera::EFormatExif;
#ifdef Q_CC_NOKIAX86
_LIT8(KCameraTemp,"test data");
#endif

S60CameraSession::S60CameraSession(QObject *parent)
    : QObject(parent)
    , m_cameraEngine(NULL)
    , m_advancedSettings(NULL)
    , m_VFProcessor(NULL)
    , m_imageQuality(QtMedia::NormalQuality*KSymbianImageQualityCoefficient)
    , m_captureSize(QSize(1600, 1200))
    , m_state(QCamera::StoppedState)
    , m_windowSize(QSize(320/2, 240/2))
    , m_pixelF(QVideoFrame::Format_RGB24)
    , m_deviceIndex(NULL)
    , m_error(NoError)
    , m_currentcodec(KSymbianDefaultImageCodec)
    , m_videoUtility(NULL)
{   
    // create initial camera
    resetCamera();
}

S60CameraSession::~S60CameraSession()
{
    delete m_videoUtility;
    m_videoUtility = NULL;
    delete m_advancedSettings;
    m_advancedSettings = NULL;
    delete m_cameraEngine;
    m_cameraEngine = NULL;
}

S60CameraSettings* S60CameraSession::advancedSettings()
{
    return m_advancedSettings;
}

void S60CameraSession::resetCamera()
{
    qDebug() << "S60CameraSession::resetCamera START";
    delete m_videoUtility;
    m_videoUtility = NULL;
    delete m_advancedSettings;
    m_advancedSettings = NULL;
    delete m_cameraEngine;
    m_cameraEngine = NULL;  
    m_error = KErrNone;
    m_state = QCamera::StoppedState;
    qDebug() << "S60CameraSession::resetCamera. Creating new camera with index=" << m_deviceIndex;
    TRAPD(err, 
        m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this);
        m_advancedSettings = new S60CameraSettings(this, m_cameraEngine);
        m_videoUtility = CVideoRecorderUtility::NewL(*this);
    );
    setError(err);

    updateVideoCaptureCodecs();

    qDebug() << "S60CameraSession::resetCamera END";
}

void S60CameraSession::setError(TInt aError)
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

QCamera::Error S60CameraSession::fromSymbianErrorToMultimediaError(int aError)
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

void S60CameraSession::startCamera()
{
    qDebug() << "S60CameraSession::startCamera START";
    #ifdef Q_CC_NOKIAX86
    MceoCameraReady(); // signal that we are ready
    #endif

    // create camera engine
    resetCamera();

    if (!m_error ) {
        qDebug() << "S60CameraSession::startCamera, ReserveAndPowerOn"<< m_error;
        m_cameraEngine->ReserveAndPowerOn();
        //updateImageCaptureCodecs();
    }
    qDebug() << "S60CameraSession::startCamera END";
}

void S60CameraSession::stopCamera()
{
    qDebug() <<  "Stopping camera";
    m_state = QCamera::StoppedState;

    if (m_cameraEngine) {
        m_cameraEngine->ReleaseAndPowerOff();
    }
    emit stateChanged(m_state);
}

void S60CameraSession::capture(const QString &fileName)
{
    qDebug() << "S60CameraSession::capture to file="<< fileName;
    m_error = KErrNone;
    m_stillCaptureFileName = fileName;
    emit readyForCaptureChanged(false);

    if (m_stillCaptureFileName.isNull() || m_stillCaptureFileName.isEmpty() ) {
        emit error(QStillImageCapture::ResourceError, QLatin1String("capture outputlocation not set properly"));
        return;
    }

    if (m_cameraEngine) {
        TSize size(m_captureSize.width(), m_captureSize.height());
        TRAPD(err, 
                m_cameraEngine->PrepareL(size, m_currentcodec);       
                m_cameraEngine->CaptureL();
        );
        setError(err);
        m_captureSize = QSize(size.iWidth,size.iHeight); // set size according to aquired value.
    }
    else {
        setError(KErrNotReady);
    }
    #ifdef Q_CC_NOKIAX86
    QImage *snapImage = new QImage(QLatin1String("C:/Data/testimage.jpg"));
    emit imageCaptured(m_stillCaptureFileName, *snapImage);
    #endif
}

bool S60CameraSession::deviceReady()
{
    #ifdef Q_CC_NOKIAX86
    qDebug() << "device ready";
    return true;
    #endif
    if ( m_cameraEngine )
        return m_cameraEngine->IsCameraReady();
    else
        return EFalse;
}

int S60CameraSession::framerate()
{
    if (m_videoUtility) {
        int rate = 0;
        TRAPD(err, rate = m_videoUtility->VideoFrameRateL());
        setError(err);
        return rate;
    }
    return -1;
}

void S60CameraSession::setFrameRate(int rate)
{
    if (m_videoUtility) {
        TRAPD(err, m_videoUtility->SetVideoFrameRateL(rate));
        setError(err);
    }
}

QSize S60CameraSession::frameSize() const
{
    qDebug() << "S60CameraSession::frameSize";
    return m_windowSize;
}

void S60CameraSession::setFrameSize(const QSize& s)
{
    qDebug() << "S60CameraSession::setFrameSize, size=" << s;
    m_windowSize = s;
}

QList<QVideoFrame::PixelFormat> S60CameraSession::supportedPixelFormats()
{

    QList<QVideoFrame::PixelFormat> list;
    #ifdef Q_CC_NOKIAX86
    list << QVideoFrame::Format_RGB565;
    #endif
    //TODO: fix supportedformats
    qDebug() << "S60CameraSession::pixeformat, returning="<<list;
    return list;
}

QVideoFrame::PixelFormat S60CameraSession::pixelFormat() const
{
    qDebug() << "S60CameraSession::pixeformat, returning="<<m_pixelF;
    return m_pixelF;
}

void S60CameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    qDebug() << "S60CameraSession::setPixelFormat, format="<<fmt;
    m_pixelF = fmt;
}

QList<QSize> S60CameraSession::supportedVideoResolutions()
{
    qDebug() << "S60CameraSession::supportedVideoResolutions";
    QList<QSize> list;
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        for (int i=0; i < m_info.iNumVideoFrameSizesSupported; i++) {
            TSize size;
            camera->EnumerateVideoFrameSizes(size,i, m_currentcodec );
            list << QSize(size.iWidth, size.iHeight);
        }
    }
    #ifdef Q_CC_NOKIAX86
    list << QSize(50, 50);
    list << QSize(100, 100);
    list << QSize(800,600);
    #endif
    return list;
}

bool S60CameraSession::setOutputLocation(const QUrl &sink)
{
    qDebug() << "S60CameraSession::setOutputlocation";
    m_sink = sink;
    return true;
}

QUrl S60CameraSession::outputLocation() const
{
    qDebug() << "S60CameraSession::outputLocation";
    return m_sink;
}

qint64 S60CameraSession::position() const
{
    qDebug() << "S60CameraSession::position";
    return m_videoUtility->DurationL().Int64() / 1000;
    //return m_timeStamp.elapsed();
}

int S60CameraSession::state() const
{
    qDebug() << "S60CameraSession::state";
    return m_state;
}

void S60CameraSession::commitVideoEncoderSettings()
{
    setVideoCaptureCodec(m_videoSettings.codec());
    setCaptureQuality(m_videoSettings.quality());
    setFrameRate(int(m_videoSettings.frameRate()));
    setBitrate(m_videoSettings.bitRate());
    setVideoResolution(m_videoSettings.resolution());
}

void S60CameraSession::saveVideoEncoderSettings(QVideoEncoderSettings &videoSettings)
{
    m_videoSettings = videoSettings;
}

void S60CameraSession::startRecording()
{
    qDebug() << "S60CameraSession::startRecording";
    QString filename = QDir::toNativeSeparators(m_sink.toString());
    TPtrC16 sink(reinterpret_cast<const TUint16*>(filename.utf16()));

    int cameraHandle = m_cameraEngine->Camera()->Handle();

    TUid controllerUid(TUid::Uid(m_videoControllerMap[m_videoCodec].controllerUid));
    TUid formatUid(TUid::Uid(m_videoControllerMap[m_videoCodec].formatUid));

    qDebug() << "Codec: " << m_videoCodec;
    qDebug() << "Controller Uid: " << m_videoControllerMap[m_videoCodec].controllerUid;
    qDebug() << "Format Uid: " << m_videoControllerMap[m_videoCodec].formatUid;

    QT_TRAP_THROWING(m_videoUtility->OpenFileL(sink, cameraHandle, controllerUid, formatUid));

}

void S60CameraSession::pauseRecording()
{
    qDebug() << "S60CameraSession::pauseRecording";
}

void S60CameraSession::stopRecording()
{
    qDebug() << "S60CameraSession::stopRecording";
    m_videoUtility->Stop();
    m_videoUtility->Close();
}

void S60CameraSession::MceoCameraReady()
{
    qDebug() << "S60CameraSession::MCeoCameraReady()";
    m_state = QCamera::ActiveState;
    emit stateChanged(m_state);
    if (m_cameraEngine) {
        m_VFSize =  TSize(m_VFWidgetSize.width(), m_VFWidgetSize.height());
        TRAPD(err, m_cameraEngine->StartViewFinderL(m_VFSize));
        if (err == KErrNotReady || err == KErrNoMemory) {
            emit readyForCaptureChanged(false);
        }
        setError(err);
        emit readyForCaptureChanged(true);
    }
}

void S60CameraSession::MceoFocusComplete()
{
    qDebug() << "S60CameraSession::MCeoFocusComplete()";
    emit focusStatusChanged(QCamera::FocusReached);
}

void S60CameraSession::MceoCapturedDataReady(TDesC8* aData)
{
    qDebug() << "S60CameraSession::MceoCapturedDataReady()";
    QImage snapImage = QImage::fromData((const uchar *)aData->Ptr(), aData->Length());
    qDebug() << "S60CameraSession::MceoCapturedDataReady(), image constructed, byte count="<<snapImage.byteCount();
    // inform capture done
    emit imageCaptured(m_stillCaptureFileName, snapImage);
    // try to save image and inform if it was succcesful
    if (snapImage.save(m_stillCaptureFileName,0, m_imageQuality))
        emit imageSaved(m_stillCaptureFileName);
    // release image resources
    releaseImageBuffer();
}

void S60CameraSession::releaseImageBuffer()
{
    if (m_cameraEngine)
        m_cameraEngine->ReleaseImageBuffer();
    // inform that we can continue taking more pictures
    emit readyForCaptureChanged(true);
}

void S60CameraSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    qDebug() << "S60CameraSession::MceoCapturedBitmapReady()";
    if(aBitmap)
    {
        TSize size = aBitmap->SizeInPixels();
        TUint32 sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()) / sizeof( TUint32 );
        qDebug() << "S60CameraSession::MceoCapturedDataReady(), image constructed";
        TUint32 *pixelData = new TUint32[sizeInWords];

        if (!pixelData)
            return;

        // convert to QImage
        aBitmap->LockHeap();
        TUint32 *dataPtr = aBitmap->DataAddress();
        memcpy(pixelData, dataPtr, sizeof(TUint32) * sizeInWords);
        aBitmap->UnlockHeap();

        TDisplayMode displayMode = aBitmap->DisplayMode();

        QImage::Format format = QImage::Format_Invalid;
        switch(displayMode)
        {
            case EColor256:
                format = QImage::Format_Indexed8;
                break;
            case EColor4K:
                format = QImage::Format_RGB444;
                break;
            case EColor64K:
                format = QImage::Format_RGB16;
                break;
            case EColor16M:
                format = QImage::Format_RGB666;
                break;
            case EColor16MU:
                format = QImage::Format_RGB32;
                break;
            case EColor16MA:
                format = QImage::Format_ARGB32;
                break;
            default:
                //User::Leave( -1 );
                break;
        }

        QImage *snapImage = new QImage(
                (uchar*)pixelData,
                size.iWidth,
                size.iHeight,
                CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()),
                format);
        qDebug() << "S60CameraSession::MceoCapturedDataReady(), image constructed, byte count="<<snapImage->byteCount();
        aBitmap = NULL;

        emit imageCaptured(m_stillCaptureFileName, *snapImage);
        // try to save image and inform if it was succcesful
        if ( snapImage->save(m_stillCaptureFileName,0, m_imageQuality) )
            emit imageSaved(m_stillCaptureFileName);

        releaseImageBuffer();
    }
    //todo error handling
}

void S60CameraSession::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{
    if (m_VFProcessor) {
        int bytesPerLine = aFrame.ScanLineLength(m_VFSize.iWidth, aFrame.DisplayMode());

        QImage image((uchar *)aFrame.DataAddress(), m_VFSize.iWidth,
                m_VFSize.iHeight, bytesPerLine, QImage::Format_RGB32);

        m_VFProcessor->ViewFinderFrameReady(image);
     }

     m_cameraEngine->ReleaseViewFinderBuffer();
}

void S60CameraSession::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
    qDebug() << "S60CameraSession::MceoHandleError, errorType"<<aErrorType;
    qDebug() << "S60CameraSession::MceoHandleError, aError"<<aError;
    Q_UNUSED(aErrorType);
    setError(aError);
}

// For S60Cameravideodevicecontrol
int S60CameraSession::deviceCount()
{
    qDebug() << "S60CameraSession::deviceCount(for emulator this is always 1)";
    #ifdef Q_CC_NOKIAX86
    return 1;
    #endif
    return CCameraEngine::CamerasAvailable();
}
/**
 * Some names for cameras with index
 */
QString S60CameraSession::name(const int index)
{
    //TODO: change these to use querycurrentcamerainfo
    // From where does the naming index start
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
QString S60CameraSession::description(const int index)
{
    //TODO: change these to use querycurrentcamerainfo
    // what information is wanted throuhg this call?
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
QIcon S60CameraSession::icon(int index) const
{
    // TODO what icons should returned here?
    // \epoc32\release\winscw\udeb\z\resource\apps\camcorder_aif.mif
    Q_UNUSED(index);
    QString filename = QLatin1String("z:\\resource\\apps\\cameraapp_aif.mif");
    return QIcon( filename );
}
int S60CameraSession::defaultDevice() const
{
    //First camera is the default
    const TInt defaultCameraDevice = 0;
    return defaultCameraDevice;
}
int S60CameraSession::selectedDevice() const
{
    qDebug() << "S60CameraSession::selectedDevice returning="<<m_deviceIndex;
    return m_deviceIndex;
}
void S60CameraSession::setSelectedDevice(int index)
{
    qDebug() << "S60CameraSession::setSelectedDevice,index="<<index;
    m_deviceIndex = index;
}

/*
 * Queries all kinds of camera properties
 * Results are returned to member variable m_info
 * @return boolean indicating if querying the info was a success
 */
bool S60CameraSession::queryCurrentCameraInfo()
{
    qDebug() << "S60CameraSession::queryCameraInfo";

    /** Version number and name of camera hardware. */
    //TVersion iHardwareVersion;
    /** Version number and name of camera software (device driver). */
    //TVersion iSoftwareVersion;
    /** Orientation of this particular camera device. */
    //TCameraOrientation iOrientation;
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

// End for S60Cameravideodevicecontrol
QSize S60CameraSession::captureSize() const
{
    qDebug() << "S60CameraSession::captureSize";
    return m_captureSize;
}
QSize S60CameraSession::minimumCaptureSize()
{
    qDebug() << "S60CameraSession::minimunCaptureSize";
    return supportedCaptureSizesForCodec(formatMap().key(m_currentcodec)).first();
}
QSize S60CameraSession::maximumCaptureSize()
{
    qDebug() << "S60CameraSession::maximumCaptureSize";
    return supportedCaptureSizesForCodec(formatMap().key(m_currentcodec)).last();
}

void S60CameraSession::setCaptureSize(const QSize &size)
{
    qDebug() << "S60CameraSession::setCaptureSizes, size="<<size;
    if (size.isEmpty()) {
        //an empty QSize indicates the encoder should make an optimal choice based on what is
        //available from the image source and the limitations of the codec.
        m_captureSize = supportedCaptureSizesForCodec(formatMap().key(m_currentcodec)).last();
    }
    else
        m_captureSize = size;
}

QList<QSize> S60CameraSession::supportedCaptureSizesForCodec(const QString &codecName)
{
    qDebug() << "S60CameraSession::supportedCaptureSizesForCodec, codec="<<codecName;
    QList<QSize> list;
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        int codecIndex = formatMap().value(codecName);
        CCamera::TFormat format = static_cast<CCamera::TFormat>( codecIndex );
        CCamera *camera = m_cameraEngine->Camera();
        for (int i=0; i < m_info.iNumImageSizesSupported; i++) {
            TSize size;
            camera->EnumerateVideoFrameSizes(size,i, format );
            list << QSize(size.iWidth, size.iHeight);
        }
    }
    #ifdef Q_CC_NOKIAX86
    list << QSize(50, 50);
    list << QSize(100, 100);
    list << QSize(800,600);
    #endif
    return list;

}

QMap<QString, int> S60CameraSession::formatMap()
{
    QMap<QString, int> formats;
    // format list copied from ecam.h CCamera::TFormat

    formats.insert("Monochrome",0);
    formats.insert("16bitRGB444",1);
    formats.insert("16bitRGB565",2);
    formats.insert("16bitRGB888",3);
    formats.insert("Jpeg",4);
    formats.insert("ExifJpeg",5);
    formats.insert("FbsBitmapColor4K",6);
    formats.insert("FbsBitmapColor64K",7);
    formats.insert("FbsBitmapColor16M",8);
    formats.insert("UserDefined",9);
    formats.insert("YUV420Interleaved",10);
    formats.insert("YUV420Planar",11);
    formats.insert("YUV422",11);
    formats.insert("YUV422Reversed",12);
    formats.insert("YUV444",13);
    formats.insert("YUV420SemiPlanar",14);
    formats.insert("FbsBitmapColor16MU",15);
    formats.insert("MJPEG",16);
    formats.insert("EncodedH264",17);

    return formats;
}
QMap<QString, int> S60CameraSession::formatDescMap()
{
    QMap<QString, int> formats;
    formats.insert("monochrome",0);
    formats.insert("RGB444",1);
    formats.insert("RGB565",2);
    formats.insert("RGB888",3);
    formats.insert("jpeg",4);
    formats.insert("exifjpeg",5);
    formats.insert("FbsBitmap4K",6);
    formats.insert("FbsBitmap64K",7);
    formats.insert("FbsBitmap16M",8);
    formats.insert("UserDefined",9);
    formats.insert("YUV420Interleaved",10);
    formats.insert("YUV420Planar",11);
    formats.insert("YUV422",11);
    formats.insert("YUV422Reversed",12);
    formats.insert("YUV444",13);
    formats.insert("YUV420SemiPlanar",14);
    formats.insert("FbsBitmapColor16MU",15);
    formats.insert("MJPEG",16);
    formats.insert("EncodedH264",17);

    return formats;
}
QStringList S60CameraSession::supportedImageCaptureCodecs()
{
    qDebug() << "S60CameraSession::supportedImageCaptureCodecs";
    QStringList list;
    #ifdef Q_CC_NOKIAX86
    return formatMap().keys();
    #endif

    for (int i = 0; i < m_formats.length() ; i++) {
        list << formatMap().key(m_formats.at(i));
    }
    qDebug()<< "S60CameraSession::supportedImageCaptureCodecs, return formatList.count()="<<list.count();
    return list;
}
void S60CameraSession::updateImageCaptureCodecs()
{
    m_formats.clear();
    qDebug() << "S60CameraSession::updateImageCaptureCodecs START";
    if (m_cameraEngine && queryCurrentCameraInfo()) {

        TUint32 supportedFormats = m_info.iImageFormatsSupported;
        QStringList allFormats = formatMap().keys();
        int formatMask = 1;

        for ( int i = 0; i < allFormats.count() ; ++i ) {
            if ( supportedFormats & formatMask ) {
                qDebug() << "S60CameraSession::updateImageCaptureCodecs, adding format="<<allFormats.at(i);
                m_formats << i; // store index of supported format
            }

            formatMask <<= 1;
        }
    }
    qDebug() << "S60CameraSession::updateImageCaptureCodecs END";
}

QString S60CameraSession::imageCaptureCodec()
{
    qDebug() << "S60CameraSession::imageCaptureCodec";
    return formatMap().key(m_currentcodec);
}
void S60CameraSession::setImageCaptureCodec(const QString &codecName)
{
    qDebug() << "S60CameraSession::setImageCaptureCodec, coded="<<codecName;
    m_currentcodec = static_cast<CCamera::TFormat>( formatMap().value(codecName) );
}

QString S60CameraSession::imageCaptureCodecDescription(const QString &codecName)
{
    qDebug() << "S60CameraSession::imageCaptureCodecDescription, codename="<<codecName;
    return formatDescMap().key(formatMap().value(codecName));
}

QtMedia::EncodingQuality S60CameraSession::captureQuality() const
{
    qDebug() << "S60CameraSession::CaptureQuality";
    if (m_imageQuality <= 1) {
        return QtMedia::VeryLowQuality;
    }
    else
        return static_cast<QtMedia::EncodingQuality> (m_imageQuality / KSymbianImageQualityCoefficient);

}

void S60CameraSession::setCaptureQuality(QtMedia::EncodingQuality quality)
{
    qDebug() << "S60CameraSession::setCaptureQuality, EncodingQuality="<<quality;
    switch (quality) {
    case QtMedia::VeryLowQuality:
        m_imageQuality = 1;
        break;
    default:
        m_imageQuality = quality * KSymbianImageQualityCoefficient;
        break;
    }
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        camera->SetJpegQuality(m_imageQuality);
        camera = NULL;
    }

}

void S60CameraSession::setVideoRenderer(QObject *videoOutput)
{
    qDebug() << "S60CameraSession::setVideoRenderer, videoOutput="<<videoOutput;
    S60ViewFinderWidgetControl* viewFinderWidgetControl =
            qobject_cast<S60ViewFinderWidgetControl*>(videoOutput);

    if (viewFinderWidgetControl) {
        m_VFProcessor = viewFinderWidgetControl->videoWidget();
        m_VFWidgetSize = viewFinderWidgetControl->videoWidget()->size();
    }
}

void S60CameraSession::setZoomFactor(qreal optical, qreal digital)
{
    qDebug() << "S60CameraSession::setZoomFactor, value(optical digital): " << optical << digital;

    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {
            if (digital > m_info.iMaxZoom && digital <= m_info.iMaxDigitalZoom) { // digitalzoom
                TRAPD(err, camera->SetDigitalZoomFactorL(digital));
                setError(err);
                qDebug() << "S60CameraSession::setDigitalZoomFactor error: " << m_error;
                if (err == KErrNone) {
                    emit digitalZoomChanged(digital);
                }
            } else if (optical >= m_info.iMinZoom && optical <= m_info.iMaxZoom) { //opticalzoom
                TRAPD(err2, camera->SetZoomFactorL(optical));
                setError(err2);
                qDebug() << "S60CameraSession::setZoomFactor error: " << m_error;
                if (err2 == KErrNone) {
                    emit opticalZoomChanged(optical);
                }
            }
        }
    }
}

int S60CameraSession::zoomFactor()
{
    qDebug() << "S60CameraSession::zoomFactor";
    int factor = 0;
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        return camera->ZoomFactor();
    }
    return factor;
}

int S60CameraSession::digitalZoomFactor()
{
    qDebug() << "S60CameraSession::digitalZoomFactor";
    int factor = 0;
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        return camera->DigitalZoomFactor();
    }
    return factor;
}

void S60CameraSession::startFocus()
{
    qDebug() << "S60CameraSession::startFocus";

    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->StartFocusL());
        setError(err);
    }
}

void S60CameraSession::cancelFocus()
{
    qDebug() << "S60CameraSession::cancelFocus";
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->FocusCancel());
        setError(err);
    }
}

int S60CameraSession::maximumZoom()
{
    qDebug() << "S60CameraSession::maximumZoom";

    if (queryCurrentCameraInfo()) {
        qDebug() << "S60CameraSession::maximumZoom value: " << m_info.iMaxZoom;
        return m_info.iMaxZoom;
    } else {
        return 0;
    }
}

int S60CameraSession::minZoom()
{
    qDebug() << "S60CameraSession::minZoom";

    if (queryCurrentCameraInfo()) {
        qDebug() << "S60CameraSession::minZoom value: " << m_info.iMinZoom;
        return m_info.iMinZoom;
    } else {
        return 0;
    }
}

int S60CameraSession::maxDigitalZoom()
{
    qDebug() << "S60CameraSession::maxDigitalZoom";
    if (queryCurrentCameraInfo()) {
        qDebug() << "S60CameraSession::maxDigitalZoom value: " << m_info.iMaxDigitalZoom;
        return m_info.iMaxDigitalZoom;
    } else {
        return 0;
    }
}

void S60CameraSession::setFlashMode(QCamera::FlashMode mode)
{
    if (m_cameraEngine) {
        switch(mode) {
            case QCamera::FlashOff:
                m_cameraEngine->SetFlash(CCamera::EFlashNone);
                break;
            case QCamera::FlashAuto:
                m_cameraEngine->SetFlash(CCamera::EFlashAuto);
                break;
            case QCamera::FlashOn:
                m_cameraEngine->SetFlash(CCamera::EFlashForced);
                break;
            case QCamera::FlashRedEyeReduction:
                m_cameraEngine->SetFlash(CCamera::EFlashRedEyeReduce);
                break;
            case QCamera::FlashFill:
                m_cameraEngine->SetFlash(CCamera::EFlashFillIn);
                break;
            default:
                break;

        }
    }
}

QCamera::FlashMode S60CameraSession::flashMode()
{
    if (m_cameraEngine) {
        TInt mode = m_cameraEngine->Flash();
        switch(mode) {
            case CCamera::EFlashAuto:
                return QCamera::FlashAuto;
            case CCamera::EFlashForced:
                return QCamera::FlashOn;
            case CCamera::EFlashRedEyeReduce:
                return QCamera::FlashRedEyeReduction;
            case CCamera::EFlashFillIn:
                return QCamera::FlashFill;
            default:
                return QCamera::FlashOff;
        }
    }
    return QCamera::FlashOff;
}

QCamera::FlashModes S60CameraSession::supportedFlashModes()
{
    QCamera::FlashModes modes = QCamera::FlashOff;
    if (m_cameraEngine) {
        TInt supportedModes =  m_cameraEngine->SupportedFlashModes();
        if (supportedModes == 0)
            return modes;
        if (supportedModes & CCamera::EFlashManual) {
             modes |= QCamera::FlashOff;
        }
        if (supportedModes & CCamera::EFlashForced) {
             modes |= QCamera::FlashOn;
        }
        if (supportedModes & CCamera::EFlashAuto) {
             modes |= QCamera::FlashAuto;
        }
        if (supportedModes & CCamera::EFlashFillIn) {
             modes |= QCamera::FlashFill;
        }
        if (supportedModes & CCamera::EFlashRedEyeReduce) {
             modes |= QCamera::FlashRedEyeReduction;
        }
    }
    return modes;
}

QCamera::ExposureMode S60CameraSession::exposureMode()
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        CCamera::TExposure mode2 = camera->Exposure();
        switch(mode2) {
            case CCamera::EExposureManual:
                return QCamera::ExposureManual;
            case CCamera::EExposureAuto:
                return QCamera::ExposureAuto;
            case CCamera::EExposureNight:
                return QCamera::ExposureNight;
            case CCamera::EExposureBacklight:
                return QCamera::ExposureBacklight;
            case CCamera::EExposureSport:
                return QCamera::ExposureSports;
            case CCamera::EExposureSnow:
                return QCamera::ExposureSnow;
            case CCamera::EExposureBeach:
                return QCamera::ExposureBeach;
            default:
                return QCamera::ExposureAuto;
        }
    }
    return QCamera::ExposureAuto;
}

QCamera::ExposureModes S60CameraSession::supportedExposureModes()
{
    QCamera::ExposureModes modes = QCamera::ExposureAuto;
    if (m_cameraEngine) {
        TInt supportedModes = m_info.iExposureModesSupported;
        if (supportedModes == 0) {
            return modes;
        }
        if (supportedModes & CCamera::EExposureManual) {
            modes |= QCamera::ExposureManual;
        }
        if (supportedModes & CCamera::EExposureAuto) {
            modes |= QCamera::ExposureAuto;
        }
        if (supportedModes & CCamera::EExposureNight) {
            modes |= QCamera::ExposureNight;
        }
        if (supportedModes & CCamera::EExposureBacklight) {
            modes |= QCamera::ExposureBacklight;
        }
        if (supportedModes & CCamera::EExposureSport) {
            modes |= QCamera::ExposureSports;
        }
        if (supportedModes & CCamera::EExposureSnow) {
            modes |= QCamera::ExposureSnow;
        }
        if (supportedModes & CCamera::EExposureBeach) {
            modes |= QCamera::ExposureBeach;
        }
    }
    return modes;
}

void S60CameraSession::setExposureMode(QCamera::ExposureMode mode)
{
    if (m_cameraEngine) {
        switch(mode) {
            case QCamera::ExposureManual:
                m_cameraEngine->SetExposure(CCamera::EExposureManual);
                break;
            case QCamera::ExposureAuto:
                m_cameraEngine->SetExposure(CCamera::EExposureAuto);
                break;
            case QCamera::ExposureNight:
                m_cameraEngine->SetExposure(CCamera::EExposureNight);
                break;
            case QCamera::ExposureBacklight:
                m_cameraEngine->SetExposure(CCamera::EExposureBacklight);
                break;
            case QCamera::ExposureSports:
                m_cameraEngine->SetExposure(CCamera::EExposureSport);
                break;
            case QCamera::ExposureSnow:
                m_cameraEngine->SetExposure(CCamera::EExposureSnow);
                break;
            case QCamera::ExposureBeach:
                m_cameraEngine->SetExposure(CCamera::EExposureBeach);
                break;
            case QCamera::ExposureLargeAperture:
            case QCamera::ExposureSmallAperture:
                //TODO:
                //m_cameraEngine->SetExposure(CCamera::EExposureAperturePriority);
                break;
            case QCamera::ExposurePortrait:
            case QCamera::ExposureSpotlight:
            default:
                // not supported
                break;
        }
    }
}

qreal S60CameraSession::contrast() const
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        return camera->Contrast();
    } else {
        return 0;
    }
}

void S60CameraSession::setContrast(qreal value)
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        TRAPD(err, camera->SetContrastL(value));
        setError(err);
        }
}


QCamera::WhiteBalanceMode S60CameraSession::whiteBalanceMode()
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        CCamera::TWhiteBalance mode = camera->WhiteBalance();
        switch(mode) {
            case CCamera::EWBAuto:
                return QCamera::WhiteBalanceAuto;
            case CCamera::EWBDaylight:
                return QCamera::WhiteBalanceSunlight;
            case CCamera::EWBCloudy:
                return QCamera::WhiteBalanceCloudy;
            case CCamera::EWBTungsten:
                return QCamera::WhiteBalanceTungsten;
            case CCamera::EWBFluorescent:
                return QCamera::WhiteBalanceFluorescent;
            case CCamera::EWBFlash:
                return QCamera::WhiteBalanceFlash;
            case CCamera::EWBBeach:
                return QCamera::WhiteBalanceSunset;
            case CCamera::EWBManual:
                return QCamera::WhiteBalanceManual;
            case CCamera::EWBShade:
                return QCamera::WhiteBalanceShade;
            default:
                return QCamera::WhiteBalanceAuto;
        } 
    }
    return QCamera::WhiteBalanceAuto;
}

void S60CameraSession::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    TRAPD(err, setWhiteBalanceModeL(mode));
    setError(err);
}

void S60CameraSession::setWhiteBalanceModeL(QCamera::WhiteBalanceMode mode)
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        switch(mode) {
            case QCamera::WhiteBalanceAuto:
                camera->SetWhiteBalanceL(CCamera::EWBAuto);
                break;
            case QCamera::WhiteBalanceSunlight:
                camera->SetWhiteBalanceL(CCamera::EWBDaylight);
                break;
            case QCamera::WhiteBalanceCloudy:
                camera->SetWhiteBalanceL(CCamera::EWBCloudy);
                break;
            case QCamera::WhiteBalanceTungsten:
            case QCamera::WhiteBalanceIncandescent:
                camera->SetWhiteBalanceL(CCamera::EWBTungsten);
                break;
            case QCamera::WhiteBalanceFluorescent:
                camera->SetWhiteBalanceL(CCamera::EWBFluorescent);
                break;
            case QCamera::WhiteBalanceFlash:
                camera->SetWhiteBalanceL(CCamera::EWBFlash);
                break;
            case QCamera::WhiteBalanceSunset:
                camera->SetWhiteBalanceL(CCamera::EWBBeach);
                break;
            case QCamera::WhiteBalanceManual:
                camera->SetWhiteBalanceL(CCamera::EWBManual);
                break;
            case QCamera::WhiteBalanceShade:
                camera->SetWhiteBalanceL(CCamera::EWBShade);
                break;
            default:
                // not supported
                break;
        }
    }
}

QCamera::WhiteBalanceModes S60CameraSession::supportedWhiteBalanceModes()
{
    QCamera::WhiteBalanceModes modes = QCamera::WhiteBalanceAuto;
    if (m_cameraEngine) { 
        TInt supportedModes = m_info.iWhiteBalanceModesSupported;
        if (supportedModes == 0) {
            return modes;
        }
        if (supportedModes & CCamera::EWBAuto) {
            modes |= QCamera::WhiteBalanceAuto;          
        }
        if (supportedModes & CCamera::EWBDaylight) {
            modes |= QCamera::WhiteBalanceSunlight;          
        }
        if (supportedModes & CCamera::EWBCloudy) {
            modes |= QCamera::WhiteBalanceCloudy;          
        }
        if (supportedModes & CCamera::EWBTungsten) {
            modes |= QCamera::WhiteBalanceTungsten;  
            modes |= QCamera::WhiteBalanceIncandescent;
        }
        if (supportedModes & CCamera::EWBFluorescent) {
            modes |= QCamera::WhiteBalanceFluorescent;          
        }
        if (supportedModes & CCamera::EWBFlash) {
            modes |= QCamera::WhiteBalanceFlash;          
        }
        if (supportedModes & CCamera::EWBBeach) {
            modes |= QCamera::WhiteBalanceSunset;          
        }
        if (supportedModes & CCamera::EWBManual) {
            modes |= QCamera::WhiteBalanceManual;          
        }
        if (supportedModes & CCamera::EWBShade) {
            modes |= QCamera::WhiteBalanceShade;          
        }
    }
    return modes;
}

void S60CameraSession::updateVideoCaptureCodecs()
{
    TRAPD(err, updateVideoCaptureCodecsL());
    setError(err);
}

void S60CameraSession::updateVideoCaptureCodecsL()
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
    
    qDebug() << "S60CameraSession::updateVideoCaptureCodecs count: " << m_videoControllerMap.keys().count();
}

QStringList S60CameraSession::supportedVideoCaptureCodecs()
{
    qDebug() << "S60CameraSession::supportedVideoCaptureCodecs";
    return m_videoControllerMap.keys();
}

bool S60CameraSession::isSupportedVideoCaptureCodec(const QString &codecName)
{
    return m_videoControllerMap.keys().contains(codecName);
}

QString S60CameraSession::videoCaptureCodec()
{
    return m_videoCodec;
}
void S60CameraSession::setVideoCaptureCodec(const QString &codecName)
{
    if (codecName == m_videoCodec)
        return;

    m_videoCodec = codecName;
}

QString S60CameraSession::videoCaptureCodecDescription(const QString &codecName)
{
    return m_videoControllerMap[codecName].formatDescription;
}

int S60CameraSession::bitrate()
{
    if (m_videoUtility) {
        TInt rate = 0;
        TRAPD(err, rate = m_videoUtility->VideoBitRateL());
        setError(err);
        return rate;
    }
    return 0;
}

void S60CameraSession::setBitrate(const int &bitrate)
{
    if (m_videoUtility) {
        TRAPD(err, m_videoUtility->SetVideoBitRateL(bitrate));
        setError(err);
    }
}

QSize S60CameraSession::videoResolution()
{
    TSize size(0,0);
    if (m_videoUtility) {
        TRAPD(err, m_videoUtility->GetVideoFrameSizeL(size));
        setError(err);
    }
    return QSize(size.iWidth, size.iHeight);
}

void S60CameraSession::setVideoResolution(const QSize &resolution)
{
    if (m_videoUtility) {
        TSize size(resolution.width(), resolution.height());
        TRAPD(err, m_videoUtility->SetVideoFrameSizeL(size));
        setError(err);
    }
}

void S60CameraSession::MvruoOpenComplete(TInt aError)
{
    qDebug() << "S60CameraSession::MvruoOpenComplete, error: " << aError;

    if(aError==KErrNone) {
        TInt err = KErrNone;
        commitVideoEncoderSettings();
#ifndef PRE_S60_50_PLATFORM
        TRAP(err, m_videoUtility->SetVideoQualityL(m_imageQuality));
        setError(err);
#endif
        m_videoUtility->Prepare();
        // TODO:
        // update recording status
    }
    setError(aError);
}

void S60CameraSession::MvruoPrepareComplete(TInt aError)
{
    qDebug() << "S60CameraSession::MvruoPrepareComplete, error: " << aError;
    if(aError==KErrNone) {
        m_videoUtility->Record();
        qDebug() << "S60CameraSession::MvruoPrepareComplete: Record called";
        
        // TODO:
        // update recording status
    }
    setError(aError);

}

void S60CameraSession::MvruoRecordComplete(TInt aError)
{
    qDebug() << "S60CameraSession::MvruoRecordComplete";
    if((aError==KErrNone) || (aError==KErrCompletion)) {
        m_videoUtility->Stop();
        m_videoUtility->Close();
    }
    setError(aError);

}

void S60CameraSession::MvruoEvent(const TMMFEvent& aEvent)
{

}
