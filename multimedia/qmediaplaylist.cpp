#include "qmediaplaylist.h"
#include "qabstractmediaplaylist_p.h"

#include <QList>

/*!
    \class QMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaPlaylistPrivate : public QAbstractMediaPlaylistPrivate
{
public:
    QList<QMediaSource> sources;
    int currentItem;
};

QMediaPlaylist::QMediaPlaylist(QObject *parent)
    :QAbstractMediaPlaylist(*new QMediaPlaylistPrivate, parent)
{
}

QMediaPlaylist::QMediaPlaylist(const QMediaSource &source, QObject* parent)
    :QAbstractMediaPlaylist(*new QMediaPlaylistPrivate, parent)
{
    append(source);
}

QMediaPlaylist::~QMediaPlaylist()
{
}

int QMediaPlaylist::currentItemPos() const
{
    return d_func()->currentItem;
}

int QMediaPlaylist::size() const
{
    return d_func()->sources.size();
}

QMediaSource QMediaPlaylist::operator [](int pos) const
{    
    return d_func()->sources[pos];
}

void QMediaPlaylist::append(const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    d->sources.append(source);
}

void QMediaPlaylist::append(const QList<QMediaSource> &sources)
{
    Q_D(QMediaPlaylist);
    d->sources.append(sources);
}

void QMediaPlaylist::insert(int pos, const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    d->sources.insert(pos, source);

    if (pos <= d->currentItem)
        d->currentItem++;
}

void QMediaPlaylist::remove(int fromPos, int toPos)
{
    Q_D(QMediaPlaylist);

    //keep the current item at the same source, if possible
    bool currentChanged = false;
    if (d->currentItem > toPos ) {
        d->currentItem -= (toPos-fromPos+1);
    } else if (d->currentItem >= fromPos && d->currentItem <= toPos) {
        currentChanged = true;
        d->currentItem = fromPos+1;
    }
    d->sources.erase(d->sources.begin()+fromPos, d->sources.begin()+toPos);

    if (currentChanged) {
        if (d->currentItem >= size())
            d->currentItem = -1;

        emit currentItemChanged(currentItem());
    }
}

bool QMediaPlaylist::isShuffled() const
{
    return false;
}

void QMediaPlaylist::jump(int pos)
{
    Q_D(QMediaPlaylist);
    if (pos != d->currentItem) {
        d->currentItem = pos;
        emit currentItemChanged(currentItem());
    }
}

void QMediaPlaylist::shuffle()
{
}

void QMediaPlaylist::unshuffle()
{
}

