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
#include "cnttransformemail.h"

QList<CContactItemField *> CntTransformEmail::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to email
	const QContactEmailAddress &email(static_cast<const QContactEmailAddress&>(detail));

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(email.emailAddress().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldEMail);
 	newField->TextStorage()->SetTextL(fieldText);
	newField->SetMapping(KUidContactFieldVCardMapEMAILINTERNET);

	//contexts
	setContextsL(email, *newField);

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformEmail::transformItemField(const CContactItemField& field, const QContact &contact)
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

bool CntTransformEmail::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldEMail.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformEmail::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactEmailAddress::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformEmail::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if( detailFieldName == QContactEmailAddress::FieldEmailAddress )
        uids << KUidContactFieldEMail;
    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformEmail::supportsSubType(const QString& subType) const
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
quint32 CntTransformEmail::getIdForField(const QString& fieldName) const
{
    if (QContactEmailAddress::FieldEmailAddress == fieldName)
        return KUidContactFieldEMail.iUid;
    else
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformEmail::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinitionField> fields;
    QContactDetailDefinitionField f;
    QContactDetailDefinition d;

    d.setName(QContactEmailAddress::DefinitionName);
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactEmailAddress::FieldEmailAddress, f);

    // Contexts
    f.setDataType(QVariant::StringList);
    f.setAllowableValues(QVariantList()
            << QString(QLatin1String(QContactDetail::ContextHome))
            << QString(QLatin1String(QContactDetail::ContextWork))
            << QString(QLatin1String(QContactDetail::ContextOther)));
    fields.insert(QContactDetail::FieldContext, f);

    d.setFields(fields);
    d.setUnique(false);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);

    definitions.insert(d.name(), d);
}
