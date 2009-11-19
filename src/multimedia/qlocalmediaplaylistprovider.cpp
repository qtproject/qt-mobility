/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qlocalmediaplaylistprovider.h>
#include <qmediaplaylistprovider_p.h>
#include <qmediacontent.h>

QTM_BEGIN_NAMESPACE

class QLocalMediaPlaylistProviderPrivate: public QMediaPlaylistProviderPrivate
{
public:
    QList<QMediaContent> resources;
};

QLocalMediaPlaylistProvider::QLocalMediaPlaylistProvider(QObject *parent)
    :QMediaPlaylistProvider(*new QLocalMediaPlaylistProviderPrivate, parent)
{
}

QLocalMediaPlaylistProvider::~QLocalMediaPlaylistProvider()
{
}

bool QLocalMediaPlaylistProvider::isReadOnly() const
{
    return false;
}

int QLocalMediaPlaylistProvider::size() const
{
    return d_func()->resources.size();
}

QMediaContent QLocalMediaPlaylistProvider::media(int pos) const
{
    return d_func()->resources.value(pos);
}

bool QLocalMediaPlaylistProvider::appendItem(const QMediaContent &content)
{
    Q_D(QLocalMediaPlaylistProvider);

    int pos = d->resources.count();

    emit itemsAboutToBeInserted(pos, pos);
    d->resources.append(content);
    emit itemsInserted(pos, pos);

    return true;
}


bool QLocalMediaPlaylistProvider::insertItem(int pos, const QMediaContent &content)
{
    Q_D(QLocalMediaPlaylistProvider);

    emit itemsAboutToBeInserted(pos, pos);
    d->resources.insert(pos, content);
    emit itemsInserted(pos,pos);

    return true;
}

bool QLocalMediaPlaylistProvider::removeItems(int fromPos, int toPos)
{
    Q_D(QLocalMediaPlaylistProvider);

    Q_ASSERT(fromPos >= 0);
    Q_ASSERT(fromPos <= toPos);
    Q_ASSERT(toPos < size());

    emit itemsAboutToBeRemoved(fromPos, toPos);
    d->resources.erase(d->resources.begin()+fromPos, d->resources.begin()+toPos+1);
    emit itemsRemoved(fromPos, toPos);

    return true;
}

bool QLocalMediaPlaylistProvider::removeItem(int pos)
{
    Q_D(QLocalMediaPlaylistProvider);

    emit itemsAboutToBeRemoved(pos, pos);
    d->resources.removeAt(pos);
    emit itemsRemoved(pos, pos);

    return true;
}

bool QLocalMediaPlaylistProvider::clear()
{
    Q_D(QLocalMediaPlaylistProvider);
    if (!d->resources.isEmpty()) {
        int lastPos = size()-1;
        emit itemsAboutToBeRemoved(0, lastPos);
        d->resources.clear();
        emit itemsRemoved(0, lastPos);
    }

    return true;
}

void QLocalMediaPlaylistProvider::shuffle()
{
    Q_D(QLocalMediaPlaylistProvider);
    if (!d->resources.isEmpty()) {
        QList<QMediaContent> resources;

        while (!d->resources.isEmpty()) {
            resources.append(d->resources.takeAt(qrand() % d->resources.size()));
        }

        d->resources = resources;
        emit itemsChanged(0,size()-1);
    }

}
QTM_END_NAMESPACE

