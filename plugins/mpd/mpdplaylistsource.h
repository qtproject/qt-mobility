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

#ifndef MPDPLAYLISTSOURCE_H
#define MPDPLAYLISTSOURCE_H


#include <qmediaplaylistprovider.h>

class MpdDaemon;


class MpdPlaylistSourcePrivate;
class MpdPlaylistSource : public QMediaPlaylistProvider
{
    Q_OBJECT

public:
    MpdPlaylistSource(MpdDaemon *daemon, QObject *parent);
    ~MpdPlaylistSource();

    bool load(const QString &location, const char *format = 0);
    bool load(QIODevice *device, const char *format = 0);
    bool save(const QString &location, const char *format = 0);
    bool save(QIODevice *device, const char *format);

    int size() const;
    QMediaSource resources(int pos) const;

    bool isReadOnly() const;

    bool appendItem(const QMediaSource &source);
    bool insertItem(int pos, const QMediaSource &source);
    bool removeItem(int pos);
    bool removeItems(int start, int end);
    bool clear();

    void shuffle();

private slots:
    void playlistChanged();
    void loadPlaylist();

private:
    Q_DECLARE_PRIVATE(MpdPlaylistSource);
};
#endif  // MPDPLAYLISTSOURCE_H
