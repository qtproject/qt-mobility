#include "qmediaplaylist.h"
#include "qmediaplaylistsource.h"
#include "qlocalmediaplaylistsource.h"

#include <QList>

#include <QtCore/private/qobject_p.h>

/*!
    \class QMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaPlaylistPrivate : public QObjectPrivate
{
public:
    QMediaPlaylistSource *source;    
    QMediaSource currentItem;

    int nextItemPos() const;
};

int QMediaPlaylistPrivate::nextItemPos() const
{
    if (source->size() == 0)
        return -1;

    int currentPos = source->currentItem();

    switch (source->playbackMode()) {
        case QMediaPlaylist::NoPlayback:
        case QMediaPlaylist::CurrentItemOnce:
            return -1;
        case QMediaPlaylist::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylist::Linear:
            return currentPos < source->size()-1 ? currentPos+1 : -1;
        case QMediaPlaylist::Loop:
            return currentPos < source->size()-1 ? currentPos+1 : 0;
    }

    return -1;
}


/*!
enum PlaybackMode { NoPlayback, CurrentItemOnce, CurrentItemInLoop, Linear, Loop };
*/
QMediaPlaylist::QMediaPlaylist(QMediaPlaylistSource *playlistSource, QObject *parent)
    :QObject(*new QMediaPlaylistPrivate, parent)
{
    Q_D(QMediaPlaylist);

    if (playlistSource) {
        d->source = playlistSource;
    } else {
        d->source = new QLocalMediaPlaylistSource(this);
    }

    connect(d->source, SIGNAL(currentItemChanged(int)), this, SLOT(updateCurrentItem(int)));
    connect(d->source, SIGNAL(itemsChanged(int,int)), this, SIGNAL(itemsChanged(int,int)));
    connect(d->source, SIGNAL(itemsRemoved(int,int)), this, SIGNAL(itemsRemoved(int,int)));
    connect(d->source, SIGNAL(itemsInserted(int,int)), this, SIGNAL(itemsInserted(int,int)));
}

QMediaPlaylist::~QMediaPlaylist()
{
}

QMediaPlaylistSource *QMediaPlaylist::playlistSource()
{
    return d_func()->source;
}

const QMediaPlaylistSource *QMediaPlaylist::playlistSource() const
{
    return d_func()->source;
}


QMediaPlaylist::PlaybackMode QMediaPlaylist::playbackMode() const
{
    return d_func()->source->playbackMode();
}

void QMediaPlaylist::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    d_func()->source->setPlaybackMode(mode);
}


QMediaSource QMediaPlaylist::currentItem() const
{
    return itemAt(d_func()->source->currentItem());
}

QMediaSource QMediaPlaylist::nextItem() const
{
    return itemAt(d_func()->nextItemPos());    
}

QMediaSource QMediaPlaylist::itemAt(int pos) const
{
    if ( pos<0 || pos>=size() )
        return QMediaSource();
    else
        return d_func()->source->itemAt(pos);
}

int QMediaPlaylist::size() const
{
    return d_func()->source->size();
}

bool QMediaPlaylist::isEmpty() const
{
    return size() > 0;
}

bool QMediaPlaylist::append(const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    return d->source->append(source);
}

bool QMediaPlaylist::append(const QList<QMediaSource> &sources)
{
    Q_D(QMediaPlaylist);
    return d->source->append(sources);
}

bool QMediaPlaylist::insert(int pos, const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    return d->source->insert(pos,source);
}

bool QMediaPlaylist::remove(int pos)
{
    Q_D(QMediaPlaylist);
    return d->source->remove(pos);
}

bool QMediaPlaylist::remove(int start, int end)
{
    Q_D(QMediaPlaylist);
    return d->source->remove(start, end);
}

bool QMediaPlaylist::clear()
{
    Q_D(QMediaPlaylist);
    return d->source->clear();
}

bool QMediaPlaylist::load(const QString &location, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->load(location,format))
        return true;

    //TODO: implement playlist loading

    return false;
}

bool QMediaPlaylist::load(QIODevice * device, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->load(device,format))
        return true;

    //TODO: implement playlist loading

    return false;
}

bool QMediaPlaylist::save(const QString &location, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->save(location,format))
        return true;

    //TODO: implement playlist saving

    return false;
}

bool QMediaPlaylist::save(QIODevice * device, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->save(device,format))
        return true;

    //TODO: implement playlist saving

    return false;
}

bool QMediaPlaylist::isShuffled() const
{
    return d_func()->source->isShuffled();
}

void QMediaPlaylist::advance()
{
    jump(d_func()->nextItemPos());
}

void QMediaPlaylist::next()
{
    Q_D(QMediaPlaylist);
    jump( d->source->currentItem()+1 );
}

void QMediaPlaylist::prev()
{
    Q_D(QMediaPlaylist);
    jump( d->source->currentItem()-1 );
}

void QMediaPlaylist::jump(int pos)
{
    Q_D(QMediaPlaylist);
    d->source->setCurrentItem(pos);
}

void QMediaPlaylist::shuffle()
{
    Q_D(QMediaPlaylist);
    d->source->shuffle();
}

void QMediaPlaylist::unshuffle()
{
    Q_D(QMediaPlaylist);
    d->source->unshuffle();
}

void QMediaPlaylist::updateCurrentItem(int pos)
{
    Q_D(QMediaPlaylist);

    emit currentItemPosChanged(pos);
    QMediaSource src = d->source->itemAt(d->source->currentItem());
    if (src != d->currentItem) {
        d->currentItem = src;
        emit currentItemChanged(src);
        emit activated(src);
    } else if ( playbackMode() == CurrentItemInLoop ||
                (playbackMode() == Loop && size() == 1) ) {
        d->currentItem = src;
        emit activated(src);
    }
}

/*
  TODO: add docs for signals:

    void activated(const QMediaSource&);

    void currentItemChanged(const QMediaSource&);
    void currentItemPosChanged(int);

    void playbackModeChanged(PlaybackMode mode);

    void itemsInserted(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);
*/
