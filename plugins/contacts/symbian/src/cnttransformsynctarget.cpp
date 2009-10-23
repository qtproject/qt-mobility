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

QList<CContactItemField *> TransformSyncTarget::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList; 
	
	//cast to sync target
	const QContactSyncTarget &syncTarget(static_cast<const QContactSyncTarget&>(detail));
	
	//create new field
	TPtrC fieldText(reinterpret_cast<const TUint16*>(syncTarget.syncTarget().utf16()));
	CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldSyncTarget);
 	newField->TextStorage()->SetTextL(fieldText);
	newField->SetMapping(KUidContactFieldVCardMapUnknown);

	fieldList.append(newField);
	CleanupStack::Pop(newField);
		
	return fieldList;
}

QContactDetail *TransformSyncTarget::transformItemField(const CContactItemField& field, const QContact &contact)
{
	Q_UNUSED(contact);
	
	QContactSyncTarget *syncTarget = new QContactSyncTarget();
	
	CContactTextField* storage = field.TextStorage();
	QString syncTargetString = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	
	syncTarget->setSyncTarget(syncTargetString);
	return syncTarget;
}

bool TransformSyncTarget::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldSyncTarget.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformSyncTarget::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactSyncTarget::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> TransformSyncTarget::supportedSortingFieldTypes(QString detailFieldName) const
{
    QList<TUid> uids;
    if (detailFieldName == QContactSyncTarget::FieldSyncTarget)
        uids << KUidContactFieldSyncTarget;   
    return uids;
}
