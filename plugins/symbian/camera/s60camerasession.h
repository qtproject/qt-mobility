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

#ifndef S60CAMERASESSION_H
#define S60CAMERASESSION_H

#include <QtCore/qobject.h>
#include <QSocketNotifier>
#include <QTime>
#include <QUrl>
#include <QtGui/qicon.h>

#include "qcamera.h"
#include <QtMultimedia/qvideoframe.h>

#include <e32base.h>
#include <fbs.h>
#include <cameraengine.h>
#include <cameraengineobserver.h>


struct video_buffer {
    void* start;
    size_t length;
};

// CLASS DECLARATION
class MVFProcessor
{
public:
      virtual void ViewFinderFrameReady(const QImage& image) = 0;
};


class S60CameraSession : public QObject, public MCameraEngineObserver
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
    QList<QSize> supportedResolutions();

    // media control
    bool setSink(const QUrl &sink);
    QUrl sink() const;
    qint64 position() const;
    int state() const;

    void setVideoOutput(QWidget* widget);
    
    //added based on s60 camera needs
    void releaseImageBuffer();
    bool startCamera(int index=0);
    void stopCamera();
    void capture();
    
    // for mediacontrol
    void startRecording();
    void pauseRecording();
    void stopRecording();

    //videodevicecontrol
    int deviceCount() const;
    QString name(int index) const;
    QString description(int index) const;
    QIcon icon(int index) const;
    int defaultDevice() const;
    int selectedDevice() const;
    void setSelectedDevice(int index);
    
    //void setDevice(const QString &device);
    
protected:
    void MceoCameraReady();
    void MceoFocusComplete();
    void MceoCapturedDataReady(TDesC8* aData);
    void MceoCapturedBitmapReady(CFbsBitmap* aBitmap);
    void MceoViewFinderFrameReady(CFbsBitmap& aFrame);
    void MceoHandleError(TCameraEngineError aErrorType, TInt aError);
    
private:
    void setVFProcessor(MVFProcessor* VFProcessor);
    bool queryCurrentCameraInfo();
    
Q_SIGNALS:
    void stateChanged(QCamera::State);
    void readyForCaptureChanged(bool);
    void imageCaptured(const QString &fileName, const QImage &preview);

private Q_SLOTS:
    void captureFrame();

private:
    QSocketNotifier *notifier;
    QList<video_buffer> buffers;

    int sfd;
    QTime timeStamp;
    bool available;
    QCamera::State m_state;
    QUrl m_sink;
    QVideoFrame::PixelFormat pixelF;
    QSize m_windowSize;
    QList<QSize> resolutions;
    QList<unsigned int> formats;
    
    //ADDED
    CCameraEngine* iCameraEngine;
    QSize iCaptureSize;
    QSize iViewFinderSize;
    MVFProcessor* iVFProcessor;
    TInt m_deviceIndex; //index indication chosen camera device
    mutable int iError;
    // information about camera
    TCameraInfo m_info;
    
};

#endif
