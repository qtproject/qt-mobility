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

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>
#include <ecamadvsettings.h>   // CCameraAdvancedSettings
#include "s60camerasession.h"
#include "s60viewfinderwidget.h"

#include <fbs.h>
#include <qglobal.h>
const int KSymbianImageQualityCoefficient = 25;
CCamera::TFormat KSymbianDefaultImageCodec = CCamera::EFormatExif;
#ifdef Q_CC_NOKIAX86
_LIT8(KCameraTemp,"test data");
#endif

S60CameraSession::S60CameraSession(QObject *parent)
    : QObject(parent)
    , m_cameraEngine(NULL)
    , m_VFProcessor(NULL)
    , m_imageQuality(QtMedia::NormalQuality*KSymbianImageQualityCoefficient)
    , m_captureSize(QSize(1600, 1200))
    , m_state(QCamera::StoppedState)
    , m_windowSize(QSize(320/2, 240/2))
    , m_pixelF(QVideoFrame::Format_RGB24)
    , m_deviceIndex(NULL)
    , m_error(NoError)
    , m_currentcodec(KSymbianDefaultImageCodec)
{

}

S60CameraSession::~S60CameraSession()
{
    delete m_cameraEngine;
    m_cameraEngine = NULL;
}
void S60CameraSession::resetCamera()
{
    delete m_cameraEngine;
    m_cameraEngine = NULL;
    m_error = KErrNone;
    m_state = QCamera::StoppedState;
    qDebug() << "S60CameraSession::resetCamera. Creating new camera with index=" << m_deviceIndex;
    QT_TRAP_THROWING(m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this));
    Q_CHECK_PTR(m_cameraEngine);

}

void S60CameraSession::startCamera()
{
    qDebug() << "S60CameraSession::startCamera";
    #ifdef Q_CC_NOKIAX86
    MceoCameraReady(); // signal that we are ready
    #endif
    
    // create camera engine
    resetCamera();

    if (!m_error ) {
        m_cameraEngine->ReserveAndPowerOn();
        updateImageCaptureCodecs();
    }
    if (m_error != KErrNone)
        emit error(m_error, QString(m_error));

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
void S60CameraSession::capture()
{
    qDebug() << "S60CameraSession::capture";
    m_error = KErrNone;
    if (m_cameraEngine) {
        TSize size(m_captureSize.width(), m_captureSize.height());
        TRAP(m_error, m_cameraEngine->PrepareL(size, m_currentcodec));
        if (m_error == KErrNotSupported)
            emit error(QCamera::NotSupportedFeatureError, QLatin1String("unable to prepare picture size and current codec"));
        TRAP(m_error, m_cameraEngine->CaptureL());
        if (m_error == KErrNotReady || m_error == KErrNoMemory)
            emit error(QCamera::NotReadyToCaptureError, QLatin1String("camera is not reserved or prepared for capture"));
    }
    else {
        m_error = KErrNotReady;
        emit error(QCamera::NotReadyToCaptureError, QLatin1String("camera is not started or no engine available"));
    }
    #ifdef Q_CC_NOKIAX86
    QImage *snapImage = new QImage(QLatin1String("C:/Data/testimage.jpg"));
    emit imageCaptured(m_sink.toLocalFile(), *snapImage);
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

int S60CameraSession::framerate() const
{
    return -1;
}

void S60CameraSession::setFrameRate(int rate)
{
    Q_UNUSED(rate)
    
}

int S60CameraSession::brightness() const
{

    return -1;
}

void S60CameraSession::setBrightness(int b)
{
    Q_UNUSED(b);
}

int S60CameraSession::contrast() const
{
    return -1;
}

void S60CameraSession::setContrast(int c)
{
    Q_UNUSED(c);
}

int S60CameraSession::saturation() const
{
    return -1;
}

void S60CameraSession::setSaturation(int s)
{
    Q_UNUSED(s);
}

int S60CameraSession::hue() const
{
    return -1;
}

void S60CameraSession::setHue(int h)
{
    Q_UNUSED(h)
}

int S60CameraSession::sharpness() const
{
    return -1;
}

void S60CameraSession::setSharpness(int s)
{
    Q_UNUSED(s)
}

int S60CameraSession::zoom() const
{
    return -1;
}

void S60CameraSession::setZoom(int z)
{
    Q_UNUSED(z)
}

bool S60CameraSession::backlightCompensation() const
{
    return false;
}

void S60CameraSession::setBacklightCompensation(bool b)
{
    Q_UNUSED(b)
}

int S60CameraSession::whitelevel() const
{
    return -1;
}

void S60CameraSession::setWhitelevel(int w)
{
    Q_UNUSED(w)
}

int S60CameraSession::rotation() const
{
    return 0;
}

void S60CameraSession::setRotation(int r)
{
    Q_UNUSED(r)
}

bool S60CameraSession::flash() const
{
    return false;
}

void S60CameraSession::setFlash(bool f)
{
    Q_UNUSED(f)
}

bool S60CameraSession::autofocus() const
{
    qDebug() << "S60CameraSession::autofocus";
    return false;
}

void S60CameraSession::setAutofocus(bool f)
{
    qDebug() << "S60CameraSession::setAutofocus, autofocus=" << f;
    Q_UNUSED(f)
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
    return m_timeStamp.elapsed();
}

int S60CameraSession::state() const
{
    qDebug() << "S60CameraSession::state";
    return m_state;
}

void S60CameraSession::startRecording()
{
    qDebug() << "S60CameraSession::startRecording";
    /*
     * Capture Video: Gets the video frame size and video frame rate for the index passed by calling 
     * the CCamera::EnumerateVideoFrameSizes() function and the CCamera::EnumerateVideoFrameRates() 
     * functions respectively. The video details such as frame sizes and frame rate index are passed 
     * to the CCamera::PrepareVideoCaptureL () function to allocate the memory for the video to be captured.
     *  Then, a call to the CCamera::StartVideoCapture() starts capturing the video and a call to 
     *  the CCamera::StopVideoCapture() stops capturing the video.
     */
}

void S60CameraSession::pauseRecording()
{
    qDebug() << "S60CameraSession::pauseRecording";
}

void S60CameraSession::stopRecording()
{
    qDebug() << "S60CameraSession::stopRecording";
}

void S60CameraSession::captureFrame()
{
    qDebug() << "S60CameraSession::captureFrame";
    capture();

}
void S60CameraSession::MceoCameraReady()
{
    qDebug() << "S60CameraSession::MCeoCameraReady()";
    m_state = QCamera::ActiveState;
    emit stateChanged(m_state);
    if (m_cameraEngine) {
        m_VFSize =  TSize(m_VFWidgetSize.width(), m_VFWidgetSize.height());
        m_error = KErrNone;
        TRAP(m_error, m_cameraEngine->StartViewFinderL(m_VFSize));
        if (m_error == KErrNotReady || m_error == KErrNoMemory) {
            emit readyForCaptureChanged(false);
            emit error(QCamera::NotReadyToCaptureError, QLatin1String("viewfinding with bitmaps is not supported"));
            
        }
        emit readyForCaptureChanged(true);
    }
}

void S60CameraSession::MceoFocusComplete()
{
    qDebug() << "S60CameraSession::MCeoFocusComplete()";
    emit focusLocked();
}

void S60CameraSession::MceoCapturedDataReady(TDesC8* aData)
{
    QImage snapImage = QImage::fromData((const uchar *)aData->Ptr(), aData->Length());
    emit imageCaptured(m_sink.toLocalFile(), snapImage);
    releaseImageBuffer();
}

void S60CameraSession::releaseImageBuffer()
{
    if (m_cameraEngine)
        m_cameraEngine->ReleaseImageBuffer();
}

void S60CameraSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    if(aBitmap)
    {
        
        TSize size = aBitmap->SizeInPixels();
        TUint32 sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, aBitmap->DisplayMode()) / sizeof( TUint32 );

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
        
        aBitmap = NULL;

        emit imageCaptured(m_sink.toLocalFile(), *snapImage);
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
    Q_UNUSED(aErrorType);
    //EErrAutoFocusMode (-5)
    m_error = aError;
    QString errorString = QLatin1String("camera engine errorcode:") + aErrorType;
    emit error(aError,errorString);
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
    qDebug() << "S60CameraSession::updateImageCaptureCodecs";
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

/*
 * Queries advanced settings information
 * Results are returned to member variable m_advancedSettings
 * @return boolean indicating if querying the info was a success
 */
bool S60CameraSession::queryAdvancedSettingsInfo()
{
    qDebug() << "S60CameraSession::queryAdvancedSettingsInfo";

    bool returnValue = false;

    if (m_cameraEngine) {
        qDebug() << "m_cameraEngine->AdvancedSettings";
        m_advancedSettings = m_cameraEngine->AdvancedSettings();   
        returnValue = true;
    }
    return returnValue;
}

void S60CameraSession::setZoomFactor(int value)
{
    qDebug() << "S60CameraSession::setZoomFactor, value: " << value;

	if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        if (camera) {
			if (value > m_info.iMaxZoom && value <= m_info.iMaxDigitalZoom) { // digitalzoom
				TRAPD(m_error, camera->SetDigitalZoomFactorL(value));
				qDebug() << "S60CameraSession::setDigitalZoomFactor error: " << m_error;
				if (m_error == KErrNone) {
					emit zoomValueChanged(value);
				}
			} else if (value >= m_info.iMinZoom && value <= m_info.iMaxZoom) { //opticalzoom
				TRAPD(m_error, camera->SetZoomFactorL(value));
				qDebug() << "S60CameraSession::setZoomFactor error: " << m_error;
				if (m_error == KErrNone) {
					emit zoomValueChanged(value);
				}
			}
        }
	}
}

int S60CameraSession::zoomFactor()
{
    qDebug() << "S60CameraSession::zoomFactor";
	int factor = 0;
	if (m_cameraEngine && queryCurrentCameraInfo()) {
        factor = m_cameraEngine->DigitalZoom();
	}
	return factor;
}

void S60CameraSession::startFocus()
{
    qDebug() << "S60CameraSession::startFocus";
    
	if (m_cameraEngine) {
		TRAPD(m_error, m_cameraEngine->StartFocusL());
	}
	if (m_error) {
	// TODO:
	}
}

void S60CameraSession::cancelFocus()
{
    qDebug() << "S60CameraSession::cancelFocus";
    
	m_cameraEngine->FocusCancel();
}

int S60CameraSession::maximumZoom()
{
    qDebug() << "S60CameraSession::maximumZoom";
    
	if (queryCurrentCameraInfo()) {
        qDebug() << "S60CameraSession::maximumZoom value: " << m_info.iMaxZoom;
		return m_info.iMaxZoom;
	}
	else {
		return 0; 
	}
}

int S60CameraSession::minZoom()
{
    qDebug() << "S60CameraSession::minZoom";
    
    if (queryCurrentCameraInfo()) {
        qDebug() << "S60CameraSession::minZoom value: " << m_info.iMinZoom;
        return m_info.iMinZoom;
    }
    else {
        return 0; 
    }
}

int S60CameraSession::maxDigitalZoom()
{
    qDebug() << "S60CameraSession::maxDigitalZoom";
    
	if (queryCurrentCameraInfo()) {
        qDebug() << "S60CameraSession::maxDigitalZoom value: " << m_info.iMaxDigitalZoom;
		return m_info.iMaxDigitalZoom;
		
	}
	else {
		return 0; 
	}
}

void S60CameraSession::setFocusMode(QCamera::FocusMode mode)
{
    if (queryAdvancedSettingsInfo()) {
        switch(mode) {
            case QCamera::ManualFocus: // Manual focus mode
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeManual);
               // m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff);
                break;
            case QCamera::AutoFocus: // One-shot auto focus mode
                qDebug() << "set auto";
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
                break;
            case QCamera::ContinuousFocus: // Continuous auto focus mode 
                qDebug() << "set auto continuous";
                m_advancedSettings->SetFocusMode(CCamera::CCameraAdvancedSettings::EFocusModeAuto);
                m_advancedSettings->SetAutoFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous);
                break;
            case QCamera::InfinityFocus:
            case QCamera::HyperfocalFocus:
                break;

        }      
    }
}

QCamera::FocusMode S60CameraSession::focusMode()
{
    if (queryAdvancedSettingsInfo()) {
        CCamera::CCameraAdvancedSettings::TFocusMode mode = m_advancedSettings->FocusMode();
        switch(mode) {
            case CCamera::CCameraAdvancedSettings::EFocusModeManual:
                return QCamera::ManualFocus;
                break;
            case CCamera::CCameraAdvancedSettings::EFocusModeAuto:
                qDebug() << "CCamera::CCameraAdvancedSettings::EFocusModeAuto";
                CCamera::CCameraAdvancedSettings::TAutoFocusType type = m_advancedSettings->AutoFocusType();
                if (type == CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle) {
                    qDebug() << "CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle";
                    return QCamera::AutoFocus;
                }
                else if (type == CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous) {
                    qDebug() << "CCamera::CCameraAdvancedSettings::EAutoFocusTypeContinuous";
                    return QCamera::ContinuousFocus;
                }
                break;

        }      
    }
}
