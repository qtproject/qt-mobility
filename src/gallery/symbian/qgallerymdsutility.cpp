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
//Backend
#include "qgallerymdsutility.h"
//Symbian
#include <mdeconstants.h>
#include <mdesession.h>
#include <utf.h>
#include <e32cmn.h>
#include <e32des8.h>

QTM_BEGIN_NAMESPACE

QDocumentGalleryMDSUtility::QDocumentGalleryMDSUtility()
{

}
QDocumentGalleryMDSUtility::~QDocumentGalleryMDSUtility()
{

}

CMdEObjectDef& QDocumentGalleryMDSUtility::ObjDefFromItemType( CMdENamespaceDef &nameSpace, QString itemType )
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

void QDocumentGalleryMDSUtility::GetDataFieldsForItemType( QStringList &propertyList, QString itemType )
{
    if( itemType == QDocumentGallery::Album.name() )
        {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
        }
    else if( itemType == QDocumentGallery::Audio.name() )
        {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
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
        }
    else if( itemType == QDocumentGallery::File.name() )
        {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
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
        }
    else if( itemType == QDocumentGallery::Image.name() )
        {
        propertyList.append( QDocumentGallery::url.name() );
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
        }
    else if( itemType == QDocumentGallery::Video.name() )
        {
        propertyList.append( QDocumentGallery::url.name() );
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

void QDocumentGalleryMDSUtility::GetMetaDataField( CMdEObject *inputItem, QVariant &output, int key )
{
    switch( key )
        {
        case EUri:
            {
            QUrl url( s60DescToQString( inputItem->Uri() ) );
            output.setValue( url );
            break;
            }
        case EFileSize:
            {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KSizeProperty );
            CMdEProperty* sizeProp = NULL;
            inputItem->Property( propDef, sizeProp );
            if ( sizeProp )
                {
                unsigned long int value = sizeProp->Uint32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ELastModified:
            {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            CMdEProperty* modProp = NULL;
            inputItem->Property( propDef, modProp );
            if ( modProp )
                {
                // TODO set TTime value to QVariant DateTime value
                //TTime value = modProp->TimeValueL();
                //output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ETitle:
            {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            CMdEProperty* titleProp = NULL;
            inputItem->Property( propDef, titleProp );
            if ( titleProp )
                {
                output.setValue( s60DescToQString( titleProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EMime:
            {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
            CMdEProperty* mimeProp = NULL;
            inputItem->Property( propDef, mimeProp );
            if ( mimeProp )
                {
                output.setValue( s60DescToQString( mimeProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EAuthor:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
            CMdEProperty* authorProp = NULL;
            inputItem->Property( propDef, authorProp );
            if ( authorProp )
                {
                output.setValue( s60DescToQString( authorProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ECopyright:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
            CMdEProperty* copyProp = NULL;
            inputItem->Property( propDef, copyProp );
            if ( copyProp )
                {
                output.setValue( s60DescToQString( copyProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EDescription:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject ) {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
            CMdEProperty* desProp = NULL;
            inputItem->Property( propDef, desProp );
            if ( desProp ) {
                output.setValue( s60DescToQString( desProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EComments:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
            CMdEProperty* comProp = NULL;
            inputItem->Property( propDef, comProp );
            if ( comProp )
                {
                output.setValue( s60DescToQString( comProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ERating:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
            CMdEProperty* rateProp = NULL;
            inputItem->Property( propDef, rateProp );
            if ( rateProp )
                {
                unsigned int value = rateProp->Uint8ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EDuration:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            CMdEProperty* duraProp = NULL;
            inputItem->Property( propDef, duraProp );
            if ( duraProp )
                {
                double value = duraProp->Real32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EPerformer:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            CMdEProperty* artistProp = NULL;
            inputItem->Property( propDef, artistProp );
            if ( artistProp )
                {
                output.setValue( s60DescToQString( artistProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EAudioCodec:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
            CMdEProperty* codecProp = NULL;
            inputItem->Property( propDef, codecProp );
            if ( codecProp )
                {
                unsigned long int value = codecProp->Uint32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EAudioBitrate:
        case EVideoBitrate:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
            CMdEProperty* brateProp = NULL;
            inputItem->Property( propDef, brateProp );
            if ( brateProp )
                {
                unsigned int value = brateProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EPlayCount:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
            CMdEProperty* countProp = NULL;
            inputItem->Property( propDef, countProp );
            if ( countProp )
                {
                unsigned long int value = countProp->Uint32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ESampleRate:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
            CMdEProperty* samplingProp = NULL;
            inputItem->Property( propDef, samplingProp );
            if ( samplingProp )
                {
                double value = samplingProp->Real32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EWidth:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
            CMdEProperty* widthProp = NULL;
            inputItem->Property( propDef, widthProp );
            if ( widthProp )
                {
                unsigned int value = widthProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EHeight:
            {
            if( inputItem->Def().Name() != MdeConstants::Audio::KAudioObject &&
                inputItem->Def().Name() != MdeConstants::Video::KVideoObject &&
                inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
            CMdEProperty* heightProp = NULL;
            inputItem->Property( propDef, heightProp );
            if ( heightProp )
                {
                unsigned int value = heightProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EOrientation:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            CMdEProperty* orientationProp = NULL;
            inputItem->Property( propDef, orientationProp );
            if ( orientationProp )
                {
                unsigned int value = orientationProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EDateTaken:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            CMdEProperty* dateProp = NULL;
            inputItem->Property( propDef, dateProp );
            if ( dateProp )
                {
                // TODO set TTime value to QVariant DateTime value
                //TTime value = dateProp->TimeValueL();
                //output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ECameraManufacturer:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            CMdEProperty* makeProp = NULL;
            inputItem->Property( propDef, makeProp );
            if ( makeProp )
                {
                output.setValue( s60DescToQString( makeProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ECameraModel:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            CMdEProperty* modelProp = NULL;
            inputItem->Property( propDef, modelProp );
            if ( modelProp )
                {
                output.setValue( s60DescToQString( modelProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EExposureProgram:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            CMdEProperty* exppProp = NULL;
            inputItem->Property( propDef, exppProp );
            if ( exppProp )
                {
                unsigned int value = exppProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EExposureTime:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            CMdEProperty* exptProp = NULL;
            inputItem->Property( propDef, exptProp );
            if ( exptProp )
                {
                double value = exptProp->Real32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EFNumber:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            CMdEProperty* fnumProp = NULL;
            inputItem->Property( propDef, fnumProp );
            if ( fnumProp )
                {
                double value = fnumProp->Real32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EFlashEnabled:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            CMdEProperty* flashProp = NULL;
            inputItem->Property( propDef, flashProp );
            if ( flashProp )
                {
                unsigned int value = flashProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EFocalLength:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            CMdEProperty* focalProp = NULL;
            inputItem->Property( propDef, focalProp );
            if ( focalProp )
                {
                unsigned int value = focalProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EMeteringMode:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            CMdEProperty* metProp = NULL;
            inputItem->Property( propDef, metProp );
            if ( metProp )
                {
                unsigned int value = metProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EWhiteBalance:
            {
            if( inputItem->Def().Name() != MdeConstants::Image::KImageObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            CMdEProperty* whiteProp = NULL;
            inputItem->Property( propDef, whiteProp );
            if ( whiteProp )
                {
                unsigned int value = whiteProp->Uint16ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case ELanguage:
            {
            if( inputItem->Def().Name() != MdeConstants::Video::KVideoObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KAudioLanguageProperty );
            CMdEProperty* langProp = NULL;
            inputItem->Property( propDef, langProp );
            if ( langProp )
                {
                output.setValue( s60DescToQString( langProp->TextValueL() ) );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EFrameRate:
            {
            if( inputItem->Def().Name() != MdeConstants::Video::KVideoObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            CMdEProperty* framerProp = NULL;
            inputItem->Property( propDef, framerProp );
            if ( framerProp )
                {
                double value = framerProp->Real32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        case EResumePosition:
            {
            if( inputItem->Def().Name() != MdeConstants::Video::KVideoObject )
                {
                output.clear();
                break;
                }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            CMdEProperty* posProp = NULL;
            inputItem->Property( propDef, posProp );
            if ( posProp )
                {
                double value = posProp->Real32ValueL();
                output.setValue( value );
                }
            else
                {
                output.clear();
                }
            break;
            }
        default:
            output.clear();
            break;
        }
}

QString QDocumentGalleryMDSUtility::GetItemTypeFromMDEObject( CMdEObject *inputItem )
{
    if( inputItem->Def().Name() == MdeConstants::Album::KAlbumObject )
        {
        return QDocumentGallery::Album.name();
        }
    else if( inputItem->Def().Name() == MdeConstants::Audio::KAudioObject )
        {
        return QDocumentGallery::Audio.name();
        }
    else if( inputItem->Def().Name() == MdeConstants::MediaObject::KMediaObject )
        {
        return QDocumentGallery::File.name();
        }
    else if( inputItem->Def().Name() == MdeConstants::Image::KImageObject )
        {
        return QDocumentGallery::Image.name();
        }
    else if( inputItem->Def().Name() == MdeConstants::Video::KVideoObject )
        {
        return QDocumentGallery::Video.name();
        }

    QString null;
    return null;
}

int QDocumentGalleryMDSUtility::GetPropertyKey( const QString &property )
{
    if( property == QDocumentGallery::url.name() )
        {
        return EUri;
        }
    else if( property == QDocumentGallery::fileSize.name() )
        {
        return EFileSize;
        }
    else if( property == QDocumentGallery::lastModified.name() )
        {
        return ELastModified;
        }
    else if( property == QDocumentGallery::title.name() )
        {
        return ETitle;
        }
    else if( property == QDocumentGallery::mimeType.name() )
        {
        return EMime;
        }
    else if( property == QDocumentGallery::author.name() )
        {
        return EAuthor;
        }
    else if( property == QDocumentGallery::copyright.name() )
        {
        return ECopyright;
        }
    else if( property == QDocumentGallery::description.name() )
        {
        return EDescription;
        }
    else if( property == QDocumentGallery::comments.name() )
        {
        return EComments;
        }
    else if( property == QDocumentGallery::rating.name() )
        {
        return ERating;
        }
    else if( property == QDocumentGallery::duration.name() )
        {
        return EDuration;
        }
    else if( property == QDocumentGallery::performer.name() )
        {
        return EPerformer;
        }
    else if( property == QDocumentGallery::audioCodec.name() )
        {
        return EAudioCodec;
        }
    else if( property == QDocumentGallery::audioBitRate.name()  )
        {
        return EAudioBitrate;
        }
    else if( property == QDocumentGallery::playCount.name() )
        {
        return EPlayCount;
        }
    else if( property == QDocumentGallery::sampleRate.name() )
        {
        return ESampleRate;
        }
    else if( property == QDocumentGallery::width.name() )
        {
        return EWidth;
        }
    else if( property == QDocumentGallery::width.name() )
        {
        return EWidth;
        }
    else if( property == QDocumentGallery::height.name() )
        {
        return EHeight;
        }
    else if( property == QDocumentGallery::width.name() )
        {
        return EWidth;
        }
    else if( property == QDocumentGallery::orientation.name() )
        {
        return EOrientation;
        }
    else if( property == QDocumentGallery::dateTaken.name() )
        {
        return EDateTaken;
        }
    else if( property == QDocumentGallery::cameraManufacturer.name() )
        {
        return ECameraManufacturer;
        }
    else if( property == QDocumentGallery::cameraModel.name() )
        {
        return ECameraModel;
        }
    else if( property == QDocumentGallery::exposureProgram.name() )
        {
        return EExposureProgram;
        }
    else if( property == QDocumentGallery::exposureTime.name() )
        {
        return EExposureTime;
        }
    else if( property == QDocumentGallery::fNumber.name() )
        {
        return EFNumber;
        }
    else if( property == QDocumentGallery::flashEnabled.name() )
        {
        return EFlashEnabled;
        }
    else if( property == QDocumentGallery::focalLength.name() )
        {
        return EFocalLength;
        }
    else if( property == QDocumentGallery::meteringMode.name() )
        {
        return EMeteringMode;
        }
    else if( property == QDocumentGallery::whiteBalance.name() )
        {
        return EWhiteBalance;
        }
    else if( property == QDocumentGallery::videoBitRate.name() )
        {
        return EVideoBitrate;
        }
    else if( property == QDocumentGallery::language.name() )
        {
        return ELanguage;
        }
    else if( property == QDocumentGallery::frameRate.name() )
        {
        return EFrameRate;
        }
    else if( property == QDocumentGallery::resumePosition.name() )
        {
        return EResumePosition;
        }
    return -1;
}

QVariant::Type QDocumentGalleryMDSUtility::GetPropertyType( int key )
{
    switch( key )
        {
        case EUri:
            {
            return QVariant::Url;
            break;
            }
        case EFileSize:
        case EAudioCodec:
        case EPlayCount:
            {
            return QVariant::ULongLong;
            break;
            }
        case ELastModified:
        case EDateTaken:
            {
            return QVariant::DateTime;
            break;
            }
        case ETitle:
        case EMime:
        case EAuthor:
        case ECopyright:
        case EDescription:
        case EComments:
        case EPerformer:
        case ECameraManufacturer:
        case ECameraModel:
        case ELanguage:
            {
            return QVariant::String;
            break;
            }
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
            {
            return QVariant::UInt;
            break;
            }
        case EDuration:
        case ESampleRate:
        case EExposureTime:
        case EFNumber:
        case EFrameRate:
        case EResumePosition:
            {
            return QVariant::Double;
            break;
            }
        default:
            return QVariant::Invalid;
            break;
        }
}

/*!
    Converts a Symbian S60 descriptor (string) to a QString

    \param desc descriptor to be converted
    \return QString containing the converted string
*/
QString QDocumentGalleryMDSUtility::s60DescToQString(const TDesC& desc)
{
    return QString::fromUtf16(desc.Ptr(), desc.Length());
}

/*!
    Converts a QString to a Symbian S60 descriptor (string).

    Note: Ownership of the returned descriptor (string) is transferred to caller.

    \param string QString to be converted
    \return Pointer to a Symbian S60 descriptor on success;
            otherwise returns NULL pointer
*/
HBufC* QDocumentGalleryMDSUtility::qStringToS60Desc(const QString& string)
{
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    return str.Alloc();
}

/*!
    Converts a Symbian S60 8 bit descriptor (UTF8 string) to a QString

    \param desc 8 bit descriptor to be converted
    \return Converted QString on success; otherwise returns null QString
*/
QString QDocumentGalleryMDSUtility::s60Desc8ToQString(const TDesC8& desc)
{
    QString qtString;
    HBufC* s60str;
    TRAPD(error, s60str = CnvUtfConverter::ConvertToUnicodeFromUtf8L(desc));
    if (error == KErrNone)
    {
        qtString = QString::fromUtf16(s60str->Ptr(), s60str->Length());
        delete s60str;
    }
    return qtString;
}

/*!
    Converts a QString to a Symbian S60 8 bit descriptor (UTF8 string).

    Note: Ownership of the returned descriptor (string) is transferred to the caller

    \param string QString to be converted
    \return Pointer to a Symbian S60 descriptor containing the UTF8 string on success;
            otherwise returns NULL pointer
*/
HBufC8* QDocumentGalleryMDSUtility::qStringToS60Desc8(const QString& string)
{
    TPtrC16 str(reinterpret_cast<const TUint16*>(string.utf16()));
    HBufC8* s60str;
    TRAPD(error, s60str = CnvUtfConverter::ConvertFromUnicodeToUtf8L(str));
    if (error != KErrNone)
    {
        return NULL;
    }
    return s60str;
}

/*!
    Converts a Symbian S60 8 bit descriptor to a QByteArray

    \param desc 8 bit descriptor to be converted
    \return QByteArray on success; otherwise returns null QByteArray
*/
QByteArray QDocumentGalleryMDSUtility::s60Desc8ToQByteArray(const TDesC8& desc)
{
    return QByteArray((const char*)desc.Ptr(), desc.Length());
}

/*!
    Converts a QByteArray to a Symbian S60 8 bit descriptor.
    Note: Ownership of the returned descriptor (string) is transferred to the caller

    \param byteArray QByteArray to be converted
    \return Pointer to an 8 bit Symbian S60 descriptor string on success;
            otherwise returns NULL pointer
*/
HBufC8* QDocumentGalleryMDSUtility::qByteArrayToS60Desc8(const QByteArray& byteArray)
{
    TPtrC8 ptr8((TUint8*)(byteArray.constData()));
    return ptr8.Alloc();
}


QTM_END_NAMESPACE
