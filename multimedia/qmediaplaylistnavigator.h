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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIAPLAYLISTNAVIGATOR_H
#define QMEDIAPLAYLISTNAVIGATOR_H

#include "qmediaplaylist.h"
#include <QtCore/qobject.h>

class QMediaPlaylistNavigatorPrivate;
class Q_MEDIA_EXPORT QMediaPlaylistNavigator : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlaybackMode)
    Q_PROPERTY(QMediaPlaylistNavigator::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
    Q_PROPERTY(int currentPosition READ currentPosition WRITE jump NOTIFY currentPositionChanged)
    Q_PROPERTY(QMediaResourceList currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(QMediaResourceList nextItem READ nextItem)
    Q_PROPERTY(QMediaResourceList previousItem READ previousItem)
public:
    enum PlaybackMode { CurrentItemOnce, CurrentItemInLoop, Linear, Loop, Random };

    QMediaPlaylistNavigator(QMediaPlaylist *playlist, QObject *parent = 0);
    virtual ~QMediaPlaylistNavigator();

    QMediaPlaylist *playlist() const;
    void setPlaylist(QMediaPlaylist *playlist);

    PlaybackMode playbackMode() const;

    QMediaResourceList currentItem() const;
    QMediaResourceList nextItem(int steps = 1) const;
    QMediaResourceList previousItem(int steps = 1) const;

    QMediaResourceList itemAt(int position) const;

    int currentPosition() const;
    int nextPosition(int steps = 1) const;
    int previousPosition(int steps = 1) const;

public Q_SLOTS:
    void advance();
    void back();

    void jump(int);

    void setPlaybackMode(PlaybackMode mode);

Q_SIGNALS:
    void activated(const QMediaResourceList &resources);
    void currentPositionChanged(int);
    void playbackModeChanged(QMediaPlaylistNavigator::PlaybackMode mode);

    void surroundingItemsChanged();

protected:
    QMediaPlaylistNavigatorPrivate *d_ptr;

private:
    Q_DISABLE_COPY(QMediaPlaylistNavigator)
    Q_DECLARE_PRIVATE(QMediaPlaylistNavigator)

    Q_PRIVATE_SLOT(d_func(), void _q_itemsInserted(int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsRemoved(int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsChanged(int start, int end))
};

#endif // QMEDIAPLAYLISTNAVIGATOR_H
