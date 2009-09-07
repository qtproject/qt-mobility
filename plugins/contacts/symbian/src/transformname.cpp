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
	
	//First Name
	CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	TPtrC fieldTextFirstName(reinterpret_cast<const TUint16*>(name.first().utf16()));
	firstName->TextStorage()->SetTextL(fieldTextFirstName);
	CleanupStack::Pop(firstName);
	fieldList.append(firstName);
	
	//Last Name
	CContactItemField* lastName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	TPtrC fieldTextLastName(reinterpret_cast<const TUint16*>(name.last().utf16()));
	lastName->TextStorage()->SetTextL(fieldTextLastName);
	CleanupStack::Pop(lastName);
	fieldList.append(lastName);

	return fieldList;
}	


QContactDetail *TransformName::transformItemFieldL(const CContactItemField& field, const QContact &contact)
{
	
	QContactName *name = new QContactName(contact.detail<QContactName>());
	
	CContactTextField* storage = field.TextStorage();
	QString nameValue = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		if (field.ContentType().FieldType(i) == KUidContactFieldGivenName)
		{
			name->setFirst(nameValue);
		}
		
		else if (field.ContentType().FieldType(i) == KUidContactFieldFamilyName)
		{
			name->setLast(nameValue);
		}
	}
	
	return name;
}
