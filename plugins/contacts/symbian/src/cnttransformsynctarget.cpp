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
#include "cnttransformsynctarget.h"
#include "cntmodelextuids.h"

QList<CContactItemField *> CntTransformSyncTarget::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;

	//cast to sync target
	const QContactSyncTarget &syncTarget(static_cast<const QContactSyncTarget&>(detail));

	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(syncTarget.syncTarget().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldClass);
 	newField->TextStorage()->SetTextL(fieldText);
	newField->SetMapping(KUidContactFieldVCardMapClass);

	fieldList.append(newField);
	CleanupStack::Pop(newField);

	return fieldList;
}

QContactDetail *CntTransformSyncTarget::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);

	QContactSyncTarget *syncTarget = new QContactSyncTarget();

	CContactTextField* storage = field.TextStorage();
	QString syncTargetString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());

	syncTarget->setSyncTarget(syncTargetString);
	return syncTarget;
}

bool CntTransformSyncTarget::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldClass.iUid) {
        ret = true;
    }
    return ret;
}

bool CntTransformSyncTarget::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactSyncTarget::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> CntTransformSyncTarget::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactSyncTarget::FieldSyncTarget)
        uids << KUidContactFieldClass;
    return uids;
}


/*!
 * Checks whether the subtype is supported
 *
 * \a subType The subtype to be checked
 * \return True if this subtype is supported
 */
bool CntTransformSyncTarget::supportsSubType(const QString& subType) const
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
quint32 CntTransformSyncTarget::getIdForField(const QString& fieldName) const
{
    if (QContactSyncTarget::FieldSyncTarget == fieldName)
        return KUidContactFieldClass.iUid;
    else 
        return 0;
}

/*!
 * Adds the detail definitions for the details this transform class supports.
 *
 * \a definitions On return, the supported detail definitions have been added.
 */
void CntTransformSyncTarget::detailDefinitions(QMap<QString, QContactDetailDefinition> &definitions) const
{
    QMap<QString, QContactDetailDefinition::Field> fields;
    QContactDetailDefinition::Field f;
    QContactDetailDefinition d;

    d.setName(QContactSyncTarget::DefinitionName);
    f.dataType = QVariant::String;
    f.allowableValues << QString("private") << QString("public") << QString("none");
    fields.insert(QContactSyncTarget::FieldSyncTarget, f);

    d.setFields(fields);
    d.setUnique(true);
    d.setAccessConstraint(QContactDetailDefinition::NoConstraint);

    definitions.insert(d.name(), d);
}
