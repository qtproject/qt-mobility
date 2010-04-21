/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGSTREAMERCAPTURESESSION_H
#define QGSTREAMERCAPTURESESSION_H

#include <qmediarecordercontrol.h>
#include <qmediarecorder.h>

#include <QtCore/qurl.h>

#include <gst/gst.h>

#include "qgstreamerbushelper.h"

QT_USE_NAMESPACE

class QGstreamerMessage;
class QGstreamerBusHelper;
class QGstreamerAudioEncode;
class QGstreamerVideoEncode;
class QGstreamerRecorderControl;
class QGstreamerMediaContainerControl;

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

    void dumpGraph(const QString &fileName);

    void setMetaData(const QMap<QByteArray, QVariant>&);

private slots:
    void busMessage(const QGstreamerMessage &message);

private:
    enum PipelineMode { EmptyPipeline, PreviewPipeline, RecordingPipeline, PreviewAndRecordingPipeline };

    GstElement *buildEncodeBin();
    GstElement *buildAudioSrc();
    GstElement *buildAudioPreview();
    GstElement *buildVideoSrc();
    GstElement *buildVideoPreview();

    void waitForStopped();
    bool rebuildGraph(QGstreamerCaptureSession::PipelineMode newMode);

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
    QGstreamerRecorderControl *m_recorderControl;
    QGstreamerMediaContainerControl *m_mediaContainerControl;

    QGstreamerBusHelper *m_busHelper;
    GstBus* m_bus;
    GstElement *m_pipeline;

    GstElement *m_audioSrc;
    GstElement *m_audioTee;
    GstElement *m_audioPreviewQueue;
    GstElement *m_audioPreview;

    GstElement *m_videoSrc;
    GstElement *m_videoTee;
    GstElement *m_videoPreviewQueue;
    GstElement *m_videoPreview;

    GstElement *m_imageCaptureBin;

    GstElement *m_encodeBin;

public:
    bool m_passImage;
    bool m_passPrerollImage;
    QString m_imageFileName;
};

#endif // QGSTREAMERCAPTURESESSION_H
