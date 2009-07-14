/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

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


void QMediaPlaylistPrivate::_q_itemsAboutToBeInserted(int start, int end)
{
    Q_ASSERT(startPendingInsert == -1);
    Q_ASSERT(endPendingInsert == -1);

    startPendingInsert = start;
    endPendingInsert = end;

    emit q_func()->itemsAboutToBeInserted(start, end);
}

void QMediaPlaylistPrivate::_q_itemsInserted()
{
    Q_ASSERT(startPendingInsert != -1);
    Q_ASSERT(endPendingInsert != -1);

    int start = startPendingInsert;
    int end = endPendingInsert;

    startPendingInsert = -1;
    endPendingInsert = -1;

    emit q_func()->itemsInserted(start, end);
}

void QMediaPlaylistPrivate::_q_itemsAboutToBeRemoved(int start, int end)
{
    Q_ASSERT(startPendingRemove == -1);
    Q_ASSERT(endPendingRemove == -1);

    startPendingRemove = start;
    endPendingRemove = end;

    emit q_func()->itemsRemoved(start, end);
}

void QMediaPlaylistPrivate::_q_itemsRemoved()
{
    Q_ASSERT(startPendingRemove != -1);
    Q_ASSERT(endPendingRemove != -1);

    int start = startPendingRemove;
    int end = endPendingRemove;

    startPendingRemove = -1;
    endPendingRemove = -1;

    emit q_func()->itemsAboutToBeRemoved(start, end);
}

/*!
    \class QMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief The QMediaPlaylist class provides a list of media sources to play.

    Depending on playlist source implementation,
    most of playlist modifcation operations can be asynchronous.

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
    connect(d->source, SIGNAL(itemsAboutToBeInserted(int,int)), this, SLOT(_q_itemsAboutToBeInserted(int,int)));
    connect(d->source, SIGNAL(itemsInserted()), this, SLOT(_q_itemsInserted()));
    connect(d->source, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SLOT(_q_itemsAboutToBeRemoved(int,int)));
    connect(d->source, SIGNAL(itemsRemoved()), this, SLOT(_q_itemsRemoved()));
}

/*!
\internal
  */
QMediaPlaylist::QMediaPlaylist(QMediaPlaylistPrivate &dd, QObject *parent)
    :QObject(dd, parent)
{
}

/*!
  Destroys the playlist.
  */
QMediaPlaylist::~QMediaPlaylist()
{
}

/*!
  Returns the number of items in the playlist.

  \sa isEmpty()
  */
int QMediaPlaylist::size() const
{
    return d_func()->source->size();
}

/*!
  Returns true if the playlist contains no items; otherwise returns false.
  \sa size()
  */
bool QMediaPlaylist::isEmpty() const
{
    return size() == 0;
}

/*!
  Returns true if the playlist can be modified; otherwise returns false.
  \sa size().
  */
bool QMediaPlaylist::isReadOnly() const
{
    return d_func()->source->isReadOnly();
}

/*!
  Returns the media source at index \a position in the playlist.  
  */
QMediaSource QMediaPlaylist::itemAt(int position) const
{
    if (position<0 || position>=size())
        return QMediaSource();
    else
        return d_func()->source->itemAt(position);
}

/*!
  Append the media \a source to the playlist.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::append(const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    return d->source->append(source);
}

/*!
  Append the list of media \a sources to the playlist.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::append(const QList<QMediaSource> &sources)
{
    Q_D(QMediaPlaylist);
    return d->source->append(sources);
}

/*!
  Insert the media \a source to the playlist at position \a pos.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::insert(int pos, const QMediaSource &source)
{
    Q_D(QMediaPlaylist);
    return d->source->insert(pos,source);
}

/*!
  Remove the item from the playlist at position \a pos.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::remove(int pos)
{
    Q_D(QMediaPlaylist);
    return d->source->remove(pos);
}

/*!
  Remove the items from the playlist from position \a start to \a end inclusive.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::remove(int start, int end)
{
    Q_D(QMediaPlaylist);
    return d->source->remove(start, end);
}

/*!
  Remove all the items from the playlist.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::clear()
{
    Q_D(QMediaPlaylist);
    return d->source->clear();
}

bool QMediaPlaylistPrivate::readItems(QMediaPlaylistReader *reader)
{
    while (!reader->atEnd()) {
        QMediaSource src = reader->readItem();
        source->append(src);
    }

    return true;
}

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
  Load playlist from \a location. If \a format is specified, it is used,
  otherwise format is guessed from location name and data.

  New items are appended to playlist.

  Returns true if playlist was loaded succesfully, otherwise returns false.
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
  Load playlist from QIODevice \a device. If \a format is specified, it is used,
  otherwise format is guessed from device data.

  New items are appended to playlist.

  Returns true if playlist was loaded succesfully, otherwise returns false.
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
  Save playlist to \a location. If \a format is specified, it is used,
  otherwise format is guessed from location name.

  Returns true if playlist was saved succesfully, otherwise returns false.
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
  Save playlist to QIODevice \a device using format \a format.

  Returns true if playlist was saved succesfully, otherwise returns false.
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
  Shuffle items in the playlist.
*/
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

#include "moc_qmediaplaylist.cpp"
