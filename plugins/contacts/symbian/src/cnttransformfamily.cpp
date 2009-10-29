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
#include "cnttransformfamily.h"

QList<CContactItemField *> CntTransformFamily::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to family
	const QContactFamily &family(static_cast<const QContactFamily&>(detail));

    //spouse
    TPtrC fieldSpouse(reinterpret_cast<const TUint16*>(family.spouse().utf16()));
    CContactItemField* spouse = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldSpouse);
    spouse->TextStorage()->SetTextL(fieldSpouse);
    spouse->SetMapping(KUidContactFieldVCardMapSpouse);
    fieldList.append(spouse);
    CleanupStack::Pop(spouse);

    //children
    foreach(QString childName, family.children()) {
        TPtrC fieldChild(reinterpret_cast<const TUint16*>(childName.utf16()));
        CContactItemField* child = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldChildren);
        child->TextStorage()->SetTextL(fieldChild);
        child->SetMapping(KUidContactFieldVCardMapChildren);
        fieldList.append(child);
        CleanupStack::Pop(child);
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

bool CntTransformFamily::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldSpouse.iUid ||
        fieldType == KUidContactFieldChildren.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformFamily::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactFamily::DefinitionName) {
        ret = true;
    }
    return ret;
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
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformFamily::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;

    d.setName(QContactFamily::DefinitionName);
    f.dataType = QVariant::String;
    f.allowableValues = QVariantList();
    fields.insert(QContactFamily::FieldSpouse, f);
    fields.insert(QContactFamily::FieldChildren, f);

    d.setFields(fields);
    d.setUnique(true);

    definitions.insert(d.name(), d);
}
