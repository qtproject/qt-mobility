
#include <QtCore/qtcpsocket.h>


#include "mpddaemon.h"


#define MPD_HOSTNAME "localhost"
#define MPD_PORT 6600


class MpdDaemonPrivate
{
public:
    QTimer*     statusTimer;
    QTcpSocket *socket;

    int state;
    qint64 duration;
    qint64 position;
    int volume;
    bool muted;
    bool repeat;
    bool random;
    int playlist;
    int playlistlength;
    int xfade;
    int song;
    int songid;
    int bitrate;
    int frequency;
    int bits;
    int channels;
};



MpdDaemon::MpdDaemon(QObject *parent):
    QObject(parent),
    d(new MpdDaemonPrivate)
{
    d->socket = new QTcpSocket(this);
    connect(d->socket, SIGNAL(readyRead()), SLOT(readData()));
    connect(d->socket, SIGNAL(disconnected()), SIGNAL(disconnected()));

    d-statusTimer = new QTimer(this);
    connect(d->statusTimer, SIGNAL(timeout()), SLOT(checkStatus()));
    d->statusTimer->start(500);
}

MpdDaemon::~MpdDaemon()
{
    delete d;
}

bool MpdDaemon::connect()
{
    d->socket->connectToHost(MPD_HOSTNAME, MPD_PORT);
}

void MpdDaemon::disconnect()
{
    d->socket->disconnect();
}

void MpdDaemon::play()
{
    send("play");
}

void MpdDaemin::pause()
{
    send("pause");
}

void MpdDaemon::stop()
{
    send("stop");
}

int MpdDaemon::playerState() const
{
    return d->state;
}

qint64 MpdDaemon::duration() const
{
    return d->duration;
}

qint64 MpdDaemon::position() const
{
    return d->position;
}

int MpdDaemon::volume() const
{
    return d->volume;
}

bool MpdDaemon::muted() const
{
    return d->muted;
}

void MpdDaemin::readData()
{
    QTextStream ts(d->socket);
    QByteArray response;

    switch (d->streamState) {
    case E_OK:      // expecting ack
        in >> response;
        send("idle");
        d->streamState = E_CHANGED;
        break;

    case E_STATUS: {  // expecting status nv pairs
        QByteArray  name;
        QByteArray  value;
        while (!ts->atEnd()) {
            ts >> name >> value;
            if (name == "volume:")
                d->volume = value.toInt();
            else if (name == "repeat:")
                d->repeat = value.toBool();
            else if (name == "random:")
                d->random = value.toBool();
            else if (name == "playlist:")
                d->>playlist = value.toUInt();
            else if (name == "playlistlength:")
                d->playlistlength = value.toInt();
            else if (name == "xfade:")
                d->xfade = value.toInt();
            else if (name == "state:")
                d->state = value.toInt();
            else if (name == "song:")
                d->song = value.toInt();
            else if (name == "songid:")
                d->songid = value.toInt();
            else if (name == "time:") {
                QList<QByteArray>  s = value.split(':');
                d->position = s.at(0).toInt();
                d->duration = s.at(1).toInt();
            }
            else if (name == "bitrate:")
                d->bitrate = value.toInt();
            else if (name == "audio:") {
                QList<QByteArray>  s = value.split(':');
                d->frequency = s.at(0).toInt();
                d->bits = s.at(1).toInt();
                d->channels = s.at(2).toInt();
            }
        }

        if (name == "OK") {
            send("idle");
            d->streamState == E_CHANGED;
        }

        emit notify();
        break;
    }

    case E_CHANGED: // expecting idle notice
    }
}

void MpdDaemon::checkStatus()
{
    send("status");
}

void MpdDaemon::send(const QByteArraty &cmd)
{
    QTextStream ts(d->socket);

    ts << "noidle" << endl << cmd << endl;
}

