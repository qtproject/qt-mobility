#ifndef QSHAREDMEDIAPLAYLIST_H
#define QSHAREDMEDIAPLAYLIST_H

#include "qabstractmediaplaylist.h"

class QSharedMediaPlaylistPrivate;
class QSharedMediaPlaylist : public QAbstractMediaPlaylist
{
    Q_OBJECT
public:
    QSharedMediaPlaylist(const QString& name, QObject *parent = 0);
    virtual ~QSharedMediaPlaylist();

    QString name() const;

    static QStringList sharedPlaylists();

    virtual int currentItemPos() const;
    virtual int size() const;

    virtual QMediaSource operator [](int pos) const;

    virtual void append(const QMediaSource &source);
    virtual void append(const QList<QMediaSource> &sources);
    virtual void insert(int pos, const QMediaSource &source);
    virtual void remove(int fromPos, int toPos);

    virtual bool isShuffled() const;

public Q_SLOTS:
    virtual void jump(int);

    virtual void shuffle();
    virtual void unshuffle();

private:
    Q_DECLARE_PRIVATE(QSharedMediaPlaylist)
};


#endif // QSHAREDMEDIAPLAYLIST_H
