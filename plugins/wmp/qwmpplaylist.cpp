#include "qwmpplaylist.h"

#include "qwmpmetadata.h"

#include <QtCore/qstringlist.h>


QWmpPlaylist::QWmpPlaylist(QObject *parent)
    : QMediaPlaylist(parent)
    , m_playlist(0)
{
}

QWmpPlaylist::~QWmpPlaylist()
{
    if (m_playlist)
        m_playlist->Release();
}

int QWmpPlaylist::count() const
{
    long count = 0;

    if (m_playlist)
        m_playlist->get_count(&count);

    return count;
}

QStringList QWmpPlaylist::keys(int index) const
{
    QStringList keys;

    IWMPMedia *media = 0;
    if (m_playlist && m_playlist->get_item(index, &media) == S_OK) {
        keys = QWmpMetaData::keys(media);

        media->Release();
    }

    return keys;
}

int QWmpPlaylist::valueCount(int index, const QString &key) const
{
    int count = 0;

    IWMPMedia *media = 0;
    if (m_playlist && m_playlist->get_item(index, &media) == S_OK) {
        count = QWmpMetaData::valueCount(media, key);

        media->Release();
    }

    return count;
}

QVariant QWmpPlaylist::value(int index, const QString &key, int value) const
{
    QVariant v;
    
    IWMPMedia *media = 0;
    if (m_playlist && m_playlist->get_item(index, &media) == S_OK) {
        v = QWmpMetaData::value(media, key, value);

        media->Release();
    }

    return v;
}

QVariantList QWmpPlaylist::values(int index, const QString &key) const
{
    QVariantList values;

    IWMPMedia *media = 0;
    if (m_playlist && m_playlist->get_item(index, &media) == S_OK) {
        values = QWmpMetaData::values(media, key);

        media->Release();
    }
    return values;
}

IWMPPlaylist *QWmpPlaylist::playlist() const
{
    return m_playlist;
}

void QWmpPlaylist::setPlaylist(IWMPPlaylist *playlist)
{
    if (m_playlist)
        m_playlist->Release();

    m_playlist = playlist;

    if (m_playlist)
        m_playlist->AddRef();

    emit changed();
}
