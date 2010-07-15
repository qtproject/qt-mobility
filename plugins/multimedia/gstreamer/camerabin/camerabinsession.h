/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CAMERABINCAPTURESESSION_MAEMO_H
#define CAMERABINCAPTURESESSION_MAEMO_H

#include <qmediarecordercontrol.h>

#include <QtCore/qurl.h>
#include <QtCore/qdir.h>

#include <gst/gst.h>

#include "qgstreamerbushelper.h"

#include "qcamera.h"


class QGstreamerMessage;
class QGstreamerBusHelper;
class CameraBinAudioEncoder;
class CameraBinVideoEncoder;
class CameraBinImageEncoder;
class CameraBinRecorder;
class CameraBinContainer;
class CameraBinExposure;
class CameraBinFocus;
class CameraBinImageProcessing;
class CameraBinLocks;

class QGstreamerElementFactory
{
public:
    virtual GstElement *buildElement() = 0;
    virtual void prepareWinId() {}
};

class QGstreamerVideoInput : public QGstreamerElementFactory
{
public:

};

class CameraBinSession : public QObject, public QGstreamerSyncEventFilter
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
public:
    CameraBinSession(QObject *parent);
    ~CameraBinSession();

    QList< QPair<int,int> > supportedFrameRates(const QSize &frameSize, bool *continuous) const;
    QList<QSize> supportedResolutions( QPair<int,int> rate, bool *continuous) const;

    QCamera::CaptureMode captureMode() { return m_captureMode; }
    void setCaptureMode(QCamera::CaptureMode mode) { m_captureMode = mode; }

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl& sink);    

    QDir defaultDir(QCamera::CaptureMode mode) const;
    QString generateFileName(const QString &prefix, const QDir &dir, const QString &ext) const;

    CameraBinAudioEncoder *audioEncodeControl() const { return m_audioEncodeControl; }
    CameraBinVideoEncoder *videoEncodeControl() const { return m_videoEncodeControl; }
    CameraBinImageEncoder *imageEncodeControl() const { return m_imageEncodeControl; }
    CameraBinExposure *cameraExposureControl() const  { return m_cameraExposureControl; }
    CameraBinFocus *cameraFocusControl() const  { return m_cameraFocusControl; }
    CameraBinImageProcessing *imageProcessingControl() const { return m_imageProcessingControl; }
    CameraBinLocks *cameraLocksControl() const { return m_cameraLocksControl; }

    CameraBinRecorder *recorderControl() const { return m_recorderControl; }
    CameraBinContainer *mediaContainerControl() const { return m_mediaContainerControl; }

    QGstreamerElementFactory *audioInput() const { return m_audioInputFactory; }
    void setAudioInput(QGstreamerElementFactory *audioInput);

    QGstreamerVideoInput *videoInput() const { return m_videoInputFactory; }
    void setVideoInput(QGstreamerVideoInput *videoInput);

    QGstreamerElementFactory *videoPreview() const { return m_viewfinderFactory; }
    void setViewfinder(QGstreamerElementFactory *videoPreview);

    void captureImage(int requestId, const QString &fileName);

    QCamera::State state() const;
    qint64 duration() const;

    void recordVideo();
    void pauseVideoRecording();
    void resumeVideoRecording();
    void stopVideoRecording();

    bool isMuted() const;

    bool processSyncMessage(const QGstreamerMessage &message);
    void processSavedImage(const QString &filename);

signals:
    void stateChanged(QCamera::State state);
    void durationChanged(qint64 duration);
    void error(int error, const QString &errorString);
    void imageExposed(int requestId);
    void imageCaptured(int requestId, const QImage &img);
    void imageSaved(int requestId, const QString &fileName);
    void focusStatusChanged(QCamera::LockStatus status, QCamera::LockChangeReason reason);
    void mutedChanged(bool);

public slots:
    void setDevice(const QString &device);
    void setState(QCamera::State);
    void setCaptureDevice(const QString &deviceName);
    void setMetaData(const QMap<QByteArray, QVariant>&);
    void setMuted(bool);


private slots:
    void busMessage(const QGstreamerMessage &message);

private:
    bool setupCameraBin();
    void setupCaptureResolution();
    void updateVideoSourceCaps();
    GstElement *buildVideoSrc();

    QUrl m_sink;
    QUrl m_actualSink;
    QString m_captureDevice;
    QCamera::State m_state;
    QCamera::State m_pendingState;
    QString m_inputDevice;

    bool m_muted;    
    QCamera::CaptureMode m_captureMode;
    QMap<QByteArray, QVariant> m_metaData;

    QGstreamerElementFactory *m_audioInputFactory;
    QGstreamerVideoInput *m_videoInputFactory;
    QGstreamerElementFactory *m_viewfinderFactory;

    CameraBinAudioEncoder *m_audioEncodeControl;
    CameraBinVideoEncoder *m_videoEncodeControl;
    CameraBinImageEncoder *m_imageEncodeControl;
    CameraBinRecorder *m_recorderControl;
    CameraBinContainer *m_mediaContainerControl;
    CameraBinExposure *m_cameraExposureControl;
    CameraBinFocus *m_cameraFocusControl;
    CameraBinImageProcessing *m_imageProcessingControl;
    CameraBinLocks *m_cameraLocksControl;

    QGstreamerBusHelper *m_busHelper;
    GstBus* m_bus;
    GstElement *m_pipeline;
    GstElement *m_videoSrc;
    bool m_viewfinderFactoryHasChanged;
    bool m_videoInputHasChanged;

    GstCaps *m_sourceCaps;

    GstElement *m_audioSrc;
    GstElement *m_audioConvert;
    GstElement *m_capsFilter;
    GstElement *m_fileSink;
    GstElement *m_audioEncoder;
    GstElement *m_muxer;

public:
    QString m_imageFileName;
    int m_requestId;
};

#endif // CAMERABINCAPTURESESSION_MAEMO_H
