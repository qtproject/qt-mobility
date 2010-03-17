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

#include "qgallery.h"
#include "qabstractgallery_p.h"

#include <QtCore/qstringlist.h>

/*!
    \class QAbstractGallery

    \ingroup gallery
    \ingroup gallery-galleries

    \brief The QAbstractGallery class provides a base class for gallery
    implementations.
*/

/*!
    Constructs a new gallery.

    The \a parent is passed to QObject.
*/

QAbstractGallery::QAbstractGallery(QObject *parent )
    : QObject(parent)
    , d_ptr(0)
{

}

/*!
    \internal
*/

QAbstractGallery::QAbstractGallery(QAbstractGalleryPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    d_ptr->q_ptr = this;
}

/*!
    Destroys a gallery.
*/

QAbstractGallery::~QAbstractGallery()
{
    delete d_ptr;
}

/*!
    \fn QAbstractGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const

    Identifies if a gallery supports a request \a type.

    Returns true if the request is supported, and false otherwise.
*/

/*!
    \fn QAbstractGallery::createResponse(QGalleryAbstractRequest *request)

    Creates a response to a gallery \a request.
*/

#define QT_GALLERY_DEFINE_LATIN1_LITERAL(Name, String) \
    const QLatin1String Name(String)

QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::Media, "Media");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::Audio, "Audio");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::Image, "Image");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::Video, "Video");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::Artist, "Artist");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::AlbumArtist, "AlbumArtist");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::Album, "Album");

// Item
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::author, "author");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::copyright, "copyright");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::description, "description");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::mimeType, "mimeType");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::rating, "rating");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::title, "title");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::url, "url");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::thumbnail, "thumbnail");

// Media
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::duration, "duration");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::producer, "producer");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::writer, "writer");

// Audio/Music
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::artist, "artist");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::albumArtist, "albumArtist");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::albumId, "albumId");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::albumTitle, "albumTitle");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::composer, "composer");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::discNumber, "discNumber");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::genre, "genre");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::trackCount, "trackCount");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::trackNumber, "trackNumber");

// Image, Video common.
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::width, "width");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::height, "height");

// Image/Photo
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::dateTaken, "dateTaken");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::cameraManufacturer, "cameraManufacturer");
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::cameraModel, "cameraModel");

// Video
QT_GALLERY_DEFINE_LATIN1_LITERAL(QDocumentGallery::director, "director");


#ifdef QT_DOCUMENT_GALLERY_NULL

/*!
    \class QDocumentGallery

    \ingroup gallery
    \ingroup gallery-galleries

    \brief The QDocumentGallery class provides a gallery of documents.
*/

/*!
    Constructs a new document gallery.

    The \a parent is passed to QAbstractGallery.
*/

QDocumentGallery::QDocumentGallery(QObject *parent)
    : QAbstractGallery(parent)
{

}

/*!
    Destroys a document gallery.
*/

QDocumentGallery::~QDocumentGallery()
{

}

/*!
    \reimp
*/

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const
{
    return false;
}

/*!
    \property QDocumentGallery::itemTypes

    Returns a list of item types supported by a document gallery.
*/

QStringList QDocumentGallery::itemTypes() const
{
    return QStringList();
}

/*!
    \reimp
*/

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    return 0;
}

#endif
