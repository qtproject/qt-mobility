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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlocalmediaplaylistsource.h"
#include "qmediaplaylistsource_p.h"

class QLocalMediaPlaylistSourcePrivate: public QMediaPlaylistSourcePrivate
{
public:
    QList<QMediaResourceList> resources;
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
    return d_func()->resources.size();
}

QMediaResourceList QLocalMediaPlaylistSource::resources(int pos) const
{
    return d_func()->resources.value(pos);
}

bool QLocalMediaPlaylistSource::appendItem(const QMediaResourceList &resources)
{
    Q_D(QLocalMediaPlaylistSource);

    int pos = d->resources.count();

    emit itemsAboutToBeInserted(pos, pos+resources.size()-1);
    d->resources.append(resources);
    emit itemsInserted();

    return true;
}

bool QLocalMediaPlaylistSource::insert(int pos, const QMediaResourceList &resources)
{
    Q_D(QLocalMediaPlaylistSource);

    emit itemsAboutToBeInserted(pos, pos);
    d->resources.insert(pos, resources);
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
    d->resources.erase(d->resources.begin()+fromPos, d->resources.begin()+toPos);
    emit itemsRemoved();

    return true;
}

bool QLocalMediaPlaylistSource::remove(int pos)
{
    Q_D(QLocalMediaPlaylistSource);

    emit itemsAboutToBeRemoved(pos, pos);
    d->resources.removeAt(pos);
    emit itemsRemoved();

    return true;
}

bool QLocalMediaPlaylistSource::clear()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->resources.isEmpty()) {
        emit itemsAboutToBeRemoved(0, size()-1);
        d->resources.clear();
        emit itemsRemoved();
    }

    return true;
}

void QLocalMediaPlaylistSource::shuffle()
{
    Q_D(QLocalMediaPlaylistSource);
    if (!d->resources.isEmpty()) {
        QList<QMediaResourceList> resources;

        while (!d->resources.isEmpty()) {
            resources.append(d->resources.takeAt(qrand() % d->resources.size()));
        }

        d->resources = resources;
        emit itemsChanged(0,size()-1);
    }

}
