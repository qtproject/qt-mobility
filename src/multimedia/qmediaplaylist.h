/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include <QtCore/qobject.h>

#include <qmediacontent.h>
#include <qmediaobject.h>


class QMediaPlaylistProvider;

class QMediaPlaylistPrivate;
class Q_MEDIA_EXPORT QMediaPlaylist : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMediaPlaylist::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
    Q_PROPERTY(QMediaContent currentMedia READ currentMedia NOTIFY currentMediaChanged)
    Q_PROPERTY(int currentPosition READ currentPosition WRITE setCurrentPosition NOTIFY playlistPositionChanged)
    Q_ENUMS(PlaybackMode Error)

public:
    enum PlaybackMode { CurrentItemOnce, CurrentItemInLoop, Linear, Loop, Random };
    enum Error { NoError, FormatError, FormatNotSupportedError, NetworkError, AccessDeniedError };

    QMediaPlaylist(QMediaObject *mediaObject = 0, QObject *parent = 0);
    virtual ~QMediaPlaylist();

    PlaybackMode playbackMode() const;
    void setPlaybackMode(PlaybackMode mode);

    int currentPosition() const;
    QMediaContent currentMedia() const;

    int nextPosition(int steps = 1) const;
    int previousPosition(int steps = 1) const;

    QMediaContent media(int position) const;

    int size() const;
    bool isEmpty() const;
    bool isReadOnly() const;

    bool appendItem(const QMediaContent &content);
    bool appendItems(const QList<QMediaContent> &items);
    bool insertItem(int index, const QMediaContent &content);
    bool insertItems(int index, const QList<QMediaContent> &items);
    bool removeItem(int pos);
    bool removeItems(int start, int end);
    bool clear();

    void load(const QUrl &location, const char *format = 0);
    void load(QIODevice * device, const char *format = 0);

    bool save(const QUrl &location, const char *format = 0);
    bool save(QIODevice * device, const char *format);

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void shuffle();

    void next();
    void previous();

    void setCurrentPosition(int position);

Q_SIGNALS:
    void playlistPositionChanged(int position);
    void playbackModeChanged(QMediaPlaylist::PlaybackMode mode);
    void currentMediaChanged(const QMediaContent&);

    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted(int start, int end);
    void itemsAboutToBeRemoved(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);

    void loaded();
    void loadFailed();

protected:
    QMediaPlaylistPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QMediaPlaylist)
    Q_PRIVATE_SLOT(d_func(), void _q_loadFailed(QMediaPlaylist::Error, const QString &))
};

Q_DECLARE_METATYPE(QMediaPlaylist::PlaybackMode)
Q_DECLARE_METATYPE(QMediaPlaylist::Error)

#endif  // QMEDIAPLAYLIST_H
