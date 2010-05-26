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

#include "qgstreamercaptureservice.h"
#include "qgstreamercapturesession.h"
#include "qgstreamerrecordercontrol.h"
#include "qgstreamermediacontainercontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamerbushelper.h"
#include "qgstreamerv4l2input.h"
#include "qgstreamercapturemetadatacontrol.h"

#include "qgstreamervideooutputcontrol.h"
#include "qgstreameraudioinputendpointselector.h"
#include "qgstreamervideoinputdevicecontrol.h"

#include "qgstreamervideooverlay.h"
#include "qgstreamervideorenderer.h"

#include "qgstreamervideowidget.h"

#include <qmediaserviceprovider.h>

#include <QtCore/qdebug.h>


class QGstreamerVideoRendererWrapper : public QGstreamerElementFactory
{
public:
    QGstreamerVideoRendererWrapper(QGstreamerVideoRendererInterface *videoRenderer)
        :m_videoRenderer(videoRenderer),
         m_bin(0),
         m_element(0),
         m_colorspace(0)
    {
    }

    virtual ~QGstreamerVideoRendererWrapper()
    {
        if (m_bin)
            gst_object_unref(GST_OBJECT(m_bin));
    }

    GstElement *buildElement()
    {
#ifdef Q_WS_MAEMO_5
        return m_element = m_videoRenderer->videoSink();
#endif
        if (m_bin == NULL) {
            GstBin * bin = GST_BIN(gst_bin_new(NULL));

            m_colorspace = gst_element_factory_make("ffmpegcolorspace", NULL);
            m_element = m_videoRenderer->videoSink();

            gst_bin_add(bin, m_colorspace);
            gst_bin_add(bin, m_element);
            gst_element_link(m_colorspace, m_element);

            // add ghostpads
            GstPad *pad = gst_element_get_static_pad(m_colorspace, "sink");
            gst_element_add_pad(GST_ELEMENT(bin), gst_ghost_pad_new("sink", pad));
            gst_object_unref(GST_OBJECT(pad));

            m_bin = GST_ELEMENT(bin);
        }

        m_videoRenderer->precessNewStream();

        gst_object_ref(GST_OBJECT(m_bin));
        return m_bin;
    }

    void prepareWinId()
    {
        m_videoRenderer->precessNewStream();
    }

private:
    QGstreamerVideoRendererInterface *m_videoRenderer;

    GstElement *m_bin;
    GstElement *m_element;
    GstElement *m_colorspace;
};


QGstreamerCaptureService::QGstreamerCaptureService(const QString &service, QObject *parent):
    QMediaService(parent)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        gst_init(NULL, NULL);
    }

    m_captureSession = 0;
    m_metaDataControl = 0;

    m_videoInput = 0;
    m_audioInputEndpointSelector = 0;
    m_videoInputDevice = 0;

    m_videoOutput = 0;
    m_videoRenderer = 0;
    m_videoRendererFactory = 0;
    m_videoWindow = 0;
    m_videoWindowFactory = 0;
    m_videoWidgetControl = 0;
    m_videoWidgetFactory = 0;

    if (service == Q_MEDIASERVICE_AUDIOSOURCE) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::Audio, this);
    }

    bool captureVideo = false;

    if (captureVideo) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::AudioAndVideo, this);        
        m_videoInput = new QGstreamerV4L2Input(this);
        m_captureSession->setVideoInput(m_videoInput);
        m_videoInputDevice = new QGstreamerVideoInputDeviceControl(this);

        connect(m_videoInputDevice, SIGNAL(selectedDeviceChanged(QString)),
                m_videoInput, SLOT(setDevice(QString)));

        if (m_videoInputDevice->deviceCount())
            m_videoInput->setDevice(m_videoInputDevice->deviceName(m_videoInputDevice->selectedDevice()));

        m_videoOutput = new QGstreamerVideoOutputControl(this);
        connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
                this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));

        m_videoRenderer = new QGstreamerVideoRenderer(this);
        m_videoRendererFactory = new QGstreamerVideoRendererWrapper(m_videoRenderer);
        m_videoWindow = new QGstreamerVideoOverlay(this);
        m_videoWindowFactory = new QGstreamerVideoRendererWrapper(m_videoWindow);

        m_videoWidgetControl = new QGstreamerVideoWidgetControl(this);
        m_videoWidgetFactory = new QGstreamerVideoRendererWrapper(m_videoWidgetControl);

        m_videoOutput->setAvailableOutputs(QList<QVideoOutputControl::Output>()
                                           << QVideoOutputControl::RendererOutput
                                           << QVideoOutputControl::WindowOutput
                                           << QVideoOutputControl::WidgetOutput);
    }

    if (!m_captureSession) {
        qWarning() << "Service type is not supported:" << service;
        return;
    }

    m_audioInputEndpointSelector = new QGstreamerAudioInputEndpointSelector(this);
    connect(m_audioInputEndpointSelector, SIGNAL(activeEndpointChanged(QString)), m_captureSession, SLOT(setCaptureDevice(QString)));

    if (m_captureSession && m_audioInputEndpointSelector->availableEndpoints().size() > 0)
        m_captureSession->setCaptureDevice(m_audioInputEndpointSelector->defaultEndpoint());

    m_metaDataControl = new QGstreamerCaptureMetaDataControl(this);
    connect(m_metaDataControl, SIGNAL(metaDataChanged(QMap<QByteArray,QVariant>)),
            m_captureSession, SLOT(setMetaData(QMap<QByteArray,QVariant>)));
}

QGstreamerCaptureService::~QGstreamerCaptureService()
{
}

QMediaControl *QGstreamerCaptureService::control(const char *name) const
{
    if (!m_captureSession)
        return 0;

    if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutput;

    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRenderer;

    if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindow;

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
        return m_videoWidgetControl;

    if (qstrcmp(name,QAudioEndpointSelector_iid) == 0)
        return m_audioInputEndpointSelector;

    if (qstrcmp(name,QVideoDeviceControl_iid) == 0)
        return m_videoInputDevice;

    if (qstrcmp(name,QMediaRecorderControl_iid) == 0)
        return m_captureSession->recorderControl();

    if (qstrcmp(name,QAudioEncoderControl_iid) == 0)
        return m_captureSession->audioEncodeControl();

    if (qstrcmp(name,QVideoEncoderControl_iid) == 0)
        return m_captureSession->videoEncodeControl();

    if (qstrcmp(name,QMediaContainerControl_iid) == 0)
        return m_captureSession->mediaContainerControl();

    if (qstrcmp(name,QMetaDataControl_iid) == 0)
        return m_metaDataControl;

    return 0;
}

void QGstreamerCaptureService::videoOutputChanged(QVideoOutputControl::Output output)
{
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_captureSession->setVideoPreview(0);
        break;
    case QVideoOutputControl::RendererOutput:
        m_captureSession->setVideoPreview(m_videoRendererFactory);
        break;
    case QVideoOutputControl::WindowOutput:
        m_captureSession->setVideoPreview(m_videoWindowFactory);
        break;
    case QVideoOutputControl::WidgetOutput:
        m_captureSession->setVideoPreview(m_videoWidgetFactory);
        break;
    default:
        qWarning("Invalid video output selection");
        m_captureSession->setVideoPreview(0);
        break;
    }
}
