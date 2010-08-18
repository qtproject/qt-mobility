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

#include "qgallerymdsutility.h"
//Symbian
#include <mdeconstants.h>
#include <mdesession.h>

QTM_BEGIN_NAMESPACE

QDocumentGalleryMDSUtility::QDocumentGalleryMDSUtility()
{

}
QDocumentGalleryMDSUtility::~QDocumentGalleryMDSUtility()
{

}

CMdEObjectDef& QDocumentGalleryMDSUtility::ObjDefFromItemType( CMdENamespaceDef& nameSpace, QString itemType )
{
    if (itemType == QDocumentGallery::Album.name())
    {
        return nameSpace.GetObjectDefL( MdeConstants::Album::KAlbumObject );
    }

    //    else if (itemType == QDocumentGallery::AlbumArtist.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }
    //    else if (itemType == QDocumentGallery::Artist.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }

    else if (itemType == QDocumentGallery::Audio.name())
    {
        return nameSpace.GetObjectDefL( MdeConstants::Audio::KAudioObject );
    }
    //    else if (itemType == QDocumentGallery::AudioGenre.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }
    //    else if (itemType == QDocumentGallery::Document.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }

    else if (itemType == QDocumentGallery::File.name())
    {
        return nameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
    }
    //    else if (itemType == QDocumentGallery::Folder.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }

    else if (itemType == QDocumentGallery::Image.name())
    {
        return nameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
    }
    //    else if (itemType == QDocumentGallery::PhotoAlbum.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }
    //    else if (itemType == QDocumentGallery::Playlist.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }
    //    else if (itemType == QDocumentGallery::Text.name())
    //        {
    //        return nameSpace.GetObjectDefL( MdeConstants:::: );
    //        }

    else if (itemType == QDocumentGallery::Video.name())
    {
        return nameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
    }

    // Return Base Object definition
    return nameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
}
QTM_END_NAMESPACE
