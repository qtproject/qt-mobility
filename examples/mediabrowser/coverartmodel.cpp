/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "coverartmodel.h"

#include "albumdelegate.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qdir.h>
#include <QtCore/qurl.h>

#define QT_GALLERY_MEDIA_ART_ILLEGAL_CHARACTERS \
    "\\(.*\\)|\\{.*\\}|\\[.*\\]|<.*>|[\\(\\)_\\{\\}\\[\\]\\!@#$\\^&\\*\\+\\=\\|\\\\/\"'\\?~`]"

CoverArtModel::CoverArtModel(QAbstractGallery *gallery, QObject *parent)
    : ThumbnailModel(gallery, parent)
#if defined(Q_OS_UNIX) && !(defined(Q_OS_SYMBIAN) || defined(Q_OS_MAC))
    , illegalCharacters(QLatin1String(QT_GALLERY_MEDIA_ART_ILLEGAL_CHARACTERS))
    , whitespace(QCryptographicHash::hash(" ", QCryptographicHash::Md5).toHex())
#endif
{
}

#if defined(Q_OS_UNIX) && !(defined(Q_OS_SYMBIAN) || defined(Q_OS_MAC))
QUrl CoverArtModel::imageUrl(const QModelIndex &index) const
{
    QString title = index.data(Qt::DisplayRole).toString();
    QString artist = index.data(AlbumDelegate::Artist).toString();

    QString fileName = QDir::homePath()
            + QLatin1String("/.cache/media-art/album-")
            + hash(artist)
            + QLatin1Char('-')
            + hash(title)
            + QLatin1String(".jpeg");

    return QUrl::fromLocalFile(fileName);
}

QString CoverArtModel::hash(const QString &identifier) const
{
    if (identifier.isEmpty()) {
        return whitespace;
    } else {
        return QCryptographicHash::hash(
                identifier.toLower().remove(illegalCharacters).simplified().toUtf8(),
                QCryptographicHash::Md5).toHex();
    }
}
#endif
