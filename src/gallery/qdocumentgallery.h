/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDOCUMENTGALLERY_H
#define QDOCUMENTGALLERY_H

#include <qabstractgallery.h>
#include <qgalleryproperty.h>
#include <qgallerytype.h>

QTM_BEGIN_NAMESPACE

class QDocumentGalleryPrivate;

class Q_GALLERY_EXPORT QDocumentGallery : public QAbstractGallery
{
    Q_OBJECT
public:
    static const QGalleryType File;
    static const QGalleryType Folder;
    static const QGalleryType Document;
    static const QGalleryType Text;
    static const QGalleryType Audio;
    static const QGalleryType Image;
    static const QGalleryType Video;
    static const QGalleryType Playlist;
    static const QGalleryType Artist;
    static const QGalleryType AlbumArtist;
    static const QGalleryType Album;
    static const QGalleryType AudioGenre;
    static const QGalleryType PhotoAlbum;

    // Common
    static const QGalleryProperty url;
    static const QGalleryProperty author;
    static const QGalleryProperty copyright;
    static const QGalleryProperty description;
    static const QGalleryProperty comments;
    static const QGalleryProperty rating;
    static const QGalleryProperty title;
    static const QGalleryProperty subject;
    static const QGalleryProperty keywords;
    static const QGalleryProperty language;

    // File
    static const QGalleryProperty path;
    static const QGalleryProperty filePath;
    static const QGalleryProperty fileName;
    static const QGalleryProperty fileExtension;
    static const QGalleryProperty fileSize;
    static const QGalleryProperty mimeType;
    static const QGalleryProperty lastModified;
    static const QGalleryProperty lastAccessed;

    // Document
    static const QGalleryProperty pageCount;
    static const QGalleryProperty wordCount;
    static const QGalleryProperty created;

    // Media
    static const QGalleryProperty duration;
    static const QGalleryProperty producer;
    static const QGalleryProperty lastPlayed;
    static const QGalleryProperty playCount;
    static const QGalleryProperty performer;

    // Audio
    static const QGalleryProperty audioCodec;
    static const QGalleryProperty audioBitRate;
    static const QGalleryProperty sampleRate;
    static const QGalleryProperty channelCount;

    // Music
    static const QGalleryProperty artist;
    static const QGalleryProperty albumArtist;
    static const QGalleryProperty albumTitle;
    static const QGalleryProperty composer;
    static const QGalleryProperty genre;
    static const QGalleryProperty lyrics;
    static const QGalleryProperty trackCount;
    static const QGalleryProperty trackNumber;
    static const QGalleryProperty discNumber;

    // Image, Video common.
    static const QGalleryProperty width;
    static const QGalleryProperty height;

    // Image
    static const QGalleryProperty orientation;

    // Photo
    static const QGalleryProperty dateTaken;
    static const QGalleryProperty cameraManufacturer;
    static const QGalleryProperty cameraModel;
    static const QGalleryProperty exposureProgram;
    static const QGalleryProperty exposureTime;
    static const QGalleryProperty fNumber;
    static const QGalleryProperty flashEnabled;
    static const QGalleryProperty focalLength;
    static const QGalleryProperty meteringMode;
    static const QGalleryProperty whiteBalance;

    // Video
    static const QGalleryProperty frameRate;
    static const QGalleryProperty videoCodec;
    static const QGalleryProperty videoBitRate;
    static const QGalleryProperty resumePosition;
    static const QGalleryProperty director;

    // Type
    static const QGalleryProperty count;

    enum Error
    {
        NoError = QGalleryAbstractRequest::NoError,
        NoGallery = QGalleryAbstractRequest::NoGallery,
        NotSupported = QGalleryAbstractRequest::NotSupported,
        ConnectionError = QGalleryAbstractRequest::GalleryError,
        ItemIdError,
        ItemTypeError,
        FilterError
    };

    QDocumentGallery(QObject *parent = 0);
    ~QDocumentGallery();

    bool isRequestSupported(QGalleryAbstractRequest::RequestType type) const;

    QStringList itemTypePropertyNames(const QString &itemType) const;
    QGalleryProperty::Attributes propertyAttributes(
            const QString &propertyName, const QString &itemType) const;

protected:
    QGalleryAbstractResponse *createResponse(QGalleryAbstractRequest *request);

private:
    Q_DECLARE_PRIVATE(QDocumentGallery)
};

QTM_END_NAMESPACE

#endif
