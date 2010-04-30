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
#include "cnttransformanniversary.h"

const char separator = ',';

QList<CContactItemField *> CntTransformAnniversary::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactAnniversary::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to anniversary
	const QContactAnniversary &anniversary(static_cast<const QContactAnniversary&>(detail));

	//create new field
	QString formattedAnniversary;
	if (anniversary.originalDate().isValid()) {
        formattedAnniversary = anniversary.originalDate().toString(Qt::ISODate);
	}
	if (formattedAnniversary.length() > 0) {
        formattedAnniversary.append(separator);
	}
	formattedAnniversary.append(anniversary.event());

	TPtrC fieldText(reinterpret_cast<const TUint16*>(formattedAnniversary.utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldAnniversary);
 	newField->TextStorage()->SetTextL(fieldText);
	newField->SetMapping(KUidContactFieldVCardMapAnniversary);

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformAnniversary::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactAnniversary *anniversary = new QContactAnniversary();

	CContactTextField* storage = field.TextStorage();
	QString unformattedAnniversary = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	int separatorPos = unformattedAnniversary.indexOf(separator);
	bool dateFound = false;
	if (separatorPos != -1) {
        // date is probably included
        QDate date = QDate::fromString(unformattedAnniversary.left(separatorPos), Qt::ISODate);
        if (date.isValid()) {
            anniversary->setOriginalDate(date);
            dateFound = true;
        }
	}

	if (dateFound) {
        if (unformattedAnniversary.length()-separatorPos-1 > 0) {
            anniversary->setEvent(unformattedAnniversary.right(unformattedAnniversary.length()-separatorPos-1));
        }
	}
	else {
        anniversary->setEvent(unformattedAnniversary);
	}
	return anniversary;
}

bool CntTransformAnniversary::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldAnniversary.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformAnniversary::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAnniversary::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformAnniversary::supportedSortingFieldTypes(QString /*detailFieldName*/) const
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
bool CntTransformAnniversary::supportsSubType(const QString& subType) const
{
    if(QContactAnniversary::FieldSubType  == subType)
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
quint32 CntTransformAnniversary::getIdForField(const QString& fieldName) const
{
    if (QContactAnniversary::FieldCalendarId  == fieldName)
        return 0;
    else if (QContactAnniversary::FieldOriginalDate == fieldName)
        return 0;
    else if (QContactAnniversary::FieldEvent == fieldName)
        return 0;
    else if (QContactAnniversary::SubTypeWedding == fieldName)
        return 0;
    else if (QContactAnniversary::SubTypeEngagement == fieldName)
        return 0;
    else if (QContactAnniversary::SubTypeHouse == fieldName)
        return 0;
    else if (QContactAnniversary::SubTypeEmployment == fieldName)
        return 0;
    else if (QContactAnniversary::SubTypeMemorial == fieldName)
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
void CntTransformAnniversary::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactAnniversary::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactAnniversary::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();

        // FieldCalendarId not supported in symbian back-end, remove
        fields.remove(QContactAnniversary::FieldCalendarId);

        // Sub-types not supported in symbian back-end, remove
        fields.remove(QContactAnniversary::FieldSubType);

        // Context not supported in symbian back-end, remove
        fields.remove(QContactAnniversary::FieldContext);

        d.setFields(fields);
        d.setUnique(true);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
