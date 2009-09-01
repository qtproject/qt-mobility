#include "qgstreamercaptureservice.h"
#include "qgstreamercapturesession.h"
#include "qgstreamerrecordercontrol.h"
#include "qgstreamermediaformatcontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamerbushelper.h"
#include "qgstreamercameracontrol.h"
#include "qgstreamercapturemetadatacontrol.h"

#include "qgstreamervideooutputcontrol.h"
#include "qgstreameraudioinputdevicecontrol.h"

#ifndef QT_NO_VIDEOSURFACE
#include "qgstreamervideooverlay.h"
#include "qgstreamervideorenderer.h"
#endif

#include "qgstreamervideowidget.h"


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


QGstreamerCaptureService::QGstreamerCaptureService(const char *interface, QObject *parent)
    :QAbstractMediaService(parent)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        gst_init(NULL, NULL);
    }

    if (QLatin1String(interface) == QLatin1String(QAudioRecorderService_iid)) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::Audio, this);
        m_cameraControl = 0;
    }

    if (QLatin1String(interface) == QLatin1String(QCameraService_iid)) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::AudioAndVideo, this);
        m_cameraControl = new QGstreamerCameraControl(m_captureSession);
        m_captureSession->setVideoInput(m_cameraControl);
    }

    m_videoOutput = new QGstreamerVideoOutputControl(this);
    connect(m_videoOutput, SIGNAL(outputChanged(QVideoOutputControl::Output)),
            this, SLOT(videoOutputChanged(QVideoOutputControl::Output)));
#ifndef QT_NO_VIDEOSURFACE
    m_videoRenderer = new QGstreamerVideoRenderer(this);
    m_videoRendererFactory = new QGstreamerVideoRendererWrapper(m_videoRenderer);
    m_videoWindow = new QGstreamerVideoOverlay(this);
    m_videoWindowFactory = new QGstreamerVideoRendererWrapper(m_videoWindow);
#endif

    m_videoWidgetControl = new QGstreamerVideoWidgetControl(this);
    m_videoWidgetFactory = new QGstreamerVideoRendererWrapper(m_videoWidgetControl);

    m_videoOutput->setAvailableOutputs(QList<QVideoOutputControl::Output>()
#ifndef QT_NO_VIDEOSURFACE
            << QVideoOutputControl::RendererOutput
            << QVideoOutputControl::WindowOutput
#endif
            << QVideoOutputControl::WidgetOutput);

    m_audioInputDevice = new QGstreamerAudioInputDeviceControl(this);
    connect(m_audioInputDevice, SIGNAL(selectedDeviceChanged(QString)), m_captureSession, SLOT(setCaptureDevice(QString)));

    m_metadataControl = new QGstreamerCaptureMetadataControl(this);
    connect(m_metadataControl, SIGNAL(metadataChanged(QMap<QByteArray,QVariant>)),
            m_captureSession, SLOT(setMetadata(QMap<QByteArray,QVariant>)));
}

QGstreamerCaptureService::~QGstreamerCaptureService()
{
}

QAbstractMediaControl *QGstreamerCaptureService::control(const char *name) const
{
    if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutput;

#ifndef QT_NO_VIDEOSURFACE
    if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRenderer;

    if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindow;
#endif

    if (qstrcmp(name, QVideoWidgetControl_iid) == 0)
        return m_videoWidgetControl;

    if (qstrcmp(name,QAudioDeviceControl_iid) == 0)
        return m_audioInputDevice;

    if (qstrcmp(name,QMediaRecorderControl_iid) == 0)
        return m_captureSession->recorderControl();

    if (qstrcmp(name,QAudioEncodeControl_iid) == 0)
        return m_captureSession->audioEncodeControl();

    if (qstrcmp(name,QVideoEncodeControl_iid) == 0)
        return m_captureSession->videoEncodeControl();

    if (qstrcmp(name,QMediaFormatControl_iid) == 0)
        return m_captureSession->mediaFormatControl();

    if (qstrcmp(name,QCameraControl_iid) == 0)
        return m_cameraControl;

    if (qstrcmp(name,QMetadataProviderControl_iid) == 0)
        return m_metadataControl;

    return 0;
}

void QGstreamerCaptureService::videoOutputChanged(QVideoOutputControl::Output output)
{
    switch (output) {
    case QVideoOutputControl::NoOutput:
        m_captureSession->setVideoPreview(0);
        break;
#ifndef QT_NO_VIDEOSURFACE
    case QVideoOutputControl::RendererOutput:
        m_captureSession->setVideoPreview(m_videoRendererFactory);
        break;
    case QVideoOutputControl::WindowOutput:
        m_captureSession->setVideoPreview(m_videoWindowFactory);
        break;
#endif
    case QVideoOutputControl::WidgetOutput:
        m_captureSession->setVideoPreview(m_videoWidgetFactory);
    default:
        qWarning("Invalid video output selection");
        m_captureSession->setVideoPreview(0);
        break;
    }
}

