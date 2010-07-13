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

#ifndef QGALLERYTRACKERMEDIAARTCOLUMN_P_H
#define QGALLERYTRACKERMEDIAARTCOLUMN_P_H

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

QTM_BEGIN_NAMESPACE

class QGalleryTrackerMediaArtLoadWatcher;

class QGalleryTrackerMediaArtColumn : public QGalleryTrackerImageColumn
{
    Q_OBJECT
public:
    QGalleryTrackerMediaArtColumn(
            int key,
            int identifierBColumn,
            int identifierCColumn,
            QVariant::Type type,
            const QString &flavor,
            const QSize &dimensions,
            QObject *parent = 0);
    ~QGalleryTrackerMediaArtColumn();

    static QGalleryTrackerImageColumn *createThumbnailImageColumn(
            QGalleryDBusInterfaceFactory *dbus,
            int key,
            const QString &profile,
            const QVector<int> &columns);

    static QGalleryTrackerImageColumn *createThumbnailPixmapColumn(
            QGalleryDBusInterfaceFactory *dbus,
            int key,
            const QString &profile,
            const QVector<int> &columns);

    static QGalleryTrackerImageColumn *createPreviewImageColumn(
            QGalleryDBusInterfaceFactory *dbus,
            int key,
            const QString &profile,
            const QVector<int> &columns);

    static QGalleryTrackerImageColumn *createPreviewPixmapColumn(
            QGalleryDBusInterfaceFactory *dbus,
            int key,
            const QString &profile,
            const QVector<int> &columns);

    void insertImages(
            int index, int count, QVector<QVariant>::const_iterator begin, int tableWidth);

    void removeImages(int index, int count);

private Q_SLOTS:
    void loadWatcherFinished();
    void finalLoadWatcherFinished();
    void imagesReady(int begin, int end);

private:
    class LoadFunc
    {
    public:
        typedef QVariant result_type;

        inline LoadFunc(QGalleryTrackerMediaArtColumn *column) : column(column) {}

        inline QVariant operator ()(const QPair<QString, QString> &identifier) {
            return column->loadMediaArt(identifier); }

    private:
        QGalleryTrackerMediaArtColumn *column;
    };

    QVariant loadMediaArt(const QPair<QString, QString> &identifier);

    const int m_identifierBColumn;
    const int m_identifierCColumn;
    const int m_width;
    const int m_height;
    const QVariant::Type m_type;
    const QString m_flavor;
    const QString m_cacheDir;
    const QByteArray m_whitespace;
    QList<QGalleryTrackerMediaArtLoadWatcher *> m_loadWatchers;
    const QList<int> m_keys;
    QVector<uint> m_imageIds;
};


QTM_END_NAMESPACE

#endif
