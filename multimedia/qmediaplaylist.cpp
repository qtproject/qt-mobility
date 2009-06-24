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

    connect(d->source, SIGNAL(itemsChanged(int,int)), this, SIGNAL(itemsChanged(int,int)));
    connect(d->source, SIGNAL(itemsAboutToBeInserted(int,int)), this, SIGNAL(itemsAboutToBeInserted(int,int)));
    connect(d->source, SIGNAL(itemsInserted()), this, SIGNAL(itemsInserted()));
    connect(d->source, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SIGNAL(itemsAboutToBeRemoved(int,int)));
    connect(d->source, SIGNAL(itemsRemoved()), this, SIGNAL(itemsRemoved()));
}

/*!
\internal
  */
QMediaPlaylist::QMediaPlaylist(QMediaPlaylistPrivate &dd, QObject *parent)
    :QObject(dd, parent)
{
    Q_D(QMediaPlaylist);

    connect(d->source, SIGNAL(itemsChanged(int,int)), this, SIGNAL(itemsChanged(int,int)));
    connect(d->source, SIGNAL(itemsAboutToBeInserted(int,int)), this, SIGNAL(itemsAboutToBeInserted(int,int)));
    connect(d->source, SIGNAL(itemsInserted()), this, SIGNAL(itemsInserted()));
    connect(d->source, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SIGNAL(itemsAboutToBeRemoved(int,int)));
    connect(d->source, SIGNAL(itemsRemoved()), this, SIGNAL(itemsRemoved()));
}

/*!
  Destroys the playlist.
  */
QMediaPlaylist::~QMediaPlaylist()
{
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

QMediaSource QMediaPlaylist::itemAt(int position) const
{
    if (position<0 || position>=size())
        return QMediaSource();
    else
        return d_func()->source->itemAt(position);
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

void QMediaPlaylist::shuffle()
{
    d_func()->source->shuffle();
}


/*!
    \fn void QMediaPlaylist::itemsInserted(int start, int end)

    This signal is emitted after media sources have been inserted into the playlist.
    The new items are those between start and end inclusive.
 */

/*!
    \fn void QMediaPlaylist::itemsRemoved(int start, int end)

    This signal is emitted after media sources have been removed from the playlist.
    The removed items are those between start and end inclusive.
 */

/*!
    \fn void QMediaPlaylist::itemsChanged(int start, int end)

    This signal is emitted after media sources have been changed in the playlist
    between start and end positions inclusive.
 */
