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

#ifndef QGALLERYTRACKERTHUMBNAILCOLUMN_P_H
#define QGALLERYTRACKERTHUMBNAILCOLUMN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qgallerytrackerlistcolumn_p.h>
#include <qgallerydbusinterface_p.h>

QT_BEGIN_NAMESPACE
class QDBusPendingCallWatcher;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QGalleryTrackerThumbnailDBusWatcher;
class QGalleryTrackerThumbnailLoadWatcher;

class QGalleryTrackerThumbnailColumn : public QGalleryTrackerImageColumn
{
    Q_OBJECT
public:
    QGalleryTrackerThumbnailColumn(
            const QGalleryDBusInterfacePointer &thumbnailInterface,
            const QString &flavor,
            int key,
            QObject *parent = 0);
    ~QGalleryTrackerThumbnailColumn();

    void insertImages(
            int index, int count, QVector<QVariant>::const_iterator begin, int tableWidth);

    void removeImages(int index, int count);

protected:
    virtual QVariant loadImage(const QString &imagePath) const = 0;

private Q_SLOTS:
    void loadWatcherFinished();
    void finalLoadWatcherFinished();
    void imagesReady(int begin, int end);

    void dbusWatcherFinished(QDBusPendingCallWatcher *watcher);
    void dbusError(uint id, const QStringList &uris, int error, const QString &errorString);
    void dbusFinished(uint id);

private:
    class LoadFunc
    {
    public:
        typedef QVariant result_type;

        inline LoadFunc(QGalleryTrackerThumbnailColumn *column) : column(column) {}

        inline QVariant operator ()(const QString &filePath) {
            return column->loadThumbnail(filePath); }

    private:
        QGalleryTrackerThumbnailColumn *column;
    };

    QVariant loadThumbnail(const QString &filePath);

    const QString m_flavor;
    const QString m_thumbnailDir;
    const QString m_failDir;
    const QString m_localDir;
    const QGalleryDBusInterfacePointer m_thumbnailInterface;
    QList<QGalleryTrackerThumbnailDBusWatcher *> m_dbusWatchers;
    QList<QGalleryTrackerThumbnailLoadWatcher *> m_loadWatchers;
    QList<int> m_keys;
    QVector<uint> m_imageIds;

};

class QGalleryTrackerThumbnailImageColumn : public QGalleryTrackerThumbnailColumn
{
public:
    QGalleryTrackerThumbnailImageColumn(
            const QGalleryDBusInterfacePointer &thumbnailInterface,
            const QString &flavor,
            int index,
            QObject *parent = 0)
        : QGalleryTrackerThumbnailColumn(thumbnailInterface, flavor, index, parent) {}
    ~QGalleryTrackerThumbnailImageColumn() {}

protected:
    QVariant loadImage(const QString &imagePath) const;
};

class QGalleryTrackerThumbnailPixmapColumn : public QGalleryTrackerThumbnailColumn
{
public:
    QGalleryTrackerThumbnailPixmapColumn(
            const QGalleryDBusInterfacePointer &thumbnailInterface,
            const QString &flavor,
            int index,
            QObject *parent = 0)
        : QGalleryTrackerThumbnailColumn(thumbnailInterface, flavor, index, parent) {}
    ~QGalleryTrackerThumbnailPixmapColumn() {}

protected:
    QVariant loadImage(const QString &imagePath) const;
};

QTM_END_NAMESPACE

#endif
