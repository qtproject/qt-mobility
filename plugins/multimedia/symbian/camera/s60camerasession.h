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

#ifndef S60CAMERASESSION_H
#define S60CAMERASESSION_H

#include <QtCore/qobject.h>
#include <QTime>
#include <QUrl>
#include <QtGui/qicon.h>
#include <QList>
#include <QtMultimedia/qvideoframe.h>
#include <QMultiMap>

#include "qcamera.h"
#include "s60camerasettings.h"
#include <e32base.h>
#include <cameraengine.h>
#include <cameraengineobserver.h>

#include <VideoRecorder.h>

QTM_USE_NAMESPACE

class MVFProcessor

{
public:
      virtual void ViewFinderFrameReady(const QImage& image) = 0;
};

struct VideoControllerData
{
	int controllerUid;
	int formatUid;
	QString formatDescription;
};

class S60CameraSession : public QObject, 
    public MCameraEngineObserver, public MVideoRecorderUtilityObserver
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
    
    S60CameraSession(QObject *parent = 0);
    ~S60CameraSession();

    bool deviceReady();

    // camera image properties
    int framerate() const;
    void setFrameRate(int rate);
    int brightness() const;
    void setBrightness(int b);
    int contrast() const;
    void setContrast(int c);
    int saturation() const;
    void setSaturation(int s);
    int hue() const;
    void setHue(int h);
    int sharpness() const;
    void setSharpness(int s);
    int zoom() const;
    void setZoom(int z);
    bool backlightCompensation() const;
    void setBacklightCompensation(bool);
    int whitelevel() const;
    void setWhitelevel(int w);
    int rotation() const;
    void setRotation(int r);
    bool flash() const;
    void setFlash(bool f);
    bool autofocus() const;
    void setAutofocus(bool f);

    QSize frameSize() const;
    void setFrameSize(const QSize& s);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats();
    QVideoFrame::PixelFormat pixelFormat() const;
    void setPixelFormat(QVideoFrame::PixelFormat fmt);
    QList<QSize> supportedVideoResolutions();


    // media control
    bool setOutputLocation(const QUrl &sink);
    QUrl outputLocation() const;
    qint64 position() const;
    int state() const;

    //added based on s60 camera needs
    void releaseImageBuffer();
    void startCamera();
    void stopCamera();
    void capture();
    
    
    // for mediacontrol
    void startRecording();
    void pauseRecording();
    void stopRecording();

    //videodevicecontrol
    static int deviceCount();
    static QString name(const int index);
    static QString description(const int index);
    QIcon icon(int index) const;
    int defaultDevice() const;
    int selectedDevice() const;
    void setSelectedDevice(int index);
    
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

    void setVideoRenderer(QObject *renderer);
    void updateImageCaptureCodecs();

    QStringList supportedVideoCaptureCodecs();
    void updateVideoCaptureCodecs();
    QString videoCaptureCodec();
    void setVideoCaptureCodec(const QString &codecName);
    
    //camerafocuscontrol
    void startFocus();
    void cancelFocus();
    int maximumZoom();
    int minZoom();
    int maxDigitalZoom();
    void setZoomFactor(int value);
    int zoomFactor();
    void setFocusMode(QCamera::FocusMode mode);
    QCamera::FocusMode focusMode();
    QCamera::FocusModes supportedFocusModes();
    
    //cameraexposurecontrol
    void setFlashMode(QCamera::FlashMode mode);
    void setExposureMode(QCamera::ExposureMode mode);
    QCamera::ExposureModes supportedExposureModes();
    QCamera::FlashModes supportedFlashModes();
    void setExposureCompensation(qreal ev);
    qreal exposureCompensation();
    QCamera::MeteringMode meteringMode();
    void setMeteringMode(QCamera::MeteringMode mode);
    QCamera::MeteringModes supportedMeteringModes();
    int isoSensitivity();
    QList<int> supportedIsoSensitivities();
    void setManualIsoSensitivity(int iso);
    qreal aperture();
    QList<qreal> supportedApertures(bool *continuous);
    void setManualAperture(qreal aperture);
    void lockExposure(bool lock);
    bool isExposureLocked();
    
    
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

    void resetCamera();

    //from  MVideoRecorderUtilityObserver
  	void MvruoOpenComplete(TInt aError);  
   	void MvruoPrepareComplete(TInt aError);
   	void MvruoRecordComplete(TInt aError);
   	void MvruoEvent(const TMMFEvent& aEvent);
    
Q_SIGNALS:
    void stateChanged(QCamera::State);
    // for capture control
    void error(int error, const QString &errorString);
    void readyForCaptureChanged(bool);
    void imageCaptured(const QString &fileName, const QImage &preview);
    void imageSaved(const QString &fileName);
    //for focuscontrol
    void focusLocked();
    void zoomValueChanged(qreal value);
    
    void exposureLocked();
    void flashReady(bool ready);
    void apertureChanged(qreal aperture);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal speed);
    void isoSensitivityChanged(int iso);

   

private Q_SLOTS:
    void captureFrame();

private:
    CCameraEngine *m_cameraEngine;
    S60CameraSettings *m_advancedSettings;
    MVFProcessor *m_VFProcessor;
    int m_imageQuality;
    QSize m_captureSize;
    QCamera::State m_state;
    QSize m_windowSize;
    QVideoFrame::PixelFormat m_pixelF;
    TInt m_deviceIndex; //index indication chosen camera device
    mutable int m_error;
    CCamera::TFormat m_currentcodec;
    QTime m_timeStamp;
    QUrl m_sink;
    QList<QSize> m_resolutions;
    QList<uint> m_formats;   
    QSize m_VFWidgetSize;
    TSize m_VFSize;

    mutable TCameraInfo m_info; // information about camera
    
    CVideoRecorderUtility* m_videoUtility;
    QHash<QString, VideoControllerData> m_videoControllerMap;
    QString m_videoCodec;

};

#endif
