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
#include "cnttransformphonenumber.h"

QList<CContactItemField *> CntTransformPhoneNumber::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to phonenumber
	const QContactPhoneNumber &phoneNumber(static_cast<const QContactPhoneNumber&>(detail));

	//get the subType
	QStringList subTypes = phoneNumber.subTypes();

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(phoneNumber.number().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText);
	newField->TextStorage()->SetTextL(fieldText);

	//no subtype
	if(!subTypes.count())
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->SetMapping(KUidContactFieldVCardMapTEL);
	}

	//landline
	else if (subTypes.contains(QContactPhoneNumber::SubTypeLandline))
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->SetMapping(KUidContactFieldVCardMapTEL);
		newField->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
	}

	//mobile
	else if	(subTypes.contains(QContactPhoneNumber::SubTypeMobile))
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->SetMapping(KUidContactFieldVCardMapTEL);
		newField->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	}

	//fax
	else if	(subTypes.contains(QContactPhoneNumber::SubTypeFacsimile))
	{
		newField->AddFieldTypeL(KUidContactFieldFax);
		newField->SetMapping(KUidContactFieldVCardMapTEL);
		newField->AddFieldTypeL(KUidContactFieldVCardMapFAX);
	}

	//pager
	else if (subTypes.contains(QContactPhoneNumber::SubTypePager))
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->SetMapping(KUidContactFieldVCardMapTEL);
		newField->AddFieldTypeL(KUidContactFieldVCardMapPAGER);
	}

	//bbs
	else if (subTypes.contains(QContactPhoneNumber::SubTypeBulletinBoardSystem))
	{
        newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
	    newField->SetMapping(KUidContactFieldVCardMapTEL);
	    newField->AddFieldTypeL(KUidContactFieldVCardMapBBS);
	}

	//car
	else if (subTypes.contains(QContactPhoneNumber::SubTypeCar))
	{
        newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
        newField->SetMapping(KUidContactFieldVCardMapTEL);
	    newField->AddFieldTypeL(KUidContactFieldVCardMapCAR);
	}

	//DTMF
	else if (subTypes.contains(QContactPhoneNumber::SubTypeDtmfMenu))
	{
        newField->AddFieldTypeL(KUidContactFieldDTMF);
        newField->SetMapping(KUidContactFieldVCardMapUnknown);
	}

	// assistant number
    else if (subTypes.contains(QContactPhoneNumber::SubTypeAssistant))
    {
        newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
        newField->SetMapping(KUidContactFieldVCardMapAssistantTel);
    }

	else
	{
        User::LeaveIfError(KErrNotSupported);
	}

	//contexts
	setContextsL(phoneNumber, *newField);

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformPhoneNumber::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactPhoneNumber *phoneNumber = new QContactPhoneNumber();

	CContactTextField* storage = field.TextStorage();
	QString number = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	phoneNumber->setNumber(number);

	if (field.ContentType().ContainsFieldType(KUidContactFieldPhoneNumber)) {
        if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapVOICE)) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeLandline);
        }
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapCELL)) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeMobile);
        }
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapPAGER)) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypePager);
        }
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapBBS)) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeBulletinBoardSystem);
        }
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapCAR)) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeCar);
        }
        else if (field.ContentType().Mapping() == KUidContactFieldVCardMapAssistantTel) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeAssistant);
        }
	}
    else if (field.ContentType().ContainsFieldType(KUidContactFieldFax)) {
        phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
    }
    else if (field.ContentType().ContainsFieldType(KUidContactFieldDTMF)) {
        phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeDtmfMenu);
    }

	// set context
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        setContexts(field.ContentType().FieldType(i), *phoneNumber);
	}

	return phoneNumber;
}

bool CntTransformPhoneNumber::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldPhoneNumber.iUid ||
        fieldType == KUidContactFieldFax.iUid ||
        fieldType == KUidContactFieldDTMF.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformPhoneNumber::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactPhoneNumber::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformPhoneNumber::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactPhoneNumber::FieldNumber) {
        uids << KUidContactFieldPhoneNumber;
        uids << KUidContactFieldFax;
        uids << KUidContactFieldDTMF;
    }
    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformPhoneNumber::supportsSubType(const QString& subType) const
{
    if(QContactPhoneNumber::FieldSubTypes  == subType)
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
quint32 CntTransformPhoneNumber::getIdForField(const QString& fieldName) const
{
    if (QContactPhoneNumber::FieldNumber  == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeLandline == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeMobile == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeFacsimile == fieldName)
        return KUidContactFieldFax.iUid;
    else if (QContactPhoneNumber::SubTypePager == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeVoice == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeModem == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeVideo == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeCar == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeBulletinBoardSystem == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeMessagingCapable == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeAssistant == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeDtmfMenu == fieldName)
        return KUidContactFieldDTMF.iUid;
    else 
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformPhoneNumber::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;
    QVariantList subTypes;

    d.setName(QContactPhoneNumber::DefinitionName);
    fields.clear();
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactPhoneNumber::FieldNumber, f);

    // Sub-types
    f.dataType = QVariant::StringList; // can implement multiple subtypes
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeAssistant));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeCar));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeDtmfMenu));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeFacsimile));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeLandline));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypeMobile));
    subTypes << QString(QLatin1String(QContactPhoneNumber::SubTypePager));
    f.allowableValues = subTypes;
    fields.insert(QContactPhoneNumber::FieldSubTypes, f);

    // Contexts
    /* TODO: does not work for some reason:
tst_QContactManager::add(mgr='symbian') A contact had extra detail: "PhoneNumber" QMap(("Context", QVariant(QStringList, ("Assistant") ) ) ( "PhoneNumber" ,  QVariant(QString, "PhoneNumber") ) ( "SubTypes" ,  QVariant(QStringList, ("Assistant") ) ) )
tst_QContactManager::add(mgr='symbian') B contact had extra detail: "PhoneNumber" QMap(("PhoneNumber", QVariant(QString, "PhoneNumber") ) ( "SubTypes" ,  QVariant(QStringList, ("Assistant") ) ) )

    f.dataType = QVariant::StringList;
    f.allowableValues << QString(QLatin1String(QContactDetail::ContextHome)) << QString(QLatin1String(QContactDetail::ContextWork)) << QString(QLatin1String(QContactDetail::ContextOther));
    fields.insert(QContactDetail::FieldContext, f);
    */

    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);

    definitions.insert(d.name(), d);
}
