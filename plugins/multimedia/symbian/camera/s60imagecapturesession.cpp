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
#include "S60ImageCaptureSession.h"
#include "s60viewfinderwidget.h"
#include "s60cameraservice.h"

#include <fbs.h>
#include <qglobal.h>
#include <QDir>
#include <pathinfo.h> 

const int KSymbianImageQualityCoefficient = 25;

#ifdef Q_CC_NOKIAX86
_LIT8(KCameraTemp,"test data");
#endif

S60ImageCaptureSession::S60ImageCaptureSession(QObject *parent)
    : QObject(parent)
    , m_cameraEngine(NULL)
    , m_advancedSettings(NULL)
    , m_imageQuality(QtMedia::NormalQuality*KSymbianImageQualityCoefficient)   
    , m_captureSize(QSize())
    , m_deviceIndex(0)    
    , m_error(NoError)    
{   
    // set defaults so that camera works with both devices..
    m_currentcodec = defaultCodec();       
}

S60ImageCaptureSession::~S60ImageCaptureSession()
{       
    delete m_advancedSettings;
    m_advancedSettings = NULL;
}

CCamera::TFormat S60ImageCaptureSession::defaultCodec()
{
    if (m_deviceIndex == 0) 
        return CCamera::EFormatExif;    
    else 
        return CCamera::EFormatFbsBitmapColor64K;    
}

bool S60ImageCaptureSession::deviceReady()
{
    #ifdef Q_CC_NOKIAX86
    //qDebug() << "device ready";
    return true;
    #endif
    if (m_cameraEngine)
        return m_cameraEngine->IsCameraReady();
    else
        return EFalse;
}

void S60ImageCaptureSession::deleteAdvancedSettings()
{    
    delete m_advancedSettings;
    m_advancedSettings = NULL;    
}

void S60ImageCaptureSession::setCameraHandle(CCameraEngine* camerahandle)
{     
    m_cameraEngine = camerahandle;    
    resetSession();    
}

void S60ImageCaptureSession::setCurrentDevice(TInt deviceindex)
{
    m_deviceIndex = deviceindex;
}

void S60ImageCaptureSession::resetSession()
{
    //qDebug() << "S60ImageCaptureSession::resetSession START";    
    m_error = KErrNone;    
    m_currentcodec = defaultCodec();       
    TRAPD(err, m_advancedSettings = new S60CameraSettings(this, m_cameraEngine);
               m_cameraEngine->SetAdvancedObserver(m_advancedSettings));        
    setError(err);    
    updateImageCaptureCodecs();       
    emit advancedSettingCreated();
    //qDebug() << "S60ImageCaptureSession::resetSession END";
}

S60CameraSettings* S60ImageCaptureSession::advancedSettings()
{
    return m_advancedSettings;
}

void S60ImageCaptureSession::setError(TInt aError)
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

QCamera::Error S60ImageCaptureSession::fromSymbianErrorToMultimediaError(int aError)
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

void S60ImageCaptureSession::capture(const QString &fileName)
{
    //qDebug() << "S60ImageCaptureSession::capture to file="<< fileName;
    m_error = KErrNone;
    m_stillCaptureFileName = fileName;
    emit readyForCaptureChanged(false);

    if (m_stillCaptureFileName.isNull() || m_stillCaptureFileName.isEmpty() ) {
        emit error(QStillImageCapture::ResourceError, QLatin1String("capture outputlocation not set properly"));
        return;
    }
    // check capture size //if not set sets the default one
    setCaptureSize(m_captureSize);

    if (m_cameraEngine) {
        TSize size(m_captureSize.width(), m_captureSize.height());
        TRAPD(err, 
                m_cameraEngine->PrepareL(size, m_currentcodec);
                m_cameraEngine->CaptureL();
        );
        setError(err);
    }
    else {
        setError(KErrNotReady);
    }
    #ifdef Q_CC_NOKIAX86
    QImage *snapImage = new QImage(QLatin1String("C:/Data/testimage.jpg"));
    emit imageCaptured(m_stillCaptureFileName, *snapImage);
    #endif
}

void S60ImageCaptureSession::MceoCameraReady()
{
}

void S60ImageCaptureSession::MceoFocusComplete()
{
    //qDebug() << "S60ImageCaptureSession::MCeoFocusComplete()";
    emit focusStatusChanged(QCamera::FocusReached);
}

void S60ImageCaptureSession::MceoCapturedDataReady(TDesC8* aData)
{
    //qDebug() << "S60ImageCaptureSession::MceoCapturedDataReady()";
    QImage snapImage = QImage::fromData((const uchar *)aData->Ptr(), aData->Length());
    //qDebug() << "S60ImageCaptureSession::MceoCapturedDataReady(), image constructed, byte count="<<snapImage.byteCount();
    // inform capture done
    TFileName path = imagePath();
    emit imageCaptured(m_stillCaptureFileName, snapImage);
    // try to save image and inform if it was succcesful
    TRAPD(err, saveImageL(aData, path));
    setError(err);
    emit imageSaved(m_stillCaptureFileName);
    // release image resources
    releaseImageBuffer();
}

TFileName S60ImageCaptureSession::imagePath()
{
    TFileName path = KNullDesC();
    QFileInfo fileInfo(m_stillCaptureFileName);
    QString absolutePath = fileInfo.path();
    QDir dir(absolutePath);    
    if (!dir.exists()) {
        //qDebug() << "S60ImageCaptureSession::imagePath(), path not exist";
        emit error(QStillImageCapture::ResourceError, QLatin1String("folder does not exist"));
        return path;
    }
    TPtrC16 attachmentPath(KNullDesC);
    if (absolutePath.length()>1) { // path is already included in filename       
        attachmentPath.Set(reinterpret_cast<const TUint16*>(QDir::toNativeSeparators(m_stillCaptureFileName).utf16()));      
        path.Append(attachmentPath);
    } else {
        path = PathInfo::PhoneMemoryRootPath(); 
        path.Append(PathInfo::ImagesPath());  
        attachmentPath.Set(reinterpret_cast<const TUint16*>(m_stillCaptureFileName.utf16()));
        path.Append(attachmentPath);
        m_stillCaptureFileName.prepend(QDesktopServices::storageLocation(QDesktopServices::PicturesLocation)+"/");
    }   
    return path;
}

void S60ImageCaptureSession::saveImageL(TDesC8* aData, TFileName aPath) 
{
    //qDebug() << "S60ImageCaptureSession::saveImageL()";
    if (aPath.Size()>0) {
        RFs fs;
        User::LeaveIfError(fs.Connect());
        CleanupClosePushL(fs);
        RFile file;
        User::LeaveIfError(file.Create(fs, aPath, EFileWrite));
        CleanupClosePushL(file);
        User::LeaveIfError(file.Write(*aData));
        
        CleanupStack::PopAndDestroy(&file);
        CleanupStack::PopAndDestroy(&fs);
    }
}

void S60ImageCaptureSession::releaseImageBuffer() 
{
    if (m_cameraEngine)
        m_cameraEngine->ReleaseImageBuffer();
    // inform that we can continue taking more pictures
    emit readyForCaptureChanged(true);
}

void S60ImageCaptureSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    //qDebug() << "S60ImageCaptureSession::MceoCapturedBitmapReady()";
    if(aBitmap)
    {
        TSize size = aBitmap->SizeInPixels();
        TUint32 sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()) / sizeof( TUint32 );
        //qDebug() << "S60ImageCaptureSession::MceoCapturedDataReady(), image constructed";
        TUint32 *pixelData = new TUint32[sizeInWords];

        if (!pixelData)
            return;

        // convert to QImage
        aBitmap->LockHeap();
        TUint32 *dataPtr = aBitmap->DataAddress();
        memcpy(pixelData, dataPtr, sizeof(TUint32) * sizeInWords);
        aBitmap->UnlockHeap();

        TDisplayMode displayMode = aBitmap->DisplayMode();
        //qDebug() << "S60ImageCaptureSession::MceoCapturedBitmapReady(), displaymode: "<<displayMode;

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
        //qDebug() << "S60ImageCaptureSession::MceoCapturedDataReady(), image constructed, byte count="<<snapImage->byteCount();

        TFileName path = imagePath();
        emit imageCaptured(m_stillCaptureFileName, *snapImage);
        TInt err = aBitmap->Save(path);      
        setError(err);
        if (err == KErrNone)
            emit imageSaved(m_stillCaptureFileName);
        aBitmap = NULL;        
        releaseImageBuffer();
    }
    //todo error handling
}

void S60ImageCaptureSession::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{
    Q_UNUSED(aFrame);
}

void S60ImageCaptureSession::MceoHandleError(TCameraEngineError aErrorType, TInt aError)
{
	//qDebug() << "S60ImageCaptureSession::MceoHandleError, errorType"<<aErrorType;
	//qDebug() << "S60ImageCaptureSession::MceoHandleError, aError"<<aError;
    Q_UNUSED(aErrorType);
    setError(aError);
}

/*
 * Queries all kinds of camera properties
 * Results are returned to member variable m_info
 * @return boolean indicating if querying the info was a success
 */
bool S60ImageCaptureSession::queryCurrentCameraInfo()
{
    //qDebug() << "S60ImageCaptureSession::queryCameraInfo";

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

QSize S60ImageCaptureSession::captureSize() const
{
    //qDebug() << "S60ImageCaptureSession::captureSize";
    return m_captureSize;
}

QSize S60ImageCaptureSession::minimumCaptureSize()
{
    //qDebug() << "S60ImageCaptureSession::minimunCaptureSize";
    return supportedCaptureSizesForCodec(formatMap().key(m_currentcodec)).first();
}
QSize S60ImageCaptureSession::maximumCaptureSize()
{
    //qDebug() << "S60ImageCaptureSession::maximumCaptureSize";
    return supportedCaptureSizesForCodec(formatMap().key(m_currentcodec)).last();
}

void S60ImageCaptureSession::setCaptureSize(const QSize &size)
{
    //qDebug() << "S60ImageCaptureSession::setCaptureSizes, size="<<size;
    if (m_captureSize.isNull() || size.isEmpty()) {
        //an empty QSize indicates the encoder should make an optimal choice based on what is
        //available from the image source and the limitations of the codec.
        m_captureSize = supportedCaptureSizesForCodec(formatMap().key(m_currentcodec)).last();
        //qDebug() << "S60ImageCaptureSession::setCaptureSizes, using optimal(last) size="<<m_captureSize;
    }
    else
        m_captureSize = size;
    //qDebug() << "S60ImageCaptureSession::setCaptureSizes, END size="<<size;
}

QList<QSize> S60ImageCaptureSession::supportedCaptureSizesForCodec(const QString &codecName)
{
    //qDebug() << "S60ImageCaptureSession::supportedCaptureSizesForCodec, codec="<<codecName;
    QList<QSize> list;
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera::TFormat format;
        if (codecName == "") {
            format = defaultCodec();
        }
        else {
            int codecIndex = formatMap().value(codecName);
            format = static_cast<CCamera::TFormat>( codecIndex );
        }
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

QMap<QString, int> S60ImageCaptureSession::formatMap()
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
    formats.insert("YUV420Interleaved", 0x0400);
    formats.insert("YUV420Planar", 0x0800);
    formats.insert("YUV422", 0x1000);
    formats.insert("YUV422Reversed", 0x2000);
    formats.insert("YUV444", 0x4000);
    formats.insert("YUV420SemiPlanar", 0x8000);
    formats.insert("FbsBitmapColor16MU", 0x00010000);
    formats.insert("MJPEG", 0x00020000);
    formats.insert("EncodedH264", 0x00040000);

    return formats;
}

QMap<QString, int> S60ImageCaptureSession::formatDescMap()
{
    QMap<QString, int> formats;
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
    formats.insert("YUV420Interleaved", 0x0400);
    formats.insert("YUV420Planar", 0x0800);
    formats.insert("YUV422", 0x1000);
    formats.insert("YUV422Reversed", 0x2000);
    formats.insert("YUV444", 0x4000);
    formats.insert("YUV420SemiPlanar", 0x8000);
    formats.insert("FbsBitmapColor16MU", 0x00010000);
    formats.insert("MJPEG", 0x00020000);
    formats.insert("EncodedH264", 0x00040000);

    return formats;
}

QStringList S60ImageCaptureSession::supportedImageCaptureCodecs()
{
    //qDebug() << "S60ImageCaptureSession::supportedImageCaptureCodecs";
    QStringList list;
    #ifdef Q_CC_NOKIAX86
    return formatMap().keys();
    #endif

    for (int i = 0; i < m_formats.length() ; i++) {
        list << formatMap().key(m_formats.at(i));
    }
    
    //qDebug()<< "S60ImageCaptureSession::supportedImageCaptureCodecs, return formatList.count()="<<list.count();
    return list;
}

void S60ImageCaptureSession::updateImageCaptureCodecs()
{
    qDebug() << "S60ImageCaptureSession::updateImageCaptureCodecs START";
    m_formats.clear();
    if (m_cameraEngine && queryCurrentCameraInfo()) {

        TUint32 supportedFormats = m_info.iImageFormatsSupported;
        
#ifdef PRE_S60_50_PLATFORM
        int maskEnd = CCamera::EFormatFbsBitmapColor16MU;
#else
        int maskEnd = CCamera::EFormatEncodedH264;        
#endif
        for ( int mask = CCamera::EFormatMonochrome; mask <= maskEnd; mask<<=1 ) {
            if ( supportedFormats & mask )
                m_formats << mask; // store mask of supported format
        }
    }
    qDebug() << "S60ImageCaptureSession::updateImageCaptureCodecs END";
}

QString S60ImageCaptureSession::imageCaptureCodec()
{
    //qDebug() << "S60ImageCaptureSession::imageCaptureCodec";
    return formatMap().key(m_currentcodec);
}
void S60ImageCaptureSession::setImageCaptureCodec(const QString &codecName)
{
    //qDebug() << "S60ImageCaptureSession::setImageCaptureCodec, coded="<<codecName;
    m_currentcodec = static_cast<CCamera::TFormat>( formatMap().value(codecName) );
}

QString S60ImageCaptureSession::imageCaptureCodecDescription(const QString &codecName)
{
    //qDebug() << "S60ImageCaptureSession::imageCaptureCodecDescription, codename="<<codecName;
    return formatDescMap().key(formatMap().value(codecName));
}

QtMedia::EncodingQuality S60ImageCaptureSession::captureQuality() const
{
    //qDebug() << "S60ImageCaptureSession::CaptureQuality";
    if (m_imageQuality <= 1) {
        return QtMedia::VeryLowQuality;
    }
    else
        return static_cast<QtMedia::EncodingQuality> (m_imageQuality / KSymbianImageQualityCoefficient);

}

void S60ImageCaptureSession::setCaptureQuality(QtMedia::EncodingQuality quality)
{
    //qDebug() << "S60ImageCaptureSession::setCaptureQuality, EncodingQuality="<<quality;
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

void S60ImageCaptureSession::setZoomFactor(qreal optical, qreal digital)
{
    //qDebug() << "S60ImageCaptureSession::setZoomFactor, value(optical digital): " << optical << digital;

    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {
            if (digital > m_info.iMaxZoom && digital <= m_info.iMaxDigitalZoom) { // digitalzoom
                TRAPD(err, camera->SetDigitalZoomFactorL(digital));
                setError(err);
                //qDebug() << "S60ImageCaptureSession::setDigitalZoomFactor error: " << m_error;
                if (err == KErrNone) {
                    emit digitalZoomChanged(digital);
                }
            } else if (optical >= m_info.iMinZoom && optical <= m_info.iMaxZoom) { //opticalzoom
                TRAPD(err2, camera->SetZoomFactorL(optical));
                setError(err2);
                //qDebug() << "S60ImageCaptureSession::setZoomFactor error: " << m_error;
                if (err2 == KErrNone) {
                    emit opticalZoomChanged(optical);
                }
            }
        }
    }
}

int S60ImageCaptureSession::zoomFactor()
{
    //qDebug() << "S60ImageCaptureSession::zoomFactor";
    int factor = 0;
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        return camera->ZoomFactor();
    }
    return factor;
}

int S60ImageCaptureSession::digitalZoomFactor()
{
    //qDebug() << "S60ImageCaptureSession::digitalZoomFactor";
    int factor = 0;
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        return camera->DigitalZoomFactor();
    }
    return factor;
}

void S60ImageCaptureSession::startFocus()
{
    //qDebug() << "S60ImageCaptureSession::startFocus";
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->StartFocusL());
        setError(err);
    }
}

void S60ImageCaptureSession::cancelFocus()
{
    //qDebug() << "S60ImageCaptureSession::cancelFocus";
    if (m_cameraEngine) {
        TRAPD(err, m_cameraEngine->FocusCancel());
        setError(err);
    }
}

int S60ImageCaptureSession::maximumZoom()
{
    //qDebug() << "S60ImageCaptureSession::maximumZoom";

    if (queryCurrentCameraInfo()) {
        //qDebug() << "S60ImageCaptureSession::maximumZoom value: " << m_info.iMaxZoom;
        return m_info.iMaxZoom;
    } else {
        return 0;
    }
}

int S60ImageCaptureSession::minZoom()
{
    //qDebug() << "S60ImageCaptureSession::minZoom";

    if (queryCurrentCameraInfo()) {
        //qDebug() << "S60ImageCaptureSession::minZoom value: " << m_info.iMinZoom;
        return m_info.iMinZoom;
    } else {
        return 0;
    }
}

int S60ImageCaptureSession::maxDigitalZoom()
{
    //qDebug() << "S60ImageCaptureSession::maxDigitalZoom";
    if (queryCurrentCameraInfo()) {
        //qDebug() << "S60ImageCaptureSession::maxDigitalZoom value: " << m_info.iMaxDigitalZoom;
        return m_info.iMaxDigitalZoom;
    } else {
        return 0;
    }
}

void S60ImageCaptureSession::setFlashMode(QCamera::FlashModes mode)
{
    TRAPD(err, setFlashModeL(mode));
    setError(err);
}

void S60ImageCaptureSession::setFlashModeL(QCamera::FlashModes mode)
{
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        switch(mode) {
        case QCamera::FlashOff:
            camera->SetFlashL(CCamera::EFlashNone);
            break;
        case QCamera::FlashAuto:
            camera->SetFlashL(CCamera::EFlashAuto);
            break;
        case QCamera::FlashOn:
            camera->SetFlashL(CCamera::EFlashForced);
            break;
        case QCamera::FlashRedEyeReduction:
            camera->SetFlashL(CCamera::EFlashRedEyeReduce);
            break;
        case QCamera::FlashFill:
            camera->SetFlashL(CCamera::EFlashFillIn);
            break;
        default:
            break;
        }
    }
}

QCamera::FlashMode S60ImageCaptureSession::flashMode()
{
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        TInt mode = camera->Flash();
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

QCamera::FlashModes S60ImageCaptureSession::supportedFlashModes()
{
  //  qDebug() << "S60ImageCaptureSession::supportedFlashModes()";
    QCamera::FlashModes modes = QCamera::FlashOff;
    if (queryCurrentCameraInfo()) {
        TInt supportedModes = m_info.iFlashModesSupported;
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

QCamera::ExposureMode S60ImageCaptureSession::exposureMode()
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

QCamera::ExposureModes S60ImageCaptureSession::supportedExposureModes()
{
   // qDebug() << "S60ImageCaptureSession::supportedExposureModes()";
    QCamera::ExposureModes modes = QCamera::ExposureAuto;
    if (queryCurrentCameraInfo()) {
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

void S60ImageCaptureSession::setExposureMode(QCamera::ExposureMode mode)
{
    TRAPD(err, setExposureModeL(mode));
    setError(err);
}

void S60ImageCaptureSession::setExposureModeL(QCamera::ExposureMode mode)
{
    //qDebug() << "S60ImageCaptureSession::setExposureModeL()";
    if (m_cameraEngine) {
        CCamera *camera = m_cameraEngine->Camera();
        switch(mode) {
        case QCamera::ExposureManual:
            camera->SetExposureL(CCamera::EExposureManual);
            break;
        case QCamera::ExposureAuto:
            camera->SetExposureL(CCamera::EExposureAuto);
            break;
        case QCamera::ExposureNight:
            camera->SetExposureL(CCamera::EExposureNight);
            break;
        case QCamera::ExposureBacklight:
            camera->SetExposureL(CCamera::EExposureBacklight);
            break;
        case QCamera::ExposureSports:
            camera->SetExposureL(CCamera::EExposureSport);
            break;
        case QCamera::ExposureSnow:
            camera->SetExposureL(CCamera::EExposureSnow);
            break;
        case QCamera::ExposureBeach:
            camera->SetExposureL(CCamera::EExposureBeach);
            break;
        case QCamera::ExposureLargeAperture:
        case QCamera::ExposureSmallAperture:
            //TODO:
            //camera->SetExposureL(CCamera::EExposureAperturePriority);
            break;
        case QCamera::ExposurePortrait:
        case QCamera::ExposureSpotlight:
        default:
            // not supported
            break;
        }
    }
}

int S60ImageCaptureSession::contrast() const
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        return camera->Contrast();
    } else {
        return 0;
    }
}

void S60ImageCaptureSession::setContrast(int value)
{
    if (m_cameraEngine) {
        CCamera* camera = m_cameraEngine->Camera();
        TRAPD(err, camera->SetContrastL(value));
        setError(err);
        }
}

QCamera::WhiteBalanceMode S60ImageCaptureSession::whiteBalanceMode()
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

void S60ImageCaptureSession::setWhiteBalanceMode(QCamera::WhiteBalanceMode mode)
{
    TRAPD(err, setWhiteBalanceModeL(mode));
    setError(err);
}

void S60ImageCaptureSession::setWhiteBalanceModeL(QCamera::WhiteBalanceMode mode)
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

QCamera::WhiteBalanceModes S60ImageCaptureSession::supportedWhiteBalanceModes()
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

