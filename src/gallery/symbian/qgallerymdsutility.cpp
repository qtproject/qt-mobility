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

#include <qurl.h>
#include <QDir>
//Backend
#include "qgallerymdsutility_p.h"

#include "qmdegallerycategoryresultset_p.h"

//Symbian
#include <mdeconstants.h>
#include <mdesession.h>
#include <utf.h>
#include <e32cmn.h>
#include <e32des8.h>
#include <QFileInfo>

QTM_BEGIN_NAMESPACE

QDocumentGalleryMDSUtility::QDocumentGalleryMDSUtility()
{

}
QDocumentGalleryMDSUtility::~QDocumentGalleryMDSUtility()
{

}

CMdEObjectDef& QDocumentGalleryMDSUtility::ObjDefFromItemTypeL( CMdENamespaceDef &nameSpace, QString itemType )
{
#ifdef MDS_25_COMPILATION_ENABLED
    return ObjDefFromItemTypeForMDS25L( nameSpace, itemType );
#else
    CMdEObjectDef* mdEObjectDef = ObjDefFromItemTypeForMDS20L( nameSpace, itemType );
    if( !mdEObjectDef )
        User::Leave( KErrBadHandle );
    
    return *mdEObjectDef;
#endif // MDS_25_COMPILATION_ENABLED 
}

#ifdef MDS_25_COMPILATION_ENABLED
CMdEObjectDef& QDocumentGalleryMDSUtility::ObjDefFromItemTypeForMDS25L( CMdENamespaceDef &nameSpace, QString itemType )
#else
//mds 2.0 return pointer instead of ref from GetObjectDefL function
CMdEObjectDef* QDocumentGalleryMDSUtility::ObjDefFromItemTypeForMDS20L( CMdENamespaceDef &nameSpace, QString itemType )
#endif // MDS_25_COMPILATION_ENABLED
{
    if (itemType == QDocumentGallery::Audio.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::Audio::KAudioObject );
#if defined(MDS_25_COMPILATION_ENABLED) || defined(MDS_25_92MCL_COMPILATION_ENABLED)         
    } else if (itemType == QDocumentGallery::File.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
#endif        
    } else if (itemType == QDocumentGallery::Image.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
    } else if (itemType == QDocumentGallery::PhotoAlbum.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::Album::KAlbumObject );
    } else if (itemType == QDocumentGallery::Video.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
    }

    // Return Base Object definition
    return nameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
}
bool QDocumentGalleryMDSUtility::isItemTypeSupported(const QString &itemType)
{
    bool isSupported = false;
    if (itemType == QDocumentGallery::Audio.name())
        isSupported = true;
    else if (itemType == QDocumentGallery::File.name())
        isSupported = true;
    else if (itemType == QDocumentGallery::Image.name())
        isSupported = true;
    else if (itemType == QDocumentGallery::PhotoAlbum.name())
        isSupported = true;
    else if (itemType == QDocumentGallery::Video.name())
        isSupported = true;
    else if (itemType == QDocumentGallery::Album.name())
        isSupported = false; // only photoalbum currently supported
    else 
        isSupported = false;
    
    return isSupported;
    
}
void QDocumentGalleryMDSUtility::GetDataFieldsForItemType( QStringList &propertyList, QString itemType )
{
    if (itemType == QDocumentGallery::PhotoAlbum.name()) {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
    } 
    else if (itemType == QDocumentGallery::Audio.name()) {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileName.name() );
        propertyList.append( QDocumentGallery::filePath.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
        // From MDS media object
        propertyList.append( QDocumentGallery::author.name() );
        propertyList.append( QDocumentGallery::copyright.name() );
        propertyList.append( QDocumentGallery::description.name() );
        propertyList.append( QDocumentGallery::comments.name() );
        propertyList.append( QDocumentGallery::rating.name() );

        propertyList.append( QDocumentGallery::duration.name() );
        propertyList.append( QDocumentGallery::performer.name() );

        propertyList.append( QDocumentGallery::audioCodec.name() );
        propertyList.append( QDocumentGallery::audioBitRate.name() );
        propertyList.append( QDocumentGallery::playCount.name() );
        //From MDS audio object
        propertyList.append( QDocumentGallery::sampleRate.name() );
        propertyList.append( QDocumentGallery::albumTitle.name() );
        propertyList.append( QDocumentGallery::trackNumber.name() );
        propertyList.append( QDocumentGallery::albumArtist.name() );
        propertyList.append( QDocumentGallery::artist.name() );
        propertyList.append( QDocumentGallery::composer.name() );
        propertyList.append( QDocumentGallery::genre.name() );
        
    } else if (itemType == QDocumentGallery::File.name()) {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileName.name() );
        propertyList.append( QDocumentGallery::filePath.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
        // From MDS media object
        propertyList.append( QDocumentGallery::author.name() );
        propertyList.append( QDocumentGallery::copyright.name() );
        propertyList.append( QDocumentGallery::description.name() );
        propertyList.append( QDocumentGallery::comments.name() );
        propertyList.append( QDocumentGallery::rating.name() );
    } else if (itemType == QDocumentGallery::Image.name()) {
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileName.name() );
        propertyList.append( QDocumentGallery::filePath.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
        // From MDS media object
        propertyList.append( QDocumentGallery::author.name() );
        propertyList.append( QDocumentGallery::copyright.name() );
        propertyList.append( QDocumentGallery::description.name() );
        propertyList.append( QDocumentGallery::comments.name() );
        propertyList.append( QDocumentGallery::rating.name() );

        propertyList.append( QDocumentGallery::duration.name() );
        propertyList.append( QDocumentGallery::performer.name() );

        propertyList.append( QDocumentGallery::playCount.name() );

        propertyList.append( QDocumentGallery::width.name() );
        propertyList.append( QDocumentGallery::height.name() );
        // From MDS image object
        propertyList.append( QDocumentGallery::orientation.name() );
        propertyList.append( QDocumentGallery::dateTaken.name() );
        propertyList.append( QDocumentGallery::cameraManufacturer.name() );
        propertyList.append( QDocumentGallery::cameraModel.name() );
        propertyList.append( QDocumentGallery::exposureProgram.name() );
        propertyList.append( QDocumentGallery::exposureTime.name() );
        propertyList.append( QDocumentGallery::fNumber.name() );
        propertyList.append( QDocumentGallery::flashEnabled.name() );
        propertyList.append( QDocumentGallery::focalLength.name() );
        propertyList.append( QDocumentGallery::meteringMode.name() );
        propertyList.append( QDocumentGallery::whiteBalance.name() );
    } else if (itemType == QDocumentGallery::Video.name()) {
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileName.name() );
        propertyList.append( QDocumentGallery::filePath.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
        // From MDS media object
        propertyList.append( QDocumentGallery::author.name() );
        propertyList.append( QDocumentGallery::copyright.name() );
        propertyList.append( QDocumentGallery::description.name() );
        propertyList.append( QDocumentGallery::comments.name() );
        propertyList.append( QDocumentGallery::rating.name() );

        propertyList.append( QDocumentGallery::duration.name() );
        propertyList.append( QDocumentGallery::performer.name() );

        propertyList.append( QDocumentGallery::videoBitRate.name() );
        propertyList.append( QDocumentGallery::playCount.name() );

        propertyList.append( QDocumentGallery::width.name() );
        propertyList.append( QDocumentGallery::height.name() );
        // From MDS video object
        propertyList.append( QDocumentGallery::language.name() );
        propertyList.append( QDocumentGallery::frameRate.name() );
        propertyList.append( QDocumentGallery::resumePosition.name() );
    }
}

void QDocumentGalleryMDSUtility::GetMetaDataFieldL( CMdEObject *inputItem, QVariant &output, int key )
{
    output.clear();
    if (!inputItem)
        return;
    
#ifdef MDS_25_COMPILATION_ENABLED
    GetMetaDataFieldForMDS25L( inputItem, output, key );
#else
    GetMetaDataFieldForMDS20L( inputItem, output, key );
#endif // MDS_25_COMPILATION_ENABLED 
}

#ifdef MDS_25_COMPILATION_ENABLED
void QDocumentGalleryMDSUtility::GetMetaDataFieldForMDS25L( CMdEObject *inputItem, QVariant &output, int key )
{
    switch (key) {
    case EUri:
        {
            QUrl url( QUrl::fromLocalFile(s60DescToQString( inputItem->Uri() ) ) );
            output.setValue( url );
            break;
        }
    case EFileName:
        {
            TFileName fullName = inputItem->Uri();
            TPtrC name;
            if( GetName(fullName, name) ) {
                QFileInfo fileinfo(s60DescToQString(name));
                output.setValue(fileinfo.fileName());
            }
            break;
        }
    case EFilePath:
        {
            QFileInfo fileinfo(s60DescToQString(inputItem->Uri()));
            output.setValue(fileinfo.absoluteFilePath());
            break;
        }
    case EPath:
        {
            TFileName fullName = inputItem->Uri();
            TPtrC path;
            if( GetPath( fullName, path) ) {
                QFileInfo fileinfo(s60DescToQString(path));
                output.setValue(fileinfo.absoluteFilePath());
            }
            break;
        }
    case EFileSize:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KSizeProperty );
            CMdEProperty* sizeProp = NULL;
            int foundIndex = inputItem->Property( propDef, sizeProp );
            if ( foundIndex != KErrNotFound && sizeProp ) {
                unsigned long int value = sizeProp->Uint32ValueL();
                output.setValue( value );
            }
            break;
        }
    case ELastModified:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            CMdEProperty* modProp = NULL;
            int foundIndex = inputItem->Property( propDef, modProp );
            if ( foundIndex != KErrNotFound && modProp ) {
                output.setValue(symbianTTimetoQDateTime(modProp->TimeValueL()));
            }
            break;
        }
    case ETitle:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            CMdEProperty* titleProp = NULL;
            int foundIndex = inputItem->Property( propDef, titleProp );
            if ( foundIndex != KErrNotFound && titleProp ) {
                output.setValue( s60DescToQString( titleProp->TextValueL() ) );
            }
            break;
        }
    case EMime:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
            CMdEProperty* mimeProp = NULL;
            int foundIndex = inputItem->Property( propDef, mimeProp );
            if ( foundIndex != KErrNotFound && mimeProp ) {
                output.setValue( s60DescToQString( mimeProp->TextValueL() ) );
            }
            break;
        }
    case EAuthor:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
            CMdEProperty* authorProp = NULL;
            int foundIndex = inputItem->Property( propDef, authorProp );
            if ( foundIndex != KErrNotFound && authorProp ) {
                output.setValue( s60DescToQString( authorProp->TextValueL() ) );
            }
            break;
        }
    case ECopyright:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
            CMdEProperty* copyProp = NULL;
            int foundIndex = inputItem->Property( propDef, copyProp );
            if ( foundIndex != KErrNotFound && copyProp ) {
                output.setValue( s60DescToQString( copyProp->TextValueL() ) );
            }
            break;
        }
    case EDescription:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
            CMdEProperty* desProp = NULL;
            int foundIndex = inputItem->Property( propDef, desProp );
            if ( foundIndex != KErrNotFound && desProp ) {
                output.setValue( s60DescToQString( desProp->TextValueL() ) );
            }
            break;
        }
    case EComments:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
            CMdEProperty* comProp = NULL;
            int foundIndex = inputItem->Property( propDef, comProp );
            if ( foundIndex != KErrNotFound && comProp ) {
                output.setValue( s60DescToQString( comProp->TextValueL() ) );
            }
            break;
        }
    case ERating:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
            CMdEProperty* rateProp = NULL;
            int foundIndex = inputItem->Property( propDef, rateProp );
            if ( foundIndex != KErrNotFound && rateProp ) {
                unsigned int value = rateProp->Uint8ValueL();
                output.setValue( value );
            }
            break;
        }
    case EDuration:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            CMdEProperty* duraProp = NULL;
            int foundIndex = inputItem->Property( propDef, duraProp );
            if ( foundIndex != KErrNotFound && duraProp ) {
                double value = duraProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EPerformer:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            CMdEProperty* artistProp = NULL;
            int foundIndex = inputItem->Property( propDef, artistProp );
            if ( foundIndex != KErrNotFound && artistProp ) {
                output.setValue( s60DescToQString( artistProp->TextValueL() ) );
            }
            break;
        }
    case EAlbumTitle:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef &propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
            CMdEProperty *albumProp = NULL;
            int foundIndex = inputItem->Property(propDef,albumProp);
            if ( foundIndex != KErrNotFound && albumProp ) {
                output.setValue(s60DescToQString(albumProp->TextValueL()));
            }
            break;
        }
    case EAlbumArtist:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
            CMdEPropertyDef &propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty);
            CMdEProperty *albumArtistProp = NULL;
            int foundIndex = inputItem->Property(propDef, albumArtistProp);
            if ( foundIndex != KErrNotFound && albumArtistProp ) {
                output.setValue(s60DescToQString(albumArtistProp->TextValueL()));
            }
            break;
        }
    case EComposer:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
            CMdEPropertyDef &propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KComposerProperty);
            CMdEProperty *composerProp = NULL;
            int foundIndex = inputItem->Property(propDef, composerProp);
            if ( foundIndex != KErrNotFound && composerProp ) {
                output.setValue(s60DescToQString(composerProp->TextValueL()));
            }
            break;
        }
    case ETrackNumber:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject)
                break;

    #ifdef MDS_25_92MCL_COMPILATION_ENABLED
            CMdEPropertyDef &propDef = inputItem->Def().GetPropertyDefL(MdeConstants::MediaObject::KTrackProperty);
    #else
            CMdEPropertyDef &propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KTrackProperty);
    #endif //MDS_25_92MCL_COMPILATION_ENABLED
            CMdEProperty* trackProp = NULL;
            int foundIndex = inputItem->Property(propDef, trackProp);
            if ( foundIndex != KErrNotFound && trackProp ) {
                unsigned long int value = trackProp->Uint32ValueL();
                output.setValue( value );
            }
            break;
        }

    case EAudioCodec:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
#ifdef MDS_25_92MCL_COMPILATION_ENABLED
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
#else
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KAudioFourCCProperty );
#endif //MDS_25_92MCL_COMPILATION_ENABLED
            CMdEProperty* codecProp = NULL;
            int foundIndex = inputItem->Property( propDef, codecProp );
            if ( foundIndex != KErrNotFound && codecProp ) {
                unsigned long int value = codecProp->Uint32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EAudioBitrate:
    case EVideoBitrate:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
            CMdEProperty* brateProp = NULL;
            int foundIndex = inputItem->Property( propDef, brateProp );
            if ( foundIndex != KErrNotFound && brateProp ) {
                unsigned int value = brateProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EPlayCount:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
            CMdEProperty* countProp = NULL;
            int foundIndex = inputItem->Property( propDef, countProp );
            if ( foundIndex != KErrNotFound && countProp ) {
                unsigned long int value = countProp->Uint32ValueL();
                output.setValue( value );
            }
            break;
        }
    case ESampleRate:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
            CMdEProperty* samplingProp = NULL;
            int foundIndex = inputItem->Property( propDef, samplingProp );
            if ( foundIndex != KErrNotFound && samplingProp ) {
                double value = samplingProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EWidth:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
            CMdEProperty* widthProp = NULL;
            int foundIndex = inputItem->Property( propDef, widthProp );
            if ( foundIndex != KErrNotFound && widthProp ) {
                unsigned int value = widthProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EHeight:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
            CMdEProperty* heightProp = NULL;
            int foundIndex = inputItem->Property( propDef, heightProp );
            if ( foundIndex != KErrNotFound && heightProp ) {
                unsigned int value = heightProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EOrientation:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            CMdEProperty* orientationProp = NULL;
            int foundIndex = inputItem->Property( propDef, orientationProp );
            if ( foundIndex != KErrNotFound && orientationProp ) {
                unsigned int value = orientationProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EDateTaken:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            CMdEProperty* dateProp = NULL;
            int foundIndex = inputItem->Property( propDef, dateProp );
            if ( foundIndex != KErrNotFound && dateProp ) {
                output.setValue(symbianTTimetoQDateTime(dateProp->TimeValueL()));
            }
            break;
        }
    case ECameraManufacturer:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            CMdEProperty* makeProp = NULL;
            int foundIndex = inputItem->Property( propDef, makeProp );
            if ( foundIndex != KErrNotFound && makeProp ) {
                output.setValue( s60DescToQString( makeProp->TextValueL() ) );
            }
            break;
        }
    case ECameraModel:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            CMdEProperty* modelProp = NULL;
            int foundIndex = inputItem->Property( propDef, modelProp );
            if ( foundIndex != KErrNotFound && modelProp ) {
                output.setValue( s60DescToQString( modelProp->TextValueL() ) );
            }
            break;
        }
    case EExposureProgram:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            CMdEProperty* exppProp = NULL;
            int foundIndex = inputItem->Property( propDef, exppProp );
            if ( foundIndex != KErrNotFound && exppProp ) {
                unsigned int value = exppProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EExposureTime:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            CMdEProperty* exptProp = NULL;
            int foundIndex = inputItem->Property( propDef, exptProp );
            if ( foundIndex != KErrNotFound && exptProp ) {
                double value = exptProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EFNumber:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            CMdEProperty* fnumProp = NULL;
            int foundIndex = inputItem->Property( propDef, fnumProp );
            if ( foundIndex != KErrNotFound && fnumProp ) {
                double value = fnumProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EFlashEnabled:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            CMdEProperty* flashProp = NULL;
            int foundIndex = inputItem->Property( propDef, flashProp );
            if ( foundIndex != KErrNotFound && flashProp ) {
                unsigned int value = flashProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EFocalLength:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            CMdEProperty* focalProp = NULL;
            int foundIndex = inputItem->Property( propDef, focalProp );
            if ( foundIndex != KErrNotFound && focalProp ) {
                unsigned int value = focalProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EMeteringMode:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            CMdEProperty* metProp = NULL;
            int foundIndex = inputItem->Property( propDef, metProp );
            if ( foundIndex != KErrNotFound && metProp ) {
                unsigned int value = metProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EWhiteBalance:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            CMdEProperty* whiteProp = NULL;
            int foundIndex = inputItem->Property( propDef, whiteProp );
            if ( foundIndex != KErrNotFound && whiteProp ) {
                unsigned int value = whiteProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case ELanguage:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KAudioLanguageProperty );
            CMdEProperty* langProp = NULL;
            int foundIndex = inputItem->Property( propDef, langProp );
            if ( foundIndex != KErrNotFound && langProp ) {
                output.setValue( s60DescToQString( langProp->TextValueL() ) );
            }
            break;
        }
    case EFrameRate:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            CMdEProperty* framerProp = NULL;
            int foundIndex = inputItem->Property( propDef, framerProp );
            if ( foundIndex != KErrNotFound && framerProp ) {
                double value = framerProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EResumePosition:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject)
                break;

            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            CMdEProperty* posProp = NULL;
            int foundIndex = inputItem->Property( propDef, posProp );
            if ( foundIndex != KErrNotFound && posProp ) {
                double value = posProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
        default:
        break;
    }
}

#else

void QDocumentGalleryMDSUtility::GetMetaDataFieldForMDS20L( CMdEObject *inputItem, QVariant &output, int key )
{
    switch (key) {
    case EUri:
        {
            QUrl url( QUrl::fromLocalFile(s60DescToQString( inputItem->Uri() ) ) );
            output.setValue( url );
            break;
        }
    case EFileName:
        {
            TFileName fullName = inputItem->Uri();
            TPtrC name;
            if( GetName(fullName, name) ) {
                QFileInfo fileinfo(s60DescToQString(name));
                output.setValue(fileinfo.fileName());
            }
            break;
        }
    case EFilePath:
        {
            QFileInfo fileinfo(s60DescToQString(inputItem->Uri()));
            output.setValue(fileinfo.absoluteFilePath());
            break;
        }
    case EPath:
        {
            TFileName fullName = inputItem->Uri();
            TPtrC path;
            if( GetPath( fullName, path) ) {
                QFileInfo fileinfo(s60DescToQString(path));
                output.setValue(fileinfo.absoluteFilePath());
            }
            break;
        }
    case EFileSize:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KSizeProperty );
            if( !propDef )
                return;

            CMdEProperty* sizeProp = NULL;
            int foundIndex = inputItem->Property( *propDef, sizeProp );
            if ( foundIndex != KErrNotFound && sizeProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )sizeProp;
                unsigned long int value = uint32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case ELastModified:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            if( !propDef )
                return;

            CMdEProperty* modProp = NULL;
            int foundIndex = inputItem->Property( *propDef, modProp );
            if ( foundIndex != KErrNotFound && modProp ) {
                CMdETimeProperty*     timeProperty = NULL;
                timeProperty = ( CMdETimeProperty* )modProp;
                output.setValue( symbianTTimetoQDateTime( timeProperty->Value() ) );
            }
            break;
        }
    case ETitle:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            if( !propDef )
                return;

            CMdEProperty* titleProp = NULL;
            int foundIndex = inputItem->Property( *propDef, titleProp );
            if ( foundIndex != KErrNotFound && titleProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )titleProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case EMime:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
            if( !propDef )
                return;

            CMdEProperty* mimeProp = NULL;
            int foundIndex = inputItem->Property( *propDef, mimeProp );
            if ( foundIndex != KErrNotFound && mimeProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )mimeProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case EAuthor:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
            if( !propDef )
                return;

            CMdEProperty* authorProp = NULL;
            int foundIndex = inputItem->Property( *propDef, authorProp );
            if ( foundIndex != KErrNotFound && authorProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )authorProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case ECopyright:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
            if( !propDef )
                return;

            CMdEProperty* copyProp = NULL;
            int foundIndex = inputItem->Property( *propDef, copyProp );
            if ( foundIndex != KErrNotFound && copyProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )copyProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case EDescription:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
            if( !propDef )
                return;

            CMdEProperty* desProp = NULL;
            int foundIndex = inputItem->Property( *propDef, desProp );
            if ( foundIndex != KErrNotFound && desProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )desProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case EComments:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
            if( !propDef )
                return;

            CMdEProperty* comProp = NULL;
            int foundIndex = inputItem->Property( *propDef, comProp );
            if ( foundIndex != KErrNotFound && comProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )comProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case ERating:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
            if( !propDef )
                return;

            CMdEProperty* rateProp = NULL;
            int foundIndex = inputItem->Property( *propDef, rateProp );
            if ( foundIndex != KErrNotFound && rateProp ) {
                CMdEUint8Property*     uint8Property = NULL;
                uint8Property = ( CMdEUint8Property* )rateProp;
                unsigned int value = uint8Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EDuration:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            if( !propDef )
                return;

            CMdEProperty* duraProp = NULL;
            int foundIndex = inputItem->Property( *propDef, duraProp );
            if ( foundIndex != KErrNotFound && duraProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )duraProp;
                double value = real32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EPerformer:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            if( !propDef )
                return;

            CMdEProperty* artistProp = NULL;
            int foundIndex = inputItem->Property( *propDef, artistProp );
            if ( foundIndex != KErrNotFound && artistProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )artistProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case EAlbumTitle:
        {
            CMdEPropertyDef *propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
            if( !propDef )
                return;

            CMdEProperty *albumProp = NULL;
            int foundIndex = inputItem->Property(*propDef,albumProp);
            if ( foundIndex != KErrNotFound && albumProp ) {
                CMdETextProperty *textProperty = NULL;
                textProperty = (CMdETextProperty*)albumProp;
                output.setValue(s60DescToQString(textProperty->Value()));
            }
            break;
        }
    case EAlbumArtist:
        {
            CMdEPropertyDef *propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty);
            if( !propDef )
                return;

            CMdEProperty *albumArtistProp = NULL;
            int foundIndex = inputItem->Property(*propDef, albumArtistProp);
            if ( foundIndex != KErrNotFound && albumArtistProp ) {
                CMdETextProperty *textProperty = NULL;
                textProperty = (CMdETextProperty*)albumArtistProp;
                output.setValue(s60DescToQString(textProperty->Value()));
            }
            break;
        }
    case EComposer:
        {
            CMdEPropertyDef *propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KComposerProperty);
            if(!propDef)
                return;

            CMdEProperty *composerProp = NULL;
            int foundIndex = inputItem->Property(*propDef, composerProp);
            if ( foundIndex != KErrNotFound && composerProp ) {
                CMdETextProperty *textProperty = NULL;
                textProperty = (CMdETextProperty*)composerProp;
                output.setValue(s60DescToQString(textProperty->Value()));
            }
            break;
        }
    case ETrackNumber:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL(MdeConstants::Audio::KTrackProperty);
            if( !propDef )
                return;
    
            CMdEProperty* trackProp = NULL;
            int foundIndex = inputItem->Property( *propDef, trackProp );
            if ( foundIndex != KErrNotFound && trackProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )trackProp;
                unsigned long int value = uint32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EAudioCodec:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KAudioFourCCProperty );
            if( !propDef )
                return;

            CMdEProperty* codecProp = NULL;
            int foundIndex = inputItem->Property( *propDef, codecProp );
            if ( foundIndex != KErrNotFound && codecProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )codecProp;
                unsigned long int value = uint32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EAudioBitrate:
    case EVideoBitrate:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
            if( !propDef )
                return;

            CMdEProperty* brateProp = NULL;
            int foundIndex = inputItem->Property( *propDef, brateProp );
            if ( foundIndex != KErrNotFound && brateProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )brateProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EPlayCount:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
            if( !propDef )
                return;

            CMdEProperty* countProp = NULL;
            int foundIndex = inputItem->Property( *propDef, countProp );
            if ( foundIndex != KErrNotFound && countProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )countProp;
                unsigned long int value = uint32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case ESampleRate:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
            if( !propDef )
                return;

            CMdEProperty* samplingProp = NULL;
            int foundIndex = inputItem->Property( *propDef, samplingProp );
            if ( foundIndex != KErrNotFound && samplingProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )samplingProp;
                double value = real32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EWidth:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
            if( !propDef )
                return;

            CMdEProperty* widthProp = NULL;
            int foundIndex = inputItem->Property( *propDef, widthProp );
            if ( foundIndex != KErrNotFound && widthProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )widthProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EHeight:
        {
            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
            if( !propDef )
                return;

            CMdEProperty* heightProp = NULL;
            int foundIndex = inputItem->Property( *propDef, heightProp );
            if ( foundIndex != KErrNotFound && heightProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )heightProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EOrientation:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            if( !propDef )
                return;

            CMdEProperty* orientationProp = NULL;
            int foundIndex = inputItem->Property( *propDef, orientationProp );
            if ( foundIndex != KErrNotFound && orientationProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )orientationProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EDateTaken:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            if( !propDef )
                return;

            CMdEProperty* dateProp = NULL;
            int foundIndex = inputItem->Property( *propDef, dateProp );
            if ( foundIndex != KErrNotFound && dateProp ) {
                CMdETimeProperty*     timeProperty = NULL;
                timeProperty = ( CMdETimeProperty* )dateProp;
                output.setValue( symbianTTimetoQDateTime( timeProperty->Value() ) );
            }
            break;
        }
    case ECameraManufacturer:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            if( !propDef )
                return;

            CMdEProperty* makeProp = NULL;
            int foundIndex = inputItem->Property( *propDef, makeProp );
            if ( foundIndex != KErrNotFound && makeProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )makeProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case ECameraModel:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            if( !propDef )
                return;

            CMdEProperty* modelProp = NULL;
            int foundIndex = inputItem->Property( *propDef, modelProp );
            if ( foundIndex != KErrNotFound && modelProp ) {
                CMdETextProperty*     textProperty = NULL;
                textProperty = ( CMdETextProperty* )modelProp;
                output.setValue( s60DescToQString( textProperty->Value() ) );
            }
            break;
        }
    case EExposureProgram:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            if( !propDef )
                return;

            CMdEProperty* exppProp = NULL;
            int foundIndex = inputItem->Property( *propDef, exppProp );
            if ( foundIndex != KErrNotFound && exppProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )exppProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EExposureTime:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            if( !propDef )
                return;

            CMdEProperty* exptProp = NULL;
            int foundIndex = inputItem->Property( *propDef, exptProp );
            if ( foundIndex != KErrNotFound && exptProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )exptProp;
                double value = real32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EFNumber:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            if( !propDef )
                return;

            CMdEProperty* fnumProp = NULL;
            int foundIndex = inputItem->Property( *propDef, fnumProp );
            if ( foundIndex != KErrNotFound && fnumProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )fnumProp;
                double value = real32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EFlashEnabled:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            if( !propDef )
                return;

            CMdEProperty* flashProp = NULL;
            int foundIndex = inputItem->Property( *propDef, flashProp );
            if ( foundIndex != KErrNotFound && flashProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )flashProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EFocalLength:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            if( !propDef )
                return;

            CMdEProperty* focalProp = NULL;
            int foundIndex = inputItem->Property( *propDef, focalProp );
            if ( foundIndex != KErrNotFound && focalProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )focalProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EMeteringMode:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            if( !propDef )
                return;

            CMdEProperty* metProp = NULL;
            int foundIndex = inputItem->Property( *propDef, metProp );
            if ( foundIndex != KErrNotFound && metProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )metProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EWhiteBalance:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            if( !propDef )
                return;

            CMdEProperty* whiteProp = NULL;
            int foundIndex = inputItem->Property( *propDef, whiteProp );
            if ( foundIndex != KErrNotFound && whiteProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )whiteProp;
                unsigned int value = uint16Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EFrameRate:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            if( !propDef )
                return;

            CMdEProperty* framerProp = NULL;
            int foundIndex = inputItem->Property( *propDef, framerProp );
            if ( foundIndex != KErrNotFound && framerProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )framerProp;
                double value = real32Property->Value();
                output.setValue( value );
            }
            break;
        }
    case EResumePosition:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject)
                break;

            CMdEPropertyDef* propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            if( !propDef )
                return;

            CMdEProperty* posProp = NULL;
            int foundIndex = inputItem->Property( *propDef, posProp );
            if ( foundIndex != KErrNotFound && posProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )posProp;
                double value = real32Property->Value();
                output.setValue( value );
            }
            break;
        }
        default:
        break;
    }
}

#endif // MDS_25_COMPILATION_ENABLED 

QString QDocumentGalleryMDSUtility::GetItemTypeFromMDEObject( CMdEObject *inputItem )
{
    if( !inputItem )
        return NULL;

    if (inputItem->Def().Name() == MdeConstants::Album::KAlbumObject) {
        return QDocumentGallery::PhotoAlbum.name();
    } else if (inputItem->Def().Name() == MdeConstants::Audio::KAudioObject) {
        return QDocumentGallery::Audio.name();
    } else if (inputItem->Def().Name() == MdeConstants::MediaObject::KMediaObject) {
        return QDocumentGallery::File.name();
    } else if (inputItem->Def().Name() == MdeConstants::Image::KImageObject) {
        return QDocumentGallery::Image.name();
    } else if (inputItem->Def().Name() == MdeConstants::Video::KVideoObject) {
        return QDocumentGallery::Video.name();
    } else
        return QDocumentGallery::File.name();
}

int QDocumentGalleryMDSUtility::GetPropertyKey( const QString &property )
{
    if (property == QDocumentGallery::url.name()) {
        return EUri;
    } else if (property == QDocumentGallery::fileName.name()) {
        return EFileName;
    } else if(property == QDocumentGallery::filePath.name()) {
        return EFilePath;
    } else if(property == QDocumentGallery::path.name()) {
        return EPath;
    } else if (property == QDocumentGallery::fileSize.name()) {
        return EFileSize;
    } else if (property == QDocumentGallery::lastModified.name()) {
        return ELastModified;
    } else if (property == QDocumentGallery::title.name()) {
        return ETitle;
    } else if (property == QDocumentGallery::mimeType.name()) {
        return EMime;
    } else if (property == QDocumentGallery::duration.name()) {
        return EDuration;
    } else if (property == QDocumentGallery::performer.name()) {
        return EPerformer;
    } else if (property == QDocumentGallery::artist.name()) {
        return EPerformer;        
    } else if (property == QDocumentGallery::albumTitle.name()) {
        return EAlbumTitle;        
    } else if (property == QDocumentGallery::albumArtist.name()) {
        return EAlbumArtist;        
    } else if (property == QDocumentGallery::trackNumber.name()) {
        return ETrackNumber;        
    } else if (property == QDocumentGallery::genre.name()) {
        return EGenre;        
    } else if (property == QDocumentGallery::composer.name()) {
        return EComposer;        
    } else if (property == QDocumentGallery::width.name()) {
        return EWidth;
    } else if (property == QDocumentGallery::height.name()) {
        return EHeight;
    } else if (property == QDocumentGallery::orientation.name()) {
        return EOrientation;
    } else if (property == QDocumentGallery::audioBitRate.name()) {
        return EAudioBitrate;
    } else if (property == QDocumentGallery::videoBitRate.name()) {
        return EVideoBitrate;
    } else if (property == QDocumentGallery::frameRate.name()) {
        return EFrameRate;
    } else if (property == QDocumentGallery::copyright.name()) {
        return ECopyright;
    } else if (property == QDocumentGallery::description.name()) {
        return EDescription;
    } else if (property == QDocumentGallery::comments.name()) {
        return EComments;
    } else if (property == QDocumentGallery::author.name()) {
        return EAuthor;
    } else if (property == QDocumentGallery::rating.name()) {
        return ERating;
    } else if (property == QDocumentGallery::audioCodec.name()) {
        return EAudioCodec;
    } else if (property == QDocumentGallery::playCount.name()) {
        return EPlayCount;
    } else if (property == QDocumentGallery::sampleRate.name()) {
        return ESampleRate;
    } else if (property == QDocumentGallery::dateTaken.name()) {
        return EDateTaken;
    } else if (property == QDocumentGallery::cameraManufacturer.name()) {
        return ECameraManufacturer;
    } else if (property == QDocumentGallery::cameraModel.name()) {
        return ECameraModel;
    } else if (property == QDocumentGallery::exposureProgram.name()) {
        return EExposureProgram;
    } else if (property == QDocumentGallery::exposureTime.name()) {
        return EExposureTime;
    } else if (property == QDocumentGallery::fNumber.name()) {
        return EFNumber;
    } else if (property == QDocumentGallery::flashEnabled.name()) {
        return EFlashEnabled;
    } else if (property == QDocumentGallery::focalLength.name()) {
        return EFocalLength;
    } else if (property == QDocumentGallery::meteringMode.name()) {
        return EMeteringMode;
    } else if (property == QDocumentGallery::whiteBalance.name()) {
        return EWhiteBalance;
    } else if (property == QDocumentGallery::language.name()) {
        return ELanguage;
    } else if (property == QDocumentGallery::resumePosition.name()) {
        return EResumePosition;
    }
    return -1;
}

QVariant::Type QDocumentGalleryMDSUtility::GetPropertyType( int key )
{
    switch( key )
    {
    case EUri:
        return QVariant::Url;
    case EFileSize:
    case EAudioCodec:
    case EPlayCount:
        return QVariant::ULongLong;
    case ELastModified:
    case EDateTaken:
        return QVariant::DateTime;
    case EFileName:
    case EFilePath:
    case EPath:
    case ETitle:
    case EMime:
    case EAuthor:
    case ECopyright:
    case EDescription:
    case EComments:
    case EPerformer:
    case EAlbumTitle:        
    case EAlbumArtist:        
    case EGenre:                
    case ECameraManufacturer:
    case ECameraModel:
    case ELanguage:
        return QVariant::String;
    case ERating:
    case EAudioBitrate:
    case EWidth:
    case EHeight:
    case EOrientation:
    case EExposureProgram:
    case EFlashEnabled:
    case EFocalLength:
    case EMeteringMode:
    case EWhiteBalance:
    case EVideoBitrate:
    case ETrackNumber:
        return QVariant::UInt;
    case EDuration:
    case ESampleRate:
    case EExposureTime:
    case EFNumber:
    case EFrameRate:
    case EResumePosition:
        return QVariant::Double;
    default:
        return QVariant::Invalid;
    }
}

/*!
 Converts a Symbian S60 descriptor (string) to a QString.
 The \a desc descriptor is converted and a QString returned.
 */
QString QDocumentGalleryMDSUtility::s60DescToQString(const TDesC& desc)
{
    return QString::fromUtf16(desc.Ptr(), desc.Length());
}

/*!
 Converts a QString to a Symbian S60 descriptor (string).

 Note: Ownership of the returned descriptor (string) is transferred to caller.

 The QString \a string is converted to the descriptor. The pointer to a
 Symbian S60 descriptor is returned on success;
 otherwise returns NULL pointer
 */
HBufC* QDocumentGalleryMDSUtility::qStringToS60Desc(const QString& string)
{
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    return str.Alloc();
}

QDateTime QDocumentGalleryMDSUtility::symbianTTimetoQDateTime(const TTime& time)
{
    TDateTime dateTime = time.DateTime();
    QDate qdate = QDate(dateTime.Year(), static_cast<int>(dateTime.Month())+1, dateTime.Day()+1);
    QTime qtime = QTime(dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()/1000 );
    return QDateTime(qdate, qtime, Qt::UTC);
}

TTime QDocumentGalleryMDSUtility::QDateTimetosymbianTTime(const QDateTime& time)
{
    TDateTime dateTime;
    QDate date( time.date() );
    QTime qtime( time.time() );
    dateTime.Set( date.year(), (TMonth)(date.month() - 1), date.day(),
        qtime.hour(), qtime.minute(), qtime.second(), qtime.msec() );

    return TTime( dateTime );
}

TBool QDocumentGalleryMDSUtility::GetName(const TDesC& aFilename, TPtrC& aName)
{
    // find name (everything after last back slash)
    TInt pos = aFilename.LocateReverseF( '\\' );
    if( pos >= 0 ) {
        aName.Set( aFilename.Mid( pos + 1 ) );

        // remove extension
        TInt pos = aName.LocateReverseF( '.' );
        if( pos >= 0 ) {
            aName.Set( aName.Left( pos ) );
        }

        if( aName.Length() > 0 ) {
            return ETrue;
        }
    }

    return EFalse;
}

TBool QDocumentGalleryMDSUtility::GetPath(const TDesC& aFilename, TPtrC& aPath)
{
    // find path (everything before last back slash)
    TInt pos = aFilename.LocateReverseF( '\\' );
    if( pos >= 0 ) {
        aPath.Set( aFilename.Left( pos + 1 ) );

        if( aPath.Length() > 0 ) {
            return ETrue;
        }
    }

    return EFalse;
}

CMdEPropertyDef *QDocumentGalleryMDSUtility::GetMDSPropertyDefL( const QString &property,
    CMdENamespaceDef& defaultNameSpace )
{
#ifdef MDS_25_COMPILATION_ENABLED
    return GetMDSPropertyDefForMDS25L( property, defaultNameSpace );
#else
    return GetMDSPropertyDefForMDS20L( property, defaultNameSpace );
#endif
}

#ifdef MDS_25_COMPILATION_ENABLED
CMdEPropertyDef *QDocumentGalleryMDSUtility::GetMDSPropertyDefForMDS25L( const QString &property,
    CMdENamespaceDef& defaultNameSpace )
{
    if (property == QDocumentGallery::fileSize.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Object::KSizeProperty );
        return &propDef;
    } else if (property == QDocumentGallery::lastModified.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
        return &propDef;
    } else if (property == QDocumentGallery::title.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Object::KTitleProperty );
        return &propDef;
    } else if (property == QDocumentGallery::mimeType.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
        return &propDef;
    } else if (property == QDocumentGallery::duration.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
        return &propDef;
    } else if (property == QDocumentGallery::performer.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
        return &propDef;
    } else if (property == QDocumentGallery::albumTitle.name()) {
        CMdEObjectDef &def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        CMdEPropertyDef &propDef = def.GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
        return &propDef;
    } else if (property == QDocumentGallery::albumArtist.name()) {
        CMdEObjectDef &def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        CMdEPropertyDef &propDef = def.GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty);
        return &propDef;
    } else if (property == QDocumentGallery::trackNumber.name()) {
#ifdef MDS_25_92MCL_COMPILATION_ENABLED
        CMdEObjectDef &def = defaultNameSpace.GetObjectDefL(MdeConstants::MediaObject::KMediaObject);
        CMdEPropertyDef& propDef = def.GetPropertyDefL(MdeConstants::MediaObject::KTrackProperty);
#else
        CMdEObjectDef &def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        CMdEPropertyDef& propDef = def.GetPropertyDefL(MdeConstants::Audio::KTrackProperty);
#endif //MDS_25_92MCL_COMPILATION_ENABLED
        return &propDef;
    } else if (property == QDocumentGallery::composer.name()) {
        CMdEObjectDef &def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        CMdEPropertyDef &propDef = def.GetPropertyDefL(MdeConstants::Audio::KComposerProperty);
        return &propDef;
    } else if (property == QDocumentGallery::genre.name()) {
        CMdEObjectDef &def = defaultNameSpace.GetObjectDefL(MdeConstants::MediaObject::KMediaObject);
        CMdEPropertyDef &propDef = def.GetPropertyDefL(MdeConstants::MediaObject::KGenreProperty);
        return &propDef;
    } else if (property == QDocumentGallery::width.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
        return &propDef;
    } else if (property == QDocumentGallery::height.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
        return &propDef;
    } else if (property == QDocumentGallery::orientation.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
        return &propDef;
    } else if (property == QDocumentGallery::audioBitRate.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
        return &propDef;
    } else if (property == QDocumentGallery::videoBitRate.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
        return &propDef;
    } else if (property == QDocumentGallery::frameRate.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
        return &propDef;
    } else if (property == QDocumentGallery::author.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
        return &propDef;
    } else if (property == QDocumentGallery::copyright.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
        return &propDef;
    } else if (property == QDocumentGallery::description.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
        return &propDef;
    } else if (property == QDocumentGallery::comments.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
        return &propDef;
    } else if (property == QDocumentGallery::rating.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
        return &propDef;
    }  else if (property == QDocumentGallery::audioCodec.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
#ifdef MDS_25_92MCL_COMPILATION_ENABLED
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
#else
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Video::KAudioFourCCProperty );
#endif //MDS_25_92MCL_COMPILATION_ENABLED
        return &propDef;
    }  else if (property == QDocumentGallery::playCount.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
        return &propDef;
    } else if (property == QDocumentGallery::sampleRate.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Audio::KAudioObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
        return &propDef;
    } else if (property == QDocumentGallery::dateTaken.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
        return &propDef;
    } else if (property == QDocumentGallery::cameraManufacturer.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KMakeProperty );
        return &propDef;
    } else if (property == QDocumentGallery::cameraModel.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KModelProperty );
        return &propDef;
    } else if (property == QDocumentGallery::exposureProgram.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
        return &propDef;
    } else if (property == QDocumentGallery::exposureTime.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
        return &propDef;
    } else if (property == QDocumentGallery::fNumber.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
        return &propDef;
    } else if (property == QDocumentGallery::flashEnabled.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KFlashProperty );
        return &propDef;
    } else if (property == QDocumentGallery::focalLength.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
        return &propDef;
    } else if (property == QDocumentGallery::meteringMode.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
        return &propDef;
    } else if (property == QDocumentGallery::whiteBalance.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
        return &propDef;
    } else if (property == QDocumentGallery::language.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Video::KAudioLanguageProperty );
        return &propDef;
    } else if (property == QDocumentGallery::resumePosition.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
        return &propDef;
    }
    return NULL;
}

#else

CMdEPropertyDef *QDocumentGalleryMDSUtility::GetMDSPropertyDefForMDS20L( const QString &property,
    CMdENamespaceDef& defaultNameSpace )
{
    if (property == QDocumentGallery::fileSize.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Object::KSizeProperty );
        return propDef;
    } else if (property == QDocumentGallery::lastModified.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
        return propDef;
    } else if (property == QDocumentGallery::title.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Object::KTitleProperty );
        return propDef;
    } else if (property == QDocumentGallery::mimeType.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Object::KBaseObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
        return propDef;
    } else if (property == QDocumentGallery::duration.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
        return propDef;
    } else if (property == QDocumentGallery::performer.name() || property == QDocumentGallery::artist.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
        return propDef;
    } else if (property == QDocumentGallery::albumTitle.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        if(!def)
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
        return propDef;
    } else if (property == QDocumentGallery::albumArtist.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        if(!def)
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty);
        return propDef;
    } else if (property == QDocumentGallery::trackNumber.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        if(!def)
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL(MdeConstants::Audio::KTrackProperty);
        return propDef;
    } else if (property == QDocumentGallery::composer.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL(MdeConstants::Audio::KAudioObject);
        if(!def)
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL(MdeConstants::Audio::KComposerProperty);
        return propDef;
    } else if (property == QDocumentGallery::genre.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL(MdeConstants::MediaObject::KMediaObject);
        if(!def)
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL(MdeConstants::MediaObject::KGenreProperty);
        return propDef;
    } else if (property == QDocumentGallery::width.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
        return propDef;
    } else if (property == QDocumentGallery::height.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
        return propDef;
    } else if (property == QDocumentGallery::orientation.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
        return propDef;
    } else if (property == QDocumentGallery::audioBitRate.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
        return propDef;
    } else if (property == QDocumentGallery::videoBitRate.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
        return propDef;
    } else if (property == QDocumentGallery::frameRate.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
        return propDef;
    } else if (property == QDocumentGallery::author.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
        return propDef;
    } else if (property == QDocumentGallery::copyright.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
        return propDef;
    } else if (property == QDocumentGallery::description.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
        return propDef;
    } else if (property == QDocumentGallery::comments.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
        return propDef;
    } else if (property == QDocumentGallery::rating.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
        return propDef;
    } else if (property == QDocumentGallery::audioCodec.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Video::KAudioFourCCProperty );
        return propDef;
    } else if (property == QDocumentGallery::playCount.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
        return propDef;
    } else if (property == QDocumentGallery::sampleRate.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Audio::KAudioObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
        return propDef;
    } else if (property == QDocumentGallery::dateTaken.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
        return propDef;
    } else if (property == QDocumentGallery::cameraManufacturer.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KMakeProperty );
        return propDef;
    } else if (property == QDocumentGallery::cameraModel.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KModelProperty );
        return propDef;
    } else if (property == QDocumentGallery::exposureProgram.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
        return propDef;
    } else if (property == QDocumentGallery::exposureTime.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
        return propDef;
    } else if (property == QDocumentGallery::fNumber.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
        return propDef;
    } else if (property == QDocumentGallery::flashEnabled.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KFlashProperty );
        return propDef;
    } else if (property == QDocumentGallery::focalLength.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
        return propDef;
    } else if (property == QDocumentGallery::meteringMode.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
        return propDef;
    } else if (property == QDocumentGallery::whiteBalance.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Image::KImageObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
        return propDef;
    } else if (property == QDocumentGallery::resumePosition.name()) {
        CMdEObjectDef* def = defaultNameSpace.GetObjectDefL( MdeConstants::Video::KVideoObject );
        if( !def )
            return NULL;
        CMdEPropertyDef* propDef = def->GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
        return propDef;
    }
    return NULL;
}

#endif // MDS_25_COMPILATION_ENABLED

int QDocumentGalleryMDSUtility::SetupQueryConditions(CMdEObjectQuery *query,
    QGalleryQueryRequest *request,
    CMdENamespaceDef& defaultNameSpace)
{
    // Add filtering conditions
    int conditionError = QDocumentGallery::NoError;
    CMdELogicCondition &rootCond = query->Conditions();

    // Add filtering conditions
    QGalleryFilter filter = request->filter();
    QVariant rootItem = request->rootItem();

    if (rootItem.isValid()) {
        rootCond.SetOperator(ELogicConditionOperatorAnd);

        conditionError = QMDEGalleryCategoryResultSet::appendScopeCondition(
                &rootCond, rootItem, defaultNameSpace);

        if (conditionError != QDocumentGallery::NoError) {
            switch (filter.type()) {
            case QGalleryFilter::Invalid:
                break;
            case QGalleryFilter::Intersection:
                conditionError = AddFilter(rootCond, filter, defaultNameSpace);
                break;
            case QGalleryFilter::Union:
                rootCond.SetOperator( ELogicConditionOperatorOr );
                conditionError = AddUnionFilter(rootCond, filter, defaultNameSpace);
                break;
            case QGalleryFilter::MetaData:
                conditionError = AddMetadataFilter(rootCond, filter, defaultNameSpace);
                break;
            default:
                return QDocumentGallery::FilterError;
            }
        }
    } else {
        switch (filter.type()) {
        case QGalleryFilter::Invalid:
            break;
        case QGalleryFilter::Intersection:
            rootCond.SetOperator( ELogicConditionOperatorAnd );
            conditionError = AddIntersectionFilter( rootCond, filter, defaultNameSpace );
            break;
        case QGalleryFilter::Union:
            rootCond.SetOperator( ELogicConditionOperatorOr );
            conditionError = AddUnionFilter( rootCond, filter, defaultNameSpace );
            break;
        case QGalleryFilter::MetaData:
            conditionError = AddMetadataFilter( rootCond, filter, defaultNameSpace );
            break;
        default:
            return QDocumentGallery::FilterError;
        }
    }
    if (conditionError != QDocumentGallery::NoError) {
        return conditionError;
    }

    // Add sorting conditions
    const QStringList sortPropertyNames = request->sortPropertyNames();
    const bool descending = !sortPropertyNames.isEmpty()
            && sortPropertyNames.first().startsWith(QLatin1Char('-'));

    for (QStringList::const_iterator it = sortPropertyNames.constBegin();
        it != sortPropertyNames.constEnd();
        ++it) {
        bool ascending = true;

        QString propertyName = *it;

        if (propertyName.startsWith(QLatin1Char('-'))) {
            propertyName = propertyName.mid(1);
            ascending = false;
        }

        CMdEPropertyDef *propDef = NULL;
        TRAPD( err, propDef = GetMDSPropertyDefL( propertyName, defaultNameSpace ) );
        if (err || !propDef) {
            continue;
        }
        int orderRuleErr = 0;
        if (ascending) {
            TRAP(orderRuleErr, query->AppendOrderRuleL(TMdEOrderRule(*propDef, ESortAscending)))
        } else {
            TRAP(orderRuleErr, query->AppendOrderRuleL(TMdEOrderRule(*propDef, ESortDescending)))
        }
        if (orderRuleErr != KErrNone)
            return QDocumentGallery::FilterError;
    }
    return QDocumentGallery::NoError;
}

bool QDocumentGalleryMDSUtility::SetMetaDataFieldL( CMdEObject *item, const QVariant &value, int key )
{
    if ( !item )
        return false;
    
#ifdef MDS_25_COMPILATION_ENABLED
    return SetMetaDataFieldForMDS25L( item, value, key );
#else
    return SetMetaDataFieldForMDS20L( item, value, key );
#endif
}

#ifdef MDS_25_COMPILATION_ENABLED
bool QDocumentGalleryMDSUtility::SetMetaDataFieldForMDS25L( CMdEObject *item, const QVariant &value, int key )
{
    switch( key )
    {
    case EUri:
    case EFileName:
    case EFilePath:
    case EPath:
        return false;
    case EFileSize:
        {
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KSizeProperty );
            CMdEProperty* sizeProp = NULL;
            int foundIndex = item->Property( propDef, sizeProp );
            if ( foundIndex != KErrNotFound && sizeProp ) {
                TRAPD( err, sizeProp->SetUint32ValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ELastModified:
        {
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            CMdEProperty* modProp = NULL;
            int foundIndex = item->Property( propDef, modProp );
            if ( foundIndex != KErrNotFound && modProp ) {
                TRAPD( err, modProp->SetTimeValueL( QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddTimePropertyL( propDef, QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ETitle:
        {
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            CMdEProperty* titleProp = NULL;
            int foundIndex = item->Property( propDef, titleProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && titleProp ) {
                TRAPD( err, titleProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EMime:
        return false;
    case EAuthor:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
            CMdEProperty* authorProp = NULL;
            int foundIndex = item->Property( propDef, authorProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && authorProp ) {
                TRAPD( err, authorProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ECopyright:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject ) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
            CMdEProperty* copyProp = NULL;
            int foundIndex = item->Property( propDef, copyProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && copyProp ) {
                TRAPD( err, copyProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EDescription:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
            CMdEProperty* desProp = NULL;
            int foundIndex = item->Property( propDef, desProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && desProp ) {
                TRAPD( err, desProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EComments:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
            CMdEProperty* comProp = NULL;
            int foundIndex = item->Property( propDef, comProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && comProp ) {
                TRAPD( err, comProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ERating:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
            CMdEProperty* rateProp = NULL;
            int foundIndex = item->Property( propDef, rateProp );
            if ( foundIndex != KErrNotFound && rateProp ) {
                TRAPD( err, rateProp->SetUint8ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint8PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EDuration:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            CMdEProperty* duraProp = NULL;
            int foundIndex = item->Property( propDef, duraProp );
            if ( foundIndex != KErrNotFound && duraProp ) {
                TRAPD( err, duraProp->SetReal32ValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EPerformer:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            CMdEProperty* artistProp = NULL;
            int foundIndex = item->Property( propDef, artistProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && artistProp ) {
                TRAPD( err, artistProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EGenre:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef &propDef = item->Def().GetPropertyDefL(MdeConstants::MediaObject::KGenreProperty);
            CMdEProperty* genreProp = NULL;
            int foundIndex = item->Property(propDef, genreProp);
            const TDesC text(qStringToS60Desc(value.toString() )->Des());
            if (foundIndex != KErrNotFound && genreProp) {
                TRAPD( err, genreProp->SetTextValueL(text));
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL(propDef, text) );
                return err == KErrNone ? true : false ;
            }
        }
    case EComposer:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef &propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KComposerProperty);
            CMdEProperty* composerProp = NULL;
            int foundIndex = item->Property(propDef, composerProp);
            const TDesC text(qStringToS60Desc(value.toString())->Des());
            if ( foundIndex != KErrNotFound && composerProp ) {
                TRAPD( err, composerProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL(propDef, text) );
                return err == KErrNone ? true : false ;
            }
        }
    case EAlbumTitle:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef &propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
            CMdEProperty* albumTitleProp = NULL;
            int foundIndex = item->Property(propDef, albumTitleProp);
            const TDesC text(qStringToS60Desc(value.toString())->Des());
            if ( foundIndex != KErrNotFound && albumTitleProp ) {
                TRAPD( err, albumTitleProp->SetTextValueL(text) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL(propDef, text));
                return err == KErrNone ? true : false ;
            }
        }
    case EAlbumArtist:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef &propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty);
            CMdEProperty* albumArtistProp = NULL;
            int foundIndex = item->Property( propDef, albumArtistProp );
            const TDesC text(qStringToS60Desc(value.toString())->Des());
            if ( foundIndex != KErrNotFound && albumArtistProp ) {
                TRAPD( err, albumArtistProp->SetTextValueL(text));
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL(propDef, text));
                return err == KErrNone ? true : false ;
            }
        }
    case ETrackNumber:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject)
                return false;

#ifdef MDS_25_92MCL_COMPILATION_ENABLED
            CMdEPropertyDef &propDef = item->Def().GetPropertyDefL(MdeConstants::MediaObject::KTrackProperty);
#else            
            CMdEPropertyDef &propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KTrackProperty);
#endif //MDS_25_92MCL_COMPILATION_ENABLED
            CMdEProperty* trackProp = NULL;
            int foundIndex = item->Property(propDef, trackProp);
            if ( foundIndex != KErrNotFound && trackProp ) {
                TRAPD( err, trackProp->SetUint32ValueL(value.toULongLong()) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL(propDef, value.toULongLong()) );
                return err == KErrNone ? true : false ;
            }
        }
    case EAudioCodec:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
#ifdef MDS_25_92MCL_COMPILATION_ENABLED
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
#else
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KAudioFourCCProperty );
#endif //MDS_25_92MCL_COMPILATION_ENABLED
            CMdEProperty* codecProp = NULL;
            int foundIndex = item->Property( propDef, codecProp );
            if ( foundIndex != KErrNotFound && codecProp ) {
                TRAPD( err, codecProp->SetUint32ValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EAudioBitrate:
    case EVideoBitrate:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
            CMdEProperty* brateProp = NULL;
            int foundIndex = item->Property( propDef, brateProp );
            if ( foundIndex != KErrNotFound && brateProp ) {
                TRAPD( err, brateProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EPlayCount:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
            CMdEProperty* countProp = NULL;
            int foundIndex = item->Property( propDef, countProp );
            if ( foundIndex != KErrNotFound && countProp ) {
                TRAPD( err, countProp->SetUint32ValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ESampleRate:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
            CMdEProperty* samplingProp = NULL;
            int foundIndex = item->Property( propDef, samplingProp );
            if ( foundIndex != KErrNotFound && samplingProp ) {
                TRAPD( err, samplingProp->SetReal32ValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EWidth:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
            CMdEProperty* widthProp = NULL;
            int foundIndex = item->Property( propDef, widthProp );
            if ( foundIndex != KErrNotFound && widthProp ) {
                TRAPD( err, widthProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EHeight:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
            CMdEProperty* heightProp = NULL;
            int foundIndex = item->Property( propDef, heightProp );
            if ( foundIndex != KErrNotFound && heightProp ) {
                TRAPD( err, heightProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EOrientation:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject )
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            CMdEProperty* orientationProp = NULL;
            int foundIndex = item->Property( propDef, orientationProp );
            if ( foundIndex != KErrNotFound && orientationProp ) {
                TRAPD( err, orientationProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EDateTaken:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            CMdEProperty* dateProp = NULL;
            int foundIndex = item->Property( propDef, dateProp );
            if ( foundIndex != KErrNotFound && dateProp ) {
                TRAPD( err, dateProp->SetTimeValueL( QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTimePropertyL( propDef, QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ECameraManufacturer:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            CMdEProperty* makeProp = NULL;
            int foundIndex = item->Property( propDef, makeProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && makeProp ) {
                TRAPD( err, makeProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ECameraModel:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            CMdEProperty* modelProp = NULL;
            int foundIndex = item->Property( propDef, modelProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && modelProp ) {
                TRAPD( err, modelProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EExposureProgram:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            CMdEProperty* exppProp = NULL;
            int foundIndex = item->Property( propDef, exppProp );
            if ( foundIndex != KErrNotFound && exppProp ) {
                TRAPD( err, exppProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EExposureTime:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            CMdEProperty* exptProp = NULL;
            int foundIndex = item->Property( propDef, exptProp );
            if ( foundIndex != KErrNotFound && exptProp ) {
                TRAPD( err, exptProp->SetReal32ValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFNumber:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            CMdEProperty* fnumProp = NULL;
            int foundIndex = item->Property( propDef, fnumProp );
            if ( foundIndex != KErrNotFound && fnumProp ) {
                TRAPD( err, fnumProp->SetReal32ValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFlashEnabled:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            CMdEProperty* flashProp = NULL;
            int foundIndex = item->Property( propDef, flashProp );
            if ( foundIndex != KErrNotFound && flashProp ) {
                TRAPD( err, flashProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFocalLength:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            CMdEProperty* focalProp = NULL;
            int foundIndex = item->Property( propDef, focalProp );
            if ( foundIndex != KErrNotFound && focalProp ) {
                TRAPD( err, focalProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EMeteringMode:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            CMdEProperty* metProp = NULL;
            int foundIndex = item->Property( propDef, metProp );
            if ( foundIndex != KErrNotFound && metProp ) {
                TRAPD( err, metProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EWhiteBalance:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            CMdEProperty* whiteProp = NULL;
            int foundIndex = item->Property( propDef, whiteProp );
            if ( foundIndex != KErrNotFound && whiteProp ) {
                TRAPD( err, whiteProp->SetUint16ValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ELanguage:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KAudioLanguageProperty );
            CMdEProperty* langProp = NULL;
            int foundIndex = item->Property( propDef, langProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( foundIndex != KErrNotFound && langProp ) {
                TRAPD( err, langProp->SetTextValueL( text ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFrameRate:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            CMdEProperty* framerProp = NULL;
            int foundIndex = item->Property( propDef, framerProp );
            if ( foundIndex != KErrNotFound && framerProp ) {
                TRAPD( err, framerProp->SetReal32ValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EResumePosition:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject)
                return false;

            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            CMdEProperty* posProp = NULL;
            int foundIndex = item->Property( propDef, posProp );
            if ( foundIndex != KErrNotFound && posProp ) {
                TRAPD( err, posProp->SetReal32ValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    default:
        return false;
    }
}

#else

bool QDocumentGalleryMDSUtility::SetMetaDataFieldForMDS20L( CMdEObject *item, const QVariant &value, int key )
{
    switch( key )
    {
    case EUri:
    case EFileName:
    case EFilePath:
    case EPath:
        return false;
    case EFileSize:
        {
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KSizeProperty );
            if( !propDef )
                return false;
            
            CMdEProperty* sizeProp = NULL;
            int foundIndex = item->Property( *propDef, sizeProp );
            if ( foundIndex != KErrNotFound && sizeProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )sizeProp;
                TRAPD( err, uint32Property->SetValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( *propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ELastModified:
        {
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            if( !propDef )
                return false;

            CMdEProperty* modProp = NULL;
            int foundIndex = item->Property( *propDef, modProp );
            if ( foundIndex != KErrNotFound && modProp ) {
                CMdETimeProperty*     timeProperty = NULL;
                timeProperty = ( CMdETimeProperty* )modProp;
                TRAPD( err, timeProperty->SetValueL( QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddTimePropertyL( *propDef, QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ETitle:
        {
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case EMime:
        return false;
    case EAuthor:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }            
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case ECopyright:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case EDescription:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case EComments:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
            return setMdeStringProperty(item, value, propDef); 
        }
    case ERating:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
            return setMdeStringProperty(item, value, propDef); 
        }
    case EDuration:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            if( !propDef )
                return false;

            CMdEProperty* duraProp = NULL;
            int foundIndex = item->Property( *propDef, duraProp );
            if ( foundIndex != KErrNotFound && duraProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )duraProp;
                TRAPD( err, real32Property->SetValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( *propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EPerformer:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case EGenre:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL(MdeConstants::MediaObject::KGenreProperty);
            return setMdeStringProperty(item, value, propDef);     
        }
    case EComposer:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KComposerProperty);
            return setMdeStringProperty(item, value, propDef);
        }
    case EAlbumTitle:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumProperty);
            return setMdeStringProperty(item, value, propDef);
        }
    case EAlbumArtist:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KAlbumArtistProperty);
            return setMdeStringProperty(item, value, propDef);
        }
    case ETrackNumber:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL(MdeConstants::Audio::KTrackProperty);
            if( !propDef )
                return false;

            CMdEProperty* trackProp = NULL;
            int foundIndex = item->Property( *propDef, trackProp );
            if ( foundIndex != KErrNotFound && trackProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )trackProp;
                TRAPD( err, uint32Property->SetValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( *propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EAudioCodec:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KAudioFourCCProperty );
            if( !propDef )
                return false;

            CMdEProperty* codecProp = NULL;
            int foundIndex = item->Property( *propDef, codecProp );
            if ( foundIndex != KErrNotFound && codecProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )codecProp;
                TRAPD( err, uint32Property->SetValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( *propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EAudioBitrate:
    case EVideoBitrate:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
            if( !propDef )
                return false;

            CMdEProperty* brateProp = NULL;
            int foundIndex = item->Property( *propDef, brateProp );
            if ( foundIndex != KErrNotFound && brateProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )brateProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EPlayCount:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
            if( !propDef )
                return false;

            CMdEProperty* countProp = NULL;
            int foundIndex = item->Property( *propDef, countProp );
            if ( foundIndex != KErrNotFound && countProp ) {
                CMdEUint32Property*     uint32Property = NULL;
                uint32Property = ( CMdEUint32Property* )countProp;
                TRAPD( err, uint32Property->SetValueL( value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint32PropertyL( *propDef, value.toULongLong() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ESampleRate:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
            if( !propDef )
                return false;

            CMdEProperty* samplingProp = NULL;
            int foundIndex = item->Property( *propDef, samplingProp );
            if ( foundIndex != KErrNotFound && samplingProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )samplingProp;
                TRAPD( err, real32Property->SetValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( *propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EWidth:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
            if( !propDef )
                return false;

            CMdEProperty* widthProp = NULL;
            int foundIndex = item->Property( *propDef, widthProp );
            if ( foundIndex != KErrNotFound && widthProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )widthProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EHeight:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject &&
                item->Def().Name() != MdeConstants::Video::KVideoObject &&
                item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
            if( !propDef )
                return false;

            CMdEProperty* heightProp = NULL;
            int foundIndex = item->Property( *propDef, heightProp );
            if ( foundIndex != KErrNotFound && heightProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )heightProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EOrientation:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            if( !propDef )
                return false;

            CMdEProperty* orientationProp = NULL;
            int foundIndex = item->Property( *propDef, orientationProp );
            if ( foundIndex != KErrNotFound && orientationProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )orientationProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EDateTaken:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            if( !propDef )
                return false;

            CMdEProperty* dateProp = NULL;
            int foundIndex = item->Property( *propDef, dateProp );
            if ( foundIndex != KErrNotFound && dateProp ) {
                CMdETimeProperty*     timeProperty = NULL;
                timeProperty = ( CMdETimeProperty* )dateProp;
                TRAPD( err, timeProperty->SetValueL( QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddTimePropertyL( *propDef, QDateTimetosymbianTTime( value.toDateTime() ) ) );
                return err == KErrNone ? true : false ;
            }
        }
    case ECameraManufacturer:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case ECameraModel:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            return setMdeStringProperty(item, value, propDef);
        }
    case EExposureProgram:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            if( !propDef )
                return false;

            CMdEProperty* exppProp = NULL;
            int foundIndex = item->Property( *propDef, exppProp );
            if ( foundIndex != KErrNotFound && exppProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )exppProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EExposureTime:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            if( !propDef )
                return false;

            CMdEProperty* exptProp = NULL;
            int foundIndex = item->Property( *propDef, exptProp );
            if ( foundIndex != KErrNotFound && exptProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )exptProp;
                TRAPD( err, real32Property->SetValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( *propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFNumber:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            if( !propDef )
                return false;

            CMdEProperty* fnumProp = NULL;
            int foundIndex = item->Property( *propDef, fnumProp );
            if ( foundIndex != KErrNotFound && fnumProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )fnumProp;
                TRAPD( err, real32Property->SetValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddReal32PropertyL( *propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFlashEnabled:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            if( !propDef )
                return false;

            CMdEProperty* flashProp = NULL;
            int foundIndex = item->Property( *propDef, flashProp );
            if ( foundIndex != KErrNotFound && flashProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )flashProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFocalLength:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            if( !propDef )
                return false;

            CMdEProperty* focalProp = NULL;
            int foundIndex = item->Property( *propDef, focalProp );
            if ( foundIndex != KErrNotFound && focalProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )focalProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EMeteringMode:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            if( !propDef )
                return false;

            CMdEProperty* metProp = NULL;
            int foundIndex = item->Property( *propDef, metProp );
            if ( foundIndex != KErrNotFound && metProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )metProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            } else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EWhiteBalance:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            if( !propDef )
                return false;

            CMdEProperty* whiteProp = NULL;
            int foundIndex = item->Property( *propDef, whiteProp );
            if ( foundIndex != KErrNotFound && whiteProp ) {
                CMdEUint16Property*     uint16Property = NULL;
                uint16Property = ( CMdEUint16Property* )whiteProp;
                TRAPD( err, uint16Property->SetValueL( value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddUint16PropertyL( *propDef, value.toUInt() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EFrameRate:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            if( !propDef )
                return false;

            CMdEProperty* framerProp = NULL;
            int foundIndex = item->Property( *propDef, framerProp );
            if ( foundIndex != KErrNotFound && framerProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )framerProp;
                TRAPD( err, real32Property->SetValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddReal32PropertyL( *propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    case EResumePosition:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject)
                return false;

            CMdEPropertyDef* propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            if( !propDef )
                return false;

            CMdEProperty* posProp = NULL;
            int foundIndex = item->Property( *propDef, posProp );
            if ( foundIndex != KErrNotFound && posProp ) {
                CMdEReal32Property*     real32Property = NULL;
                real32Property = ( CMdEReal32Property* )posProp;
                TRAPD( err, real32Property->SetValueL( value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
            else {
                TRAPD( err, item->AddReal32PropertyL( *propDef, value.toDouble() ) );
                return err == KErrNone ? true : false ;
            }
        }
    default:
        return false;
    }
}

bool QDocumentGalleryMDSUtility::setMdeStringProperty(CMdEObject *item, const QVariant &value, CMdEPropertyDef* propDef)
{
    if (!propDef)
        return false;
    CMdEProperty *propertyToSet = NULL;
    int foundIndex = item->Property(*propDef, propertyToSet);
    TRAPD(err,
        HBufC *text = qStringToS60Desc(value.toString());
        if (!text)
            return false;
        CleanupStack::PushL(text);
        if (foundIndex != KErrNotFound && propertyToSet) {
            CMdETextProperty *textProperty = NULL;
            textProperty = (CMdETextProperty*) propertyToSet;
            textProperty->SetValueL(*text);
        } else
            item->AddTextPropertyL(*propDef, *text);
        CleanupStack::PopAndDestroy(text);
        )
    return err == KErrNone;     
}
#endif // MDS_25_COMPILATION_ENABLED

int QDocumentGalleryMDSUtility::InsertUInt32PropertyCondition( CMdELogicCondition &rootCond,
    CMdEPropertyDef *propDef,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEInt64Equal(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::LessThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEInt64Less(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::GreaterThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEInt64Greater(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::LessThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEInt64LessEqual(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::GreaterThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEInt64GreaterEqual(valueToMatch.toULongLong())));
        break;
    default:
        return QDocumentGallery::FilterError;
    }
    if (err) {
        return QDocumentGallery::FilterError;
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::InsertTTImePropertyCondition( CMdELogicCondition &rootCond,
    CMdEPropertyDef *propDef,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    TTime time( QDateTimetosymbianTTime( valueToMatch.toDateTime() ) );
    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdETimeEqual(time)));
        break;
    case QGalleryFilter::LessThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdETimeLess(time)));
        break;
    case QGalleryFilter::GreaterThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdETimeGreater(time)));
        break;
    case QGalleryFilter::LessThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdETimeLessEqual(time)));
        break;
    case QGalleryFilter::GreaterThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdETimeGreaterEqual(time)));
        break;
    default:
        return QDocumentGallery::FilterError;
    }
    if (err) {
        return QDocumentGallery::FilterError;
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::InsertStringPropertyCondition( CMdELogicCondition &rootCond,
    CMdEPropertyDef *propDef,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    int returnValue = QDocumentGallery::NoError;

    HBufC *text = qStringToS60Desc(valueToMatch.toString());
    if (!text)
        return QDocumentGallery::NoError; 

    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareEquals,
                *text) );
        if ( err != KErrNone ) {
            delete text;
            text = NULL;
        }
        break;
    case QGalleryFilter::Contains:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareContains,
                *text) );
        if ( err != KErrNone ) {
            delete text;
            text = NULL;
        }
        break;
    case QGalleryFilter::StartsWith:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareBeginsWith,
                *text));
        if ( err != KErrNone ) {
            delete text;
            text = NULL;
        }
        break;
    case QGalleryFilter::EndsWith:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareEndsWith,
                *text));
        if ( err != KErrNone ) {
            delete text;
            text = NULL;
        }
        break;
    default:
        returnValue = QDocumentGallery::FilterError;
    }

    if (err)
        returnValue = QDocumentGallery::FilterError;
    
    return returnValue;
}

int QDocumentGalleryMDSUtility::InsertUriPropertyCondition( CMdELogicCondition &rootCond,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err = 0;
    int returnValue = QDocumentGallery::NoError;
    if (rootCond.Locked())
        return QDocumentGallery::FilterError;

    HBufC *buffer = qStringToS60Desc(QDir::toNativeSeparators(valueToMatch.toString()));
    if (!buffer)
        return QDocumentGallery::FilterError;

    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddObjectConditionL(EObjectConditionCompareUri, *buffer));
        if ( err != KErrNone ) {
            delete buffer;
            buffer = NULL;
        }
        break;
    case QGalleryFilter::StartsWith:
        TRAP(err, rootCond.AddObjectConditionL(EObjectConditionCompareUriBeginsWith, *buffer));
        if ( err != KErrNone ) {
            delete buffer;
            buffer = NULL;
        }
        break;
    default:
        returnValue = QDocumentGallery::FilterError;
    }

    if (err)
        returnValue = QDocumentGallery::FilterError;
    return returnValue;
}

int QDocumentGalleryMDSUtility::InsertUIntPropertyCondition( CMdELogicCondition &rootCond,
    CMdEPropertyDef *propDef,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEUintEqual(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::LessThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEUintLess(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::GreaterThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEUintGreater(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::LessThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEUintLessEqual(valueToMatch.toULongLong())));
        break;
    case QGalleryFilter::GreaterThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdEUintGreaterEqual(valueToMatch.toULongLong())));
        break;
    default:
        return QDocumentGallery::FilterError;
    }
    if (err) {
        return QDocumentGallery::FilterError;
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::InsertReal32PropertyCondition( CMdELogicCondition &rootCond,
    CMdEPropertyDef *propDef,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdERealEqual(valueToMatch.toDouble())));
        break;
    case QGalleryFilter::LessThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdERealLess(valueToMatch.toDouble())));
        break;
    case QGalleryFilter::GreaterThan:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdERealGreater(valueToMatch.toDouble())));
        break;
    case QGalleryFilter::LessThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdERealLessEqual(valueToMatch.toDouble())));
        break;
    case QGalleryFilter::GreaterThanEquals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef, TMdERealGreaterEqual(valueToMatch.toDouble())));
        break;
    default:
        return QDocumentGallery::FilterError;
    }
    if (err) {
        return QDocumentGallery::FilterError;
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::AddFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        return QDocumentGallery::NoError;
    case QGalleryFilter::Intersection:
        {
            CMdELogicCondition *rootNode = NULL;
            TRAPD( err, rootNode = &rootCond.AddLogicConditionL( ELogicConditionOperatorAnd ) );
            if (err) {
                return QDocumentGallery::FilterError;
            }

            int conditionError = QDocumentGallery::NoError;
            conditionError = AddIntersectionFilter( *rootNode, filter, defaultNameSpace );
            if (conditionError != QDocumentGallery::NoError) {
                return conditionError;
            }
            break;
        }
    case QGalleryFilter::Union:
        {
            CMdELogicCondition *rootNode = NULL;
            TRAPD( err, rootNode = &rootCond.AddLogicConditionL( ELogicConditionOperatorOr ) );
            if (err) {
                return QDocumentGallery::FilterError;
            }

            int conditionError = QDocumentGallery::NoError;
            conditionError = AddUnionFilter( *rootNode, filter, defaultNameSpace );
            if (conditionError != QDocumentGallery::NoError) {
                return conditionError;
            }
            break;
        }
    case QGalleryFilter::MetaData:
        return AddMetadataFilter(rootCond, filter, defaultNameSpace);
    default:
        return QDocumentGallery::FilterError;
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::AddIntersectionFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    QGalleryIntersectionFilter intersectionFilter = filter.toIntersectionFilter();
    QList<QGalleryFilter> filters = intersectionFilter.filters();
    int filterCount = filters.count();
    int conditionError = QDocumentGallery::NoError;
    for (int i = 0; i < filterCount; i++) {
        conditionError = AddFilter( rootCond, filters[i], defaultNameSpace );
        if (conditionError != QDocumentGallery::NoError) {
            return conditionError;
        }
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::AddUnionFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    QGalleryUnionFilter unionFilter = filter.toUnionFilter();
    QList<QGalleryFilter> filters = unionFilter.filters();
    int filterCount = filters.count();
    int conditionError = QDocumentGallery::NoError;
    for (int i = 0; i < filterCount; i++) {
        conditionError = AddFilter( rootCond, filters[i], defaultNameSpace );
        if (conditionError != QDocumentGallery::NoError) {
            return conditionError;
        }
    }
    return QDocumentGallery::NoError;
}

int QDocumentGalleryMDSUtility::AddMetadataFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    QGalleryMetaDataFilter metaDataFilter = filter.toMetaDataFilter();
    QString propertyToMatch = metaDataFilter.propertyName();

    bool uriComparison = false;
    if (propertyToMatch == QDocumentGallery::url.name() ||
        propertyToMatch == QDocumentGallery::filePath.name() ) {
        uriComparison = true;
    }

    CMdEPropertyDef *propDef = NULL;
    if (!uriComparison) {
        TRAPD( err, propDef = GetMDSPropertyDefL( propertyToMatch, defaultNameSpace ) );
        if (err || !propDef) {
            return QDocumentGallery::FilterError;
        }
    }

    QVariant valueToMatch = metaDataFilter.value();
    int conditionError = QDocumentGallery::NoError;
    if (uriComparison) {
        conditionError = InsertUriPropertyCondition( rootCond, valueToMatch, metaDataFilter );
    } else {
        TPropertyType propertyType = propDef->PropertyType();
        switch (propertyType) {
        case EPropertyUint32:
        case EPropertyInt64:
            conditionError = InsertUInt32PropertyCondition(rootCond, propDef, valueToMatch, metaDataFilter);
            break;
        case EPropertyTime:
            conditionError = InsertTTImePropertyCondition(rootCond, propDef, valueToMatch, metaDataFilter);
            break;
        case EPropertyText:
            conditionError = InsertStringPropertyCondition(rootCond, propDef, valueToMatch, metaDataFilter);
            break;
        case EPropertyBool:
        case EPropertyUint8:
        case EPropertyUint16:
            conditionError = InsertUIntPropertyCondition(rootCond, propDef, valueToMatch, metaDataFilter);
            break;
        case EPropertyReal32:
        case EPropertyReal64:
            conditionError = InsertReal32PropertyCondition(rootCond, propDef, valueToMatch, metaDataFilter);
            break;
        default:
            return QDocumentGallery::FilterError;
        }
    }
    return conditionError;
}

QTM_END_NAMESPACE
