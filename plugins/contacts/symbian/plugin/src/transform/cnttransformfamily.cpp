/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "cnttransformfamily.h"

QList<CContactItemField *> CntTransformFamily::transformDetailL(const QContactDetail &detail)
{
    if(detail.definitionName() != QContactFamily::DefinitionName)
       User::Leave(KErrArgument);

    QList<CContactItemField *> fieldList;

    //cast to family
    const QContactFamily &family(static_cast<const QContactFamily&>(detail));

    //create new fields without contexts
    transformToTextFieldL(family, fieldList, family.spouse(), KUidContactFieldSpouse, KUidContactFieldVCardMapSpouse, false);
    foreach(const QString& childName, family.children()) {
        transformToTextFieldL(family, fieldList, childName, KUidContactFieldChildren, KUidContactFieldVCardMapChildren, false);
    }

	return fieldList;
}

QContactDetail *CntTransformFamily::transformItemField(const CContactItemField& field, const QContact &contact)
{
    QContactFamily *family = new QContactFamily(contact.detail<QContactFamily>());

	CContactTextField* storage = field.TextStorage();
	QString familyString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++) {
        //Spouse
	    if (field.ContentType().FieldType(i) == KUidContactFieldSpouse) {
            family->setSpouse(familyString);
	    }

	    //Children
	    else if (field.ContentType().FieldType(i) == KUidContactFieldChildren) {
            QStringList childrenList = family->children();
            childrenList.append(familyString);
            family->setChildren(childrenList);
	    }
	}

	return family;
}

bool CntTransformFamily::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactFamily::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformFamily::supportedFields() const
{
    return QList<TUid>()
        << KUidContactFieldSpouse
        << KUidContactFieldChildren;
}

QList<TUid> CntTransformFamily::supportedSortingFieldTypes(QString /*detailFieldName*/) const
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
bool CntTransformFamily::supportsSubType(const QString& subType) const
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
quint32 CntTransformFamily::getIdForField(const QString& fieldName) const
{
    if (QContactFamily::FieldSpouse == fieldName)
        return KUidContactFieldSpouse.iUid;
    else if (QContactFamily::FieldChildren == fieldName)
        return KUidContactFieldChildren.iUid;
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
void CntTransformFamily::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions, const QString& contactType) const
{
    Q_UNUSED(contactType);

    // Note: Family is not defined in the default schema
    QMap<QString, QContactDetailFieldDefinition> fields;
    QContactDetailFieldDefinition f;
    QContactDetailDefinition d;

    d.setName(QContactFamily::DefinitionName);
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactFamily::FieldSpouse, f);
    f.setDataType(QVariant::StringList);
    fields.insert(QContactFamily::FieldChildren, f);

    d.setFields(fields);
    d.setUnique(true);

    definitions.insert(d.name(), d);
}
