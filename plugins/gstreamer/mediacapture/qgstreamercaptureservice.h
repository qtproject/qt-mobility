#ifndef QGSTREAMERCAPTURESERVICE_H
#define QGSTREAMERCAPTURESERVICE_H

#include "qaudiorecorderservice.h"
#include "qcameraservice.h"
#include "qgstreamervideooutputcontrol.h"

#include <gst/gst.h>

class QGstreamerCaptureSession;
class QGstreamerCameraControl;
class QAudioInputDeviceControl;
class QGstreamerMessage;
class QGstreamerBusHelper;
class QGstreamerVideoRenderer;
class QGstreamerVideoOverlay;
class QGstreamerElementFactory;
class QGstreamerCaptureMetadataControl;

class QGstreamerCaptureService : public QAbstractMediaService
{
Q_OBJECT
public:
    QGstreamerCaptureService(const char *interface, QObject *parent = 0);
    virtual ~QGstreamerCaptureService();

    bool isEndpointSupported(QAbstractMediaService::MediaEndpoint endpointType);
    QString activeEndpoint(QAbstractMediaService::MediaEndpoint endpointType);
    void setActiveEndpoint(QAbstractMediaService::MediaEndpoint endpointType, const QString& endpoint);
    QList<QString> supportedEndpoints(QAbstractMediaService::MediaEndpoint endpointType) const;
    void setInputStream(QIODevice* stream) {};
    QIODevice* inputStream() const { return 0; };
    void setOutputStream(QIODevice* stream) {};
    QIODevice* outputStream() const { return 0; }

    QAbstractMediaControl *control(const char *name) const;

private slots:
    void videoOutputChanged(QVideoOutputControl::Output output);

private:
    void setAudioPreview(GstElement*);

    QGstreamerCaptureSession *m_captureSession;
    QGstreamerCameraControl *m_cameraControl;
    QGstreamerCaptureMetadataControl *m_metadataControl;

    QAudioInputDeviceControl *m_audioInputDevice;

    QGstreamerVideoOutputControl *m_videoOutput;
#ifndef QT_NO_VIDEOSURFACE
    QGstreamerVideoRenderer *m_videoRenderer;
    QGstreamerElementFactory *m_videoRendererFactory;
    QGstreamerVideoOverlay *m_videoWindow;
    QGstreamerElementFactory *m_videoWindowFactory;
#endif
};

#endif // QGSTREAMERCAPTURESERVICE_H
