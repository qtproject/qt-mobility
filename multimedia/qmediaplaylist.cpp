/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmediaplaylist.h"
#include "qmediaplaylist_p.h"
#include "qmediaplaylistprovider.h"
#include "qlocalmediaplaylistprovider.h"
#include "qmediaplaylistioplugin.h"
#include <qabstractmediaservice.h>
#include <qmediaplaylistcontrol.h>
#include <qmediaplayercontrol.h>

#include <QtCore/qlist.h>
#include <QtCore/qfile.h>
#include <QtCore/qurl.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qcoreapplication.h>

#include <qmediapluginloader_p.h>

Q_GLOBAL_STATIC_WITH_ARGS(QMediaPluginLoader, playlistIOLoader,
        (QMediaPlaylistIOInterface_iid, QLatin1String("/playlistformats"), Qt::CaseInsensitive))


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
enum QMediaPlaylist::PlaybackMode

The QMediaPlaylist::PlaybackMode describes the order items in playlist are played.

\item
    CurrentItemOnce The current item is played only once.

\item
    CurrentItemInLoop The current item is played in the loop.

\item Linear
    Playback starts from the first to the last items and stops.
    next item is a null item when the last one is currently playing.

\item Loop
    Playback continues from the first item after the last one finished playing.

\item Random
    Play items in random order.
*/



/*!
  Create a new playlist object for playlist \a source with the given \a parent.
  If source is null, internal local memory playlist source will be created.
  */
QMediaPlaylist::QMediaPlaylist(QAbstractMediaObject *parent)
    : QObject(parent)
    , d_ptr(new QMediaPlaylistPrivate)
{
    Q_D(QMediaPlaylist);

    d->q_ptr = this;

    if (parent)
        d->control = qobject_cast<QMediaPlaylistControl*>(parent->service()->control(QMediaPlaylistControl_iid));

    if (!d->control)
        d->control = new QLocalMediaPlaylistControl(this);

    QMediaPlaylistProvider *playlist = d->control->playlistProvider();

    connect(playlist, SIGNAL(itemsChanged(int,int)), this, SIGNAL(itemsChanged(int,int)));
    connect(playlist, SIGNAL(itemsAboutToBeInserted(int,int)), this, SIGNAL(itemsAboutToBeInserted(int,int)));
    connect(playlist, SIGNAL(itemsInserted(int,int)), this, SIGNAL(itemsInserted(int,int)));
    connect(playlist, SIGNAL(itemsAboutToBeRemoved(int,int)), this, SIGNAL(itemsAboutToBeRemoved(int,int)));
    connect(playlist, SIGNAL(itemsRemoved(int,int)), this, SIGNAL(itemsRemoved(int,int)));

    connect(d->control, SIGNAL(playbackModeChanged(QMediaPlaylist::PlaybackMode)),
            this, SIGNAL(playbackModeChanged(QMediaPlaylist::PlaybackMode)));
    connect(d->control, SIGNAL(playlistProviderChanged()),
            this, SIGNAL(playlistProviderChanged()));
    connect(d->control, SIGNAL(playlistPositionChanged(int)),
            this, SIGNAL(playlistPositionChanged(int)));
    connect(d->control, SIGNAL(currentMediaChanged(QMediaSource)),
            this, SIGNAL(currentMediaChanged(QMediaSource)));

    if (parent)
        parent->bind(this);
}

/*!
  Destroys the playlist.
  */
QMediaPlaylist::~QMediaPlaylist()
{
    delete d_ptr;
}

/*!
  Returns the playlist used by this media player.
*/
QMediaPlaylistProvider* QMediaPlaylist::playlistProvider() const
{
    return d_func()->playlist();
}

/*!
  Set the playlist of this media player to \a mediaPlaylist.

  In many cases it is possible just to use the playlist
  constructed by player, but sometimes replacing the whole
  playlist allows to avoid copyting of all the items bettween playlists.

  Returns true if player can use this passed playlist; otherwise returns false.
*/
bool QMediaPlaylist::setPlaylistProvider(QMediaPlaylistProvider *playlist)
{
    return d_func()->control->setPlaylistProvider(playlist);
}

/*!
  \property QMediaPlaylist::playbackMode

  This property defines the order, items in playlist are played.

  \sa QMediaPlaylist::PlaybackMode
*/

QMediaPlaylist::PlaybackMode QMediaPlaylist::playbackMode() const
{
    return d_func()->control->playbackMode();
}

void QMediaPlaylist::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    Q_D(QMediaPlaylist);
    d->control->setPlaybackMode(mode);
}

/*!
  Returns position of the current media source in the playlist.
*/
int QMediaPlaylist::currentPosition() const
{
    return d_func()->control->currentPosition();
}

/*!
  Returns the current media source.
*/

QMediaSource QMediaPlaylist::currentMedia() const
{
    return d_func()->playlist()->media(currentPosition());
}

/*!
  Returns the position of item, which were current after calling advance()
  \a step times.

  Returned value depends on the size of playlist, current position
  and playback mode.

  \sa previousPosition QMediaPlaylist::playbackMode
*/
int QMediaPlaylist::nextPosition(int steps) const
{
    return d_func()->control->nextPosition(steps);
}

/*!
  Returns the position of item, which were current after calling back()
  \a step times.

  \sa nextPosition QMediaPlaylist::playbackMode
*/

int QMediaPlaylist::previousPosition(int steps) const
{
    return d_func()->control->previousPosition(steps);
}


/*!
  Returns the number of items in the playlist.

  \sa isEmpty()
  */
int QMediaPlaylist::size() const
{
    return d_func()->playlist()->size();
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
  \sa size()
  */
bool QMediaPlaylist::isReadOnly() const
{
    return d_func()->playlist()->isReadOnly();
}

/*!
  Returns the media source at index \a position in the playlist.
*/

QMediaSource QMediaPlaylist::media(int position) const
{
    return d_func()->playlist()->media(position);
}

/*!
  Append the list of media \a sources to the playlist.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::appendItem(const QMediaSource &source)
{
    return d_func()->control->playlistProvider()->appendItem(source);
}

/*!
  Insert the media \a source to the playlist at position \a pos.

  Returns true if the operation is successful, otherwise false.
*/

bool QMediaPlaylist::insertItem(int pos, const QMediaSource &source)
{
    return d_func()->playlist()->insertItem(pos, source);
}

/*!
  Remove the item from the playlist at position \a pos.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::removeItem(int pos)
{
    Q_D(QMediaPlaylist);
    return d->playlist()->removeItem(pos);
}

/*!
  Remove the items from the playlist from position \a start to \a end inclusive.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::removeItems(int start, int end)
{
    Q_D(QMediaPlaylist);
    return d->playlist()->removeItems(start, end);
}

/*!
  Remove all the items from the playlist.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylist::clear()
{
    Q_D(QMediaPlaylist);
    return d->playlist()->clear();
}

bool QMediaPlaylistPrivate::readItems(QMediaPlaylistReader *reader)
{
    while (!reader->atEnd())
        playlist()->appendItem(reader->readItem());

    return true;
}

bool QMediaPlaylistPrivate::writeItems(QMediaPlaylistWritter *writter)
{
    for (int i=0; i<playlist()->size(); i++) {
        if (!writter->writeItem(playlist()->media(i)))
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
bool QMediaPlaylist::load(const QUrl &location, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->playlist()->load(location,format))
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
    if (d->playlist()->load(device,format))
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
bool QMediaPlaylist::save(const QUrl &location, const char *format)
{
    Q_D(QMediaPlaylist);
    if (d->playlist()->save(location,format))
        return true;

    QFile file(location.toLocalFile());

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
    if (d->playlist()->save(device,format))
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
    d_func()->playlist()->shuffle();
}


/*!
    Advance to the next media source in playlist.
*/
void QMediaPlaylist::advance()
{
    d_func()->control->advance();
}

/*!
    Return to the previous media source in playlist.
*/
void QMediaPlaylist::back()
{
    d_func()->control->back();
}

/*!
    Activate media source from playlist at position \a playlistPosition.
*/

void QMediaPlaylist::setCurrentPosition(int playlistPosition)
{
    d_func()->control->setCurrentPosition(playlistPosition);
}

/*!
    \fn void QMediaPlaylist::itemsInserted(int start, int end)

    This signal is emitted after media sources have been inserted into the playlist.
    The new items are those between \a start and \a end inclusive.
 */

/*!
    \fn void QMediaPlaylist::itemsRemoved(int start, int end)

    This signal is emitted after media sources have been removed from the playlist.
    The removed items are those between \a start and \a end inclusive.
 */

/*!
    \fn void QMediaPlaylist::itemsChanged(int start, int end)

    This signal is emitted after media sources have been changed in the playlist
    between \a start and \a end positions inclusive.
 */

#include "moc_qmediaplaylist.cpp"
