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

#include "playlistmodel.h"

#include <QtCore/qfileinfo.h>
#include <QtCore/qurl.h>

#include <qmediaplaylist.h>
#include <qmediametadata.h>



PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_playlist(0)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() ? m_playlist->size() : 0;
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid()
            && row >= 0 && row < m_playlist->size()
            && column >= 0 && column < ColumnCount
        ? createIndex(row, column)
        : QModelIndex();
}

QModelIndex PlaylistModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);

    return QModelIndex();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole) {
        QVariant    value = m_data[index];
        if (!value.isValid() && index.column() == Title) {
            QUrl location = m_playlist->resource(index.row()).uri();
            return QFileInfo(location.path()).fileName();
        }

        return value;
        /*
        switch (index.column()) {
        case Track:
            return m_playlist->itemAt(index.row()).mimeType();
//            return m_playlist->value(index.row(), QLatin1String("WM/TrackNumber"));
        case Title:
            {
                QVariant location = m_playlist->itemAt(index.row()).dataLocation();
                return QFileInfo(location.toString()).fileName();
            }
            //return m_playlist->itemAt(index.row()).mimeType();
//            return m_playlist->value(index.row(), QLatin1String("Title"));
        case Album:
            return m_playlist->itemAt(index.row()).mimeType();
//            return m_playlist->value(index.row(), QLatin1String("WM/AlbumTitle"));
        case AlbumArtist:
            return m_playlist->itemAt(index.row()).mimeType();
//            return m_playlist->value(index.row(), QLatin1String("WM/AlbumArtist"));
        case ContributingArtist:
            {
            return m_playlist->itemAt(index.row()).mimeType();
            *//*
                QVariantList values = m_playlist->values(
                        index.row(), QLatin1String("Author"));

                if (!values.isEmpty()) {
                    QString artists = values.first().toString();
                    for (int i = 1; i < values.count(); ++i)
                        artists += QLatin1String("; ") + values.at(i).toString();
                    return artists;
                };
            *//*}
            break;
        case ColumnCount:
            break;
        }
*/
    }
    return QVariant();
}

QVariant PlaylistModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case Track:
            return tr("Track");
        case Title:
            return tr("Title");
        case Album:
            return tr("Album");
        case AlbumArtist:
            return tr("Album Artist");
        case ContributingArtist:
            return tr("Contributing Artist");
        case ColumnCount:
            break;
        }
    }
    return QVariant();
}

QMediaPlaylist *PlaylistModel::playlist() const
{
    return m_playlist;
}

void PlaylistModel::setPlaylist(QMediaPlaylist *playlist)
{
    if (m_playlist) {
        disconnect(m_playlist, SIGNAL(itemsAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
        disconnect(m_playlist, SIGNAL(itemsInserted(int,int)), this, SLOT(endInsertItems()));
        disconnect(m_playlist, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
        disconnect(m_playlist, SIGNAL(itemsRemoved(int,int)), this, SLOT(endRemoveItems()));
        disconnect(m_playlist, SIGNAL(itemsChanged(int,int)), this, SLOT(changeItems(int,int)));
    }

    m_playlist = playlist;

    if (m_playlist) {
        connect(m_playlist, SIGNAL(itemsAboutToBeInserted(int,int)), this, SLOT(beginInsertItems(int,int)));
        connect(m_playlist, SIGNAL(itemsInserted(int,int)), this, SLOT(endInsertItems()));
        connect(m_playlist, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SLOT(beginRemoveItems(int,int)));
        connect(m_playlist, SIGNAL(itemsRemoved(int,int)), this, SLOT(endRemoveItems()));
        connect(m_playlist, SIGNAL(itemsChanged(int,int)), this, SLOT(changeItems(int,int)));
    }


    reset();
}

bool PlaylistModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role);
    m_data[index] = value;
    emit dataChanged(index, index);
	return true;
}

void PlaylistModel::beginInsertItems(int start, int end)
{
    m_data.clear();
    beginInsertRows(QModelIndex(), start, end);
}

void PlaylistModel::endInsertItems()
{
    endInsertRows();
}

void PlaylistModel::beginRemoveItems(int start, int end)
{
    m_data.clear();
    beginRemoveRows(QModelIndex(), start, end);
}

void PlaylistModel::endRemoveItems()
{
    endInsertRows();
}

void PlaylistModel::changeItems(int start, int end)
{
    m_data.clear();
    emit dataChanged(index(start,0), index(end,ColumnCount));
}


