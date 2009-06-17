#ifndef QABSTRACTMEDIAPLAYLIST_H
#define QABSTRACTMEDIAPLAYLIST_H

#include "qmediasource.h"

#include <QObject>

//do we need playlist iterator?

class QAbstractMediaPlaylistPrivate;
class QAbstractMediaPlaylist : public QObject
{
    Q_OBJECT
public:
    QAbstractMediaPlaylist(QObject* parent = 0);
    virtual ~QAbstractMediaPlaylist();

    bool load(const QString &location, const char *format = 0);
    bool save(const QString &location, const char *format = 0);

    virtual int currentItemPos() const = 0;
    QMediaSource currentItem() const;
    QMediaSource nextItem() const;

    virtual int size() const = 0;
    bool isEmpty() const;

    virtual QMediaSource operator [](int pos) const = 0;

    virtual void append(const QMediaSource &source) = 0;
    virtual void append(const QList<QMediaSource> &sources) = 0;
    virtual void insert(int pos, const QMediaSource &source) = 0;
    virtual void remove(int pos);
    virtual void remove(int start, int end) = 0;
    virtual void clear();

    virtual bool isShuffled() const = 0;

public Q_SLOTS:
    void next();
    void prev();

    virtual void jump(int) = 0;

    virtual void shuffle() = 0;
    virtual void unshuffle() = 0;

Q_SIGNALS:
    void currentItemChanged(const QMediaSource&);

    void itemsInserted(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);

protected:
    QAbstractMediaPlaylist(QAbstractMediaPlaylistPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QAbstractMediaPlaylist)
};

#endif // QABSTRACTMEDIAPLAYLIST_H
