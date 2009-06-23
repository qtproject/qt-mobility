#include "qmediaplaylist.h"
#include "qmediaplaylist_p.h"
#include "qmediaplaylistsource.h"
#include "qlocalmediaplaylistsource.h"
#include "qmediaplaylistioplugin.h"

#include <QtCore/qlist.h>
#include <QtCore/qfile.h>

#include <private/qfactoryloader_p.h>

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, playlistIOLoader,
        (QMediaPlaylistIOInterface_iid, QLatin1String("/playlistformats"), Qt::CaseInsensitive))

/*!
    \class QMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief The QMediaPlaylist class provides a list of media sources to play.

    \sa QMediaSource
*/

int QMediaPlaylistPrivate::nextItemPos(int currentPos) const
{
    if (source->size() == 0)
        return -1;

    if (currentPos == -1)
        currentPos = source->currentItem();

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

int QMediaPlaylistPrivate::previousItemPos(int currentPos) const
{
    if (source->size() == 0)
        return -1;

    if (currentPos == -1)
        currentPos = source->currentItem();

    switch (source->playbackMode()) {
        case QMediaPlaylist::NoPlayback:
        case QMediaPlaylist::CurrentItemOnce:
            return -1;
        case QMediaPlaylist::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylist::Linear:
            return currentPos>0 ? currentPos-1 : -1;
        case QMediaPlaylist::Loop:
            return currentPos>0 ? currentPos-1 : source->size()-1;
    }

    return -1;
}


/*!
enum QMediaPlaylist::PlaybackMode

\item NoPlayback No item is playing right now.
\item CurrentItemOnce The current item is played only once.
\item CurrentItemInLoop The current item is played in the loop.
\item Linear
    Playback starts from the first to the last items and stops.
    QMediaPlaylist::nextItem() returns null item when the last
    one is currently playing.

\item Loop
    Playback continues from the first item after the last one finished playing.
*/

/*!
  Create a new playlist object for playlist \a source.
  If source is null, internal local memory playlist source will be created.
  */
QMediaPlaylist::QMediaPlaylist(QMediaPlaylistSource *source, QObject *parent)
    :QObject(*new QMediaPlaylistPrivate, parent)
{
    Q_D(QMediaPlaylist);

    if (source) {
        d->source = source;
    } else {
        d->source = new QLocalMediaPlaylistSource(this);
    }

    connect(d->source, SIGNAL(currentItemChanged(int)), this, SLOT(updateCurrentItem(int)));
    connect(d->source, SIGNAL(itemsChanged(int,int)), this, SIGNAL(itemsChanged(int,int)));
    connect(d->source, SIGNAL(itemsRemoved(int,int)), this, SIGNAL(itemsRemoved(int,int)));
    connect(d->source, SIGNAL(itemsInserted(int,int)), this, SIGNAL(itemsInserted(int,int)));
}

/*!
\internal
  */
QMediaPlaylist::QMediaPlaylist(QMediaPlaylistPrivate &dd, QObject *parent)
    :QObject(dd, parent)
{
    Q_D(QMediaPlaylist);

    connect(d->source, SIGNAL(currentItemChanged(int)), this, SLOT(updateCurrentItem(int)));
    connect(d->source, SIGNAL(itemsChanged(int,int)), this, SIGNAL(itemsChanged(int,int)));
    connect(d->source, SIGNAL(itemsRemoved(int,int)), this, SIGNAL(itemsRemoved(int,int)));
    connect(d->source, SIGNAL(itemsInserted(int,int)), this, SIGNAL(itemsInserted(int,int)));
}

/*!
  Destroys the playlist.
  */
QMediaPlaylist::~QMediaPlaylist()
{
}

/*!
  */
QMediaPlaylist::PlaybackMode QMediaPlaylist::playbackMode() const
{
    return d_func()->source->playbackMode();
}

/*!
  */
void QMediaPlaylist::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    d_func()->source->setPlaybackMode(mode);
}

/*!
  */
QMediaSource QMediaPlaylist::currentItem() const
{
    return itemAt(d_func()->source->currentItem());
}

/*!
  */
QMediaSource QMediaPlaylist::nextItem() const
{
    return itemAt(d_func()->nextItemPos());    
}

/*!
  */
QMediaSource QMediaPlaylist::itemAt(int pos) const
{
    if ( pos<0 || pos>=size() )
        return QMediaSource();
    else
        return d_func()->source->itemAt(pos);
}

/*!
  */
int QMediaPlaylist::currentPosition() const
{
    return d_func()->source->currentItem();
}

/*!
  */
int QMediaPlaylist::nextPosition(int position) const
{
    return d_func()->nextItemPos(position);
}

/*!
  */
int QMediaPlaylist::previousPosition(int position) const
{
    return d_func()->previousItemPos(position);
}

/*!
  */
int QMediaPlaylist::size() const
{
    return d_func()->source->size();
}

/*!
  */
bool QMediaPlaylist::isEmpty() const
{
    return size() > 0;
}

/*!
  */
bool QMediaPlaylist::append(const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    return d->source->append(source);
}

/*!
  */
bool QMediaPlaylist::append(const QList<QMediaSource> &sources)
{
    Q_D(QMediaPlaylist);
    return d->source->append(sources);
}

/*!
  */
bool QMediaPlaylist::insert(int pos, const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    return d->source->insert(pos,source);
}

/*!
  */
bool QMediaPlaylist::remove(int pos)
{
    Q_D(QMediaPlaylist);
    return d->source->remove(pos);
}

/*!
  */
bool QMediaPlaylist::remove(int start, int end)
{
    Q_D(QMediaPlaylist);
    return d->source->remove(start, end);
}

/*!
  */
bool QMediaPlaylist::clear()
{
    Q_D(QMediaPlaylist);
    return d->source->clear();
}

/*!
  */
bool QMediaPlaylistPrivate::readItems(QMediaPlaylistReader *reader)
{
    while (!reader->atEnd()) {
        QMediaSource src = reader->readItem();
        source->append(src);
    }

    return true;
}

/*!
  */
bool QMediaPlaylistPrivate::writeItems(QMediaPlaylistWritter *writter)
{
    for (int i=0; i<source->size(); i++) {
        if (!writter->writeItem(source->itemAt(i)))
            return false;
    }
    writter->close();
    return true;
}

/*!
  */
bool QMediaPlaylist::load(const QString &location, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->load(location,format))
        return true;

    foreach (QString const& key, playlistIOLoader()->keys()) {
        QMediaPlaylistIOInterface* plugin = qobject_cast<QMediaPlaylistIOInterface*>(playlistIOLoader()->instance(key));
        if (plugin && plugin->canRead(location,format)) {
            QMediaPlaylistReader *reader = plugin->createReader(location,QByteArray(format));
            if (reader && d->readItems(reader)) {
                delete reader;
                delete plugin;
                return true;
            }
            delete reader;
        }
        delete plugin;
    }

    return false;
}

/*!
  */
bool QMediaPlaylist::load(QIODevice * device, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->load(device,format))
        return true;

    foreach (QString const& key, playlistIOLoader()->keys()) {
        QMediaPlaylistIOInterface* plugin = qobject_cast<QMediaPlaylistIOInterface*>(playlistIOLoader()->instance(key));
        if (plugin && plugin->canRead(device,format)) {
            QMediaPlaylistReader *reader = plugin->createReader(device,QByteArray(format));
            if (reader && d->readItems(reader)) {
                delete reader;
                delete plugin;
                return true;
            }
            delete reader;
        }
        delete plugin;
    }

    return false;
}

/*!
  */
bool QMediaPlaylist::save(const QString &location, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->save(location,format))
        return true;

    QFile file(location);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return save(&file, format);

    return false;
}

/*!
  */
bool QMediaPlaylist::save(QIODevice * device, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->source->save(device,format))
        return true;

    foreach (QString const& key, playlistIOLoader()->keys()) {
        QMediaPlaylistIOInterface* plugin = qobject_cast<QMediaPlaylistIOInterface*>(playlistIOLoader()->instance(key));
        if (plugin && plugin->canWrite(device,format)) {
            QMediaPlaylistWritter *writter = plugin->createWritter(device,QByteArray(format));
            if (writter && d->writeItems(writter)) {
                delete writter;
                delete plugin;
                return true;
            }
            delete writter;
        }
        delete plugin;
    }

    return false;
}

/*!
  */
void QMediaPlaylist::advance()
{
    jump(d_func()->nextItemPos());
}

/*!
  */
void QMediaPlaylist::back()
{
    jump(d_func()->previousItemPos());
}

/*!
  */
void QMediaPlaylist::jump(int pos)
{
    Q_D(QMediaPlaylist);
    d->source->setCurrentItem(pos);
}

/*!
  Shuffle the list.
  */
void QMediaPlaylist::shuffle()
{
    Q_D(QMediaPlaylist);
    d->source->shuffle();
}

void QMediaPlaylist::updateCurrentItem(int pos)
{
    Q_D(QMediaPlaylist);

    emit currentPositionChanged(pos);
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

/*!
    \fn void QMediaPlaylist::activated(const QMediaSource &source)

    Signal the playback of \a source should be started.
    it's usually related to change of the current item
    in playlist.
*/

/*!
  \fn void QMediaPlaylist::currentItemChanged(const QMediaSource &source)
  */


/*
  TODO: add docs for signals:

    void QMediaPlaylist::currentPositionChanged(int);

    void QMediaPlaylist::playbackModeChanged(PlaybackMode mode);

    void QMediaPlaylist::itemsInserted(int start, int end);
    void QMediaPlaylist::itemsRemoved(int start, int end);
    void QMediaPlaylist::itemsChanged(int start, int end);
*/
