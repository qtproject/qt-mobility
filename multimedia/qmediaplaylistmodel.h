#ifndef QMEDIAPLAYLISTMODEL_H
#define QMEDIAPLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QtCore/qobject.h>

class QMediaPlaylist;

class QMediaPlaylistModel : public QAbstractListModel
{
    Q_OBJECT
public:
    QMediaPlaylistModel(QMediaPlaylist *playlist, QObject *parent = 0);
    virtual ~QMediaPlaylistModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void beginInsertItems(int start, int end);
    void endInsertItems();
    void beginRemoveItems(int start, int end);
    void endRemoveItems();    
    void changeItems(int start, int end);

private:
    QMediaPlaylist *m_playlist;
};

#endif // QMEDIAPLAYLISTMODEL_H
