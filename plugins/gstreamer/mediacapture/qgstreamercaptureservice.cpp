#include "qgstreamercaptureservice.h"
#include "qaudiodeviceendpoint.h"
#include "qalsaaudiodeviceendpoint.h"
#include "qgstreamercapturesession.h"
#include "qgstreamerrecordercontrol.h"
#include "qgstreamermediaformatcontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamervideowidget.h"
#include "qgstreamervideorenderer.h"
#include "qgstreamerbushelper.h"
#include "qgstreamercameracontrol.h"


QGstreamerCaptureService::QGstreamerCaptureService(const char *interface, QObject *parent)
    :QAbstractMediaService(parent)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        gst_init(NULL, NULL);
    }

    if (QLatin1String(interface) == QLatin1String("com.nokia.qt.AudioRecorder/1.0")) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::Audio, this);
        m_cameraControl = 0;
    }

    if (QLatin1String(interface) == QLatin1String("com.nokia.qt.Camera/1.0")) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::AudioAndVideo, this);
        m_cameraControl = new QGstreamerCameraControl(m_captureSession);
        m_captureSession->setVideoInput(m_cameraControl);
    }
}

QGstreamerCaptureService::~QGstreamerCaptureService()
{
}

QList<QByteArray> QGstreamerCaptureService::supportedEndpointInterfaces(
      QMediaEndpointInterface::Direction direction) const
{
    QList<QByteArray> res;

    if (direction == QMediaEndpointInterface::Input)
        res << QByteArray(QAudioDeviceEndpoint_iid);

    if (direction == QMediaEndpointInterface::Output) {
        res << QMediaWidgetEndpoint_iid;
#ifndef QT_NO_VIDEOSURFACE
        res << QVideoRendererEndpoint_iid;
#endif
    }

    return res;
}

QObject *QGstreamerCaptureService::createEndpoint(const char *interface)
{
    if (qstrcmp(interface, QAudioDeviceEndpoint_iid) == 0) {
        return new QAlsaAudioDeviceEndpoint(this);
    }

    if (qstrcmp(interface,QMediaWidgetEndpoint_iid) == 0) {
        return new QGstreamerVideoWidget;
    }

#ifndef QT_NO_VIDEOSURFACE
    if (qstrcmp(interface,QVideoRendererEndpoint_iid) == 0) {
        return new QGstreamerVideoRenderer;
    }
#endif


    return 0;
}

void QGstreamerCaptureService::setAudioInput(QObject *input)
{
    QAlsaAudioDeviceEndpoint *endPoint = qobject_cast<QAlsaAudioDeviceEndpoint*>(input);

    if (endPoint) {
        endPoint->setDirectionFilter(QAlsaAudioDeviceEndpoint::InputDevice);

        if (audioInput())
            disconnect(audioInput(), SIGNAL(selectedDeviceChanged(QString)), m_captureSession, SLOT(setCaptureDevice(QString)));

        connect(endPoint, SIGNAL(selectedDeviceChanged(QString)), m_captureSession, SLOT(setCaptureDevice(QString)));

    }
    QAbstractMediaService::setAudioInput(endPoint);
}

class QGstreamerVideoRendererWrapper : public QGstreamerElementFactory
{
public:
    QGstreamerVideoRendererWrapper(QGstreamerVideoRendererInterface *videoRenderer)
        :m_videoRenderer(videoRenderer)
    {
        m_element = videoRenderer->videoSink();
    }

    virtual ~QGstreamerVideoRendererWrapper()
    {
        gst_object_unref(GST_OBJECT(m_element));
    }

    GstElement *buildElement()
    {
        m_videoRenderer->precessNewStream();

        gst_object_ref(GST_OBJECT(m_element));
        return m_element;
    }

    void prepareWinId()
    {
        m_videoRenderer->precessNewStream();
    }

private:
    QGstreamerVideoRendererInterface *m_videoRenderer;
    GstElement *m_element;
};

void QGstreamerCaptureService::setVideoOutput(QObject *output)
{
    QGstreamerVideoWidget *videoWidget = qobject_cast<QGstreamerVideoWidget*>(output);
    if (videoWidget) {
        m_captureSession->setVideoPreview(new QGstreamerVideoRendererWrapper(videoWidget));
    }

#ifndef QT_NO_VIDEOSURFACE
    QGstreamerVideoRenderer *videoRenderer = qobject_cast<QGstreamerVideoRenderer*>(output);
    if (videoRenderer) {
        m_captureSession->setVideoPreview(new QGstreamerVideoRendererWrapper(videoRenderer));
    }
#endif

    QAbstractMediaService::setVideoOutput(output);
}

QAbstractMediaControl *QGstreamerCaptureService::control(const char *name) const
{
    if (qstrcmp(name,"com.nokia.qt.MediaRecorderControl") == 0)
        return m_captureSession->recorderControl();

    if (qstrcmp(name,"com.nokia.qt.AudioEncodeControl") == 0)
        return m_captureSession->audioEncodeControl();

    if (qstrcmp(name,"com.nokia.qt.VideoEncodeControl") == 0)
        return m_captureSession->videoEncodeControl();

    if (qstrcmp(name,"com.nokia.qt.MediaFormatControl") == 0)
        return m_captureSession->mediaFormatControl();

    if (qstrcmp(name,"com.nokia.qt.CameraControl") == 0)
        return m_cameraControl;

    return 0;
}
