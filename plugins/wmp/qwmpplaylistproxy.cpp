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

#include "qwmpplaylistproxy.h"

#include "qmediaplaylist.h"
#include "qwmpmetadata.h"

QWmpPlaylistProxy::QWmpPlaylistProxy(QMediaPlaylist *playlist, IWMPCore3 *player)
        : m_mediaPlaylist(playlist)
        , m_player(player)
        , m_wmpPlaylist(0)
        , m_startPendingInserts(-1)
        , m_endPendingInserts(-1)
{
    connect(m_mediaPlaylist, SIGNAL(itemsAboutToBeInserted(int,int)),
            this, SLOT(itemsAboutToBeInserted(int,int)));
    connect(m_mediaPlaylist, SIGNAL(itemsInserted()), this, SLOT(itemsInserted()));
    connect(m_mediaPlaylist, SIGNAL(itemsAboutToBeRemoved(int,int)),
            this, SLOT(itemsAboutToBeRemoved(int,int)));

    m_player->AddRef();

    if (m_player->newPlaylist(0, 0, &m_wmpPlaylist) == S_OK) {
        for (int i = 0; i < m_mediaPlaylist->size(); ++i) {
            QMediaSource source = m_mediaPlaylist->itemAt(i);

            IWMPMedia *media = 0;
            if (m_player->newMedia(QAutoBStr(source.dataLocation().toString()), &media) == S_OK)
                m_wmpPlaylist->appendItem(media);
        }
    }
}

QWmpPlaylistProxy::~QWmpPlaylistProxy()
{
    if (m_wmpPlaylist)
        m_wmpPlaylist->Release();

    m_player->Release();
}

IWMPPlaylist *QWmpPlaylistProxy::wmpPlaylist() const
{
    return m_wmpPlaylist;
}

void QWmpPlaylistProxy::itemsAboutToBeInserted(int start, int end)
{
    m_startPendingInserts = start;
    m_endPendingInserts = end;
}

void QWmpPlaylistProxy::itemsInserted()
{
    if (m_wmpPlaylist) {
        for (int i = m_startPendingInserts; i <= m_endPendingInserts; ++i) {
            QMediaSource source = m_mediaPlaylist->itemAt(i);

            IWMPMedia *media = 0;
            if (m_player->newMedia(QAutoBStr(source.dataLocation().toString()), &media) == S_OK)
                m_wmpPlaylist->appendItem(media);
        }
    }
}

void QWmpPlaylistProxy::itemsAboutToBeRemoved(int start, int end)
{
    if (m_wmpPlaylist) {
        for (int i = start; i <= end; ++i) {
            IWMPMedia *media = 0;
            if (m_wmpPlaylist->get_item(start, &media) == S_OK)
                m_wmpPlaylist->removeItem(media);
        }
    }

    m_startPendingInserts = -1;
    m_endPendingInserts = -1;
}
void QWmpPlaylistProxy::itemsChanged(int start, int end)
{
    itemsAboutToBeRemoved(start, end);
    itemsAboutToBeInserted(start, end);
    itemsInserted();
}
