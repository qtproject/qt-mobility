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
#include "cnttransformavatarsimple.h"
#include "cntthumbnailcreator.h"
#include "cntsymbiantransformerror.h"

// S60 specific contact field type containing image call object data
#define KUidContactFieldCodImageValue 0x101F8841
const TUid KUidContactFieldCodImage={KUidContactFieldCodImageValue};
// Extra field that is defined in TB10.1 platform for video ring tone
#define KUidContactFieldVideoRingToneValue  0x200100E5
const TUid KUidContactFieldVideoRingTone={KUidContactFieldVideoRingToneValue};
// The max. size of the thumbnail image that is saved into contacts database
const TSize KMaxThumbnailSize(80, 96);

CntTransformAvatarSimple::CntTransformAvatarSimple() :
    m_thumbnailCreator(0)
{
}

CntTransformAvatarSimple::~CntTransformAvatarSimple()
{
    delete m_thumbnailCreator;
}

QList<CContactItemField *> CntTransformAvatarSimple::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactAvatar::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to avatar
    const QContactAvatar &avatar(static_cast<const QContactAvatar&>(detail));

    //create new field
    TPtrC fieldText(reinterpret_cast<const TUint16*>(avatar.avatar().utf16()));

    //copy filename and replace slash with a backslash
    TFileName filename;
    for(TInt i(0); i < fieldText.Length(); ++i) {
        if(i >= filename.MaxLength())
            User::Leave(KErrTooBig);
        if(fieldText[i] == '/') {
            filename.Append('\\');
        } else {
            filename.Append(fieldText[i]);
        }
    }

	if(filename.Length()) {
        const QString& subTypeImage(QContactAvatar::SubTypeImage);
        const QString& subTypeAudioRingtone(QContactAvatar::SubTypeAudioRingtone);
	    const QString& subTypeVideoRingtone(QContactAvatar::SubTypeVideoRingtone);

        QString subType = avatar.subType();
        TUid uid(KNullUid);
        // Default to SubTypeImage
        if(subType.isEmpty() || subType.compare(subTypeImage) == 0) {
            uid = KUidContactFieldCodImage;
	    } else if (subType.compare(subTypeAudioRingtone) == 0) {
	        uid = KUidContactFieldRingTone;
	    } else if (subType.compare(subTypeVideoRingtone) == 0) {
	        uid = KUidContactFieldVideoRingTone;
	    } else {
	        User::LeaveIfError(KErrNotSupported);
	    }
	    CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, uid);

	    newField->SetMapping(KUidContactFieldVCardMapUnknown);
	    newField->TextStorage()->SetTextL(filename);

	    fieldList.append(newField);
	    CleanupStack::Pop(newField);
    }

	if(!avatar.pixmap().isNull()) {
	    // lazy instantiation
	    if(!m_thumbnailCreator) {
	        m_thumbnailCreator = new (ELeave) CntThumbnailCreator();
	    }

        // Scaling is done before converting to CFbsBitmap because
        // toSymbianCFbsBitmap may generate a duplicate of the bitmap data
        // Note: scaling to thumbnail may take some time if the image is big
        // TODO: aspect ratio?
        QPixmap scaled = avatar.pixmap().scaled(KMaxThumbnailSize.iWidth, KMaxThumbnailSize.iHeight);
        CFbsBitmap* bitmap = scaled.toSymbianCFbsBitmap();
        CleanupStack::PushL(bitmap);

        m_thumbnailCreator->addThumbnailFieldL(&fieldList, bitmap, KMaxThumbnailSize);
        CleanupStack::Pop(bitmap); // ownership transferred
    }

	return fieldList;
}

QContactDetail *CntTransformAvatarSimple::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);
	QContactAvatar *avatar = new QContactAvatar();

	if (field.ContentType().ContainsFieldType(KUidContactFieldCodImage)) {
	    CContactTextField* storage = field.TextStorage();
	    QString avatarString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	    avatar->setAvatar(avatarString);
        avatar->setSubType(QContactAvatar::SubTypeImage);
    } else if (field.ContentType().ContainsFieldType(KUidContactFieldRingTone)) {
        CContactTextField* storage = field.TextStorage();
        QString avatarString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
        avatar->setAvatar(avatarString);
        avatar->setSubType(QContactAvatar::SubTypeAudioRingtone);
	} else if (field.ContentType().ContainsFieldType(KUidContactFieldPicture)
	    || field.ContentType().ContainsFieldType(KUidContactFieldVCardMapJPEG)) {
	    // use the existing QContactAvatar (if available) in case of a pixmap
	    // field.
	    delete avatar;
	    avatar = 0;
	    avatar = new QContactAvatar(contact.detail<QContactAvatar>());
	    CContactStoreField* storage = field.StoreStorage();
        QPixmap pixmap;
        HBufC8 *theThing = storage->Thing();
        QByteArray bytes((char*)theThing->Ptr(), theThing->Length());
        bool loaded = pixmap.loadFromData(bytes, "JPG");
        if (loaded) {
            avatar->setPixmap(pixmap);
        } else {
            User::Leave(KErrInvalidContactDetail);
        }
	}
    else if (field.ContentType().ContainsFieldType(KUidContactFieldVideoRingTone)) {
        CContactTextField* storage = field.TextStorage();
        QString avatarString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
        avatar->setAvatar(avatarString);
        avatar->setSubType(QContactAvatar::SubTypeVideoRingtone);
    }

	return avatar;
}

bool CntTransformAvatarSimple::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldCodImage.iUid
        || fieldType == KUidContactFieldRingTone.iUid
        || fieldType == KUidContactFieldVideoRingTone.iUid
        || fieldType == KUidContactFieldPicture.iUid
        // Used as "extra mapping/extra field type" by thumbnail data fields
        || fieldType == KUidContactFieldVCardMapJPEG.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformAvatarSimple::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAvatar::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformAvatarSimple::supportedSortingFieldTypes(QString /*detailFieldName*/) const
{
    // Sorting not supported
    return QList<TUid>();
}


/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformAvatarSimple::supportsSubType(const QString& subType) const
{
    if(QContactAvatar::FieldSubType  == subType)
      return true;
    else
      return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformAvatarSimple::getIdForField(const QString& fieldName) const
{
   if (QContactAvatar::FieldAvatar  == fieldName)
        return 0;
    else if (QContactAvatar::SubTypeImage == fieldName)
        return 0;
    else if (QContactAvatar::SubTypeVideo == fieldName)
        return 0;
    else if (QContactAvatar::SubTypeTexturedMesh == fieldName)
        return 0;
    else if (QContactAvatar::SubTypeAudioRingtone == fieldName)
        return 0;
    else if (QContactAvatar::SubTypeVideoRingtone == fieldName)
        return 0;
    else
        return 0;
}

/*!
 * Modifies the detail definitions. The default detail definitions are
 * queried from QContactManagerEngine::schemaDefinitions and then modified
 * with this function in the transform leaf classes.
 *
 * \a definitions The detail definitions to modify.
 * \a contactType The contact type the definitions apply for.
 */
void CntTransformAvatarSimple::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactAvatar::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactAvatar::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();

        // Update sub-types
        QContactDetailFieldDefinition f;
        f.setDataType(QVariant::String); // only allowed to be a single subtype
        f.setAllowableValues(QVariantList()
                << QString(QLatin1String(QContactAvatar::SubTypeImage))
                << QString(QLatin1String(QContactAvatar::SubTypeAudioRingtone))
                << QString(QLatin1String(QContactAvatar::SubTypeVideoRingtone)));
        fields.insert(QContactAvatar::FieldSubType, f);

        // Context not supported in symbian back-end, remove
        fields.remove(QContactAvatar::FieldContext);

        d.setFields(fields);
        d.setUnique(true);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
