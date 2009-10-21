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
#include "transformanniversary.h"
const char separator = ',';

QList<CContactItemField *> TransformAnniversary::transformDetailL(const QContactDetail &detail)
{
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

QContactDetail *TransformAnniversary::transformItemField(const CContactItemField& field, const QContact &contact)
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
        anniversary->setEvent(unformattedAnniversary.right(unformattedAnniversary.length()-separatorPos-1));
	}
	else {
        anniversary->setEvent(unformattedAnniversary);
	}
	return anniversary;
}

bool TransformAnniversary::supportsField(TUint32 fieldType) const
{
    bool ret = false;
    if (fieldType == KUidContactFieldAnniversary.iUid) {
        ret = true;
    }
    return ret;
}

bool TransformAnniversary::supportsDetail(QString detailName) const
{
    bool ret = false;
    if (detailName == QContactAnniversary::DefinitionName) {
        ret = true;
    }
    return ret;
}

QList<TUid> TransformAnniversary::supportedSortingFieldTypes(QString /*detailFieldName*/) const
{
    // Sorting not supported
    return QList<TUid>();
}
