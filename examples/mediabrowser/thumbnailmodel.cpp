/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "thumbnailmodel.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qdir.h>
#include <QtCore/qtconcurrentrun.h>
#include <QtCore/qurl.h>
#include <QtGui/qimagereader.h>

#if defined(Q_WS_MAEMO_5)
const QSize ThumbnailModel::thumbnailSize(124, 124);
#else
const QSize ThumbnailModel::thumbnailSize(128, 128);
#endif

ThumbnailModel::ThumbnailModel(QAbstractGallery *gallery, QObject *parent)
    : QGalleryQueryModel(gallery, parent)
{
}

ThumbnailModel::~ThumbnailModel()
{
}

#ifndef QT_NO_FUTURE

QVariant ThumbnailModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole && index.isValid()) {
        QString id = itemId(index).toString();

        QFutureWatcher<QImage> *future = cache.object(id);

        if (!future) {
            future = new QFutureWatcher<QImage>;

            QString path = imagePath(index);

            if (!path.isEmpty()) {
                future->setFuture(QtConcurrent::run(ThumbnailModel::load, path));

                connect(future, SIGNAL(finished()), this, SLOT(thumbnailLoaded()));
            }

            cache.insert(id, future);
        }

        return !future->isCanceled()
                ? future->result()
                : QVariant();
    } else {
        return QGalleryQueryModel::data(index, role);
    }
}

QString ThumbnailModel::imagePath(const QModelIndex &index) const
{
    QUrl url = itemUrl(index);

    return url.isValid() ? thumbnailPath(url) : QString();
}

QString ThumbnailModel::thumbnailPath(const QUrl &url) const
{
#if defined(Q_OS_UNIX) && !(defined(Q_OS_SYMBIAN) || defined(Q_OS_MAC))
#if defined(Q_WS_MAEMO_5)
    QString thumbnailPath = QDir::homePath()
            + QLatin1String("/.thumbnails/cropped/")
            + QCryptographicHash::hash(url.toString().toUtf8(), QCryptographicHash::Md5).toHex()
            + QLatin1String(".jpeg");
#else
    QString thumbnailPath = QDir::homePath()
            + QLatin1String("/.thumbnails/normal/")
            + QCryptographicHash::hash(url.toEncoded(), QCryptographicHash::Md5).toHex()
            + QLatin1String(".png");
#endif
    if (QFile::exists(thumbnailPath))
        return thumbnailPath;
#endif

    return url.path();
}

void ThumbnailModel::thumbnailLoaded()
{
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() -1));
}

QImage ThumbnailModel::load(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setQuality(25);

    if (reader.supportsOption(QImageIOHandler::Size)) {
        QSize size = reader.size();

        if (!reader.supportsOption(QImageIOHandler::ScaledSize)
                && (size.width() > 1280 || size.height() > 1280)) {
            return QImage();
        }

        if (size.width() > thumbnailSize.width() || size.height() > thumbnailSize.height())
            size.scale(thumbnailSize, Qt::KeepAspectRatio);

        reader.setScaledSize(size);
    } else {
        reader.setScaledSize(thumbnailSize);
    }

    return reader.read();
}

#endif

