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

#include "camerabinimagecapture.h"
#include "camerabincapturedestination.h"
#include "camerabincapturebufferformat.h"
#include "camerabinsession.h"
#include "qgstvideobuffer.h"
#include "qvideosurfacegstsink.h"
#include "qgstutils.h"
#include <QtCore/QDebug>

//#define DEBUG_CAPTURE

#ifdef Q_WS_MAEMO_5
#define IMAGE_DONE_SIGNAL "img-done"
#else
#define IMAGE_DONE_SIGNAL "image-done"
#endif


Q_DECLARE_METATYPE(QVideoFrame)
Q_DECLARE_METATYPE(QtMultimediaKit::MetaData)

namespace
{
class CameraRegisterMetaTypes
{
public:
    CameraRegisterMetaTypes()
    {
        qRegisterMetaType<QVideoFrame>("QVideoFrame");
        qRegisterMetaType<QtMultimediaKit::MetaData>("QtMultimediaKit::MetaData");
    }
} _registerCameraMetaTypes;
}


CameraBinImageCapture::CameraBinImageCapture(CameraBinSession *session)
    :QCameraImageCaptureControl(session)
    , m_session(session)
    , m_ready(false)
    , m_requestId(0)
    , m_jpegEncoderElement(0)
    , m_metadataMuxerElement(0)
{
    connect(m_session, SIGNAL(stateChanged(QCamera::State)), SLOT(updateState()));
    connect(m_session, SIGNAL(imageExposed(int)), this, SIGNAL(imageExposed(int)));
    connect(m_session, SIGNAL(imageCaptured(int,QImage)), this, SIGNAL(imageCaptured(int,QImage)));
    connect(m_session, SIGNAL(busMessage(QGstreamerMessage)), SLOT(handleBusMessage(QGstreamerMessage)));

    g_signal_connect(G_OBJECT(m_session->cameraBin()), IMAGE_DONE_SIGNAL, G_CALLBACK(handleImageSaved), this);
}

CameraBinImageCapture::~CameraBinImageCapture()
{
}

bool CameraBinImageCapture::isReadyForCapture() const
{
    return m_ready;
}

int CameraBinImageCapture::capture(const QString &fileName)
{
    m_requestId++;
#ifdef DEBUG_CAPTURE
    qDebug() << Q_FUNC_INFO << requestId << fileName;
#endif
    m_session->captureImage(m_requestId, fileName);
    return m_requestId;
}

void CameraBinImageCapture::cancelCapture()
{
}

void CameraBinImageCapture::updateState()
{
    bool ready = m_session->state() == QCamera::ActiveState;
    if (m_ready != ready) {
#ifdef DEBUG_CAPTURE
        qDebug() << "readyForCaptureChanged" << ready;
#endif
        emit readyForCaptureChanged(m_ready = ready);
    }
}

gboolean CameraBinImageCapture::handleImageSaved(GstElement *camera,
                                                 const gchar *filename,
                                                 CameraBinImageCapture *self)
{
#if DEBUG_CAPTURE
    qDebug() << "Image saved" << filename;
#endif

    Q_UNUSED(camera);

    if (self->m_session->captureDestinationControl()->captureDestination() & QCameraImageCapture::CaptureToFile) {
        QMetaObject::invokeMethod(self, "imageSaved",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, self->m_requestId),
                                  Q_ARG(QString, QString::fromUtf8(filename)));
    } else {
#ifdef DEBUG_CAPTURE
        qDebug() << Q_FUNC_INFO << "Dropped saving file" << filename;
#endif
        //camerabin creates an empty file when captured buffer is dropped,
        //let's remove it
        QFileInfo info(QString::fromUtf8(filename));
        if (info.isFile() &&
                info.filePath().startsWith("/home") &&
                info.size() == 0) {
            QFile(info.absoluteFilePath()).remove();
        }
    }
    return true;
}

gboolean CameraBinImageCapture::uncompressedBufferProbe(GstPad *pad, GstBuffer *buffer, CameraBinImageCapture *self)
{
    Q_UNUSED(pad);
    CameraBinSession *session = self->m_session;

#ifdef DEBUG_CAPTURE
    qDebug() << "Uncompressed buffer probe" << gst_caps_to_string(GST_BUFFER_CAPS(buffer));
#endif

    QCameraImageCapture::CaptureDestinations destination =
            session->captureDestinationControl()->captureDestination();
    QVideoFrame::PixelFormat format = session->captureBufferFormatControl()->bufferFormat();

    if (destination & QCameraImageCapture::CaptureToBuffer) {
        if (format != QVideoFrame::Format_Jpeg) {
            GstCaps *caps = GST_BUFFER_CAPS(buffer);
            int bytesPerLine = -1;
            QVideoSurfaceFormat format = QVideoSurfaceGstSink::formatForCaps(caps, &bytesPerLine);
#ifdef DEBUG_CAPTURE
            qDebug() << "imageAvailable(uncompressed):" << format;
#endif
            QGstVideoBuffer *videoBuffer = new QGstVideoBuffer(buffer, bytesPerLine);

            QVideoFrame frame(videoBuffer,
                              format.frameSize(),
                              format.pixelFormat());

            QMetaObject::invokeMethod(self, "imageAvailable",
                                      Qt::QueuedConnection,
                                      Q_ARG(int, self->m_requestId),
                                      Q_ARG(QVideoFrame, frame));
        }
    }

    //keep the buffer if capture to file or jpeg buffer capture was reuqsted
    bool keepBuffer = (destination & QCameraImageCapture::CaptureToFile) ||
            ((destination & QCameraImageCapture::CaptureToBuffer) &&
              format == QVideoFrame::Format_Jpeg);

    return keepBuffer;
}

gboolean CameraBinImageCapture::jpegBufferProbe(GstPad *pad, GstBuffer *buffer, CameraBinImageCapture *self)
{
    Q_UNUSED(pad);
    CameraBinSession *session = self->m_session;

#ifdef DEBUG_CAPTURE
    qDebug() << "Jpeg buffer probe" << gst_caps_to_string(GST_BUFFER_CAPS(buffer));
#endif

    QCameraImageCapture::CaptureDestinations destination =
            session->captureDestinationControl()->captureDestination();

    if ((destination & QCameraImageCapture::CaptureToBuffer) &&
         session->captureBufferFormatControl()->bufferFormat() == QVideoFrame::Format_Jpeg) {
        QGstVideoBuffer *videoBuffer = new QGstVideoBuffer(buffer,
                                                           -1); //bytesPerLine is not available for jpegs
        QVideoFrame frame(videoBuffer,
                          QGstUtils::capsCorrectedResolution(GST_BUFFER_CAPS(buffer)),
                          QVideoFrame::Format_Jpeg);

        QMetaObject::invokeMethod(self, "imageAvailable",
                                  Qt::QueuedConnection,
                                  Q_ARG(int, self->m_requestId),
                                  Q_ARG(QVideoFrame, frame));
    }

    //drop the buffer if capture to file was disabled
    return destination & QCameraImageCapture::CaptureToFile;
}

void CameraBinImageCapture::handleBusMessage(const QGstreamerMessage &message)
{
    //Install buffer probes

    //The image capture pipiline is built dynamically,
    //it's necessary to wait until jpeg encoder is added to pipeline

    GstMessage *gm = message.rawMessage();
    if (GST_MESSAGE_TYPE(gm) == GST_MESSAGE_STATE_CHANGED) {
        GstState    oldState;
        GstState    newState;
        GstState    pending;
        gst_message_parse_state_changed(gm, &oldState, &newState, &pending);

        if (newState == GST_STATE_READY) {
            GstElement *element = GST_ELEMENT(GST_MESSAGE_SRC(gm));
            if (!element)
                return;

            QString elementName = QString::fromLatin1(gst_element_get_name(element));
            if (elementName.contains("jpegenc") && element != m_jpegEncoderElement) {
                m_jpegEncoderElement = element;
                GstPad *sinkpad = gst_element_get_static_pad(element, "sink");

#ifdef DEBUG_CAPTURE
                qDebug() << "install uncompressed buffer probe";
#endif
                gst_pad_add_buffer_probe(sinkpad,
                                         G_CALLBACK(CameraBinImageCapture::uncompressedBufferProbe),
                                         this);

                gst_object_unref(sinkpad);
            } else if (elementName.contains("jifmux") && element != m_metadataMuxerElement) {
                //Jpeg encoded buffer probe is added after jifmux
                //element to ensure the resulting jpeg buffer contains capture metadata
                m_metadataMuxerElement = element;

                GstPad *srcpad = gst_element_get_static_pad(element, "src");
#ifdef DEBUG_CAPTURE
                qDebug() << "install jpeg buffer probe";
#endif
                gst_pad_add_buffer_probe(srcpad,
                                         G_CALLBACK(CameraBinImageCapture::jpegBufferProbe),
                                         this);
                gst_object_unref(srcpad);
            }
        }
    }
}
