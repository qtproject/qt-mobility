#include "qgstreamercaptureservice.h"
#include "qaudiodeviceendpoint.h"
#include "qalsaaudiodeviceendpoint.h"
#include "qgstreamercapturesession.h"
#include "qgstreameraudioencode.h"

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
            disconnect(audioInput(), SIGNAL(selectedDeviceChanged(QString)), m_session, SLOT(setCaptureDevice(QString)));

        connect(endPoint, SIGNAL(selectedDeviceChanged(QString)), m_session, SLOT(setCaptureDevice(QString)));

    }
    QAbstractMediaService::setAudioInput(endPoint);
}

QAbstractMediaControl *QGstreamerCaptureService::control(const char *name) const
{
    if (qstrcmp(name,"com.nokia.qt.MediaCaptureControl") == 0)
        return m_session;

    if (qstrcmp(name,"com.nokia.qt.AudioEncodeControl") == 0)
        return m_session->audioEncodeControl();

    return 0;
}
