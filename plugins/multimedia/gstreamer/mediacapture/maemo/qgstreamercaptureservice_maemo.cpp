/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgstreamercaptureservice_maemo.h"
#include "qgstreamercapturesession_maemo.h"
#include "qgstreamerrecordercontrol_maemo.h"
#include "qgstreamermediacontainercontrol_maemo.h"
#include "qgstreameraudioencode_maemo.h"
#include "qgstreamervideoencode_maemo.h"
#include "qgstreamerbushelper.h"
#include "qgstreamercapturemetadatacontrol_maemo.h"

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
    m_cameraControl = 0;
    m_metaDataControl = 0;

    m_audioInputEndpointSelector = 0;
    m_videoInputDevice = 0;

    m_videoRenderer = 0;
    m_videoRendererFactory = 0;
    m_videoWindow = 0;
    m_videoWindowFactory = 0;
    m_videoWidgetControl = 0;
    m_videoWidgetFactory = 0;
    m_imageCaptureControl = 0;


    if (service == Q_MEDIASERVICE_AUDIOSOURCE) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::Audio, this);
    }

    bool captureVideo = false;
    
    if (captureVideo) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::AudioAndVideo, this);
        //TODO:m_captureSession->setVideoInput(m_cameraControl);
        m_videoInputDevice = new QGstreamerVideoInputDeviceControl(m_captureSession);

        //TODO:connect(m_videoInputDevice, SIGNAL(selectedDeviceChanged(QString)),
        //        m_cameraControl, SLOT(setDevice(QString)));

        //TODO:if (m_videoInputDevice->deviceCount())
        //    m_cameraControl->setDevice(m_videoInputDevice->deviceName(m_videoInputDevice->selectedDevice()));

        m_videoRenderer = new QGstreamerVideoRenderer(this);
        m_videoRendererFactory = new QGstreamerVideoRendererWrapper(m_videoRenderer);
        m_videoWindow = new QGstreamerVideoOverlay(this);
        m_videoWindowFactory = new QGstreamerVideoRendererWrapper(m_videoWindow);

        m_videoWidgetControl = new QGstreamerVideoWidgetControl(this);
        m_videoWidgetFactory = new QGstreamerVideoRendererWrapper(m_videoWidgetControl);

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

QMediaControl *QGstreamerCaptureService::requestControl(const char *name)
{
    if (!m_captureSession)
        return 0;

    if (!m_videoOutput) {
        if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
            m_videoOutput = m_videoRenderer;
            m_captureSession->setVideoPreview(m_videoRendererFactory);
        } else if (qstrcmp(name, QVideoWindowControl_iid) == 0) {
            m_videoOutput = m_videoWindow;
            m_captureSession->setVideoPreview(m_videoWindowFactory);
        } else if (qstrcmp(name, QVideoWidgetControl_iid) == 0) {
            m_captureSession->setVideoPreview(m_videoWidgetFactory);
            m_videoOutput = m_videoWidgetControl;
        }

        if (m_videoOutput)
            return m_videoOutput;
    }

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

    if (qstrcmp(name,QMetaDataWriterControl_iid) == 0)
        return m_metaDataControl;

    return 0;
}

void QGstreamerCaptureService::releaseControl(QMediaControl *control)
{
    if (control && control == m_videoOutput) {
        m_videoOutput = 0;
        m_captureSession->setVideoPreview(0);
    }
}


