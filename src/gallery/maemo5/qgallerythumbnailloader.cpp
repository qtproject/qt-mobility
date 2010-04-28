/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgallerythumbnailloader_p.h"


#include <QtCore/qcryptographichash.h>
#include <QtCore/qtconcurrentmap.h>
#include <QtCore/qfile.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qurl.h>
#include <QtDBus/qdbusinterface.h>
#include <QtDBus/qdbuspendingcall.h>
#include <QtDBus/qdbuspendingreply.h>
#include <QtGui/qdesktopservices.h>

QTM_BEGIN_NAMESPACE

static QImage qt_loadThumbnail(const QString &filePath)
{
    return QImage(filePath);
}

class QGalleryThumbnailDBusWatcher : public QDBusPendingCallWatcher
{
public:
    QGalleryThumbnailDBusWatcher(
            const QDBusPendingCall &call,
            uint cancelId,
            const QList<QGalleryImage> &images,
            const QStringList &filePaths)
        : QDBusPendingCallWatcher(call)
        , id(0)
        , cancelId(cancelId)
        , images(images)
        , filePaths(filePaths)
    {
    }

    uint id;
    uint cancelId;
    QList<QGalleryImage> images;
    QStringList filePaths;
};

class QGalleryThumbnailLoadWatcher : public QFutureWatcher<QImage>
{
public:
    QGalleryThumbnailLoadWatcher(const QList<QGalleryImage> images)
        : images(images)
    {
    }

    QList<QGalleryImage> images;
};

class QGalleryThumbnailLoaderPrivate : public QGalleryImageLoaderPrivate
{
    Q_DECLARE_PUBLIC(QGalleryThumbnailLoader)
public:
    QGalleryThumbnailLoaderPrivate()
        : thumbnailDir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)
#ifdef Q_WS_MAEMO_5
                + QLatin1String("/.thumbnails/cropped/"))
#else
                + QLatin1String("/.thumbnails/normal/"))
#endif
        , dbusInterface(
            QLatin1String("org.freedesktop.thumbnailer"),
            QLatin1String("/org/freedesktop/thumbnailer/Generic"),
            QLatin1String("org.freedesktop.thumbnailer.Generic"))
    {
    }

    void _q_dbusWatcherFinished(QDBusPendingCallWatcher *watcher) {
        dbusWatcherFinished(static_cast<QGalleryThumbnailDBusWatcher *>(watcher)); }
    void _q_loadWatcherFinished() {
        loadWatcherFinished(static_cast<QGalleryThumbnailLoadWatcher *>(q_func()->sender())); }
    void _q_imagesLoaded(int begin, int end) {
        imagesLoaded(static_cast<QGalleryThumbnailLoadWatcher *>(q_func()->sender()), begin, end); }

    void _q_dbusError(uint id, const QStringList &uris, int error, const QString &errorString);

    void _q_dbusFinished(uint id);

    void dbusWatcherFinished(QGalleryThumbnailDBusWatcher *watcher);
    void loadWatcherFinished(QGalleryThumbnailLoadWatcher *watcher);
    void imagesLoaded(QGalleryThumbnailLoadWatcher *watcher, int begin, int end);

    QString thumbnailPath(const QUrl &url) const;

    const QString thumbnailDir;
    QDBusInterface dbusInterface;

    QList<QGalleryThumbnailDBusWatcher *> dbusWatchers;
    QList<QGalleryThumbnailLoadWatcher *> loadWatchers;
};

void QGalleryThumbnailLoaderPrivate::dbusWatcherFinished(QGalleryThumbnailDBusWatcher *watcher)
{
    if (watcher->isError()) {
        qWarning("DBUS Error %s", qPrintable(watcher->error().message()));
        dbusWatchers.removeAll(watcher);

        watcher->deleteLater();
    } else {
        QDBusPendingReply<uint> reply(*watcher);

        watcher->id = reply.value();
    }
}

void QGalleryThumbnailLoaderPrivate::_q_dbusError(
        uint id, const QStringList &uris, int error, const QString &errorString)
{
    qWarning("Thumbnail Error %d, %s", error, qPrintable(errorString));
}

void QGalleryThumbnailLoaderPrivate::_q_dbusFinished(uint id)
{
    for (QList<QGalleryThumbnailDBusWatcher *>::iterator it = dbusWatchers.begin();
            it != dbusWatchers.end();
            ++it) {
        if ((*it)->id == id) {
            QGalleryThumbnailLoadWatcher *watcher = new QGalleryThumbnailLoadWatcher((*it)->images);

            loadWatchers.append(watcher);

            QObject::connect(watcher, SIGNAL(finished()), q_func(), SLOT(_q_loadWatcherFinished()));
            QObject::connect(watcher, SIGNAL(resultsReadyAt(int,int)),
                    q_func(), SLOT(_q_imagesLoaded(int,int)));

            watcher->setFuture(QtConcurrent::mapped((*it)->filePaths, qt_loadThumbnail));

            (*it)->deleteLater();

            dbusWatchers.erase(it);

            return;
        }
    }
}

void QGalleryThumbnailLoaderPrivate::loadWatcherFinished(QGalleryThumbnailLoadWatcher *watcher)
{
    loadWatchers.removeAll(watcher);

    watcher->deleteLater();
}

void QGalleryThumbnailLoaderPrivate::imagesLoaded(
        QGalleryThumbnailLoadWatcher *watcher, int begin, int end)
{
    QList<uint> ids;

    for (int i = begin; i < end; ++i) {
        watcher->images[i].data()->image = watcher->resultAt(i);

        ids.append(watcher->images.at(i).id());
    }

    emit q_func()->imagesLoaded(ids);
}

QString QGalleryThumbnailLoaderPrivate::thumbnailPath(const QUrl &url) const
{
    QString hash = QCryptographicHash::hash(url.toEncoded(), QCryptographicHash::Md5).toHex();

#ifdef Q_WS_MAEMO_5
    return thumbnailDir + hash + QLatin1String(".jpeg");
#else
    return thumbnailDir + hash + QLatin1String(".png");
#endif
}

QGalleryThumbnailLoader::QGalleryThumbnailLoader(QObject *parent)
    : QGalleryImageLoader(*new QGalleryThumbnailLoaderPrivate, parent)
{
    connect(&d_func()->dbusInterface, SIGNAL(Error(uint,QStringList,int,QString)),
            this, SLOT(_q_dbusError(uint,QStringList,int,QString)));
    connect(&d_func()->dbusInterface, SIGNAL(Finished(uint)), this, SLOT(_q_dbusFinished(uint)));
}

QGalleryThumbnailLoader::~QGalleryThumbnailLoader()
{
}

QVector<QGalleryImage> QGalleryThumbnailLoader::loadImages(
            QVector<QStringList>::const_iterator beginRows,
            QVector<QStringList>::const_iterator endRows)
{
    Q_D(QGalleryThumbnailLoader);

    QStringList dbusUrls;
    QStringList dbusFilePaths;
    QStringList dbusMimeTypes;
    QList<QGalleryImage> dbusImages;

    uint dbusCancelId = 0;

    QStringList loadFilePaths;
    QList<QGalleryImage> loadImages;

    QVector<QGalleryImage> images;

    for (QVector<QStringList>::const_iterator row = beginRows; row != endRows; ++row) {
        const QUrl url = QUrl::fromLocalFile(row->value(0));

        if (url.isValid()) {
            QString path = d->thumbnailPath(url);

            uint id = qHash(path);

            QGalleryImageData *data = new QGalleryImageData(id);

            if (QFile::exists(path)) {
                loadFilePaths.append(path);
                loadImages.append(data);
            } else {
                dbusUrls.append(url.toString());
                dbusFilePaths.append(path);
                dbusMimeTypes.append(QString());
                dbusImages.append(data);

                dbusCancelId ^= id;
            }

            images.append(QGalleryImage(data));
        } else {
            images.append(QGalleryImage());
        }
    }

    if (!dbusImages.isEmpty()) {
        QDBusPendingCall call = d->dbusInterface.asyncCall(
                QLatin1String("Queue"), dbusUrls, dbusMimeTypes, dbusCancelId);

        QGalleryThumbnailDBusWatcher *watcher = new QGalleryThumbnailDBusWatcher(
                call, dbusCancelId, dbusImages, dbusFilePaths);

        d->dbusWatchers.append(watcher);

        if (watcher->isFinished()) {
            d->dbusWatcherFinished(watcher);
        } else {
            connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SLOT(_q_dbusWatcherFinished(QDBusPendingCallWatcher*)));
        }
    }

    if (!loadImages.isEmpty()) {
        QGalleryThumbnailLoadWatcher *watcher = new QGalleryThumbnailLoadWatcher(loadImages);
        d->loadWatchers.append(watcher);

        connect(watcher, SIGNAL(finished()), this, SLOT(_q_loadWatcherFinished()));
        connect(watcher, SIGNAL(resultsReadyAt(int,int)), this, SLOT(_q_imagesLoaded(int,int)));

        watcher->setFuture(QtConcurrent::mapped(loadFilePaths, qt_loadThumbnail));
    }

    return images;
}

#include "moc_qgallerythumbnailloader_p.cpp"

QTM_END_NAMESPACE
