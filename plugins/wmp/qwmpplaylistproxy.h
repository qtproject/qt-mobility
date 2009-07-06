#ifndef QWMPPLAYLISTPROXY_H
#define QWMPPLAYLISTPROXY_H

#include <QtCore/qobject.h>

#include <wmp.h>

class QMediaPlaylist;

class QWmpPlaylistProxy : public QObject
{
    Q_OBJECT
public:
    QWmpPlaylistProxy(QMediaPlaylist *playlist, IWMPCore3 *player);
    ~QWmpPlaylistProxy();

    IWMPPlaylist *wmpPlaylist() const;

private Q_SLOTS:
    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted();
    void itemsAboutToBeRemoved(int start, int end);
    void itemsChanged(int start, int end);

private:
    QMediaPlaylist *m_mediaPlaylist;
    IWMPCore3 *m_player;
    IWMPPlaylist *m_wmpPlaylist;

    int m_startPendingInserts;
    int m_endPendingInserts;
};

#endif
