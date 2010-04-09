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

/*!
    This type matches all files in the document gallery.
*/

const QGalleryType QDocumentGallery::File("File");

/*!
    This type matches all file-system folders in the document gallery.
*/

const QGalleryType QDocumentGallery::Folder("Folder");

/*!
    This type matches all document files in the document gallery.
*/

const QGalleryType QDocumentGallery::Document("Document");

/*!
    This type matches all text files in the document gallery.
*/

const QGalleryType QDocumentGallery::Text("Text");

/*!
    This type matches all audio files in the document gallery.
*/

const QGalleryType QDocumentGallery::Audio("Audio");

/*!
    This type matches all image files in the document gallery.
*/

const QGalleryType QDocumentGallery::Image("Image");

/*!
    This type matches all video files in the document gallery.
*/

const QGalleryType QDocumentGallery::Video("Video");

/*!
    This type matches all playlist files in the document gallery.
*/

const QGalleryType QDocumentGallery::Playlist("Playlist");

/*!
    This type matches all artists in the document gallery.
*/

const QGalleryType QDocumentGallery::Artist("Artist");

/*!
    This type matches all album artists in the document gallery.
*/

const QGalleryType QDocumentGallery::AlbumArtist("AlbumArtist");

/*!
    This type matches all albums in the document gallery.
*/

const QGalleryType QDocumentGallery::Album("Album");

/*!
    This type matches all audio genres in the document gallery.
*/

const QGalleryType QDocumentGallery::AudioGenre("AudioGenre");

/*!
    This type matches all photo albums in the document gallery.
*/

const QGalleryType QDocumentGallery::PhotoAlbum("PhotoAlbum");

// Common

/*!
    This property contains the name of the author of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::author("author");

/*!
    This property contains a copyright statement for an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::copyright("copyright");

/*!
    This property contains a description of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::description("description");

/*!
    This property contains a user comment about an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::comments("comments");

/*!
    This property contains a rating for an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::rating("rating");

/*!
    This property contains the title of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::title("title");

/*!
    This property contains the subject of item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::subject("subject");

/*!
    This property contains keywords relevant to an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::keywords("keywords");

/*!
    This property contains the language of the content of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::language("language");

/*!
    This property contains a thumbnail representation of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::thumbnail("thumbnail");

// File

/*!
    This property contains the path of a file in the document gallery.
*/

const QGalleryProperty QDocumentGallery::path("path");

/*!
    This property contains the path including the file name of a file in the document gallery.
*/

const QGalleryProperty QDocumentGallery::filePath("filePath");

/*!
    This property contains the file name of a file in the document gallery.
*/

const QGalleryProperty QDocumentGallery::fileName("fileName");

/*!
    This property contains the file extension of a file.
*/

const QGalleryProperty QDocumentGallery::fileExtension("fileExtension");

/*!
    This property contains the size in bytes of a file.
*/

const QGalleryProperty QDocumentGallery::fileSize("fileSize");

/*!
    This property contains the MIME type of a file.
*/

const QGalleryProperty QDocumentGallery::mimeType("mimeType");

/*!
    This property contains the date and time a file was last modified.
*/

const QGalleryProperty QDocumentGallery::lastModified("lastModified");

/*!
    This property contains the date and time a file was last accessed.
*/

const QGalleryProperty QDocumentGallery::lastAccessed("lastAccessed");

// Document

/*!
    This property contains the number of pages in a document.
*/

const QGalleryProperty QDocumentGallery::pageCount("pageCount");

/*!
    This property contains the number of words in document.
*/

const QGalleryProperty QDocumentGallery::wordCount("wordCount");

/*!
    This property contains the date and time a document was created.
*/

const QGalleryProperty QDocumentGallery::created("created");

// Media

/*!
    This property contains the duration of an audio or video file.
*/

const QGalleryProperty QDocumentGallery::duration("duration");

/*!
    This property contains the name of the producer of a media file.
*/

const QGalleryProperty QDocumentGallery::producer("producer");

/*!
    This property contains the date and time an audio or video file was last played.
*/

const QGalleryProperty QDocumentGallery::lastPlayed("lastPlayed");

/*!
    This property contains the number of times an audio or video file has been played.
*/

const QGalleryProperty QDocumentGallery::playCount("playCount");

/*!
    This property contains the names of performers in a media file.
*/

const QGalleryProperty QDocumentGallery::performer("performer");

// Audio

/*!
    This property contains the name of the codec used to encode audio in a media file.
*/

const QGalleryProperty QDocumentGallery::audioCodec("audioCodec");

/*!
    This property contains the bit rate of the audio in a media file.
*/

const QGalleryProperty QDocumentGallery::audioBitRate("audioBitRate");

/*!
    This property contains the sample rate of the audio in a media file.
*/

const QGalleryProperty QDocumentGallery::sampleRate("sampleRate");

/*!
    This property contains the number of audio channels in a media file.
*/

const QGalleryProperty QDocumentGallery::channelCount("channelCount");

// Music

/*!
    This property contains the names of artists contributing to a music track.
*/

const QGalleryProperty QDocumentGallery::artist("artist");

/*!
    This property contains the name of the title artist of a music album.
*/

const QGalleryProperty QDocumentGallery::albumArtist("albumArtist");

/*!
    This property contains the title of the album a music track belongs to.
*/

const QGalleryProperty QDocumentGallery::albumTitle("albumTitle");

/*!
    This property contains the name of the composer of a music track.
*/

const QGalleryProperty QDocumentGallery::composer("composer");

/*!
    This property contains the genre of a media file.
*/

const QGalleryProperty QDocumentGallery::genre("genre");

/*!
    This property contains the lyrics to a music track.
*/

const QGalleryProperty QDocumentGallery::lyrics("lyrics");

/*!
    This property contains the number of music tracks in an album.
*/

const QGalleryProperty QDocumentGallery::trackCount("trackCount");

/*!
    This property contains the track number of a music track in an album.
*/

const QGalleryProperty QDocumentGallery::trackNumber("trackNumber");

/*!
    This property contains the disc number of an album in a set.
*/

const QGalleryProperty QDocumentGallery::discNumber("discNumber");

// Image, Video common.

/*!
    This property contains the width in pixels of an image or video file.
*/

const QGalleryProperty QDocumentGallery::width("width");

/*!
    This property contains the height in pixels of an image or video file.
*/

const QGalleryProperty QDocumentGallery::height("height");

// Image
/*!
    This property contains the orientation of an image.
*/

const QGalleryProperty QDocumentGallery::orientation("orientation");

// Photo
/*!
    This property contains the name of the album a photo belongs to.
*/

const QGalleryProperty QDocumentGallery::photoAlbum("photoAlbum");

/*!
    This property contains the date and time a photo was taken.
*/

const QGalleryProperty QDocumentGallery::dateTaken("dateTaken");

/*!
    This property contains the manufacturer name of the camera used to take a photo.
*/

const QGalleryProperty QDocumentGallery::cameraManufacturer("cameraManufacturer");

/*!
    This property contains the model name of the camera used to take a photo.
*/

const QGalleryProperty QDocumentGallery::cameraModel("cameraModel");

/*!
    This property contains the name of the exposure program used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::exposureProgram("exposureProgram");

/*!
    This property contains the exposure time of a photo in seconds.
*/

const QGalleryProperty QDocumentGallery::exposureTime("exposureTime");

/*!
    This property contains the F-number of a photo.
*/

const QGalleryProperty QDocumentGallery::fNumber("fNumber");

/*!
    This property contains whether a flash was used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::flashEnabled("flashEnabled");

/*!
    This property contains the focal length used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::focalLength("focalLength");

/*!
    This property contains the metering mode used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::meteringMode("meteringMode");

/*!
    This property contains the white balance setting used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::whiteBalance("whiteBalance");

// Video

/*!
    This property contains the frame rate of a video.
*/

const QGalleryProperty QDocumentGallery::frameRate("frameRate");

/*!
    This property contains the codec used to encode the video in a media file.
*/

const QGalleryProperty QDocumentGallery::videoCodec("videoCodec");

/*!
    This property contains the bit rate of the video in a media file.
*/

const QGalleryProperty QDocumentGallery::videoBitRate("videoBitRate");

/*!
    This property contains the position in a video where playback was interrupted.
*/

const QGalleryProperty QDocumentGallery::resumePosition("resumePosition");

/*!
    This property contains the name of the director of a video.
*/

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
