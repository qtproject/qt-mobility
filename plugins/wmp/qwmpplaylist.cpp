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
