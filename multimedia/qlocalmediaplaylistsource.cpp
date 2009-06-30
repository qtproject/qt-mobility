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

#include "qlocalmediaplaylistsource.h"
#include "qmediaplaylistsource_p.h"

class QLocalMediaPlaylistSourcePrivate: public QMediaPlaylistSourcePrivate
{
public:
    QList<QMediaSource> sources;
};

QLocalMediaPlaylistSource::QLocalMediaPlaylistSource(QObject *parent)
    :QMediaPlaylistSource(*new QLocalMediaPlaylistSourcePrivate, parent)
{
}

QLocalMediaPlaylistSource::~QLocalMediaPlaylistSource()
{
}

bool QLocalMediaPlaylistSource::isReadOnly() const
{
    return false;
}

int QLocalMediaPlaylistSource::size() const
{
    return d_func()->sources.size();
}

QMediaSource QLocalMediaPlaylistSource::itemAt(int pos) const
{
    return d_func()->sources[pos];
}

bool QLocalMediaPlaylistSource::append(const QMediaSource &source)
{
    Q_D(QLocalMediaPlaylistSource);
    int pos = d->sources.size();
    emit itemsAboutToBeInserted(pos, pos);
    d->sources.append(source);
    emit itemsInserted();
    return true;
}

bool QLocalMediaPlaylistSource::append(const QList<QMediaSource> &sources)
{
    Q_D(QLocalMediaPlaylistSource);

    int pos = d->sources.size();

    emit itemsAboutToBeInserted(pos, pos+sources.size()-1);
    d->sources.append(sources);
    emit itemsInserted();

    return true;
}

bool QLocalMediaPlaylistSource::insert(int pos, const QMediaSource &source)
{
    Q_D(QLocalMediaPlaylistSource);

    emit itemsAboutToBeInserted(pos, pos);
    d->sources.insert(pos, source);
    emit itemsInserted();

    return true;
}

bool QLocalMediaPlaylistSource::remove(int fromPos, int toPos)
{
    Q_D(QLocalMediaPlaylistSource);

    Q_ASSERT(fromPos > 0);
    Q_ASSERT(fromPos <= toPos);
    Q_ASSERT(toPos < size());

    emit itemsAboutToBeRemoved(fromPos, toPos);
    d->sources.erase(d->sources.begin()+fromPos, d->sources.begin()+toPos);
    emit itemsRemoved();

    return true;
}

bool QLocalMediaPlaylistSource::remove(int pos)
{
    Q_D(QLocalMediaPlaylistSource);

    emit itemsAboutToBeRemoved(pos, pos);
    d->sources.removeAt(pos);
    emit itemsRemoved();

    return true;
}

bool QLocalMediaPlaylistSource::clear()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->sources.isEmpty()) {        
        emit itemsAboutToBeRemoved(0, size()-1);
        d->sources.clear();
        emit itemsRemoved();
    }

    return true;
}

void QLocalMediaPlaylistSource::shuffle()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->sources.isEmpty()) {
        QList<QMediaSource> sources;

        while (!d->sources.isEmpty()) {
            sources.append(d->sources.takeAt(qrand() % d->sources.size()));
        }

        d->sources = sources;
        emit itemsChanged(0,size()-1);
    }

}
