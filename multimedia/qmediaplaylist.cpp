#include "qmediaplaylist.h"
#include "qabstractmediaplaylist_p.h"

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
}

int QMediaPlaylist::size() const
{
}

QMediaSource QMediaPlaylist::operator [](int pos) const
{
}

void QMediaPlaylist::append(const QMediaSource &source)
{
}

void QMediaPlaylist::append(const QList<QMediaSource> &sources)
{
}

void QMediaPlaylist::insert(int pos, const QMediaSource &source)
{
}

void QMediaPlaylist::remove(int fromPos, int toPos)
{
}

bool QMediaPlaylist::isShuffled() const
{
}

void QMediaPlaylist::jump(int)
{
}

void QMediaPlaylist::shuffle()
{
}

void QMediaPlaylist::unshuffle()
{
}
