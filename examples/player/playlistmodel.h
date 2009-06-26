#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QtCore/qabstractitemmodel.h>

class QMediaPlaylist;

class PlaylistModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Column
    {
        Track = 0,
        Title,
        Album,
        AlbumArtist,
        ContributingArtist,
        ColumnCount
    };

    PlaylistModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(
            int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QMediaPlaylist *playlist() const;
    void setPlaylist(QMediaPlaylist *playlist);

private slots:
    void playlistChanged();

private:
    QMediaPlaylist *m_playlist;
};

#endif
