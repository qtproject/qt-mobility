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
#include "cnttransformavatar.h"
#include "cntmodelextuids.h"

QList<CContactItemField *> CntTransformAvatar::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to avatar
	const QContactAvatar &avatar(static_cast<const QContactAvatar&>(detail));

	//supported subTypes
	const QString& subTypeImage(QContactAvatar::SubTypeImage);
	const QString& subTypeAudioRingtone(QContactAvatar::SubTypeAudioRingtone);
	const QString& subTypeVideoRingtone(QContactAvatar::SubTypeVideoRingtone);

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(avatar.avatar().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPicture);
	newField->TextStorage()->SetTextL(fieldText);

    //image
	if (avatar.subType().compare(subTypeImage) == 0) {
        newField->AddFieldTypeL(KUidContactFieldPicture);
	    newField->SetMapping(KUidContactFieldVCardMapUnknown);
	}

    //audio ringtone
    else if (avatar.subType().compare(subTypeAudioRingtone) == 0) {
        newField->AddFieldTypeL(KUidContactFieldRingTone);
	    newField->SetMapping(KUidContactFieldVCardMapUnknown);
	}

    //video ringtone
    else if (avatar.subType().compare(subTypeVideoRingtone) == 0) {
        newField->AddFieldTypeL(KUidContactFieldVideoRingTone);
        newField->SetMapping(KUidContactFieldVCardMapUnknown);
    }
    else {
        User::LeaveIfError(KErrNotSupported);
    }

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformAvatar::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactAvatar *avatar = new QContactAvatar();

	CContactTextField* storage = field.TextStorage();
	QString avatarString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	avatar->setAvatar(avatarString);

	if (field.ContentType().ContainsFieldType(KUidContactFieldPicture)) {
        avatar->setSubType(QContactAvatar::SubTypeImage);
    }
	else if (field.ContentType().ContainsFieldType(KUidContactFieldRingTone)) {
        avatar->setSubType(QContactAvatar::SubTypeAudioRingtone);
	}
    else if (field.ContentType().ContainsFieldType(KUidContactFieldVideoRingTone)) {
        avatar->setSubType(QContactAvatar::SubTypeVideoRingtone);
    }

	return avatar;
}

bool CntTransformAvatar::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldPicture.iUid ||
        fieldType == KUidContactFieldRingTone.iUid ||
        fieldType == KUidContactFieldVideoRingTone.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformAvatar::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAvatar::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformAvatar::supportedSortingFieldTypes(QString /*detailFieldName*/) const
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
bool CntTransformAvatar::supportsSubType(const QString& subType) const
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
quint32 CntTransformAvatar::getIdForField(const QString& fieldName) const
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
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformAvatar::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;
    QVariantList subTypes;

    // fields
    d.setName(QContactAvatar::DefinitionName);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactAvatar::FieldAvatar, f);

    // Sub-types
    f.dataType = QVariant::String; // only allowed to be a single subtype
    subTypes << QString(QLatin1String(QContactAvatar::SubTypeImage));
    subTypes << QString(QLatin1String(QContactAvatar::SubTypeAudioRingtone));
    subTypes << QString(QLatin1String(QContactAvatar::SubTypeVideoRingtone));
    f.allowableValues = subTypes;
    fields.insert(QContactUrl::FieldSubType, f);

    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);
    definitions.insert(d.name(), d);
}
