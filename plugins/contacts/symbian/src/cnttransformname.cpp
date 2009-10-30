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
#include "cnttransformname.h"

#include <QDebug>

QList<CContactItemField *> CntTransformName::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to name
	const QContactName &name(static_cast<const QContactName&>(detail));

	//Prefix
	TPtrC fieldTextPrefix(reinterpret_cast<const TUint16*>(name.prefix().utf16()));
	CContactItemField* prefix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPrefixName);
	prefix->TextStorage()->SetTextL(fieldTextPrefix);
	prefix->SetMapping(KUidContactFieldVCardMapUnusedN);
	fieldList.append(prefix);
	CleanupStack::Pop(prefix);

	//First Name
	TPtrC fieldTextFirstName(reinterpret_cast<const TUint16*>(name.first().utf16()));
	CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
	firstName->TextStorage()->SetTextL(fieldTextFirstName);
	firstName->SetMapping(KUidContactFieldVCardMapUnusedN);
	fieldList.append(firstName);
	CleanupStack::Pop(firstName);

	//Middle Name
	TPtrC fieldTextMiddleName(reinterpret_cast<const TUint16*>(name.middle().utf16()));
	CContactItemField* middleName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAdditionalName);
	middleName->TextStorage()->SetTextL(fieldTextMiddleName);
	middleName->SetMapping(KUidContactFieldVCardMapUnusedN);
	fieldList.append(middleName);
	CleanupStack::Pop(middleName);

	//Last Name
	TPtrC fieldTextLastName(reinterpret_cast<const TUint16*>(name.last().utf16()));
	CContactItemField* lastName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
	lastName->TextStorage()->SetTextL(fieldTextLastName);
	lastName->SetMapping(KUidContactFieldVCardMapUnusedN);
	fieldList.append(lastName);
	CleanupStack::Pop(lastName);

	//Suffix
	TPtrC fieldTextSuffix(reinterpret_cast<const TUint16*>(name.suffix().utf16()));
	CContactItemField* suffix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldSuffixName);
	suffix->TextStorage()->SetTextL(fieldTextSuffix);
	suffix->SetMapping(KUidContactFieldVCardMapUnusedN);
	fieldList.append(suffix);
	CleanupStack::Pop(suffix);

	return fieldList;
}


QContactDetail *CntTransformName::transformItemField(const CContactItemField& field, const QContact &contact)
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

bool CntTransformName::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldPrefixName.iUid ||
        fieldType == KUidContactFieldGivenName.iUid ||
        fieldType == KUidContactFieldAdditionalName.iUid ||
        fieldType == KUidContactFieldFamilyName.iUid ||
        fieldType == KUidContactFieldSuffixName.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformName::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactName::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformName::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;

    if (detailFieldName == QContactName::FieldPrefix)
        return uids << KUidContactFieldPrefixName;

    if (detailFieldName == QContactName::FieldFirst)
        return uids << KUidContactFieldGivenName;

    if (detailFieldName == QContactName::FieldMiddle)
        return uids << KUidContactFieldAdditionalName;

    if (detailFieldName == QContactName::FieldLast)
        return uids << KUidContactFieldFamilyName;

    if (detailFieldName == QContactName::FieldSuffix)
        return uids << KUidContactFieldSuffixName;

    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformName::supportsSubType(const QString& subType) const
{
    Q_UNUSED(subType);
    return false;
}

/*!
 * Returns the filed id corresponding to a field
 *
 * \a fieldName The name of the supported field
 * \return fieldId for the fieldName, 0  if not supported
 */
quint32 CntTransformName::getIdForField(const QString& fieldName) const
{
    if (QContactName::FieldPrefix == fieldName)
        return KUidContactFieldPrefixName.iUid;
    else if (QContactName::FieldFirst == fieldName)
        return KUidContactFieldGivenName.iUid;
    else if (QContactName::FieldMiddle == fieldName)
        return KUidContactFieldAdditionalName.iUid;
    else if (QContactName::FieldLast == fieldName)
        return KUidContactFieldFamilyName.iUid;
    else if (QContactName::FieldSuffix == fieldName)
        return KUidContactFieldSuffixName.iUid;
    else 
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformName::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;

    // name
    d.setName(QContactName::DefinitionName);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactName::FieldPrefix, f);
    fields.insert(QContactName::FieldFirst, f);
    fields.insert(QContactName::FieldMiddle, f);
    fields.insert(QContactName::FieldLast, f);
    fields.insert(QContactName::FieldSuffix, f);
    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);

    definitions.insert(d.name(), d);
}
