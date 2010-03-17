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

#ifndef QGALLERY_H
#define QGALLERY_H

#include <qgalleryrequest.h>

#include <QtCore/qstringbuilder.h>

class QGalleryAbstractResponse;
class QGalleryAbstractRequest;

class QAbstractGalleryPrivate;

class Q_GALLERY_EXPORT QAbstractGallery : public QObject
{
    Q_OBJECT
public:
    QAbstractGallery(QObject *parent = 0);
    ~QAbstractGallery();

    virtual bool isRequestSupported(QGalleryAbstractRequest::Type type) const = 0;

protected:
    virtual QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request) = 0;

    QAbstractGallery(QAbstractGalleryPrivate &dd, QObject *parent);

    QAbstractGalleryPrivate *d_ptr;

    friend class QGalleryAbstractRequest;
};

class QDocumentGalleryPrivate;

class Q_GALLERY_EXPORT QDocumentGallery : public QAbstractGallery
{
    Q_OBJECT
    Q_PROPERTY(QStringList itemTypes READ itemTypes)
    Q_DECLARE_PRIVATE(QDocumentGallery)
public:
    static const QLatin1String Media;
    static const QLatin1String Audio;
    static const QLatin1String Image;
    static const QLatin1String Video;
    static const QLatin1String Artist;
    static const QLatin1String AlbumArtist;
    static const QLatin1String Album;

    // Item
    static const QLatin1String author;
    static const QLatin1String copyright;
    static const QLatin1String description;
    static const QLatin1String mimeType;
    static const QLatin1String rating;
    static const QLatin1String title;
    static const QLatin1String url;
    static const QLatin1String thumbnail;

    // Media
    static const QLatin1String duration;
    static const QLatin1String producer;
    static const QLatin1String writer;

    // Audio/Music
    static const QLatin1String artist;
    static const QLatin1String albumArtist;
    static const QLatin1String albumId;
    static const QLatin1String albumTitle;
    static const QLatin1String composer;
    static const QLatin1String discNumber;
    static const QLatin1String genre;
    static const QLatin1String trackCount;
    static const QLatin1String trackNumber;

    // Image, Video common.
    static const QLatin1String width;
    static const QLatin1String height;

    // Image/Photo
    static const QLatin1String dateTaken;
    static const QLatin1String cameraManufacturer;
    static const QLatin1String cameraModel;

    // Video
    static const QLatin1String director;

    QDocumentGallery(QObject *parent = 0);
    ~QDocumentGallery();

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const;

    QStringList itemTypes() const;

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request);
};

#endif
