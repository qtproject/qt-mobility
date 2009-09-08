/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "transformphonenumber.h"

QList<CContactItemField *> TransformPhoneNumber::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList; 
	
	//cast to phonenumber
	const QContactPhoneNumber &phoneNumber(static_cast<const QContactPhoneNumber&>(detail));
	
	//create new field
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText);
	TPtrC fieldText(reinterpret_cast<const TUint16*>(phoneNumber.number().utf16()));
	newField->TextStorage()->SetTextL(fieldText);

	//get the subType
	QStringList subTypes = phoneNumber.subTypes();
	
	//no subtype
	if(!subTypes.count())
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
	}
	
	//landline
	else if (subTypes.contains(QContactPhoneNumber::SubTypeLandline))
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->AddFieldTypeL(KUidContactFieldVCardMapTEL);
	}
	
	//mobile
	else if	(subTypes.contains(QContactPhoneNumber::SubTypeMobile))
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->AddFieldTypeL(KUidContactFieldVCardMapCELL);
	}
	
	//fax
	else if	(subTypes.contains(QContactPhoneNumber::SubTypeFacsimile))
	{
		newField->AddFieldTypeL(KUidContactFieldFax);
		newField->AddFieldTypeL(KUidContactFieldVCardMapTEL);
	}
	
	//pager
	else if (subTypes.contains(QContactPhoneNumber::SubTypePager))
	{
		newField->AddFieldTypeL(KUidContactFieldPhoneNumber);
		newField->AddFieldTypeL(KUidContactFieldVCardMapPAGER);
	}
	
	//contexts
	setContextsL(phoneNumber, *newField);
	
	CleanupStack::Pop(newField);
	
	fieldList.append(newField);
	
	return fieldList;
}

QContactDetail *TransformPhoneNumber::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);
	
	QContactPhoneNumber *phoneNumber = new QContactPhoneNumber();
	
	CContactTextField* storage = field.TextStorage();
	QString number = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	
	phoneNumber->setNumber(number);
	
	
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		if (field.ContentType().FieldType(i) == KUidContactFieldVCardMapTEL)
		{
			phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeLandline);
		}
		
		else if (field.ContentType().FieldType(i) == KUidContactFieldVCardMapCELL)
		{
			phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeMobile);
		}
		else if (field.ContentType().FieldType(i) == KUidContactFieldFax)
		{
			phoneNumber->setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
		}
		else if (field.ContentType().FieldType(i) == KUidContactFieldVCardMapPAGER)
		{
			phoneNumber->setSubTypes(QContactPhoneNumber::SubTypePager);
		}
		
		else
		{
			setContexts(field.ContentType().FieldType(i), *phoneNumber);
		}
			
	}
	
	return phoneNumber;
}
