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


#ifndef QGSTREAMERCAPTURESESSION_H
#define QGSTREAMERCAPTURESESSION_H

#include "qmediarecordercontrol.h"
#include "qmediarecorder.h"

#include <QtCore/qurl.h>

#include <gst/gst.h>

class QGstreamerMessage;
class QGstreamerBusHelper;
class QGstreamerAudioEncode;
class QGstreamerVideoEncode;
class QGstreamerRecorderControl;
class QGstreamerMediaFormatControl;

class QGstreamerElementFactory
{
public:
    virtual GstElement *buildElement() = 0;
    virtual void prepareWinId() {}
};

class QGstreamerCaptureSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
public:
    enum CaptureMode { Audio = 1, Video = 2, AudioAndVideo = Audio | Video };
    enum State { StoppedState, PreviewState, PausedState, RecordingState };

    QGstreamerCaptureSession(CaptureMode captureMode, QObject *parent);
    ~QGstreamerCaptureSession();

    QUrl sink() const;
    bool setSink(const QUrl& sink);

    QGstreamerAudioEncode *audioEncodeControl() const { return m_audioEncodeControl; }
    QGstreamerVideoEncode *videoEncodeControl() const { return m_videoEncodeControl; }
    QGstreamerRecorderControl *recorderControl() const { return m_recorderControl; }
    QGstreamerMediaFormatControl *mediaFormatControl() const { return m_mediaFormatControl; }

    void setAudioInput(QGstreamerElementFactory *audioInput);
    void setAudioPreview(QGstreamerElementFactory *audioPreview);

    void setVideoInput(QGstreamerElementFactory *videoInput);
    void setVideoPreview(QGstreamerElementFactory *videoPreview);

    State state() const;
    qint64 duration() const;

    bool isPreviewEnabled() const { return m_previewEnabled; }

signals:
    void stateChanged(QGstreamerCaptureSession::State state);
    void durationChanged(qint64 duration);
    void error(int error, const QString &errorString);

public slots:
    void setState(QGstreamerCaptureSession::State);
    void setCaptureDevice(const QString &deviceName);

    void dumpGraph(const QString &fileName);
    void enablePreview(bool enabled);

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
    void rebuildGraph(QGstreamerCaptureSession::PipelineMode newMode);

    QUrl m_sink;
    QString m_captureDevice;
    State m_state;
    PipelineMode m_pipelineMode;
    QGstreamerCaptureSession::CaptureMode m_captureMode;
    bool m_previewEnabled;

    QGstreamerElementFactory *m_audioInputFactory;
    QGstreamerElementFactory *m_audioPreviewFactory;
    QGstreamerElementFactory *m_videoInputFactory;
    QGstreamerElementFactory *m_videoPreviewFactory;

    QGstreamerAudioEncode *m_audioEncodeControl;
    QGstreamerVideoEncode *m_videoEncodeControl;
    QGstreamerRecorderControl *m_recorderControl;
    QGstreamerMediaFormatControl *m_mediaFormatControl;

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

    GstElement *m_encodeBin;

};

#endif // QGSTREAMERCAPTURESESSION_H
