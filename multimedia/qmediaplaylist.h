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

#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include "qmediaresource.h"

#include <QtCore/qobject.h>

class QMediaPlaylistSource;

class QMediaPlaylistPrivate;
class Q_MEDIA_EXPORT QMediaPlaylist : public QObject
{
    Q_OBJECT
public:
    QMediaPlaylist(QMediaPlaylistSource *playlistSource = 0, QObject *parent = 0);
    virtual ~QMediaPlaylist();

    QMediaResource resource(int index) const;
    QMediaResourceList resources(int position) const;

    int size() const;
    bool isEmpty() const;
    bool isReadOnly() const;

    bool appendItem(const QMediaResource &resource);
    bool appendItem(const QMediaResourceList &resources);
    bool insertItem(int index, const QMediaResource &resource);
    bool insertItem(int index, const QMediaResourceList &resources);
    bool removeItem(int pos);
    bool removeItems(int start, int end);
    bool clear();

    bool load(const QString &location, const char *format = 0);
    bool load(QIODevice * device, const char *format = 0);
    bool save(const QString &location, const char *format = 0);
    bool save(QIODevice * device, const char *format);

public Q_SLOTS:
    void shuffle();

Q_SIGNALS:
    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted(int start, int end);
    void itemsAboutToBeRemoved(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);

protected:
    QMediaPlaylist(QMediaPlaylistPrivate &dd, QObject *parent);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_itemsAboutToBeInserted(int start, int end));
    Q_PRIVATE_SLOT(d_func(), void _q_itemsInserted());
    Q_PRIVATE_SLOT(d_func(), void _q_itemsAboutToBeRemoved(int start, int end));
    Q_PRIVATE_SLOT(d_func(), void _q_itemsRemoved());

    Q_DECLARE_PRIVATE(QMediaPlaylist)
};

#endif  // QMEDIAPLAYLIST_H
