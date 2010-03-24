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

#include <qgallerykey.h>
#include <qgalleryrequest.h>
#include <qgallerytype.h>

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
    static const QGalleryType File;
    static const QGalleryType Folder;
    static const QGalleryType Document;
    static const QGalleryType Media;
    static const QGalleryType Audio;
    static const QGalleryType Image;
    static const QGalleryType Video;
    static const QGalleryType Location;
    static const QGalleryType Artist;
    static const QGalleryType AlbumArtist;
    static const QGalleryType Album;

    // Item
    static const QGalleryKey author;
    static const QGalleryKey copyright;
    static const QGalleryKey description;
    static const QGalleryKey mimeType;
    static const QGalleryKey rating;
    static const QGalleryKey title;
    static const QGalleryKey url;
    static const QGalleryKey thumbnail;

    // Media
    static const QGalleryKey duration;
    static const QGalleryKey producer;
    static const QGalleryKey writer;

    // Audio/Music
    static const QGalleryKey artist;
    static const QGalleryKey albumArtist;
    static const QGalleryKey albumId;
    static const QGalleryKey albumTitle;
    static const QGalleryKey composer;
    static const QGalleryKey discNumber;
    static const QGalleryKey genre;
    static const QGalleryKey trackCount;
    static const QGalleryKey trackNumber;

    // Image, Video common.
    static const QGalleryKey width;
    static const QGalleryKey height;

    // Image/Photo
    static const QGalleryKey dateTaken;
    static const QGalleryKey cameraManufacturer;
    static const QGalleryKey cameraModel;

    // Video
    static const QGalleryKey director;

    QDocumentGallery(QObject *parent = 0);
    ~QDocumentGallery();

    bool isRequestSupported(QGalleryAbstractRequest::Type type) const;

    QStringList itemTypes() const;

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request);
};

#endif
