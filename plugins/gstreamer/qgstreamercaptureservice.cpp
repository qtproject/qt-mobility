#include "qgstreamercaptureservice.h"
#include "qaudiodeviceendpoint.h"
#include "qalsaaudiodeviceendpoint.h"
#include "qgstreamercapturesession.h"

QGstreamerCaptureService::QGstreamerCaptureService(QObject *parent)
    :QAbstractMediaService(parent)
{
    m_session = new QGstreamerCaptureSession(this);
}

QGstreamerCaptureService::~QGstreamerCaptureService()
{
}

QList<QByteArray> QGstreamerCaptureService::supportedEndpointInterfaces(
      QMediaEndpointInterface::Direction direction) const
{
    return QList<QByteArray>() << QByteArray(QAudioDeviceEndpoint_iid);
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
            disconnect(audioInput(), SIGNAL(selectedDeviceChanged(QString)), m_session, SLOT(setCaptureDevice(QString)));

        connect(endPoint, SIGNAL(selectedDeviceChanged(QString)), m_session, SLOT(setCaptureDevice(QString)));

    }
    QAbstractMediaService::setAudioInput(endPoint);
}

QAbstractMediaControl *QGstreamerCaptureService::control(const char *name) const
{
    if (qstrcmp(name,"com.nokia.qt.MediaCaptureControl") == 0)
        return m_session;

    return 0;
}
