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

const QGalleryType QDocumentGallery::File("File");
const QGalleryType QDocumentGallery::Folder("Folder");
const QGalleryType QDocumentGallery::Document("Document");
const QGalleryType QDocumentGallery::Media("Media");
const QGalleryType QDocumentGallery::Audio("Audio");
const QGalleryType QDocumentGallery::Image("Image");
const QGalleryType QDocumentGallery::Video("Video");
const QGalleryType QDocumentGallery::Location("Location");
const QGalleryType QDocumentGallery::Artist("Artist");
const QGalleryType QDocumentGallery::AlbumArtist("AlbumArtist");
const QGalleryType QDocumentGallery::Album("Album");


// Item
const QGalleryProperty QDocumentGallery::author("author");
const QGalleryProperty QDocumentGallery::copyright("copyright");
const QGalleryProperty QDocumentGallery::description("description");
const QGalleryProperty QDocumentGallery::mimeType("mimeType");
const QGalleryProperty QDocumentGallery::rating("rating");
const QGalleryProperty QDocumentGallery::title("title");
const QGalleryProperty QDocumentGallery::url("url");
const QGalleryProperty QDocumentGallery::thumbnail("thumbnail");

// File
const QGalleryProperty QDocumentGallery::fileName("fileName");

// Media

const QGalleryProperty QDocumentGallery::duration("duration");
const QGalleryProperty QDocumentGallery::producer("producer");
const QGalleryProperty QDocumentGallery::writer("writer");

// Audio/Music
const QGalleryProperty QDocumentGallery::artist("artist");
const QGalleryProperty QDocumentGallery::albumArtist("albumArtist");
const QGalleryProperty QDocumentGallery::albumTitle("albumTitle");
const QGalleryProperty QDocumentGallery::composer("composer");
const QGalleryProperty QDocumentGallery::discNumber("discNumber");
const QGalleryProperty QDocumentGallery::genre("genre");
const QGalleryProperty QDocumentGallery::trackCount("trackCount");
const QGalleryProperty QDocumentGallery::trackNumber("trackNumber");

// Image, Video common.
const QGalleryProperty QDocumentGallery::width("width");
const QGalleryProperty QDocumentGallery::height("height");

// Image/Photo
const QGalleryProperty QDocumentGallery::dateTaken("author");
const QGalleryProperty QDocumentGallery::cameraManufacturer("cameraManufacturer");
const QGalleryProperty QDocumentGallery::cameraModel("cameraModel");

// Video
const QGalleryProperty QDocumentGallery::director("director");


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
