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

CMdEObjectDef& QDocumentGalleryMDSUtility::ObjDefFromItemTypeL( CMdENamespaceDef &nameSpace, QString itemType )
{
    if (itemType == QDocumentGallery::Audio.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::Audio::KAudioObject );
    } else if (itemType == QDocumentGallery::File.name()) {
        return nameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
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

void QDocumentGalleryMDSUtility::GetDataFieldsForItemType( QStringList &propertyList, QString itemType )
{
    if (itemType == QDocumentGallery::Album.name()) {
        // From MDS base object
        propertyList.append( QDocumentGallery::url.name() );
        propertyList.append( QDocumentGallery::fileSize.name() );
        propertyList.append( QDocumentGallery::lastModified.name() );
        propertyList.append( QDocumentGallery::title.name() );
        propertyList.append( QDocumentGallery::mimeType.name() );
    } else if (itemType == QDocumentGallery::Audio.name()) {
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
    } else if (itemType == QDocumentGallery::File.name()) {
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
    } else if (itemType == QDocumentGallery::Image.name()) {
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
    } else if (itemType == QDocumentGallery::Video.name()) {
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

void QDocumentGalleryMDSUtility::GetMetaDataFieldL( CMdEObject *inputItem, QVariant &output, int key )
{
    output.clear();
    switch (key) {
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
            if ( sizeProp ) {
                unsigned long int value = sizeProp->Uint32ValueL();
                output.setValue( value );
            }
            break;
        }
    case ELastModified:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            CMdEProperty* modProp = NULL;
            inputItem->Property( propDef, modProp );
            if ( modProp ) {
                output.setValue(symbianTTimetoQDateTime(modProp->TimeValueL()));
            }
            break;
        }
    case ETitle:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            CMdEProperty* titleProp = NULL;
            inputItem->Property( propDef, titleProp );
            if ( titleProp ) {
                output.setValue( s60DescToQString( titleProp->TextValueL() ) );
            }
            break;
        }
    case EMime:
        {
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Object::KItemTypeProperty );
            CMdEProperty* mimeProp = NULL;
            inputItem->Property( propDef, mimeProp );
            if ( mimeProp ) {
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
            inputItem->Property( propDef, authorProp );
            if ( authorProp ) {
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
            inputItem->Property( propDef, copyProp );
            if ( copyProp ) {
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
            inputItem->Property( propDef, desProp );
            if ( desProp ) {
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
            inputItem->Property( propDef, comProp );
            if ( comProp ) {
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
            inputItem->Property( propDef, rateProp );
            if ( rateProp ) {
                unsigned int value = rateProp->Uint8ValueL();
                output.setValue( value );
            }
            break;
        }
    case EDuration:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            CMdEProperty* duraProp = NULL;
            inputItem->Property( propDef, duraProp );
            if ( duraProp ) {
                double value = duraProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
        case EPerformer:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            CMdEProperty* artistProp = NULL;
            inputItem->Property( propDef, artistProp );
            if ( artistProp ) {
                output.setValue( s60DescToQString( artistProp->TextValueL() ) );
            }
            break;
        }
#ifdef MDS_25_COMPILATION_ENABLED            
    case EAudioCodec:
        {
            if (inputItem->Def().Name() != MdeConstants::Audio::KAudioObject
                    && inputItem->Def().Name() != MdeConstants::Video::KVideoObject
                    && inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
            CMdEProperty* codecProp = NULL;
            inputItem->Property( propDef, codecProp );
            if ( codecProp ) {
                unsigned long int value = codecProp->Uint32ValueL();
                output.setValue( value );
            }
            break;
        }
#endif //MDS_25_COMPILATION_ENABLED
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
            inputItem->Property( propDef, brateProp );
            if ( brateProp ) {
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
            inputItem->Property( propDef, countProp );
            if ( countProp ) {
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
            inputItem->Property( propDef, samplingProp );
            if ( samplingProp ) {
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
            inputItem->Property( propDef, widthProp );
            if ( widthProp ) {
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
            inputItem->Property( propDef, heightProp );
            if ( heightProp ) {
                unsigned int value = heightProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EOrientation:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            CMdEProperty* orientationProp = NULL;
            inputItem->Property( propDef, orientationProp );
            if ( orientationProp ) {
                unsigned int value = orientationProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EDateTaken:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            CMdEProperty* dateProp = NULL;
            inputItem->Property( propDef, dateProp );
            if ( dateProp ) {
                output.setValue(symbianTTimetoQDateTime(dateProp->TimeValueL()));
            }
            break;
        }
    case ECameraManufacturer:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            CMdEProperty* makeProp = NULL;
            inputItem->Property( propDef, makeProp );
            if ( makeProp ) {
                output.setValue( s60DescToQString( makeProp->TextValueL() ) );
            }
            break;
        }
    case ECameraModel:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            CMdEProperty* modelProp = NULL;
            inputItem->Property( propDef, modelProp );
            if ( modelProp ) {
                output.setValue( s60DescToQString( modelProp->TextValueL() ) );
            }
            break;
        }
    case EExposureProgram:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            CMdEProperty* exppProp = NULL;
            inputItem->Property( propDef, exppProp );
            if ( exppProp ) {
                unsigned int value = exppProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EExposureTime:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            CMdEProperty* exptProp = NULL;
            inputItem->Property( propDef, exptProp );
            if ( exptProp ) {
                double value = exptProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EFNumber:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            CMdEProperty* fnumProp = NULL;
            inputItem->Property( propDef, fnumProp );
            if ( fnumProp ) {
                double value = fnumProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EFlashEnabled:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            CMdEProperty* flashProp = NULL;
            inputItem->Property( propDef, flashProp );
            if ( flashProp ) {
                unsigned int value = flashProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EFocalLength:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            CMdEProperty* focalProp = NULL;
            inputItem->Property( propDef, focalProp );
            if ( focalProp ) {
                unsigned int value = focalProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EMeteringMode:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            CMdEProperty* metProp = NULL;
            inputItem->Property( propDef, metProp );
            if ( metProp ) {
                unsigned int value = metProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case EWhiteBalance:
        {
            if (inputItem->Def().Name() != MdeConstants::Image::KImageObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            CMdEProperty* whiteProp = NULL;
            inputItem->Property( propDef, whiteProp );
            if ( whiteProp ) {
                unsigned int value = whiteProp->Uint16ValueL();
                output.setValue( value );
            }
            break;
        }
    case ELanguage:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KAudioLanguageProperty );
            CMdEProperty* langProp = NULL;
            inputItem->Property( propDef, langProp );
            if ( langProp ) {
                output.setValue( s60DescToQString( langProp->TextValueL() ) );
            }
            break;
        }
    case EFrameRate:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            CMdEProperty* framerProp = NULL;
            inputItem->Property( propDef, framerProp );
            if ( framerProp ) {
                double value = framerProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
    case EResumePosition:
        {
            if (inputItem->Def().Name() != MdeConstants::Video::KVideoObject) {
                break;
            }
            CMdEPropertyDef& propDef = inputItem->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            CMdEProperty* posProp = NULL;
            inputItem->Property( propDef, posProp );
            if ( posProp ) {
                double value = posProp->Real32ValueL();
                output.setValue( value );
            }
            break;
        }
        default:
        break;
    }
}

QString QDocumentGalleryMDSUtility::GetItemTypeFromMDEObject( CMdEObject *inputItem )
{
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
    }

    QString null;
    return null;
}

int QDocumentGalleryMDSUtility::GetPropertyKey( const QString &property )
{
    if (property == QDocumentGallery::url.name()) {
        return EUri;
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
    //TODO: remove compile warnings
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
    if (error == KErrNone) {
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
    if (error != KErrNone) {
        return NULL;
    }
    return s60str;
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

CMdEPropertyDef *QDocumentGalleryMDSUtility::GetMDSPropertyDefL( const QString &property,
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
#ifdef MDS_25_COMPILATION_ENABLED
    } else if (property == QDocumentGallery::audioCodec.name()) {
        CMdEObjectDef& def = defaultNameSpace.GetObjectDefL( MdeConstants::MediaObject::KMediaObject );
        CMdEPropertyDef& propDef = def.GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
        return &propDef;
#endif //MDS_25_COMPILATION_ENABLED
    } else if (property == QDocumentGallery::playCount.name()) {
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

int QDocumentGalleryMDSUtility::SetupQueryConditions(CMdEObjectQuery *query,
    QGalleryQueryRequest *request,
    CMdENamespaceDef& defaultNameSpace)
{
    // Add filtering conditions
    int conditionError = QGalleryAbstractRequest::NoResult;
    CMdELogicCondition &rootCond = query->Conditions();

    // Add filtering conditions
    QGalleryFilter filter = request->filter();

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
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }

    if (conditionError != QGalleryAbstractRequest::NoResult) {
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

        if (ascending) {
            TRAP_IGNORE( query->AppendOrderRuleL( TMdEOrderRule( *propDef, ESortAscending ) ) );
        } else {
            TRAP_IGNORE( query->AppendOrderRuleL( TMdEOrderRule( *propDef, ESortDescending ) ) );
        }
    }
    return QGalleryAbstractRequest::NoResult;
}

bool QDocumentGalleryMDSUtility::SetMetaDataFieldL( CMdEObject *item, const QVariant &value, int key )
{
    switch( key )
    {
    case EUri:
        return false;
    case EFileSize:
        {
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KSizeProperty );
            CMdEProperty* sizeProp = NULL;
            item->Property( propDef, sizeProp );
            if ( sizeProp ) {
                TRAPD( err, sizeProp->SetUint32ValueL( value.toULongLong() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint32PropertyL( propDef, value.toULongLong() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ELastModified:
        {
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KLastModifiedDateProperty );
            CMdEProperty* modProp = NULL;
            item->Property( propDef, modProp );
            if ( modProp ) {
                TRAPD( err, modProp->SetTimeValueL( QDateTimetosymbianTTime( value.toDateTime() ) ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
            else {
                TRAPD( err, item->AddTimePropertyL( propDef, QDateTimetosymbianTTime( value.toDateTime() ) ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ETitle:
        {
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Object::KTitleProperty );
            CMdEProperty* titleProp = NULL;
            item->Property( propDef, titleProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( titleProp ) {
                TRAPD( err, titleProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EMime:
        return false;
    case EAuthor:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAuthorProperty );
            CMdEProperty* authorProp = NULL;
            item->Property( propDef, authorProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( authorProp ) {
                TRAPD( err, authorProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ECopyright:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject ) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KCopyrightProperty );
            CMdEProperty* copyProp = NULL;
            item->Property( propDef, copyProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( copyProp ) {
                TRAPD( err, copyProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EDescription:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KDescriptionProperty );
            CMdEProperty* desProp = NULL;
            item->Property( propDef, desProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( desProp ) {
                TRAPD( err, desProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EComments:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KCommentProperty );
            CMdEProperty* comProp = NULL;
            item->Property( propDef, comProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( comProp ) {
                TRAPD( err, comProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ERating:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KRatingProperty );
            CMdEProperty* rateProp = NULL;
            item->Property( propDef, rateProp );
            if ( rateProp ) {
                TRAPD( err, rateProp->SetUint8ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint8PropertyL( propDef, value.toUInt() ) );
                if(err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EDuration:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KDurationProperty );
            CMdEProperty* duraProp = NULL;
            item->Property( propDef, duraProp );
            if ( duraProp ) {
                TRAPD( err, duraProp->SetReal32ValueL( value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EPerformer:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KArtistProperty );
            CMdEProperty* artistProp = NULL;
            item->Property( propDef, artistProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( artistProp ) {
                TRAPD( err, artistProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
#ifdef MDS_25_COMPILATION_ENABLED            
    case EAudioCodec:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject ) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAudioFourCCProperty );
            CMdEProperty* codecProp = NULL;
            item->Property( propDef, codecProp );
            if ( codecProp ) {
                TRAPD( err, codecProp->SetUint32ValueL( value.toULongLong() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint32PropertyL( propDef, value.toULongLong() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
#endif //MDS_25_COMPILATION_ENABLED
    case EAudioBitrate:
    case EVideoBitrate:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KBitrateProperty );
            CMdEProperty* brateProp = NULL;
            item->Property( propDef, brateProp );
            if ( brateProp ) {
                TRAPD( err, brateProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EPlayCount:
        {
            if(item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KAccessCountProperty );
            CMdEProperty* countProp = NULL;
            item->Property( propDef, countProp );
            if ( countProp ) {
                TRAPD( err, countProp->SetUint32ValueL( value.toULongLong() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint32PropertyL( propDef, value.toULongLong() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ESampleRate:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Audio::KSamplingFrequencyProperty );
            CMdEProperty* samplingProp = NULL;
            item->Property( propDef, samplingProp );
            if ( samplingProp ) {
                TRAPD( err, samplingProp->SetReal32ValueL( value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EWidth:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KWidthProperty );
            CMdEProperty* widthProp = NULL;
            item->Property( propDef, widthProp );
            if ( widthProp ) {
                TRAPD( err, widthProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EHeight:
        {
            if (item->Def().Name() != MdeConstants::Audio::KAudioObject
                    && item->Def().Name() != MdeConstants::Video::KVideoObject
                    && item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::MediaObject::KHeightProperty );
            CMdEProperty* heightProp = NULL;
            item->Property( propDef, heightProp );
            if ( heightProp ) {
                TRAPD( err, heightProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EOrientation:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject ) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KOrientationProperty );
            CMdEProperty* orientationProp = NULL;
            item->Property( propDef, orientationProp );
            if ( orientationProp ) {
                TRAPD( err, orientationProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
            else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EDateTaken:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KDateTimeOriginalProperty );
            CMdEProperty* dateProp = NULL;
            item->Property( propDef, dateProp );
            if ( dateProp ) {
                TRAPD( err, dateProp->SetTimeValueL( QDateTimetosymbianTTime( value.toDateTime() ) ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTimePropertyL( propDef, QDateTimetosymbianTTime( value.toDateTime() ) ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ECameraManufacturer:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KMakeProperty );
            CMdEProperty* makeProp = NULL;
            item->Property( propDef, makeProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( makeProp ) {
                TRAPD( err, makeProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case ECameraModel:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KModelProperty );
            CMdEProperty* modelProp = NULL;
            item->Property( propDef, modelProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( modelProp ) {
                TRAPD( err, modelProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EExposureProgram:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KExposureProgramProperty );
            CMdEProperty* exppProp = NULL;
            item->Property( propDef, exppProp );
            if ( exppProp ) {
                TRAPD( err, exppProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EExposureTime:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KExposureTimeProperty );
            CMdEProperty* exptProp = NULL;
            item->Property( propDef, exptProp );
            if ( exptProp ) {
                TRAPD( err, exptProp->SetReal32ValueL( value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EFNumber:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFNumberProperty );
            CMdEProperty* fnumProp = NULL;
            item->Property( propDef, fnumProp );
            if ( fnumProp ) {
                TRAPD( err, fnumProp->SetReal32ValueL( value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EFlashEnabled:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFlashProperty );
            CMdEProperty* flashProp = NULL;
            item->Property( propDef, flashProp );
            if ( flashProp ) {
                TRAPD( err, flashProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EFocalLength:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KFocalLengthIn35mmFilmProperty );
            CMdEProperty* focalProp = NULL;
            item->Property( propDef, focalProp );
            if ( focalProp ) {
                TRAPD( err, focalProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    case EMeteringMode:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KMeteringModeProperty );
            CMdEProperty* metProp = NULL;
            item->Property( propDef, metProp );
            if ( metProp ) {
                TRAPD( err, metProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                }
                else {
                    return true;
                }
            } else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                }
                else {
                    return true;
                }
            }
        }
        case EWhiteBalance:
        {
            if (item->Def().Name() != MdeConstants::Image::KImageObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Image::KWhiteBalanceProperty );
            CMdEProperty* whiteProp = NULL;
            item->Property( propDef, whiteProp );
            if ( whiteProp ) {
                TRAPD( err, whiteProp->SetUint16ValueL( value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
            else {
                TRAPD( err, item->AddUint16PropertyL( propDef, value.toUInt() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
        case ELanguage:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KAudioLanguageProperty );
            CMdEProperty* langProp = NULL;
            item->Property( propDef, langProp );
            const TDesC text( qStringToS60Desc( value.toString() )->Des() );
            if ( langProp ) {
                TRAPD( err, langProp->SetTextValueL( text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
            else {
                TRAPD( err, item->AddTextPropertyL( propDef, text ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
        case EFrameRate:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KFramerateProperty );
            CMdEProperty* framerProp = NULL;
            item->Property( propDef, framerProp );
            if ( framerProp ) {
                TRAPD( err, framerProp->SetReal32ValueL( value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
            else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
        case EResumePosition:
        {
            if (item->Def().Name() != MdeConstants::Video::KVideoObject) {
                return false;
            }
            CMdEPropertyDef& propDef = item->Def().GetPropertyDefL( MdeConstants::Video::KLastPlayPositionProperty );
            CMdEProperty* posProp = NULL;
            item->Property( propDef, posProp );
            if ( posProp ) {
                TRAPD( err, posProp->SetReal32ValueL( value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
            else {
                TRAPD( err, item->AddReal32PropertyL( propDef, value.toDouble() ) );
                if (err) {
                    return false;
                } else {
                    return true;
                }
            }
        }
    default:
        return false;
    }
}

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
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    if (err) {
        return QGalleryAbstractRequest::PropertyTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
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
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    if (err) {
        return QGalleryAbstractRequest::PropertyTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
}

int QDocumentGalleryMDSUtility::InsertStringPropertyCondition( CMdELogicCondition &rootCond,
    CMdEPropertyDef *propDef,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    const TDesC text(qStringToS60Desc(valueToMatch.toString() )->Des());
    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareEquals,
                text) );
        break;
    case QGalleryFilter::Contains:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareContains,
                text) );
        break;
    case QGalleryFilter::StartsWith:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareBeginsWith,
                text));
        break;
    case QGalleryFilter::EndsWith:
        TRAP(err, rootCond.AddPropertyConditionL(*propDef,
                ETextPropertyConditionCompareEndsWith,
                text));
        break;
    default:
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    if (err) {
        return QGalleryAbstractRequest::PropertyTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
}

int QDocumentGalleryMDSUtility::InsertUriPropertyCondition( CMdELogicCondition &rootCond,
    QVariant &valueToMatch,
    QGalleryMetaDataFilter &filter )
{
    int err;
    const TDesC text( qStringToS60Desc( valueToMatch.toString() )->Des() );
    switch (filter.comparator()) {
    case QGalleryFilter::Equals:
        TRAP(err, rootCond.AddObjectConditionL(EObjectConditionCompareUri, text));
        break;
    case QGalleryFilter::StartsWith:
        TRAP(err, rootCond.AddObjectConditionL(EObjectConditionCompareUriBeginsWith, text));
        break;
    default:
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    if (err) {
        return QGalleryAbstractRequest::PropertyTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
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
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    if (err) {
        return QGalleryAbstractRequest::PropertyTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
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
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    if (err) {
        return QGalleryAbstractRequest::PropertyTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
}

int QDocumentGalleryMDSUtility::AddFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    switch (filter.type()) {
    case QGalleryFilter::Invalid:
        return QGalleryAbstractRequest::NoResult;
    case QGalleryFilter::Intersection:
        {
            CMdELogicCondition *rootNode = NULL;
            TRAPD( err, rootNode = &rootCond.AddLogicConditionL( ELogicConditionOperatorAnd ) );
            if (err) {
                return QGalleryAbstractRequest::RequestError;
            }

            int conditionError = QGalleryAbstractRequest::NoResult;
            conditionError = AddIntersectionFilter( *rootNode, filter, defaultNameSpace );
            if (conditionError != QGalleryAbstractRequest::NoResult) {
                return conditionError;
            }
            break;
        }
    case QGalleryFilter::Union:
        {
            CMdELogicCondition *rootNode = NULL;
            TRAPD( err, rootNode = &rootCond.AddLogicConditionL( ELogicConditionOperatorOr ) );
            if (err) {
                return QGalleryAbstractRequest::RequestError;
            }

            int conditionError = QGalleryAbstractRequest::NoResult;
            conditionError = AddUnionFilter( *rootNode, filter, defaultNameSpace );
            if (conditionError != QGalleryAbstractRequest::NoResult) {
                return conditionError;
            }
            break;
        }
    case QGalleryFilter::MetaData:
        return AddMetadataFilter(rootCond, filter, defaultNameSpace);
    default:
        return QGalleryAbstractRequest::UnsupportedFilterTypeError;
    }
    return QGalleryAbstractRequest::NoResult;
}

int QDocumentGalleryMDSUtility::AddIntersectionFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    QGalleryIntersectionFilter intersectionFilter = filter.toIntersectionFilter();
    QList<QGalleryFilter> filters = intersectionFilter.filters();
    int filterCount = filters.count();
    int conditionError = QGalleryAbstractRequest::NoResult;
    for (int i = 0; i < filterCount; i++) {
        conditionError = AddFilter( rootCond, filters[i], defaultNameSpace );
        if (conditionError != QGalleryAbstractRequest::NoResult) {
            return conditionError;
        }
    }
    return QGalleryAbstractRequest::NoResult;
}

int QDocumentGalleryMDSUtility::AddUnionFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    QGalleryUnionFilter unionFilter = filter.toUnionFilter();
    QList<QGalleryFilter> filters = unionFilter.filters();
    int filterCount = filters.count();
    int conditionError = QGalleryAbstractRequest::NoResult;
    for (int i = 0; i < filterCount; i++) {
        conditionError = AddFilter( rootCond, filters[i], defaultNameSpace );
        if (conditionError != QGalleryAbstractRequest::NoResult) {
            return conditionError;
        }
    }
    return QGalleryAbstractRequest::NoResult;
}

int QDocumentGalleryMDSUtility::AddMetadataFilter(CMdELogicCondition &rootCond,
    QGalleryFilter &filter,
    CMdENamespaceDef& defaultNameSpace)
{
    QGalleryMetaDataFilter metaDataFilter = filter.toMetaDataFilter();
    QString propertyToMatch = metaDataFilter.propertyName();

    bool uriComparison = false;
    if (propertyToMatch == QDocumentGallery::url.name()) {
        uriComparison = true;
    }

    CMdEPropertyDef *propDef = NULL;
    if (!uriComparison) {
        TRAPD( err, propDef = GetMDSPropertyDefL( propertyToMatch, defaultNameSpace ) );
        if (err || !propDef) {
            return QGalleryAbstractRequest::PropertyTypeError;
        }
    }

    QVariant valueToMatch = metaDataFilter.value();
    int conditionError = QGalleryAbstractRequest::NoResult;
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
            return QGalleryAbstractRequest::PropertyTypeError;
        }
    }
    return conditionError;
}

QTM_END_NAMESPACE
