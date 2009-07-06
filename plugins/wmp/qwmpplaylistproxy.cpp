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
