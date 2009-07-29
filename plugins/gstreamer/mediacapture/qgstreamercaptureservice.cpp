#include "qgstreamercaptureservice.h"
#include "qaudiodeviceendpoint.h"
#include "qalsaaudiodeviceendpoint.h"
#include "qgstreamercapturesession.h"
#include "qgstreamercapturecontrol.h"
#include "qgstreameraudioencode.h"
#include "qgstreamervideoencode.h"
#include "qgstreamerbushelper.h"

QGstreamerCaptureService::QGstreamerCaptureService(const char *interface, QObject *parent)
    :QAbstractMediaService(parent)
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        gst_init(NULL, NULL);
    }

    if (QLatin1String(interface) == QLatin1String("com.nokia.qt.AudioCapture/1.0")) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::Audio, this);
    }

    if (QLatin1String(interface) == QLatin1String("com.nokia.qt.CameraCapture/1.0")) {
        m_captureSession = new QGstreamerCaptureSession(QGstreamerCaptureSession::AudioAndVideo, this);
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

    return res;
}

QObject *QGstreamerCaptureService::createEndpoint(const char *interface)
{
    if (qstrcmp(interface, QAudioDeviceEndpoint_iid) == 0) {
        return new QAlsaAudioDeviceEndpoint(this);
    }

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

QAbstractMediaControl *QGstreamerCaptureService::control(const char *name) const
{
    if (qstrcmp(name,"com.nokia.qt.MediaCaptureControl") == 0)
        return m_captureSession->captureControl();

    if (qstrcmp(name,"com.nokia.qt.AudioEncodeControl") == 0)
        return m_captureSession->audioEncodeControl();

    if (qstrcmp(name,"com.nokia.qt.VideoEncodeControl") == 0)
        return m_captureSession->videoEncodeControl();

    return 0;
}
