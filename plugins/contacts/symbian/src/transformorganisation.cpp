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
#include "TransformOrganisation.h"

QList<CContactItemField *> TransformOrganisation::transformDetailL(const QContactDetail &detail)
{
	QList<CContactItemField *> fieldList;
	
	//cast to orgenisation
	const QContactOrganisation &orgDetails(static_cast<const QContactOrganisation&>(detail));
	
	//Company
	TPtrC fieldTextCompany(reinterpret_cast<const TUint16*>(orgDetails.displayLabel().utf16()));
	CContactItemField* company = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldCompanyName);
	company->TextStorage()->SetTextL(fieldTextCompany);
	company->SetMapping(KUidContactFieldVCardMapORG);
	fieldList.append(company);
	CleanupStack::Pop(company);
	
	//Department
	TPtrC fieldTextDepartment(reinterpret_cast<const TUint16*>(orgDetails.department().utf16()));
	CContactItemField* department = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldDepartmentName);
	department->TextStorage()->SetTextL(fieldTextDepartment);
	department->SetMapping(KUidContactFieldVCardMapDepartment);
	fieldList.append(department);
	CleanupStack::Pop(department);

	//Job title
	TPtrC fieldTextJobTitle(reinterpret_cast<const TUint16*>(orgDetails.title().utf16()));
	CContactItemField* jobTitle = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldJobTitle);
	jobTitle->TextStorage()->SetTextL(fieldTextJobTitle);
	jobTitle->SetMapping(KUidContactFieldVCardMapTITLE);
	fieldList.append(jobTitle);
	CleanupStack::Pop(jobTitle);
	
	return fieldList;
}	


QContactDetail *TransformOrganisation::transformItemField(const CContactItemField& field, const QContact &contact)
{
	QContactOrganisation *organisation = new QContactOrganisation(contact.detail<QContactOrganisation>());
	
	CContactTextField* storage = field.TextStorage();
	QString orgDetail = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
	
	for (int i = 0; i < field.ContentType().FieldTypeCount(); i++)
	{
		//Company
		if (field.ContentType().FieldType(i) == KUidContactFieldCompanyName)
		{
            organisation->setDisplayLabel(orgDetail);
		}
		
		//Department
		else if (field.ContentType().FieldType(i) == KUidContactFieldDepartmentName)
		{
            organisation->setDepartment(orgDetail);
		}
		
		//Job title
		else if (field.ContentType().FieldType(i) == KUidContactFieldJobTitle)
		{
            organisation->setTitle(orgDetail);
		}
	}
	
	return organisation;
}

bool TransformOrganisation::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldCompanyName.iUid ||
        fieldType == KUidContactFieldDepartmentName.iUid ||
        fieldType == KUidContactFieldJobTitle.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformOrganisation::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactOrganisation::DefinitionName) {
        ret = true;
    }
    return ret;
}
