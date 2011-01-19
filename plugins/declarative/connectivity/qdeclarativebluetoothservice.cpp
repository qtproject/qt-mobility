#include "qdeclarativebluetoothservice_p.h"

#include <bluetooth/qbluetoothdeviceinfo.h>
#include <bluetooth/qbluetoothaddress.h>

class QDeclarativeBluetoothServicePrivate
{
public:
    QDeclarativeBluetoothServicePrivate()
        : m_componentComplete(false),
          m_service(0), m_port(0),
          m_needsRegistration(false)
    {

    }

    ~QDeclarativeBluetoothServicePrivate()
    {
        delete m_service;
    }

    bool m_componentComplete;
    QBluetoothServiceInfo *m_service;
    QString m_protocol;
    qint32 m_port;
    QString m_description;
    QString m_name;
    QString m_uuid;
    bool m_needsRegistration;

};

QDeclarativeBluetoothService::QDeclarativeBluetoothService(QObject *parent) :
    QObject(parent)
{
    d = new QDeclarativeBluetoothServicePrivate;
}

QDeclarativeBluetoothService::QDeclarativeBluetoothService(const QBluetoothServiceInfo &service, QObject *parent)
    : QObject(parent)
{
    d = new QDeclarativeBluetoothServicePrivate;
    d->m_service = new QBluetoothServiceInfo(service);
}

QDeclarativeBluetoothService::~QDeclarativeBluetoothService()
{
    delete d;
}

void QDeclarativeBluetoothService::componentComplete()
{
    d->m_componentComplete = true;

    if(d->m_needsRegistration)
        setRegistered(true);
}

QString QDeclarativeBluetoothService::deviceName() const
{
    if(!d->m_service)
        return QString();

    return d->m_service->device().name();
}

QString QDeclarativeBluetoothService::deviceAddress() const
{
    if(!d->m_service)
        return QString();

    return d->m_service->device().address().toString();
}

void QDeclarativeBluetoothService::setDeviceAddress(QString address)
{
    if(!d->m_service)
        d->m_service = new QBluetoothServiceInfo();

    QBluetoothAddress a(address);
    QBluetoothDeviceInfo device(a, QString(), QBluetoothDeviceInfo::ComputerDevice);
    d->m_service->setDevice(device);
}

QString QDeclarativeBluetoothService::serviceName() const
{
    if(!d->m_service)
        return QString();

    if(!d->m_name.isEmpty())
        return d->m_name;

    return d->m_service->serviceName();
}

void QDeclarativeBluetoothService::setServiceName(QString name)
{
    d->m_name = name;
}

QString QDeclarativeBluetoothService::serviceDescription() const
{
    if(!d->m_service)
        return QString();

    if(!d->m_description.isEmpty())
        return d->m_name;

    return d->m_service->serviceDescription();
}

void QDeclarativeBluetoothService::setServiceDescription(QString description)
{
    d->m_description = description;
    emit detailsChanged();
}

QString QDeclarativeBluetoothService::serviceProtocol() const
{
    if(!d->m_protocol.isEmpty())
        return d->m_protocol;

    if(!d->m_service)
        return QString();

    if(d->m_service->socketProtocol() == QBluetoothServiceInfo::L2capProtocol)
        return QLatin1String("l2cap");
    if(d->m_service->socketProtocol() == QBluetoothServiceInfo::RfcommProtocol)
        return QLatin1String("rfcomm");

    return QLatin1String("unkown");
}

void QDeclarativeBluetoothService::setServiceProtocol(QString protocol)
{
    if(protocol != "rfcomm" && protocol != "l2cap")
        qWarning() << "Invalid protocol" << protocol;

    d->m_protocol = protocol;
    emit detailsChanged();
}

QString QDeclarativeBluetoothService::serviceUuid() const
{
    if(!d->m_uuid.isEmpty())
        return d->m_uuid;

    if(!d->m_service)
        return QString();

    return d->m_service->attribute(QBluetoothServiceInfo::ServiceId).toString();
}

void QDeclarativeBluetoothService::setServiceUuid(QString uuid)
{
    d->m_uuid = uuid;
    if(!d->m_service)
        d->m_service = new QBluetoothServiceInfo();
    d->m_service->setAttribute(QBluetoothServiceInfo::ServiceId, QBluetoothUuid(uuid));

    emit detailsChanged();
}


qint32 QDeclarativeBluetoothService::servicePort() const
{
    if(d->m_port > 0)
        return d->m_port;

    if(!d->m_service)
        return -1;

    if(d->m_service->serverChannel() > 0)
        return d->m_service->serverChannel();
    if(d->m_service->protocolServiceMultiplexer() > 0)
        return d->m_service->protocolServiceMultiplexer();

    return -1;
}

void QDeclarativeBluetoothService::setServicePort(qint32 port)
{
    if(d->m_port != port){
        d->m_port = port;
        if(isRegistered())
            setRegistered(true);
        emit detailsChanged();
    }
}

bool QDeclarativeBluetoothService::isRegistered() const
{
    if(!d->m_service)
        return false;

    return d->m_service->isRegistered();
}

void QDeclarativeBluetoothService::setRegistered(bool registered)
{

    d->m_needsRegistration = registered;

    if(!d->m_componentComplete){
        return;
    }

    if(!registered) {
        if(!d->m_service)
            return;
        d->m_service->unregisterService();
        emit registeredChanged();
    }

    if(!d->m_service){
        d->m_service = new QBluetoothServiceInfo();
    }

    qDebug() << "Starting registration";

    d->m_service->setAttribute(QBluetoothServiceInfo::ServiceRecordHandle, (uint)0x00010010);

//    QBluetoothServiceInfo::Sequence classId;
////    classId << QVariant::fromValue(QBluetoothUuid(serviceUuid));
//    classId << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::SerialPort));
//    d->m_service->setAttribute(QBluetoothServiceInfo::ServiceClassIds, classId);

    d->m_service->setAttribute(QBluetoothServiceInfo::ServiceName, d->m_name);
    d->m_service->setAttribute(QBluetoothServiceInfo::ServiceDescription,
                             d->m_description);

    d->m_service->setServiceUuid(QBluetoothUuid(d->m_uuid));

    qDebug() << "name/uuid" << d->m_name << d->m_uuid;

    d->m_service->setAttribute(QBluetoothServiceInfo::BrowseGroupList,
                             QBluetoothUuid(QBluetoothUuid::PublicBrowseGroup));

    QBluetoothServiceInfo::Sequence protocolDescriptorList;
    QBluetoothServiceInfo::Sequence protocol;
    if(d->m_protocol == "l2cap"){
        protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::L2cap))
                 << QVariant::fromValue(quint16(d->m_port)); //endian convertion? FIXME
        protocolDescriptorList.append(QVariant::fromValue(protocol));
    }
    else if(d->m_protocol == "rfcomm"){
        protocol << QVariant::fromValue(QBluetoothUuid(QBluetoothUuid::Rfcomm))
                 << QVariant::fromValue(quint8(d->m_port));
        protocolDescriptorList.append(QVariant::fromValue(protocol));
    }
    else {
        qWarning() << "No protocol speficied for bluetooth service";
    }
    d->m_service->setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                             protocolDescriptorList);

    if(d->m_service->registerService()) {
        qDebug() << "registered";
        emit registeredChanged();
    }
    else {
        qDebug() << "Failed";
    }
}

QBluetoothServiceInfo *QDeclarativeBluetoothService::serviceInfo() const
{
    return d->m_service;
}
