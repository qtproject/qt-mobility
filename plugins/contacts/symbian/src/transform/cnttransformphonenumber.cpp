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
    if(detail.definitionName() != QContactPhoneNumber::DefinitionName)
        User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to phonenumber
	const QContactPhoneNumber &phoneNumber(static_cast<const QContactPhoneNumber&>(detail));

	//get the subType
	QStringList subTypes = phoneNumber.subTypes();

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(phoneNumber.number().utf16()));
	if(fieldText.Length()) {
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
	    else if (subTypes.contains(QContactPhoneNumber::SubTypeMobile))
	    {
	        newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
	        newField->SetMapping(KUidContactFieldVCardMapTEL);
	        newField->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	    }

	    //fax
	    else if (subTypes.contains(QContactPhoneNumber::SubTypeFax))
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
        
	    // video calls
        else if (subTypes.contains(QContactPhoneNumber::SubTypeVideo))
        {
            newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
            newField->SetMapping(KUidContactFieldVCardMapTEL);
            newField->AddFieldTypeL(KUidContactFieldVCardMapVIDEO);
        }
	    else
	    {
	        User::LeaveIfError(KErrNotSupported);
	    }

	    //contexts
	    setContextsL(phoneNumber, *newField);

	    fieldList.append(newField);
	    CleanupStack::Pop(newField);
	}

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
        else if (field.ContentType().ContainsFieldType(KUidContactFieldVCardMapVIDEO)) {
            phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeVideo);
        }
	}
    else if (field.ContentType().ContainsFieldType(KUidContactFieldFax)) {
        phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeFax);
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

bool CntTransformPhoneNumber::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactPhoneNumber::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformPhoneNumber::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldPhoneNumber
        << KUidContactFieldFax
        << KUidContactFieldDTMF;
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
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeMobile == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeFax == fieldName)
        return KUidContactFieldFax.iUid;
    else if (QContactPhoneNumber::SubTypePager == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeVoice == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeModem == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeVideo == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeCar == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeBulletinBoardSystem == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeMessagingCapable == fieldName)
        return 0;
    else if (QContactPhoneNumber::SubTypeAssistant == fieldName)
        return KUidContactFieldPhoneNumber.iUid;
    else if (QContactPhoneNumber::SubTypeDtmfMenu == fieldName)
        return KUidContactFieldDTMF.iUid;
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
void CntTransformPhoneNumber::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactPhoneNumber::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactPhoneNumber::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();
        
        // Don't support "ContextOther"
        QContactDetailFieldDefinition f;
        f.setDataType(QVariant::StringList);
        f.setAllowableValues(QVariantList() 
            << QLatin1String(QContactDetail::ContextHome) 
            << QLatin1String(QContactDetail::ContextWork));
        fields[QContactDetail::FieldContext] = f;
        d.setFields(fields);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
