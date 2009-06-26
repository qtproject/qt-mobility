#include "playlistmodel.h"

#include "qwmpplaylist.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_playlist(0)
{
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid() ? m_playlist->count() : 0;
    
}

int PlaylistModel::columnCount(const QModelIndex &parent) const
{
    return !parent.isValid() ? ColumnCount : 0;
}

QModelIndex PlaylistModel::index(int row, int column, const QModelIndex &parent) const
{
    return m_playlist && !parent.isValid()
            && row >= 0 && row < m_playlist->count()
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
        switch (index.column()) {
        case Track:
            return m_playlist->value(index.row(), QLatin1String("WM/TrackNumber"));
        case Title:
            return m_playlist->value(index.row(), QLatin1String("Title"));
        case Album:
            return m_playlist->value(index.row(), QLatin1String("WM/AlbumTitle"));
        case AlbumArtist:
            return m_playlist->value(index.row(), QLatin1String("WM/AlbumArtist"));
        case ContributingArtist:
            {
                QVariantList values = m_playlist->values(
                        index.row(), QLatin1String("Author"));

                if (!values.isEmpty()) {
                    QString artists = values.first().toString();
                    for (int i = 1; i < values.count(); ++i)
                        artists += QLatin1String("; ") + values.at(i).toString();
                    return artists;
                };
            }
            break;
        case ColumnCount:
            break;
        }
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
    if (m_playlist)
        disconnect(m_playlist, SIGNAL(changed()), this, SLOT(playlistChanged()));

    m_playlist = playlist;

    if (m_playlist)
        connect(m_playlist, SIGNAL(changed()), this, SLOT(playlistChanged()));

    reset();
}

void PlaylistModel::playlistChanged()
{
    reset();
}