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

#include "qdocumentgallery.h"
#include "qabstractgallery_p.h"

#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QDocumentGallery

    \ingroup gallery
    \ingroup gallery-galleries

    \brief The QDocumentGallery class provides a gallery of documents.
*/

/*!
    \variable QDocumentGallery::File

    This type matches all files in the document gallery.
*/

const QGalleryType QDocumentGallery::File("File");

/*!
    \variable QDocumentGallery::Folder

    This type matches all file-system folders in the document gallery.
*/

const QGalleryType QDocumentGallery::Folder("Folder");

/*!
    \variable QDocumentGallery::Document

    This type matches all document files in the document gallery.
*/

const QGalleryType QDocumentGallery::Document("Document");

/*!
    \variable QDocumentGallery::Text

    This type matches all text files in the document gallery.
*/

const QGalleryType QDocumentGallery::Text("Text");

/*!
    \variable QDocumentGallery::Audio

    This type matches all audio files in the document gallery.
*/

const QGalleryType QDocumentGallery::Audio("Audio");

/*!
    \variable QDocumentGallery::Image

    This type matches all image files in the document gallery.
*/

const QGalleryType QDocumentGallery::Image("Image");

/*!
    \variable QDocumentGallery::Video

    This type matches all video files in the document gallery.
*/

const QGalleryType QDocumentGallery::Video("Video");

/*!
    \variable QDocumentGallery::Playlist

    This type matches all playlist files in the document gallery.
*/

const QGalleryType QDocumentGallery::Playlist("Playlist");

/*!
    \variable QDocumentGallery::Artist

    This type matches all artists in the document gallery.
*/

const QGalleryType QDocumentGallery::Artist("Artist");

/*!
    \variable QDocumentGallery::AlbumArtist

    This type matches all album artists in the document gallery.
*/

const QGalleryType QDocumentGallery::AlbumArtist("AlbumArtist");

/*!
    \variable QDocumentGallery::Album

    This type matches all albums in the document gallery.
*/

const QGalleryType QDocumentGallery::Album("Album");

/*!
    \variable QDocumentGallery::AudioGenre

    This type matches all audio genres in the document gallery.
*/

const QGalleryType QDocumentGallery::AudioGenre("AudioGenre");

/*!
    \variable QDocumentGallery::PhotoAlbum

    This type matches all photo albums in the document gallery.
*/

const QGalleryType QDocumentGallery::PhotoAlbum("PhotoAlbum");

// Common

/*!
    \variable QDocumentGallery::author

    This property contains the name of the author of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::author("author");

/*!
    \variable QDocumentGallery::copyright

    This property contains a copyright statement for an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::copyright("copyright");

/*!
    \variable QDocumentGallery::description

    This property contains a description of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::description("description");

/*!
    \variable QDocumentGallery::comments

    This property contains a user comment about an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::comments("comments");

/*!
    \variable QDocumentGallery::rating

    This property contains a rating for an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::rating("rating");

/*!
    \variable QDocumentGallery::title

    This property contains the title of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::title("title");

/*!
    \variable QDocumentGallery::subject

    This property contains the subject of item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::subject("subject");

/*!
    \variable QDocumentGallery::keywords

    This property contains keywords relevant to an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::keywords("keywords");

/*!
    \variable QDocumentGallery::language

    This property contains the language of the content of an item in the document gallery.
*/

const QGalleryProperty QDocumentGallery::language("language");

/*!
    \variable QDocumentGallery::thumbnailImage

    This property contains a QImage thumbnail representation of an item in the
    document gallery.
*/

const QGalleryProperty QDocumentGallery::thumbnailImage("thumbnailImage");

/*!
    \variable QDocumentGallery::thumbnailPixmap

    This property contains a QPixmap thumbnail representation of an item in the
    document gallery.
*/

const QGalleryProperty QDocumentGallery::thumbnailPixmap("thumbnailPixmap");

// File

/*!
    \variable QDocumentGallery::path

    This property contains the path of a file in the document gallery.
*/

const QGalleryProperty QDocumentGallery::path("path");

/*!
    \variable QDocumentGallery::filePath

    This property contains the path including the file name of a file in the document gallery.
*/

const QGalleryProperty QDocumentGallery::filePath("filePath");

/*!
    \variable QDocumentGallery::fileName

    This property contains the file name of a file in the document gallery.
*/

const QGalleryProperty QDocumentGallery::fileName("fileName");

/*!
    \variable QDocumentGallery::fileExtension

    This property contains the file extension of a file.
*/

const QGalleryProperty QDocumentGallery::fileExtension("fileExtension");

/*!
    \variable QDocumentGallery::fileSize

    This property contains the size in bytes of a file.
*/

const QGalleryProperty QDocumentGallery::fileSize("fileSize");

/*!
    \variable QDocumentGallery::mimeType

    This property contains the MIME type of a file.
*/

const QGalleryProperty QDocumentGallery::mimeType("mimeType");

/*!
    \variable QDocumentGallery::lastModified

    This property contains the date and time a file was last modified.
*/

const QGalleryProperty QDocumentGallery::lastModified("lastModified");

/*!
    \variable QDocumentGallery::lastAccessed

    This property contains the date and time a file was last accessed.
*/

const QGalleryProperty QDocumentGallery::lastAccessed("lastAccessed");

// Document

/*!
    \variable QDocumentGallery::pageCount

    This property contains the number of pages in a document.
*/

const QGalleryProperty QDocumentGallery::pageCount("pageCount");

/*!
    \variable QDocumentGallery::wordCount

    This property contains the number of words in document.
*/

const QGalleryProperty QDocumentGallery::wordCount("wordCount");

/*!
    \variable QDocumentGallery::created

    This property contains the date and time a document was created.
*/

const QGalleryProperty QDocumentGallery::created("created");

// Media

/*!
    \variable QDocumentGallery::duration

    This property contains the duration of an audio or video file.
*/

const QGalleryProperty QDocumentGallery::duration("duration");

/*!
    \variable QDocumentGallery::producer

    This property contains the name of the producer of a media file.
*/

const QGalleryProperty QDocumentGallery::producer("producer");

/*!
    \variable QDocumentGallery::lastPlayed

    This property contains the date and time an audio or video file was last played.
*/

const QGalleryProperty QDocumentGallery::lastPlayed("lastPlayed");

/*!
    \variable QDocumentGallery::playCount

    This property contains the number of times an audio or video file has been played.
*/

const QGalleryProperty QDocumentGallery::playCount("playCount");

/*!
    \variable QDocumentGallery::performer

    This property contains the names of performers in a media file.
*/

const QGalleryProperty QDocumentGallery::performer("performer");

// Audio

/*!
    \variable QDocumentGallery::audioCodec

    This property contains the name of the codec used to encode audio in a media file.
*/

const QGalleryProperty QDocumentGallery::audioCodec("audioCodec");

/*!
    \variable QDocumentGallery::audioBitRate

    This property contains the bit rate of the audio in a media file.
*/

const QGalleryProperty QDocumentGallery::audioBitRate("audioBitRate");

/*!
    \variable QDocumentGallery::sampleRate

    This property contains the sample rate of the audio in a media file.
*/

const QGalleryProperty QDocumentGallery::sampleRate("sampleRate");

/*!
    \variable QDocumentGallery::channelCount

    This property contains the number of audio channels in a media file.
*/

const QGalleryProperty QDocumentGallery::channelCount("channelCount");

// Music

/*!
    \variable QDocumentGallery::artist

    This property contains the names of artists contributing to a music track.
*/

const QGalleryProperty QDocumentGallery::artist("artist");

/*!
    \variable QDocumentGallery::albumArtist

    This property contains the name of the title artist of a music album.
*/

const QGalleryProperty QDocumentGallery::albumArtist("albumArtist");

/*!
    \variable QDocumentGallery::albumTitle

    This property contains the title of the album a music track belongs to.
*/

const QGalleryProperty QDocumentGallery::albumTitle("albumTitle");

/*!
    \variable QDocumentGallery::composer

    This property contains the name of the composer of a music track.
*/

const QGalleryProperty QDocumentGallery::composer("composer");

/*!
    \variable QDocumentGallery::genre

    This property contains the genre of a media file.
*/

const QGalleryProperty QDocumentGallery::genre("genre");

/*!
    \variable QDocumentGallery::lyrics

    This property contains the lyrics to a music track.
*/

const QGalleryProperty QDocumentGallery::lyrics("lyrics");

/*!
    \variable QDocumentGallery::trackCount

    This property contains the number of music tracks in an album.
*/

const QGalleryProperty QDocumentGallery::trackCount("trackCount");

/*!
    \variable QDocumentGallery::trackNumber

    This property contains the track number of a music track in an album.
*/

const QGalleryProperty QDocumentGallery::trackNumber("trackNumber");

/*!
    \variable QDocumentGallery::discNumber

    This property contains the disc number of an album in a set.
*/

const QGalleryProperty QDocumentGallery::discNumber("discNumber");

// Image, Video common.

/*!
    \variable QDocumentGallery::width

    This property contains the width in pixels of an image or video file.
*/

const QGalleryProperty QDocumentGallery::width("width");

/*!
    \variable QDocumentGallery::height

    This property contains the height in pixels of an image or video file.
*/

const QGalleryProperty QDocumentGallery::height("height");

// Image
/*!
    \variable QDocumentGallery::orientation

    This property contains the orientation of an image.
*/

const QGalleryProperty QDocumentGallery::orientation("orientation");

// Photo
/*!
    \variable QDocumentGallery::photoAlbum

    This property contains the name of the album a photo belongs to.
*/

const QGalleryProperty QDocumentGallery::photoAlbum("photoAlbum");

/*!
    \variable QDocumentGallery::dateTaken

    This property contains the date and time a photo was taken.
*/

const QGalleryProperty QDocumentGallery::dateTaken("dateTaken");

/*!
    \variable QDocumentGallery::cameraManufacturer

    This property contains the manufacturer name of the camera used to take a photo.
*/

const QGalleryProperty QDocumentGallery::cameraManufacturer("cameraManufacturer");

/*!
    \variable QDocumentGallery::cameraModel

    This property contains the model name of the camera used to take a photo.
*/

const QGalleryProperty QDocumentGallery::cameraModel("cameraModel");

/*!
    \variable QDocumentGallery::exposureProgram

    This property contains the name of the exposure program used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::exposureProgram("exposureProgram");

/*!
    \variable QDocumentGallery::exposureTime

    This property contains the exposure time of a photo in seconds.
*/

const QGalleryProperty QDocumentGallery::exposureTime("exposureTime");

/*!
    \variable QDocumentGallery::fNumber

    This property contains the F-number of a photo.
*/

const QGalleryProperty QDocumentGallery::fNumber("fNumber");

/*!
    \variable QDocumentGallery::flashEnabled

    This property contains whether a flash was used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::flashEnabled("flashEnabled");

/*!
    \variable QDocumentGallery::focalLength

    This property contains the focal length used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::focalLength("focalLength");

/*!
    \variable QDocumentGallery::meteringMode

    This property contains the metering mode used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::meteringMode("meteringMode");

/*!
    \variable QDocumentGallery::whiteBalance

    This property contains the white balance setting used when taking a photo.
*/

const QGalleryProperty QDocumentGallery::whiteBalance("whiteBalance");

// Video

/*!
    \variable QDocumentGallery::frameRate

    This property contains the frame rate of a video.
*/

const QGalleryProperty QDocumentGallery::frameRate("frameRate");

/*!
    \variable QDocumentGallery::videoCodec

    This property contains the codec used to encode the video in a media file.
*/

const QGalleryProperty QDocumentGallery::videoCodec("videoCodec");

/*!
    \variable QDocumentGallery::videoBitRate

    This property contains the bit rate of the video in a media file.
*/

const QGalleryProperty QDocumentGallery::videoBitRate("videoBitRate");

/*!
    \variable QDocumentGallery::resumePosition

    This property contains the position in a video where playback was interrupted.
*/

const QGalleryProperty QDocumentGallery::resumePosition("resumePosition");

/*!
    \variable QDocumentGallery::director

    This property contains the name of the director of a video.
*/

const QGalleryProperty QDocumentGallery::director("director");

/*!
    \fn QDocumentGallery::QDocumentGallery(QObject *parent)

    Constructs a new document gallery.

    The \a parent is passed to QAbstractGallery.
*/

/*!
    \fn QDocumentGallery::~QDocumentGallery()

    Destroys a document gallery.
*/

/*!
    \fn QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const

    \reimp
*/

/*!
    \fn QDocumentGallery::itemTypePropertyNames(const QString &itemType) const;

    Returns a list of names of properties that are valid for an \a itemType.
*/

/*!
    \fn QDocumentGallery::propertyAttributes(const QString &propertyName, const QString &itemType) const

    Returns the attributes of a property of \a itemType identified by \a propertyName
*/

/*!
    \fn QDocumentGallery::createResponse(QGalleryAbstractRequest *request)

    \reimp
*/

#ifdef QT_DOCUMENT_GALLERY_NULL

QDocumentGallery::QDocumentGallery(QObject *parent)
    : QAbstractGallery(parent)
{

}


QDocumentGallery::~QDocumentGallery()
{

}

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const
{
    return false;
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &) const
{
    return QStringList();
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(const QString &, const QString &) const
{
    return QGalleryProperty::Attributes();
}

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    return 0;
}

#include "moc_qdocumentgallery.cpp"

#endif

QTM_END_NAMESPACE

