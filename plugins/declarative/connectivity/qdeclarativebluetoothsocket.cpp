#include "qdeclarativebluetoothsocket_p.h"

#include <QPointer>
#include <QStringList>
#include <QDataStream>
#include <QByteArray>


#include <bluetooth/qbluetoothdeviceinfo.h>
#include <bluetooth/qbluetoothaddress.h>
#include <bluetooth/qbluetoothsocket.h>
#include <bluetooth/ql2capserver.h>
#include <bluetooth/qrfcommserver.h>


class QDeclarativeBluetoothSocketPrivate
{
public:
    QDeclarativeBluetoothSocketPrivate(QDeclarativeBluetoothSocket *bs)
        : m_dbs(bs), m_service(0), m_socket(0),
          m_l2cap(0), m_rfcomm(0),
          m_error(QLatin1String("No Error")),
          m_state(QLatin1String("No Service Set")),
          m_componentCompleted(false),
          m_connected(false), m_listen(false)
    {

    }

    ~QDeclarativeBluetoothSocketPrivate()
    {
        delete m_socket;
    }

    void connect()
    {
        Q_ASSERT(m_service);
        qDebug() << "Connecting to: " << m_service->serviceInfo()->device().address().toString();

        m_error = QLatin1String("No Error");

        if(m_socket)
            m_socket->deleteLater();
//        delete m_socket;
        m_socket = new QBluetoothSocket();
        m_socket->connectToService(*m_service->serviceInfo());
        QObject::connect(m_socket, SIGNAL(connected()), m_dbs, SLOT(socket_connected()));
        QObject::connect(m_socket, SIGNAL(disconnected()), m_dbs, SLOT(socket_disconnected()));
        QObject::connect(m_socket, SIGNAL(error(QBluetoothSocket::SocketError)), m_dbs, SLOT(socket_error(QBluetoothSocket::SocketError)));
        QObject::connect(m_socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), m_dbs, SLOT(socket_state(QBluetoothSocket::SocketState)));
        QObject::connect(m_socket, SIGNAL(readyRead()), m_dbs, SLOT(socket_readyRead()));

        m_stream = new QDataStream(m_socket);
    }

    QDeclarativeBluetoothSocket *m_dbs;
    QDeclarativeBluetoothService *m_service;
    QBluetoothSocket *m_socket;
    QL2capServer *m_l2cap;
    QRfcommServer *m_rfcomm;
    QString m_error;
    QString m_state;
    bool m_componentCompleted;
    bool m_connected;
    QDataStream *m_stream;
    bool m_listen;

};

QDeclarativeBluetoothSocket::QDeclarativeBluetoothSocket(QObject *parent) :
    QObject(parent)
{
    d = new QDeclarativeBluetoothSocketPrivate(this);
}

QDeclarativeBluetoothSocket::QDeclarativeBluetoothSocket(QDeclarativeBluetoothService *service, QObject *parent)
    : QObject(parent)
{
    d = new QDeclarativeBluetoothSocketPrivate(this);
    d->m_service = service;
}

QDeclarativeBluetoothSocket::~QDeclarativeBluetoothSocket()
{
    delete d;
}

void QDeclarativeBluetoothSocket::componentComplete()
{
    d->m_componentCompleted = true;

    if(d->m_connected && d->m_service)
        d->connect();
    else if(d->m_listen)
        setListening(true);
}

QDeclarativeBluetoothService *QDeclarativeBluetoothSocket::service()
{
    return d->m_service;
}

void QDeclarativeBluetoothSocket::setService(QDeclarativeBluetoothService *service)
{
    d->m_service = service;

    if(!d->m_componentCompleted)
        return;

    if(d->m_connected)
        d->connect();
    emit serviceChanged();
}

bool QDeclarativeBluetoothSocket::connected()
{
    if(!d->m_socket)
        return false;

    return d->m_socket->state() == QBluetoothSocket::ConnectedState;
}

void QDeclarativeBluetoothSocket::setConnected(bool connected)
{
    d->m_connected = connected;
    if(connected && d->m_componentCompleted) {
        if(d->m_service) {
            d->connect();
        }
        else {
            qWarning() << "BluetoothSocket::setConnected called before a service was set";
        }
    }

    if(!connected && d->m_socket){
        d->m_socket->close();
    }
}

QString QDeclarativeBluetoothSocket::error()
{
    return d->m_error;
}

void QDeclarativeBluetoothSocket::socket_connected()
{
    emit connectedChanged();
}

void QDeclarativeBluetoothSocket::socket_disconnected()
{
    d->m_socket->deleteLater();
    d->m_socket = 0;
    emit connectedChanged();
}

void QDeclarativeBluetoothSocket::socket_error(QBluetoothSocket::SocketError err)
{
    if(err == QBluetoothSocket::ConnectionRefusedError)
        d->m_error = QLatin1String("Connection Refused");
    else if(err == QBluetoothSocket::RemoteHostClosedError)
        d->m_error = QLatin1String("Connection Closed by Remote Host");
    else if(err == QBluetoothSocket::HostNotFoundError)
        d->m_error = QLatin1String("Host Not Found");
    else if(err == QBluetoothSocket::ServiceNotFoundError)
        d->m_error = QLatin1String("Could not find service at remote host");
    else
        d->m_error = QLatin1String("Unkown Error");

    emit errorChanged();
}

void QDeclarativeBluetoothSocket::socket_state(QBluetoothSocket::SocketState state)
{
    switch(state){
    case QBluetoothSocket::UnconnectedState:
        d->m_state = QLatin1String("Unconnected");
        break;
    case QBluetoothSocket::ServiceLookupState:
        d->m_state = QLatin1String("Service Lookup");
        break;
    case QBluetoothSocket::ConnectingState:
        d->m_state = QLatin1String("Connecting");
        break;
    case QBluetoothSocket::ConnectedState:
        d->m_state = QLatin1String("Connected");
        break;
    case QBluetoothSocket::ClosingState:
        d->m_state = QLatin1String("Closing");
        break;
    case QBluetoothSocket::ListeningState:
        d->m_state = QLatin1String("Listening");
        break;
    case QBluetoothSocket::BoundState:
        d->m_state = QLatin1String("Bound");
        break;
    }

    emit stateChanged();
}

QString QDeclarativeBluetoothSocket::state()
{
    return d->m_state;
}

bool QDeclarativeBluetoothSocket::listening()
{
    if(d->m_rfcomm || d->m_l2cap)
        return true;

    return false;
}

void QDeclarativeBluetoothSocket::setListening(bool listen)
{
    if(listen == false && (d->m_rfcomm || d->m_l2cap)) {
        qWarning() << "Once socket is in listening state, can not be returned to client socket";
        return;
    }

    if(!d->m_componentCompleted){
        d->m_listen = listen;
        return;
    }

    if(!d->m_service){
        qWarning() << "Can not put socket into listening state without an assigned service";
        return;
    }

    if(d->m_service->serviceProtocol() == "l2cap") {
        d->m_l2cap = new QL2capServer();

        d->m_l2cap->setMaxPendingConnections(1);
        qint32 port = d->m_service->servicePort();
        if(port== -1)
            port = 0;
        d->m_l2cap->listen(QBluetoothAddress(), port);

        if(!port)
            d->m_service->setServicePort(d->m_l2cap->serverPort());

        connect(d->m_l2cap, SIGNAL(newConnection()), this, SLOT(l2cap_connection()));
    }
    else if(d->m_service->serviceProtocol() == "rfcomm") {
        d->m_rfcomm = new QRfcommServer();

        d->m_rfcomm->setMaxPendingConnections(1);
        qint32 port = d->m_service->servicePort();
        if(port== -1)
            port = 0;
        d->m_rfcomm->listen(QBluetoothAddress(), port);

        if(!port)
            d->m_service->setServicePort(d->m_rfcomm->serverPort());
        connect(d->m_rfcomm, SIGNAL(newConnection()), this, SLOT(rfcomm_connection()));
    }

    emit listeningChanged();
}

void QDeclarativeBluetoothSocket::socket_readyRead()
{
    emit dataAvailable();
}

QString QDeclarativeBluetoothSocket::stringData()
{
    if(!d->m_socket|| !d->m_socket->bytesAvailable())
        return QString();

    QString data;
    *d->m_stream >> data;
    return data;
}

void QDeclarativeBluetoothSocket::sendStringData(QString data)
{
    if(!d->m_connected || !d->m_socket){
        qWarning() << "Writing data to unconnected socket";
        return;
    }

    QByteArray b;
    QDataStream s(&b, QIODevice::WriteOnly);
    s << data;
    d->m_socket->write(b);
}

void QDeclarativeBluetoothSocket::l2cap_connection()
{

    QBluetoothSocket *socket = d->m_l2cap->nextPendingConnection();
    if (!socket)
        return;

    if(d->m_socket) {
        socket->close();
        return;
    }

    d->m_socket = socket;

    connect(socket, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socket_error(QBluetoothSocket::SocketError)));
    connect(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), this, SLOT(socket_state(QBluetoothSocket::SocketState)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(socket_readyRead()));

    d->m_stream = new QDataStream(socket);

    void connectedChanged();
}

void QDeclarativeBluetoothSocket::rfcomm_connection()
{

    QBluetoothSocket *socket = d->m_rfcomm->nextPendingConnection();
    if (!socket)
        return;

    if(d->m_socket) {
        socket->close();
        return;
    }

    d->m_socket = socket;

    connect(socket, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));
    connect(socket, SIGNAL(error(QBluetoothSocket::SocketError)), this, SLOT(socket_error(QBluetoothSocket::SocketError)));
    connect(socket, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), this, SLOT(socket_state(QBluetoothSocket::SocketState)));
    connect(socket, SIGNAL(readyRead()), this, SLOT(socket_readyRead()));


    d->m_stream = new QDataStream(socket);

    void connectedChanged();
}
