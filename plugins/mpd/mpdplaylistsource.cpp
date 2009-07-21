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

#include "mpddaemon.h"
#include "mpdplaylistsource.h"
#include <qmediaplaylistsource_p.h>



class MpdPlaylistSourcePrivate : public QMediaPlaylistSourcePrivate
{
public:
    int entries;
    MpdDaemon* daemon;
};

MpdPlaylistSource::MpdPlaylistSource(MpdDaemon *daemon, QObject *parent):
    QMediaPlaylistSource(*new MpdPlaylistSourcePrivate, parent)
{
    Q_D(MpdPlaylistSource);

    d->entries = 0;
    d->daemon = daemon;

    connect(d->daemon, SIGNAL(connected()), SLOT(loadPlaylist()));
    connect(d->daemon, SIGNAL(playlistChanged()), SLOT(playlistChanged()));
}

MpdPlaylistSource::~MpdPlaylistSource()
{
}

bool MpdPlaylistSource::load(const QString &location, const char *format)
{
    return false;
}

bool MpdPlaylistSource::load(QIODevice * device, const char *format)
{
    return false;
}

bool MpdPlaylistSource::save(const QString &location, const char *format)
{
    return false;
}

bool MpdPlaylistSource::save(QIODevice * device, const char *format)
{
    return false;
}

int MpdPlaylistSource::size() const
{
    return d_func()->entries;
}

QMediaResourceList MpdPlaylistSource::resources(int pos) const
{
    Q_ASSERT(size() >= pos);
    return QMediaResourceList();
}

bool MpdPlaylistSource::isReadOnly() const
{
    return false;
}

bool MpdPlaylistSource::appendItem(const QMediaResourceList &resource)
{
    return false;
}

bool MpdPlaylistSource::insertItem(int pos, const QMediaResourceList &resource)
{
    return false;
}

bool MpdPlaylistSource::removeItem(int pos)
{
    bool    ok = false;
    d_func()->daemon->send(QString("delete %1").arg(pos), &ok);
    return ok;
}

bool MpdPlaylistSource::removeItems(int start, int end)
{
    for (int i = start; i < end; ++i)
        removeItem(i);

    return true;
}

bool MpdPlaylistSource::clear()
{
    bool    ok = false;
    d_func()->daemon->send("clear", &ok);
    return ok;
}

//public Q_SLOTS:
void MpdPlaylistSource::shuffle()
{
    d_func()->daemon->send("shuffe");
}

void MpdPlaylistSource::playlistChanged()
{
}

void MpdPlaylistSource::loadPlaylist()
{
    Q_D(MpdPlaylistSource);

    bool ok = false;
    QStringList r = d->daemon->send("playlist", &ok);

    if (ok) {
        d->entries = r.size();
        emit itemsAboutToBeInserted(0, d->entries);
        emit itemsInserted();
    }
    else
        d->entries = 0;
}

//Q_SIGNALS:
/*
    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted();
    void itemsAboutToBeRemoved(int start, int end);
    void itemsRemoved();
    void itemsChanged(int start, int end);
    */
