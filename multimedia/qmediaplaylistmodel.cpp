#include "qmediaplaylistmodel.h"
#include "qmediaplaylist.h"

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
        QVariant location = m_playlist->itemAt(index.row()).dataLocation();
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
