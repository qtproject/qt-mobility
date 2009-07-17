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

#include "qwmpplaylist.h"

#include "qwmpevents.h"
#include "qwmpmetadata.h"
#include "qwmpglobal.h"
#include "qmediasource.h"

#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QWmpPlaylist::QWmpPlaylist(IWMPCore3 *player, QWmpEvents *events, QObject *parent)
    : QMediaPlaylistSource(parent)
    , m_player(player)
    , m_playlist(0)
    , m_count(0)
{
    if (m_player && m_player->get_currentPlaylist(&m_playlist) == S_OK)
        m_playlist->get_count(&m_count);

    connect(events, SIGNAL(CurrentPlaylistChange(WMPPlaylistChangeEventType)),
            this, SLOT(currentPlaylistChangeEvent(WMPPlaylistChangeEventType)));
    connect(events, SIGNAL(OpenPlaylistSwitch(IDispatch*)),
            this, SLOT(openPlaylistChangeEvent(IDispatch*)));
    connect(events, SIGNAL(MediaChange(IDispatch*)), this, SLOT(mediaChangeEvent(IDispatch*)));
}

QWmpPlaylist::~QWmpPlaylist()
{
    if (m_playlist)
        m_playlist->Release();

    if (m_player)
        m_player->Release();
}

bool QWmpPlaylist::load(const QString &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);

    return false;
}

bool QWmpPlaylist::load(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);

    return false;
}

bool QWmpPlaylist::save(const QString &location, const char *format)
{
    Q_UNUSED(location);
    Q_UNUSED(format);

    return false;
}

bool QWmpPlaylist::save(QIODevice * device, const char *format)
{
    Q_UNUSED(device);
    Q_UNUSED(format);

    return false;
}

int QWmpPlaylist::size() const
{
    return m_count;
}

QMediaSource QWmpPlaylist::itemAt(int pos) const
{
    QMediaSource source;

    IWMPMedia *media = 0;
    if (m_playlist && m_playlist->get_item(pos, &media) == S_OK) {
        BSTR uri = 0;
        if (media->get_sourceURL(&uri) == S_OK) {
            source.setDataLocation(QUrl(
                    QString::fromWCharArray(static_cast<const wchar_t *>(uri))));
            ::SysFreeString(uri);
        }
        media->Release();
    }

    return source;
}

bool QWmpPlaylist::isReadOnly() const
{
    return false;
}

bool QWmpPlaylist::append(const QMediaSource &source)
{
    bool appended = false;

    IWMPMedia *media = 0;
    if (m_playlist && m_player && m_player->newMedia(
            QAutoBStr(source.dataLocation().toString()), &media) == S_OK) {
        appended = m_playlist->appendItem(media) == S_OK;

        media->Release();
    }

    return appended;
}

bool QWmpPlaylist::append(const QList<QMediaSource> &sources)
{
    foreach (const QMediaSource &source, sources) {
        if (!append(source))
            return false;
    }
    return true;
}

bool QWmpPlaylist::insert(int pos, const QMediaSource &source)
{
    bool inserted = false;

    IWMPMedia *media = 0;
    if (m_playlist && m_player && m_player->newMedia(
            QAutoBStr(source.dataLocation().toString()), &media) == S_OK) {
        inserted = m_playlist->insertItem(pos, media) == S_OK;

        media->Release();
    }

    return inserted;
}

bool QWmpPlaylist::remove(int pos)
{
    IWMPMedia *media = 0;
    if (m_playlist->get_item(pos, &media) == S_OK) {
        bool removed = m_playlist->removeItem(media) == S_OK;

        media->Release();

        return removed;
    } else {
        return false;
    }
}

bool QWmpPlaylist::remove(int start, int end)
{
    if (!m_playlist)
        return false;

    for (int i = start; i <= end; ++i) {
        IWMPMedia *media = 0;
        if (m_playlist->get_item(start, &media) == S_OK) {
            bool removed = m_playlist->removeItem(media) == S_OK;

            media->Release();

            if (!removed)
                return false;
        }
    }
    return true;
}

bool QWmpPlaylist::clear()
{
    return m_playlist && m_playlist->clear() == S_OK;
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

void QWmpPlaylist::shuffle()
{
}


void QWmpPlaylist::currentPlaylistChangeEvent(WMPPlaylistChangeEventType change)
{
    Q_UNUSED(change);

    long count = 0;
    if (m_playlist && m_playlist->get_count(&count) == S_OK && count > 0) {
        if (count > m_count) {
            emit itemsAboutToBeInserted(m_count, count - 1);
            m_count = count;
            emit itemsInserted();
        } else if (count < m_count) {
            emit itemsAboutToBeRemoved(count, m_count - 1);
            m_count = count;
            emit itemsRemoved();
        }
    }
    if (m_count > 0)
        emit itemsChanged(0, m_count - 1);
}

void QWmpPlaylist::openPlaylistChangeEvent(IDispatch *dispatch)
{
    if (m_playlist && m_count > 0) {
        emit itemsAboutToBeRemoved(0, m_count - 1);
        m_playlist->Release();
        m_playlist = 0;
        m_count = 0;
        emit itemsRemoved();
    } else if (m_playlist) {
        m_playlist->Release();
        m_playlist = 0;
    }

    IWMPPlaylist *playlist = 0;
    if (dispatch && dispatch->QueryInterface(
            __uuidof(IWMPPlaylist), reinterpret_cast<void **>(&playlist))) {

        long count = 0;
        if (playlist->get_count(&count) == S_OK && count > 0) {
            emit itemsAboutToBeInserted(0, count - 1);
            m_playlist = playlist;
            m_count = count;
            emit itemsInserted();
        } else {
            m_playlist = playlist;
        }
    }
}

void QWmpPlaylist::mediaChangeEvent(IDispatch *dispatch)
{

    IWMPMedia *media = 0;
    if (dispatch &&  dispatch->QueryInterface(
            __uuidof(IWMPMedia), reinterpret_cast<void **>(&media)) == S_OK) {
        VARIANT_BOOL isMember = VARIANT_FALSE;

        if (media->isMemberOf(m_playlist, &isMember) == S_OK && isMember) {
            int index = QWmpMetaData::value(media, QLatin1String("PlaylistIndex"), 0).toInt();

            if (index >= 0)
                emit itemsChanged(index, index);
        }
        media->Release();
    }
}
