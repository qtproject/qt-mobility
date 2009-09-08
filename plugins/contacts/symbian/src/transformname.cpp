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
#include "transformname.h"

#include <QDebug>

QList<CContactItemField *> TransformName::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;
	
	//cast to name
	const QContactName &name(static_cast<const QContactName&>(detail));
	
	//Prefix
	TPtrC fieldTextPrefix(reinterpret_cast<const TUint16*>(name.prefix().utf16()));
	CContactItemField* prefix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPrefixName);
	prefix->TextStorage()->SetTextL(fieldTextPrefix);
	fieldList.append(prefix);
	CleanupStack::Pop(prefix);
	
	//First Name
	TPtrC fieldTextFirstName(reinterpret_cast<const TUint16*>(name.first().utf16()));
	CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	firstName->TextStorage()->SetTextL(fieldTextFirstName);
	fieldList.append(firstName);
	CleanupStack::Pop(firstName);

	//Middle Name
	TPtrC fieldTextMiddleName(reinterpret_cast<const TUint16*>(name.middle().utf16()));
	CContactItemField* middleName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAdditionalName);
	middleName->TextStorage()->SetTextL(fieldTextMiddleName);
	QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE(fieldList.append(middleName));
	CleanupStack::Pop(middleName);
	
	//Last Name
	TPtrC fieldTextLastName(reinterpret_cast<const TUint16*>(name.last().utf16()));
	CContactItemField* lastName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	lastName->TextStorage()->SetTextL(fieldTextLastName);
	fieldList.append(lastName);
	CleanupStack::Pop(lastName);
	
	//Suffix
	TPtrC fieldTextSuffix(reinterpret_cast<const TUint16*>(name.suffix().utf16()));
	CContactItemField* suffix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldSuffixName);
	suffix->TextStorage()->SetTextL(fieldTextSuffix);
	fieldList.append(suffix);
	CleanupStack::Pop(suffix);
	
	return fieldList;
}	


QContactDetail *TransformName::transformItemField(const CContactItemField& field, const QContact &contact)
{
	QContactName *name = new QContactName(contact.detail<QContactName>());
	
	CContactTextField* storage = field.TextStorage();
	QString nameValue = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		//Prefix
		if (field.ContentType().FieldType(i) == KUidContactFieldPrefixName)
		{
			name->setPrefix(nameValue);
		}
		
		//First name
		else if (field.ContentType().FieldType(i) == KUidContactFieldGivenName)
		{
			name->setFirst(nameValue);
		}
		
		//Middle name
		else if (field.ContentType().FieldType(i) == KUidContactFieldAdditionalName)
		{
			name->setMiddle(nameValue);
		}
		
		//Last name
		else if (field.ContentType().FieldType(i) == KUidContactFieldFamilyName)
		{
			name->setLast(nameValue);
		}
		
		//Suffix
		else if (field.ContentType().FieldType(i) == KUidContactFieldSuffixName)
		{
			name->setSuffix(nameValue);
		}
	}
	
	return name;
}
