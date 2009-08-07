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

#include "qmediaplaylistmodel.h"
#include "qmediaplaylist.h"

#include <QtCore/qurl.h>

#include <QFileInfo>

QMediaPlaylistModel::QMediaPlaylistModel(QMediaPlaylist *playlist, QObject *parent)
    :QAbstractListModel(parent), m_playlist(playlist)
{
    connect(playlist, SIGNAL(itemsAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
    connect(playlist, SIGNAL(itemsInserted()), this, SLOT(endInsertItems()));
    connect(playlist, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
    connect(playlist, SIGNAL(itemsRemoved()), this, SLOT(endRemoveItems()));
    connect(playlist, SIGNAL(itemsChanged(int,int)), this, SLOT(changeItems(int,int)));
}

QMediaPlaylistModel::~QMediaPlaylistModel()
{
}

int QMediaPlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_playlist->size();
}

QVariant QMediaPlaylistModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        QUrl location = m_playlist->resource(index.row()).uri();
        return QFileInfo(location.toString()).fileName();
    } else
        return QVariant();
}

void QMediaPlaylistModel::beginInsertItems(int start, int end)
{
    beginInsertRows(QModelIndex(), start, end);
}

void QMediaPlaylistModel::endInsertItems()
{
    endInsertRows();
}

void QMediaPlaylistModel::beginRemoveItems(int start, int end)
{
    beginRemoveRows(QModelIndex(), start, end);
}

void QMediaPlaylistModel::endRemoveItems()
{
    endInsertRows();
}

void QMediaPlaylistModel::changeItems(int start, int end)
{
    emit dataChanged(index(start), index(end));
}
