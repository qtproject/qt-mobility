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

#ifndef S60IMAGECAPTURESESSION_H
#define S60IMAGECAPTURESESSION_H

#include <QtCore/qobject.h>
#include <QTime>
#include <QUrl>
#include <QtGui/qicon.h>
#include <QList>
#include <QtMultimedia/qvideoframe.h>
#include <QMultiMap>

#include "qcamera.h"
#include "qstillimagecapture.h"
#include "s60camerasettings.h"
#include <e32base.h>
#include <s60cameraengine.h>
#include <s60cameraengineobserver.h>

QTM_USE_NAMESPACE

class S60CameraService;

class S60ImageCaptureSession : public QObject,
    public MCameraEngineObserver    
{
    Q_OBJECT
public:

    enum Error {
        NoError = 0,
        OutOfMemoryError,
        InUseError,
        NotReadyError,
        UnknownError = -1
    };
    
    enum EcamErrors {
        KErrECamCameraDisabled = -12100, // The camera has been disabled, hence calls do not succeed 
        KErrECamSettingDisabled = -12101, //  This parameter or operation is supported, but presently is disabled. 
        KErrECamParameterNotInRange = -12102, //  This value is out of range. 
        KErrECamSettingNotSupported = -12103, //  This parameter or operation is not supported. 
        KErrECamNotOptimalFocus = -12104 // The optimum focus is lost  
    };
    
    S60ImageCaptureSession(QObject *parent = 0);
    ~S60ImageCaptureSession();
    
    CCamera::TFormat defaultCodec();
    void setError(TInt aError);
    QCamera::Error fromSymbianErrorToMultimediaError(int aError);
    
    bool deviceReady();
    void setCameraHandle(CCameraEngine* camerahandle);
    void setCurrentDevice(TInt deviceindex);
    
    S60CameraSettings* advancedSettings();
    void deleteAdvancedSettings();
    
/*    QList<QVideoFrame::PixelFormat> supportedPixelFormats();
    QVideoFrame::PixelFormat pixelFormat() const;
    void setPixelFormat(QVideoFrame::PixelFormat fmt);*/    

    //added based on s60 camera needs
    void releaseImageBuffer();    
    void capture(const QString &fileName);     
   
    //imageencodercontrol
    QSize captureSize() const;
    QSize minimumCaptureSize();
    QSize maximumCaptureSize();
    QList<QSize> supportedCaptureSizesForCodec(const QString &codecName);
    void setCaptureSize(const QSize &size);
    QStringList supportedImageCaptureCodecs();
    QString imageCaptureCodec();
    void setImageCaptureCodec(const QString &codecName);
    QString imageCaptureCodecDescription(const QString &codecName);
    QtMedia::EncodingQuality captureQuality() const;
    void setCaptureQuality(QtMedia::EncodingQuality);
    void updateImageCaptureCodecs();

    //camerafocuscontrol
    void startFocus();
    void cancelFocus();
    int maximumZoom();
    int minZoom();
    int maxDigitalZoom();
    void setZoomFactor(qreal optical, qreal digital);
    int zoomFactor();
    int digitalZoomFactor();

    //cameraexposurecontrol
    void setFlashMode(QCamera::FlashModes mode);
    void setExposureMode(QCamera::ExposureMode mode);
    QCamera::ExposureMode exposureMode();
    QCamera::ExposureModes supportedExposureModes();
    QCamera::FlashModes supportedFlashModes();
    QCamera::FlashMode flashMode();
    
    //cameraimageprocessingcontrol
    int contrast() const;
    void setContrast(int value);
    QCamera::WhiteBalanceMode whiteBalanceMode();
    void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode);
    QCamera::WhiteBalanceModes supportedWhiteBalanceModes();

protected: // From MCameraEngineObserver
    void MceoCameraReady();
    void MceoFocusComplete();
    void MceoCapturedDataReady(TDesC8* aData);
    void MceoCapturedBitmapReady(CFbsBitmap* aBitmap);
    void MceoViewFinderFrameReady(CFbsBitmap& aFrame);
    void MceoHandleError(TCameraEngineError aErrorType, TInt aError);

private:
    bool queryCurrentCameraInfo();
    QMap<QString, int> formatMap();
    QMap<QString, int> formatDescMap();
    
    void setWhiteBalanceModeL(QCamera::WhiteBalanceMode mode);    
    void resetSession();
    void setFlashModeL(QCamera::FlashModes mode);
    void setExposureModeL(QCamera::ExposureMode mode);
    void saveImageL(TDesC8* aData, TFileName aPath);
    TFileName imagePath();
    
Q_SIGNALS:    
    void stateChanged(QCamera::State);
    // for capture control
    void error(int error, const QString &errorString);
    void readyForCaptureChanged(bool);
    void imageCaptured(const QString &fileName, const QImage &preview);
    void imageSaved(const QString &fileName);
    //for focuscontrol
    void focusStatusChanged(QCamera::FocusStatus);
    void opticalZoomChanged(qreal opticalZoom);
    void digitalZoomChanged(qreal digitalZoom);   
    void advancedSettingCreated();

private:
    CCameraEngine *m_cameraEngine;
    S60CameraSettings *m_advancedSettings;
    int m_imageQuality;    
    QSize m_captureSize;
    //QCamera::State m_state;    
    TInt m_deviceIndex; //index indication chosen camera device
//    QVideoFrame::PixelFormat m_pixelF;
    mutable int m_error;
    CCamera::TFormat m_currentcodec; 
    //QList<QSize> m_resolutions;
    QList<uint> m_formats;    
    QString m_stillCaptureFileName;
    mutable TCameraInfo m_info; // information about camera                
};

#endif
