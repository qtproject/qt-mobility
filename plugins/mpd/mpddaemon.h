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

#ifndef MPDDAEMON_H
#define MPDDAEMON_H

#include <QtCore/qobject.h>


class MpdDaemonPrivate;
class MpdDaemon : public QObject
{
    Q_OBJECT

public:
    enum { Playing = 1, Paused, Stopped };

    MpdDaemon(QObject *parent);
    ~MpdDaemon();

    int playerState() const;
    qint64 duration() const;
    int volume() const;
    bool muted() const;
    qint64 position() const;
    int currentSongPos() const;

    QStringList send(QString const &command, bool *ok = 0);

signals:
    void connected();
    void disconnected();
    void notify();
    void playlistChanged();
    void playerStateChanged(int state);
    void playlistItemChanged(int item);
    void mixerChanged();

private slots:
    void readData();
    void checkStatus();
    void setup();
    void shutdown();

private:
    MpdDaemonPrivate *d;

    QStringList rawSend(QByteArray const &cmd, bool expect, bool *ok = 0);
    QStringList rawRecv(bool *ok = 0);
};


#endif  // MPDDAEMON_H
