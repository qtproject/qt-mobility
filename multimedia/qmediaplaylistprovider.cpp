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

#include "qmediaplaylistprovider.h"
#include "qmediaplaylistprovider_p.h"

/*!
    \class QMediaPlaylistProvider
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

QMediaPlaylistProvider::QMediaPlaylistProvider(QObject *parent)
    :QObject(*new QMediaPlaylistProviderPrivate, parent)
{
}

QMediaPlaylistProvider::QMediaPlaylistProvider(QMediaPlaylistProviderPrivate &dd, QObject *parent)
    :QObject(dd,parent)
{
}

QMediaPlaylistProvider::~QMediaPlaylistProvider()
{
}

/*!
    \fn QMediaPlaylistProvider::size() const;
*/

/*!
    \fn QMediaPlaylistProvider::operator [](int pos) const;
*/


/*!
  Load playlist from \a location. If \a format is specified, it is used,
  otherwise format is guessed from location name and data.

  New items are appended to playlist.

  QMediaPlaylist uses QMediaPlaylistProvider to load/save playlists first,
  than it tries available playlist I/O plugins.

  Returns true if playlist was loaded succesfully, otherwise returns false.
*/
bool QMediaPlaylistProvider::load(const QUrl &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

/*!
  Load playlist from QIODevice \a device. If \a format is specified, it is used,
  otherwise format is guessed from device data.

  New items are appended to playlist.

  Returns true if playlist was loaded succesfully, otherwise returns false.
*/
bool QMediaPlaylistProvider::load(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

/*!
  Save playlist to \a location. If \a format is specified, it is used,
  otherwise format is guessed from location name.

  Returns true if playlist was saved succesfully, otherwise returns false.
  */
bool QMediaPlaylistProvider::save(const QUrl &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);
    return false;
}

/*!
  Save playlist to QIODevice \a device using format \a format.

  Returns true if playlist was saved succesfully, otherwise returns false.
*/
bool QMediaPlaylistProvider::save(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);
    return false;
}

/*!
  Returns true if the playlist is read-only; otherwise returns false.
*/
bool QMediaPlaylistProvider::isReadOnly() const
{
    return true;
}

/*!
  Append \a source to the playlist.

  Returns true if the operation is successfull, other wise return false.
*/
bool QMediaPlaylistProvider::appendItem(const QMediaSource &source)
{
    Q_UNUSED(source);
    return false;
}

/*!
  Insert a \a sources to the playlist at position \a pos.

  Returns true if the operation is successfull, other wise return false.
*/
bool QMediaPlaylistProvider::insertItem(int pos, const QMediaSource &source)
{
    Q_UNUSED(pos);
    Q_UNUSED(source);
    return false;
}

/*!
  Remove a \a sources from the playlist at position \a pos.

  Returns true if the operation is successfull, other wise return false.
*/
bool QMediaPlaylistProvider::removeItem(int pos)
{
    Q_UNUSED(pos);
    return false;
}

/*!
  Remove the items from the playlist from position \a start to \a end inclusive.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylistProvider::removeItems(int start, int end)
{
    for (int pos=start; pos<=end; pos++) {
        if (!removeItem(pos))
            return false;
    }

    return true;
}

/*!
  Remove all the items from the playlist.

  Returns true if the operation is successfull, other wise return false.
  */
bool QMediaPlaylistProvider::clear()
{
    return removeItems(0, size()-1);
}

/*!
  Shuffle items in the playlist.
*/
void QMediaPlaylistProvider::shuffle()
{
}
