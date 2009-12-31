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

#include "s60camerasession.h"
#include "s60viewfinderwidget.h"

#include <fbs.h>

S60CameraSession::S60CameraSession(QObject *parent)
    : QObject(parent)
    , m_cameraEngine(NULL)
    , m_VFProcessor(NULL)
    , m_quality(QtMedia::NormalQuality)
    , m_captureSize(QSize(1600, 1200))
    , m_state(QCamera::StoppedState)
    , m_windowSize(QSize(320/2, 240/2))
    , m_pixelF(QVideoFrame::Format_RGB24)
    , m_deviceIndex(0)
    , m_error(NoError)
{
}

S60CameraSession::~S60CameraSession()
{
    delete m_cameraEngine;
    m_cameraEngine = NULL;
}
bool S60CameraSession::startCamera()
{
    if (m_cameraEngine) {
        delete m_cameraEngine;
        m_cameraEngine = NULL;
        m_error = KErrNone;
    }

    TRAP(m_error, m_cameraEngine = CCameraEngine::NewL(m_deviceIndex, 0, this));
    if (!m_error)
        m_cameraEngine->ReserveAndPowerOn();
	
    return (m_error == KErrNone);
}

void S60CameraSession::stopCamera()
{
    if (m_cameraEngine) {
        m_cameraEngine->ReleaseAndPowerOff();
        emit stateChanged(QCamera::StoppedState);
    }
}
void S60CameraSession::capture()
{
    
    /**
     * Capture image: Gets the image size for the index passed by calling the 
     * CCamera::EnumerateCaptureSizes() function. The image details such as image format and 
     * the size index are passed to the CCamera::PrepareImageCaptureL() function to allocate
     * the memory for the image to be captured. Then, a call to the CCamera::CaptureImage() 
     * captures the image.
     */
    if (m_cameraEngine)
    {
        // TODO: not yet support for ImageEncodeControl
        TSize size(m_captureSize.width(), m_captureSize.height());
        // TODO check supported formats
        if(m_deviceIndex == 0)
        {
            m_cameraEngine->PrepareL(size);
            m_cameraEngine->CaptureL();
        }
        else
        {
            m_cameraEngine->PrepareL(size, CCamera::EFormatFbsBitmapColor64K);
            m_cameraEngine->CaptureL();
        }
    }
}

bool S60CameraSession::deviceReady()
{
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
    return false;
}

void S60CameraSession::setAutofocus(bool f)
{
    Q_UNUSED(f)
}

QSize S60CameraSession::frameSize() const
{
    return m_windowSize;
}

void S60CameraSession::setFrameSize(const QSize& s)
{
    m_windowSize = s;
//    if(m_output)
//        m_output->setFrameSize(s);
}


QList<QVideoFrame::PixelFormat> S60CameraSession::supportedPixelFormats()
{
    QList<QVideoFrame::PixelFormat> list;
    //TODO: add supportedformats 
    return list;
}

QVideoFrame::PixelFormat S60CameraSession::pixelFormat() const
{
    return m_pixelF;
}

void S60CameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    m_pixelF = fmt;
}

QList<QSize> S60CameraSession::supportedVideoResolutions()
{
    QList<QSize> list;
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
		CCamera *camera = m_cameraEngine->Camera();
		for (int i=0; i < m_info.iNumVideoFrameSizesSupported; i++) {
			TSize size;
			// TODO check formats
			camera->EnumerateVideoFrameSizes(size,i, CCamera::EFormatFbsBitmapColor16MU );
			list << QSize(size.iWidth, size.iHeight);
		}
    }
    return list;
}

bool S60CameraSession::setOutputLocation(const QUrl &sink)
{
    m_sink = sink;
    return true;
}

QUrl S60CameraSession::outputLocation() const
{
    return m_sink;
}

qint64 S60CameraSession::position() const
{
    return m_timeStamp.elapsed();
}

int S60CameraSession::state() const
{
    if (m_cameraEngine ) {
        if (m_cameraEngine->State() > 0)
            return QCamera::ActiveState;
    }
    return QCamera::StoppedState;
        
}

void S60CameraSession::startRecording()
{
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
}

void S60CameraSession::stopRecording()
{
}

void S60CameraSession::captureFrame()
{
    capture();

}
void S60CameraSession::MceoCameraReady()
{
    emit stateChanged(QCamera::ActiveState);
    if (m_cameraEngine) {
        m_VFSize =  TSize(m_VFWidgetSize.width(), m_VFWidgetSize.height());
        m_error = KErrNone;
        TRAP(m_error, m_cameraEngine->StartViewFinderL(m_VFSize));
        if (m_error) {
            // TODO add error emitting to cameracontrol
        }

        emit readyForCaptureChanged(true);
    }
}

void S60CameraSession::MceoFocusComplete()
{
    //TODO: focus operation completed, emit signal
    // add method for this
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
}

// For S60Cameravideodevicecontrol
int S60CameraSession::deviceCount()
{
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
            cameraName = QLatin1String("Primary camera");
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
            cameraDesc = QLatin1String("Main camera");
        break;
        case 1:
            cameraDesc = QLatin1String("front camera ");
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
    return QIcon( );
}
int S60CameraSession::defaultDevice() const
{
	const TInt defaultCameraDevice = 0;
    return defaultCameraDevice;
}
int S60CameraSession::selectedDevice() const
{
    return m_deviceIndex;
}
void S60CameraSession::setSelectedDevice(int index)
{
    m_deviceIndex = index;
    m_state = QCamera::StoppedState;
    emit stateChanged( m_state );
}

/*
 * Queries all kinds of camera properties
 * Results are returned to member variable m_info
 * @return boolean indicating if querying the info was a success
 */
bool S60CameraSession::queryCurrentCameraInfo()
{
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
    return QSize();
}
QSize S60CameraSession::minimumCaptureSize() const
{
    return QSize();
}
QSize S60CameraSession::maximumCaptureSize() const
{
    return QSize();
}

void S60CameraSession::setCaptureSize(const QSize &size)
{
    Q_UNUSED(size);
}

QList<QSize> S60CameraSession::supportedCaptureSizes()
{
    QList<QSize> list;
    // if we have cameraengine loaded and we can update camerainfo
    if (m_cameraEngine && queryCurrentCameraInfo()) {
        CCamera *camera = m_cameraEngine->Camera();
        for (int i=0; i < m_info.iNumImageSizesSupported; i++) {
            TSize size;
            // TODO check formats
            camera->EnumerateCaptureSizes(size,i, CCamera::EFormatFbsBitmapColor16MU);
            list << QSize(size.iWidth, size.iHeight);
        }
    }
    return list;
}


QStringList S60CameraSession::supportedImageCaptureCodecs() const
{
    return QStringList();
}
QString S60CameraSession::imageCaptureCodec() const
{
    return QString();
}
bool S60CameraSession::setImageCaptureCodec(const QString &codecName)
{
    Q_UNUSED(codecName);
    return false;
}

QString S60CameraSession::imageCaptureCodecDescription(const QString &codecName) const
{
    Q_UNUSED(codecName);
    return QString();
}

QtMedia::EncodingQuality S60CameraSession::captureQuality() const
{
    return m_quality;
}

void S60CameraSession::setCaptureQuality(QtMedia::EncodingQuality quality)
{
    m_quality = quality;
}

void S60CameraSession::setVideoRenderer(QObject *videoOutput)
{
    S60ViewFinderWidgetControl* viewFinderWidgetControl = 
            qobject_cast<S60ViewFinderWidgetControl*>(videoOutput);

    if (viewFinderWidgetControl) {
        m_VFProcessor = viewFinderWidgetControl->videoWidget();
        m_VFWidgetSize = viewFinderWidgetControl->videoWidget()->size();
    }   
}
