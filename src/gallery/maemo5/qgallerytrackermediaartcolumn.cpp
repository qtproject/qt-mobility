/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgallerytrackermediaartcolumn_p.h"

#include <QtCore/qcryptographichash.h>
#include <QtCore/qfile.h>
#include <QtCore/qfuturewatcher.h>
#include <QtCore/qtconcurrentmap.h>
#include <QtDBus/qdbuspendingcall.h>
#include <QtDBus/qdbuspendingreply.h>
#include <QtGui/qdesktopservices.h>
#include <QtGui/qimage.h>
#include <QtGui/qimagereader.h>
#include <QtGui/qpixmap.h>

QTM_BEGIN_NAMESPACE

class QGalleryTrackerMediaArtLoadWatcher : public QFutureWatcher<QVariant>
{
public:
    QGalleryTrackerMediaArtLoadWatcher(const QVector<uint> imageIds)
        : imageIds(imageIds)
    {
    }

    QVector<uint> imageIds;
};


QGalleryTrackerMediaArtColumn::QGalleryTrackerMediaArtColumn(
        int key,
        int identifierBColumn,
        int identifierCColumn,
        QVariant::Type type,
        const QString &flavor,
        const QSize &dimensions,
        QObject *parent)
    : QGalleryTrackerImageColumn(parent)
    , m_identifierBColumn(identifierBColumn)
    , m_identifierCColumn(identifierCColumn)
    , m_width(dimensions.width())
    , m_height(dimensions.height())
    , m_type(type)
    , m_flavor(flavor)
    , m_cacheDir(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)
                 + QLatin1String("/.cache/media-art/") + flavor + QLatin1Char('-'))
    , m_whitespace(" ")
    , m_keys(QList<int>() << key)
{
}

QGalleryTrackerMediaArtColumn::~QGalleryTrackerMediaArtColumn()
{
    typedef QList<QGalleryTrackerMediaArtLoadWatcher *>::iterator iterator;

    for (iterator it = m_loadWatchers.begin(), end = m_loadWatchers.end(); it != end; ++it)
        (*it)->cancel();

    for (iterator it = m_loadWatchers.begin(), end = m_loadWatchers.end(); it != end; ++it)
        (*it)->waitForFinished();

    qDeleteAll(m_loadWatchers);
}

QGalleryTrackerImageColumn *QGalleryTrackerMediaArtColumn::createThumbnailImageColumn(
        QGalleryDBusInterfaceFactory *dbus,
        int key,
        const QString &profile,
        const QVector<int> &columns)
{
    Q_UNUSED(dbus);

    return new QGalleryTrackerMediaArtColumn(
#ifdef Q_WS_MAEMO_5
            key, columns.first(), columns.value(1, -1), QVariant::Image, profile, QSize(124, 124));
#else
            key, columns.first(), columns.value(1, -1), QVariant::Image, profile, QSize(128, 128));
#endif
}

QGalleryTrackerImageColumn *QGalleryTrackerMediaArtColumn::createThumbnailPixmapColumn(
        QGalleryDBusInterfaceFactory *dbus,
        int key,
        const QString &profile,
        const QVector<int> &columns)
{
    Q_UNUSED(dbus);

    return new QGalleryTrackerMediaArtColumn(
#ifdef Q_WS_MAEMO_5
            key, columns.first(), columns.value(1, -1), QVariant::Pixmap, profile, QSize(124, 124));
#else
            key, columns.first(), columns.value(1, -1), QVariant::Pixmap, profile, QSize(128, 128));
#endif
}

QGalleryTrackerImageColumn *QGalleryTrackerMediaArtColumn::createPreviewImageColumn(
        QGalleryDBusInterfaceFactory *dbus,
        int key,
        const QString &profile,
        const QVector<int> &columns)
{
    Q_UNUSED(dbus);

    return new QGalleryTrackerMediaArtColumn(
            key, columns.first(), columns.value(1, -1), QVariant::Image, profile, QSize());
}

QGalleryTrackerImageColumn *QGalleryTrackerMediaArtColumn::createPreviewPixmapColumn(
        QGalleryDBusInterfaceFactory *dbus,
        int key,
        const QString &profile,
        const QVector<int> &columns)
{
    Q_UNUSED(dbus);

    return new QGalleryTrackerMediaArtColumn(
            key, columns.first(), columns.value(1, -1), QVariant::Pixmap, profile, QSize());
}

void QGalleryTrackerMediaArtColumn::insertImages(
        int index, int count, QVector<QVariant>::const_iterator row, int tableWidth)
{
    QVector<QPair<QString, QString> > identifiers;

    m_images.insert(index, count, QVariant());
    m_imageIds.insert(index, count, 0);

    for (int i = index; i < count; ++i, row += tableWidth) {
        const QString identifierB = (row + m_identifierBColumn)->toString();
        const QString identifierC = m_identifierCColumn != -1
                ? (row + m_identifierCColumn)->toString()
                : QString();

        const uint imageId = qHash(identifierB + identifierC);

        m_imageIds.replace(i, imageId);

        identifiers.append(qMakePair(identifierB, identifierC));

    }

    QGalleryTrackerMediaArtLoadWatcher *watcher = new QGalleryTrackerMediaArtLoadWatcher(
            m_imageIds.mid(index, count));

    m_loadWatchers.append(watcher);

    connect(watcher, SIGNAL(finished()), this, SLOT(loadWatcherFinished()));
    connect(watcher, SIGNAL(resultsReadyAt(int,int)), this, SLOT(imagesReady(int,int)));

    watcher->setFuture(QtConcurrent::mapped(identifiers, LoadFunc(this)));
}

void QGalleryTrackerMediaArtColumn::removeImages(int index, int count)
{
    count = qMin(m_images.count() - index, count);

    if (count > 0) {
        m_images.remove(index, count);
        m_imageIds.remove(index, count);
    }
}

void QGalleryTrackerMediaArtColumn::loadWatcherFinished()
{
    QGalleryTrackerMediaArtLoadWatcher *loadWatcher
            = static_cast<QGalleryTrackerMediaArtLoadWatcher *>(sender());

    m_loadWatchers.removeAll(loadWatcher);

    loadWatcher->deleteLater();
}

void QGalleryTrackerMediaArtColumn::finalLoadWatcherFinished()
{
    QGalleryTrackerMediaArtLoadWatcher *loadWatcher
            = static_cast<QGalleryTrackerMediaArtLoadWatcher *>(sender());

    m_loadWatchers.removeAll(loadWatcher);

    loadWatcher->deleteLater();
}

void QGalleryTrackerMediaArtColumn::imagesReady(int begin, int end)
{
    QGalleryTrackerMediaArtLoadWatcher *watcher
            = static_cast<QGalleryTrackerMediaArtLoadWatcher *>(sender());

    for (int i = begin, index = 0, count = 0; i < end && index < m_imageIds.count(); ++i) {
        while (index < m_imageIds.count() && m_imageIds.at(index) != watcher->imageIds.at(i))
            ++index;

        while (i < end
               && index + count < m_imageIds.count()
               && m_imageIds.at(index + count) == watcher->imageIds.at(i)) {
            QVariant image = watcher->resultAt(i);

            if (!image.isNull()) {
                image.convert(m_type);

                m_images.replace(index + count, image);

                ++i;
                ++count;
            } else {
                if (count > 0)
                    emit imagesChanged(index + m_offset, count, m_keys);

                index += count;

                break;
            }
        }

        if (count > 0) {
            emit imagesChanged(index + m_offset, count, m_keys);

            index += count;
            count = 0;
        }
    }
}


QVariant QGalleryTrackerMediaArtColumn::loadMediaArt(const QPair<QString, QString> &identifier)
{
    const QString hashB = QCryptographicHash::hash(!identifier.first.isEmpty()
            ? identifier.first.toLower().toUtf8() : m_whitespace, QCryptographicHash::Md5).toHex();

    const QString hashC = QCryptographicHash::hash(!identifier.second.isEmpty()
            ? identifier.second.toLower().toUtf8() : m_whitespace, QCryptographicHash::Md5).toHex();

    QString imagePath = m_cacheDir + hashB + QLatin1Char('-') + hashC + QLatin1String(".jpeg");

    if (QFile::exists(imagePath)) {
        QImageReader reader(imagePath);
        reader.setQuality(50);

        if (m_width > 0) {
            if (reader.supportsOption(QImageIOHandler::Size)) {
                QSize size = reader.size();
#ifdef Q_WS_MAEMO_5
                if (size.width() > m_width && size.height() > m_height) {
                    size.scale(m_width, m_height, Qt::KeepAspectRatioByExpanding);
                    reader.setScaledSize(size);
                }

                if (size.width() > m_width || size.height() > m_height) {
                    QRect rect(0, 0, qMin(size.width(), m_width), qMin(size.height(), m_height));
                    rect.moveCenter(QPoint(size.width() / 2, size.height() / 2));

                    reader.setScaledClipRect(rect);
                }
#else
                if (size.width() > m_width || size.height() > m_height) {
                    size.scale(m_width, m_height, Qt::KeepAspectRatio);
                    reader.setScaledSize(size);
                }
#endif
            } else {
                reader.setScaledSize(QSize(m_width, m_height));
            }
        }

        return reader.read();
    } else {
        return QImage();

    }
}

#include "moc_qgallerytrackermediaartcolumn_p.cpp"

QTM_END_NAMESPACE
