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
#include "cnttransformgender.h"
#include "cntmodelextuids.h"

QList<CContactItemField *> CntTransformGender::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to gender
	const QContactGender &gender(static_cast<const QContactGender&>(detail));

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(gender.gender().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGender);
 	newField->TextStorage()->SetTextL(fieldText);
	newField->SetMapping(KUidContactFieldVCardMapUnknown);

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformGender::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactGender *gender = new QContactGender();

	CContactTextField* storage = field.TextStorage();
	QString genderInfo = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	gender->setGender(genderInfo);
	return gender;
}

bool CntTransformGender::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldGender.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformGender::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactGender::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformGender::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if( detailFieldName == QContactGender::FieldGender )
        uids << KUidContactFieldGender;
    return uids;
}

/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformGender::supportsSubType(const QString& subType) const
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
quint32 CntTransformGender::getIdForField(const QString& fieldName) const
{
if (QContactGender::FieldGender == fieldName)
        return KUidContactFieldGender.iUid;
    else if (QContactGender::GenderMale == fieldName)
        return 0;
    else if (QContactGender::GenderFemale == fieldName)
        return 0;
    else if (QContactGender::GenderUnspecified == fieldName)
        return 0;
    else 
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformGender::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;

    d.setName(QContactGender::DefinitionName);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList() << QString(QLatin1String(QContactGender::GenderMale)) << QString(QLatin1String(QContactGender::GenderFemale)) << QString(QLatin1String(QContactGender::GenderUnspecified));
    fields.insert(QContactGender::FieldGender, f);

    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);

    definitions.insert(d.name(), d);
}
