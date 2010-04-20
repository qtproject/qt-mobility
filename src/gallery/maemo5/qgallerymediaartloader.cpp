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

#include "qgallerymediaartloader_p.h"

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

static QImage qt_loadMediaArt(const QString &filePath)
{
    return QImage(filePath);
}

class QGalleryMediaArtDBusWatcher : public QDBusPendingCallWatcher
{
public:
    QGalleryMediaArtDBusWatcher(
            const QDBusPendingCall &call,
            uint cancelId,
            const QString &artistName,
            const QString &albumTitle,
            const QGalleryImage &image,
            const QString &filePath)
        : QDBusPendingCallWatcher(call)
        , id(0)
        , cancelId(cancelId)
        , artistName(artistName)
        , albumTitle(albumTitle)
        , image(image)
        , filePath(filePath)
    {
    }

    uint id;
    uint cancelId;
    QString artistName;
    QString albumTitle;
    QGalleryImage image;
    QString filePath;
};

class QGalleryMediaArtLoadWatcher : public QFutureWatcher<QImage>
{
public:
    QGalleryMediaArtLoadWatcher(const QList<QGalleryImage> images)
        : images(images)
    {
    }

    QList<QGalleryImage> images;
};

class QGalleryMediaArtLoaderPrivate : public QGalleryImageLoaderPrivate
{
    Q_DECLARE_PUBLIC(QGalleryMediaArtLoader)
public:
    QGalleryMediaArtLoaderPrivate()
        : mediaArtPrefix(QDesktopServices::storageLocation(QDesktopServices::CacheLocation)
                + QLatin1String("media-art/album-"))
        , dbusInterface(
            QLatin1String("com.nokia.albumart"),
            QLatin1String("/com/nokia/albumart/Requester"),
            QLatin1String("com.nokia.albumart.Requester"))
    {
    }

    void _q_dbusWatcherFinished(QDBusPendingCallWatcher *watcher) {
        dbusWatcherFinished(static_cast<QGalleryMediaArtDBusWatcher *>(watcher)); }
    void _q_loadWatcherFinished() {
        loadWatcherFinished(static_cast<QGalleryMediaArtLoadWatcher *>(q_func()->sender())); }
    void _q_imagesLoaded(int begin, int end) {
        imagesLoaded(static_cast<QGalleryMediaArtLoadWatcher *>(q_func()->sender()), begin, end); }


    void _q_dbusReady(
            const QString &artistName, const QString &albumTitle, const QString &filePath);
    void _q_dbusError(uint id, int error, const QString &errorString);
    void _q_dbusFinished(uint id);

    void dbusWatcherFinished(QGalleryMediaArtDBusWatcher *watcher);
    void loadWatcherFinished(QGalleryMediaArtLoadWatcher *watcher);
    void imagesLoaded(QGalleryMediaArtLoadWatcher *watcher, int begin, int end);

    QString imagePath(const QString &artistName, const QString &albumTitle) const;

    const QString mediaArtPrefix;
    QDBusInterface dbusInterface;

    QList<QGalleryMediaArtDBusWatcher *> dbusWatchers;
    QList<QGalleryMediaArtLoadWatcher *> loadWatchers;
};

void QGalleryMediaArtLoaderPrivate::dbusWatcherFinished(QGalleryMediaArtDBusWatcher *watcher)
{
    if (watcher->isError()) {
        dbusWatchers.removeAll(watcher);

        watcher->deleteLater();
    } else {
        QDBusPendingReply<uint> reply(*watcher);

        watcher->id = reply.value();
    }
}

void QGalleryMediaArtLoaderPrivate::_q_dbusError(uint id, int error, const QString &errorString)
{
    qWarning("DBus Error %d %d: %s", id, error, qPrintable(errorString));
}

void QGalleryMediaArtLoaderPrivate::_q_dbusReady(
        const QString &artistName, const QString &albumTitle, const QString &filePath)
{
    for (QList<QGalleryMediaArtDBusWatcher *>::iterator it = dbusWatchers.begin();
            it != dbusWatchers.end();
            ++it) {
        if ((*it)->artistName == artistName && (*it)->albumTitle == albumTitle) {
            (*it)->filePath = filePath;

            return;
        }
    }
}

void QGalleryMediaArtLoaderPrivate::_q_dbusFinished(uint id)
{
    for (QList<QGalleryMediaArtDBusWatcher *>::iterator it = dbusWatchers.begin();
            it != dbusWatchers.end();
            ++it) {
        if ((*it)->id == id) {
            QGalleryMediaArtLoadWatcher *watcher = new QGalleryMediaArtLoadWatcher(
                    QList<QGalleryImage>() << (*it)->image);

            loadWatchers.append(watcher);

            QObject::connect(watcher, SIGNAL(finished()), q_func(), SLOT(_q_loadWatcherFinished()));
            QObject::connect(watcher, SIGNAL(resultsReadyAt(int,int)),
                    q_func(), SLOT(_q_imagesLoaded(int,int)));

            watcher->setFuture(QtConcurrent::mapped(
                    QStringList() << (*it)->filePath, qt_loadMediaArt));

            (*it)->deleteLater();

            dbusWatchers.erase(it);

            return;
        }
    }
}

void QGalleryMediaArtLoaderPrivate::loadWatcherFinished(QGalleryMediaArtLoadWatcher *watcher)
{
    loadWatchers.removeAll(watcher);

    watcher->deleteLater();
}

void QGalleryMediaArtLoaderPrivate::imagesLoaded(
        QGalleryMediaArtLoadWatcher *watcher, int begin, int end)
{
    QList<uint> ids;

    for (int i = begin; i < end; ++i) {
        watcher->images[i].data()->image = watcher->resultAt(i);

        ids.append(watcher->images.at(i).id());
    }

    emit q_func()->imagesLoaded(ids);
}

QString QGalleryMediaArtLoaderPrivate::imagePath(const QString &artist, const QString &album) const
{
    QString artistHash = QCryptographicHash::hash(artist.toUtf8(), QCryptographicHash::Md5).toHex();
    QString albumHash = QCryptographicHash::hash(album.toUtf8(), QCryptographicHash::Md5).toHex();

    return mediaArtPrefix + artistHash + QLatin1Char('-') + albumHash + QLatin1String(".jpeg");
}

QGalleryMediaArtLoader::QGalleryMediaArtLoader(QObject *parent)
    : QGalleryImageLoader(*new QGalleryMediaArtLoaderPrivate, parent)
{
    connect(&d_func()->dbusInterface, SIGNAL(Ready(QString,QString,QString)),
            this, SLOT(_q_dbusReady(QString,QString,QString)));
    connect(&d_func()->dbusInterface, SIGNAL(Error(uint,int,QString)),
            this, SLOT(_q_dbusError(uint,int,QString)));
    connect(&d_func()->dbusInterface, SIGNAL(Finished(uint)), this, SLOT(_q_dbusFinished(uint)));
}

QGalleryMediaArtLoader::~QGalleryMediaArtLoader()
{
}

QVector<QGalleryImage> QGalleryMediaArtLoader::loadImages(
            QVector<QStringList>::const_iterator beginRows,
            QVector<QStringList>::const_iterator endRows)
{
    Q_D(QGalleryMediaArtLoader);

    QStringList loadFilePaths;
    QList<QGalleryImage> loadImages;

    QVector<QGalleryImage> images;

    for (QVector<QStringList>::const_iterator row = beginRows; row != endRows; ++row) {
        QString albumTitle = row->value(0).toLower();
        QString artistName = row->value(1).toLower();

        if (!albumTitle.isEmpty()) {
            QString path = d->imagePath(artistName, albumTitle);

            uint id = qHash(path);

            QGalleryImageData *data = new QGalleryImageData(id);
            QGalleryImage image(data);

            if (QFile::exists(path)) {
                loadFilePaths.append(path);
                loadImages.append(data);
            } else {
                QDBusPendingCall call = d->dbusInterface.asyncCall(
                        QLatin1String("Queue"),
                        artistName,
                        albumTitle,
                        QLatin1String("album"),
                        id);

                QGalleryMediaArtDBusWatcher *watcher = new QGalleryMediaArtDBusWatcher(
                        call, id, artistName, albumTitle, image, path);

                d->dbusWatchers.append(watcher);

                if (watcher->isFinished()) {
                    d->dbusWatcherFinished(watcher);
                } else {
                    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                            this, SLOT(_q_dbusWatcherFinished(QDBusPendingCallWatcher*)));
                }
            }

            images.append(image);
        } else {
            images.append(QGalleryImage());
        }
    }

    if (!loadImages.isEmpty()) {
        QGalleryMediaArtLoadWatcher *watcher = new QGalleryMediaArtLoadWatcher(loadImages);
        d->loadWatchers.append(watcher);

        connect(watcher, SIGNAL(finished()), this, SLOT(_q_loadWatcherFinished()));
        connect(watcher, SIGNAL(resultsReadyAt(int,int)), this, SLOT(_q_imagesLoaded(int,int)));

        watcher->setFuture(QtConcurrent::mapped(loadFilePaths, qt_loadMediaArt));
    }

    return images;
}

#include "moc_qgallerymediaartloader_p.cpp"

QTM_END_NAMESPACE
