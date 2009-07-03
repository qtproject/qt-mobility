/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstringlist.h>
#include <QtCore/qtimer.h>>
#include <QtNetwork/qtcpsocket.h>

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
//    connect(d->socket, SIGNAL(readyRead()), SLOT(readData()));
    connect(d->socket, SIGNAL(connected()), SLOT(setup()));
    connect(d->socket, SIGNAL(disconnected()), SIGNAL(shutdown()));

    d->statusTimer = new QTimer(this);
    connect(d->statusTimer, SIGNAL(timeout()), SLOT(checkStatus()));

    d->socket->connectToHost(MPD_HOSTNAME, MPD_PORT);

    // init
    d->state = Stopped;
}

MpdDaemon::~MpdDaemon()
{
    delete d;
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

int MpdDaemon::currentSongPos() const
{
    return d->song;
}

int MpdDaemon::volume() const
{
    return d->volume;
}

bool MpdDaemon::muted() const
{
    return d->muted;
}

QStringList MpdDaemon::send(QString const &command, bool *ok)
{
    QStringList r;

    rawSend("noidle", true);
    r = rawSend(command.toAscii().constData(), true, ok);
    rawSend("idle", false);

    return r;
}

void MpdDaemon::readData()
{
    QStringList r = rawRecv();

    /*
    QStringList t = r.at(0).split(' ');
    QString const &name = t.at(0);
    QString const &value = t.at(1);
    */
    checkStatus();

    /*
    if (name == "changed:") {
        if (value == "playlist")
            emit playlistChanged();
        else if (value == "player")
            emit playerChanged();
        else if (value == "mixer")
            emit mixerChanged();
    }
    */
    /*
      database: the song database has been updated
      stored_playlist: a stored playlist has been modified, renamed, created or deleted
      playlist: the current playlist has been modified
      player: the player has been started, stopped or seeked
      mixer: the volume has been changed
      output: an audio output has been enabled or disabled
      options: options like repeat, random, crossfade 
    */

    emit notify();
}

void MpdDaemon::checkStatus()
{
    bool     ok = false;
    QStringList r = send("status", &ok);

    if (!ok)
        return;

    foreach (QString const &rs, r) {
        QStringList t = rs.trimmed().split(' ');
        QString const &name = t.at(0);
        QString const &value = t.at(1);

        if (name == "volume:")
            d->volume = value.toInt();
        else if (name == "repeat:")
            d->repeat = value.toInt() == 1;
        else if (name == "random:")
            d->random = value.toInt() == 1;
        else if (name == "playlist:")
            d->playlist = value.toUInt();
        else if (name == "playlistlength:")
            d->playlistlength = value.toInt();
        else if (name == "xfade:")
            d->xfade = value.toInt();
        else if (name == "state:") {
            int state = -1;
            if (value == "play")
                state = Playing;
            else if (value == "pause")
                state = Paused;
            else if (value == "stop")
                state = Stopped;

            if (state != -1 && state != d->state) {
                emit playerStateChanged(d->state = state);
                if (d->state == Playing)
                    d->statusTimer->start(250);
                else
                    d->statusTimer->stop();
            }
        }
        else if (name == "song:") {
            const int song = value.toInt();
            if (d->song != song)
                emit playlistItemChanged(d->song = song);
        }
        else if (name == "songid:")
            d->songid = value.toInt();
        else if (name == "time:") {
            QStringList s = value.split(':');
            d->position = s.at(0).toInt();
            d->duration = s.at(1).toInt();
        }
        else if (name == "bitrate:")
            d->bitrate = value.toInt();
        else if (name == "audio:") {
            QStringList s = value.split(':');
            d->frequency = s.at(0).toInt();
            d->bits = s.at(1).toInt();
            d->channels = s.at(2).toInt();
        }
    }
}

void MpdDaemon::setup()
{
    rawRecv();  // read off helo
    rawSend("idle", false);
    checkStatus();

    emit connected();
}

void MpdDaemon::shutdown()
{
    emit disconnected();
}

QStringList MpdDaemon::rawSend(QByteArray const &cmd, bool expect, bool *ok)
{
    d->socket->write(cmd);
    d->socket->write("\n", 1);
    d->socket->flush();

    if (expect)
        return rawRecv(ok);

    if (ok)
        *ok = true;
    return QStringList();
}

QStringList MpdDaemon::rawRecv(bool *ok)
{
    d->socket->waitForReadyRead(1000);
    QByteArray s;
    QStringList r;
    for (;;) {
        s = d->socket->readLine();

        if (s.startsWith("OK") || s.startsWith("ACK"))
            break;

        r << QString(s);
    }

    if (ok != 0)
        *ok = s.startsWith("OK");

    return r;
}

