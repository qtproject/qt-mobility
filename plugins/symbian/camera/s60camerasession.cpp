/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qdebug.h>
#include <QWidget>

#include "s60camerasession.h"
//#include "s60videowidget.h"
#include <fbs.h>

S60CameraSession::S60CameraSession(QObject *parent)
    :QObject(parent)
{
    available = false;
    resolutions.clear();
    formats.clear();
    m_state = QCamera::StoppedState;

    m_windowSize = QSize(320,240);
    pixelF = QVideoFrame::Format_RGB24;
}

S60CameraSession::~S60CameraSession()
{
    delete iCameraEngine;
}
bool S60CameraSession::startCamera(int index)
{
    /*
     * Try to start camera. Boolean indicating status of openging returned.
     */
    iCameraEngine = CCameraEngine::NewL(index, 0, this);
    iCameraEngine->ReserveAndPowerOn();
    iError = KErrNone;
    iIndex = index;
    return (iError == KErrNone);
}
void S60CameraSession::capture()
{
    
    /**
     * Capture image: Gets the image size for the index passed by calling the CCamera::EnumerateCaptureSizes() function. 
     * The image details such as image format and the size index are passed to the CCamera::PrepareImageCaptureL() 
     * function to allocate the memory for the image to be captured. 
     * Then, a call to the CCamera::CaptureImage() captures the image.
     */
    if (iCameraEngine)
    {
        TSize size(iCaptureSize.width(), iCaptureSize.height());
        if(iIndex == 0)
        {
            iCameraEngine->PrepareL(size);
            iCameraEngine->CaptureL();
        }
        else
        {
            iCameraEngine->PrepareL(size, CCamera::EFormatFbsBitmapColor64K);
            iCameraEngine->CaptureL();
        }
    }
}

bool S60CameraSession::deviceReady()
{
    if ( iCameraEngine )
        return iCameraEngine->IsCameraReady();
    else
        return EFalse;
}

void S60CameraSession::setVideoOutput(QWidget* widget)
{
    qWarning()<<"Testingtesting";
//    m_output = qobject_cast<S60VideoWidget*>(widget);
//    m_output->setBaseSize(m_windowSize);
    //m_output->setFrameSize(m_windowSize);
    qWarning()<<widget->size();
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

}

int S60CameraSession::contrast() const
{

    return -1;
}

void S60CameraSession::setContrast(int c)
{

}

int S60CameraSession::saturation() const
{

    return -1;
}

void S60CameraSession::setSaturation(int s)
{

}

int S60CameraSession::hue() const
{
    return -1;
}

void S60CameraSession::setHue(int h)
{

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

void S60CameraSession::setDevice(const QString &device)
{
    available = true;
    //FIXME: Symbian camerawrapper does not support naming of devices
    
//    m_state = QCamera::StoppedState;
//    m_device = QByteArray(device.toLocal8Bit().constData());

}

QList<QVideoFrame::PixelFormat> S60CameraSession::supportedPixelFormats()
{
    QList<QVideoFrame::PixelFormat> list;

    return list;
}

QVideoFrame::PixelFormat S60CameraSession::pixelFormat() const
{
    return pixelF;
}

void S60CameraSession::setPixelFormat(QVideoFrame::PixelFormat fmt)
{
    pixelF = fmt;
}

QList<QSize> S60CameraSession::supportedResolutions()
{
    QList<QSize> list;

    if(available) {
        list << resolutions;
    }

    return list;
}

bool S60CameraSession::setSink(const QUrl &sink)
{
    m_sink = sink;

    return true;
}

QUrl S60CameraSession::sink() const
{
    return m_sink;
}

qint64 S60CameraSession::position() const
{
    return timeStamp.elapsed();
}

int S60CameraSession::state() const
{
    if (iCameraEngine ) {
        if (iCameraEngine->State() > 0 )
            return QCamera::ActiveState;
        else
            return QCamera::StoppedState;
    }
}
void S60CameraSession::record()
{
    capture();
}

void S60CameraSession::pause()
{
       
}

void S60CameraSession::stop()
{
    
    iCameraEngine->StopViewFinder();
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
    TSize windowSize (m_windowSize.width(), m_windowSize.height());
    iCameraEngine->StartViewFinderL(windowSize);
}

void S60CameraSession::MceoFocusComplete()
{
    //TODO: focus operation completed, emit signal
    // add method for this
}

void S60CameraSession::MceoCapturedDataReady(TDesC8* aData)
{
    //emit q->captureCompleted(QByteArray::fromRawData((const char *)aData->Ptr(), aData->Length()));
}

void S60CameraSession::releaseImageBuffer()
{
    iCameraEngine->ReleaseImageBuffer();
}

void S60CameraSession::MceoCapturedBitmapReady(CFbsBitmap* aBitmap)
{
    if(aBitmap)
    {
        
        TSize size = aBitmap->SizeInPixels();
        TUint32 sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength( size.iWidth, aBitmap->DisplayMode()) / sizeof( TUint32 );

        TUint32* pixelData = new TUint32[ sizeInWords ];

        if ( !pixelData )
            return;

        // convert to QImage
        aBitmap->LockHeap();
        TUint32* dataPtr = aBitmap->DataAddress();
        memcpy(pixelData, dataPtr, sizeof(TUint32) * sizeInWords);
        aBitmap->UnlockHeap();

        TDisplayMode displayMode = aBitmap->DisplayMode();  

        QImage::Format format;
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
                User::Leave( -1 );
                break;
        }

        QImage *snapImage = new QImage(
                (uchar*)pixelData,
                size.iWidth,
                size.iHeight,
                CFbsBitmap::ScanLineLength( size.iWidth, aBitmap->DisplayMode() ),
                format );
        
        aBitmap = NULL;
        
    }
}

void S60CameraSession::MceoViewFinderFrameReady(CFbsBitmap& aFrame)
{
    if (iVFProcessor)
    {
        int bytesPerLine = aFrame.ScanLineLength(iViewFinderSize.width(),
            aFrame.DisplayMode());
        QImage image((uchar *)aFrame.DataAddress(), iViewFinderSize.width(), 
            iViewFinderSize.height(), bytesPerLine, QImage::Format_RGB32);
        iVFProcessor->ViewFinderFrameReady(image);     
        iCameraEngine->ReleaseViewFinderBuffer();
    }
}

void S60CameraSession::MceoHandleError(TCameraEngineError /*aErrorType*/,
    TInt aError)
{   
    iError = aError;
    //emit q->error(error());
}

void S60CameraSession::setVFProcessor(MVFProcessor* VFProcessor)
{
    iVFProcessor = VFProcessor;
}
