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

#include "qdeclarativedocumentgallery.h"

#include <qgalleryabstractrequest.h>

QTM_BEGIN_NAMESPACE

static const char *qt_documentGalleryTypes[] =
{
    "File",
    "Folder",
    "Document",
    "Text",
    "Audio",
    "Image",
    "Video",
    "Playlist",
    "Artist",
    "AlbumArtist",
    "Album",
    "AudioGenre",
    "PhotoAlbum"
};

Q_GLOBAL_STATIC(QDocumentGallery, qt_declarativeDocumentGalleryInstance);

QString QDeclarativeDocumentGallery::toString(ItemType type)
{
    return type > InvalidType && type < NItemTypes
            ? QLatin1String(qt_documentGalleryTypes[type - 1])
            : QString();
}

QDeclarativeDocumentGallery::ItemType QDeclarativeDocumentGallery::itemTypeFromString(
        const QString &string)
{
    for (int i = InvalidType; i < NItemTypes; ++i) {
        if (string == QLatin1String(qt_documentGalleryTypes[i]))
            return ItemType(i);
    }

    return QDeclarativeDocumentGallery::InvalidType;
}

static const char *qt_declarativeDocumentGalleryErrorStrings[] =
{
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "The document gallery doesn't support this request."),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "There was an error connecting to the document gallery"),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "The value of an item ID property is invalid."),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "The value of an item type property is invalid."),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "A filter references a property that doesn't exist."),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "A filter includes an incompatible property value."),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "A filter type is not supported by the document gallery."),
    QT_TRANSLATE_NOOP("QDeclarativeDocumentGallery", "A filter option is not supported by the document gallery.")
};

QString QDeclarativeDocumentGallery::toErrorString(int result)
{
    if (result >= QGalleryAbstractRequest::NotSupported
            && result <= QGalleryAbstractRequest::UnsupportedFilterOptionError) {
        return tr(qt_declarativeDocumentGalleryErrorStrings[
                result - QGalleryAbstractRequest::NotSupported]);
    } else {
        return QString();
    }
}

QAbstractGallery *QDeclarativeDocumentGallery::gallery()
{
    return qt_declarativeDocumentGalleryInstance();
}

#include "moc_qdeclarativedocumentgallery.cpp"

QTM_END_NAMESPACE
