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
#include "transformemail.h"

QList<CContactItemField *> TransformEmail::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList; 
	
	//cast to phonenumber
	const QContactEmailAddress &email(static_cast<const QContactEmailAddress&>(detail));
	
	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(email.emailAddress().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
 	newField->TextStorage()->SetTextL(fieldText);
	newField->AddFieldTypeL(KUidContactFieldVCardMapEMAILINTERNET);

	//contexts
	setContextsL(email, *newField);
	
	fieldList.append(newField);
	CleanupStack::Pop(newField);
		
	return fieldList;
}

QContactDetail *TransformEmail::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);
	
	QContactEmailAddress *email = new QContactEmailAddress();
	
	CContactTextField* storage = field.TextStorage();
	QString emailAddress = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	
	email->setEmailAddress(emailAddress);
	
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		setContexts(field.ContentType().FieldType(i), *email);
	}
	
	return email;
}

bool TransformEmail::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldEMail.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformEmail::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactEmailAddress::DefinitionName) {
        ret = true;
    }
    return ret;
}
