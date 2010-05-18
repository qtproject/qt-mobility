/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
    if(detail.definitionName() != QContactGender::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

	//cast to gender
	const QContactGender &gender(static_cast<const QContactGender&>(detail));

    //create new field without contexts
    transformToTextFieldL(gender, fieldList, gender.gender(), KUidContactFieldGender, KUidContactFieldVCardMapUnknown, false);

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

bool CntTransformGender::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactGender::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformGender::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldGender;
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
 * Modifies the detail definitions. The default detail definitions are
 * queried from QContactManagerEngine::schemaDefinitions and then modified
 * with this function in the transform leaf classes.
 *
 * \a definitions The detail definitions to modify.
 * \a contactType The contact type the definitions apply for.
 */
void CntTransformGender::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    if(definitions.contains(QContactGender::DefinitionName)) {
        QContactDetailDefinition d = definitions.value(QContactGender::DefinitionName);
        QMap<QString, QContactDetailFieldDefinition> fields = d.fields();

        // Context not supported in symbian back-end, remove
        fields.remove(QContactGender::FieldContext);

        d.setFields(fields);
        d.setUnique(true);

        // Replace original definitions
        definitions.insert(d.name(), d);
    }
}
