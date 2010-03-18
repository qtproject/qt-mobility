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

#ifndef QGSTREAMERCAPTURESESSION_MAEMO_H
#define QGSTREAMERCAPTURESESSION_MAEMO_H

#include <qmediarecordercontrol.h>

#include <QtCore/qurl.h>

#include <gst/gst.h>

#include "qgstreamerbushelper.h"

QTM_USE_NAMESPACE

class QGstreamerMessage;
class QGstreamerBusHelper;
class QGstreamerAudioEncode;
class QGstreamerVideoEncode;
class QGstreamerImageEncode;
class QGstreamerRecorderControl;
class QGstreamerMediaContainerControl;
class QGstreamerCameraExposureControl;
class QGstreamerCameraFocusControl;
class QGstreamerImageProcessingControl;

class QGstreamerElementFactory
{
public:
    virtual GstElement *buildElement() = 0;
    virtual void prepareWinId() {}
};

class QGstreamerVideoInput : public QGstreamerElementFactory
{
public:
    virtual QList<qreal> supportedFrameRates(const QSize &frameSize = QSize()) const = 0;
    virtual QList<QSize> supportedResolutions(qreal frameRate = -1) const = 0;
};

class QGstreamerCaptureSession : public QObject, public QGstreamerSyncEventFilter
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_ENUMS(State)
    Q_ENUMS(CaptureMode)

public:
    enum CaptureMode { Audio = 1, Video = 2, AudioAndVideo = Audio | Video };
    enum State { StoppedState, PreviewState, PausedState, RecordingState };

    QGstreamerCaptureSession(CaptureMode captureMode, QObject *parent);
    ~QGstreamerCaptureSession();

    CaptureMode captureMode() const { return m_captureMode; }

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl& sink);

    QGstreamerAudioEncode *audioEncodeControl() const { return m_audioEncodeControl; }
    QGstreamerVideoEncode *videoEncodeControl() const { return m_videoEncodeControl; }
    QGstreamerImageEncode *imageEncodeControl() const { return m_imageEncodeControl; }

    QGstreamerRecorderControl *recorderControl() const { return m_recorderControl; }
    QGstreamerMediaContainerControl *mediaContainerControl() const { return m_mediaContainerControl; }

    QGstreamerElementFactory *audioInput() const { return m_audioInputFactory; }
    void setAudioInput(QGstreamerElementFactory *audioInput);

    QGstreamerElementFactory *audioPreview() const { return m_audioPreviewFactory; }
    void setAudioPreview(QGstreamerElementFactory *audioPreview);

    QGstreamerVideoInput *videoInput() const { return m_videoInputFactory; }
    void setVideoInput(QGstreamerVideoInput *videoInput);

    QGstreamerElementFactory *videoPreview() const { return m_videoPreviewFactory; }
    void setVideoPreview(QGstreamerElementFactory *videoPreview);
    
    State state() const;
    qint64 duration() const;

    bool processSyncMessage(const QGstreamerMessage &message);

signals:
    void stateChanged(QGstreamerCaptureSession::State state);
    void durationChanged(qint64 duration);
    void error(int error, const QString &errorString);

public slots:
    void setState(QGstreamerCaptureSession::State);
    void setCaptureDevice(const QString &deviceName);
    void setMetaData(const QMap<QByteArray, QVariant>&);

private slots:
    void busMessage(const QGstreamerMessage &message);

private:
    enum PipelineMode { EmptyPipeline, PreviewPipeline, RecordingPipeline, PreviewAndRecordingPipeline };

    void setupCameraBin();
    GstElement *buildVideoSrc();
    void buildAudioEncodeBin();

    QUrl m_sink;
    QString m_captureDevice;
    State m_state;
    State m_pendingState;
    bool m_waitingForEos;
    PipelineMode m_pipelineMode;
    QGstreamerCaptureSession::CaptureMode m_captureMode;
    QMap<QByteArray, QVariant> m_metaData;

    QGstreamerElementFactory *m_audioInputFactory;
    QGstreamerElementFactory *m_audioPreviewFactory;
    QGstreamerVideoInput *m_videoInputFactory;
    QGstreamerElementFactory *m_videoPreviewFactory;

    QGstreamerAudioEncode *m_audioEncodeControl;
    QGstreamerVideoEncode *m_videoEncodeControl;
    QGstreamerImageEncode *m_imageEncodeControl;
    QGstreamerRecorderControl *m_recorderControl;
    QGstreamerMediaContainerControl *m_mediaContainerControl;
    QGstreamerCameraExposureControl *m_cameraExposureControl;
    QGstreamerCameraFocusControl *m_cameraFocusControl;
    QGstreamerImageProcessingControl *m_imageProcessingControl;

    QGstreamerBusHelper *m_busHelper;
    GstBus* m_bus;
    GstElement *m_pipeline;
    GstElement *m_videoSrc;
    bool m_videoPreviewFactoryHasChanged;

    GstElement *m_audioSrc;
    GstElement *m_audioConvert;
    GstElement *m_capsFilter;
    GstElement *m_fileSink;
    GstElement *m_audioEncoder;
public:
    QString m_imageFileName;
};

#endif // QGSTREAMERCAPTURESESSION_MAEMO_H
