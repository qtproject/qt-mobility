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

#include "transformcontact.h"
#include <cntfldst.h>

#include <qtcontacts.h>

#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>

TransformContact::TransformContact()
{
	// TODO Auto-generated constructor stub

}

TransformContact::~TransformContact()
{
	// TODO Auto-generated destructor stub
}

void TransformContact::initializeContactMapping()
{
}

QContact TransformContact::transformContact(CContactItem &contact) const
{
	// TODO: Error Handling
	    // TODO: Undecided implementation for: labels, context, attributes

		// Create a new QContact
		QContact newQtContact;
		newQtContact.setId(contact.Id());
		
		// We need to keep a map of the details so that we append fields
		// rather than adding a new detail for each field instance
		QMap<QString, QContactDetail> details;
		
		// Iterate through the CContactItemFieldSet, creating
		// new fields for the QContact
		CContactItemFieldSet& fields(contact.CardFields());
		
		const int numFields(fields.Count());
		for(int i(0); i < numFields; ++i) {
			CContactItemField& currentField(fields[i]);
			
			// TODO: Support all instances of this type (hardcoded 0 must go)
			// Check that the identifier was valid, if not, skip this field.
			QPair<QString, QString> detailField = m_fieldMap.qtDetailFieldPair(currentField.ContentType().FieldType(0));
			if (detailField.first.isEmpty()) {
	            // Skipping fields for which there is no mapping provided
				continue;
			}
			
			// Check that this is a text field.
			// TODO: Remove this check when other field types are supported.
			if (currentField.StorageType() != KStorageTypeText) {
				// Skipping non-text fields until they are supported.
				continue;
			}
			
			// Retrieve and set the contact text
			CContactTextField* storage = currentField.TextStorage();
			QString fieldText = QString::fromUtf16(storage->Text().Ptr(), storage->Text().Length());
		
	        if (details.contains(detailField.first)) {
	            // This detail already exists, so just add this field to it
	            details[detailField.first].setValue(detailField.second, fieldText);
	            } else {
	            // We need to create a new detail object for this field
	            QContactDetail newDetail(detailField.first);
	            newDetail.setValue(detailField.second, fieldText);
	            details.insertMulti(detailField.first, newDetail);
	        }
		}
		
		// Add all of the converted details to the new Qt Contact
		QList<QContactDetail> detailValues = details.values();
		const int detailCount = detailValues.count();
		for (int i(0); i < detailCount; ++i) {
	        // TODO: Update this code when the method for adding
	        // multiple details of the same type is established.
	        newQtContact.saveDetail(&detailValues[i]);
		}
		
		return newQtContact;
}

CContactItem *TransformContact::transformContact(QContact &contact) const
{
	// TODO: Error Handling (see uses of new)
	// TODO: Undecided implementation: labels, context, attributes.
	
	// Create a new contact card.
	CContactCard* symContact = CContactCard::NewLC();
	
	// Set the contact id.
	symContact->SetId(contact.id());
	
	// Copy all fields to the Symbian contact.
	QList<QContactDetail> detailList(contact.details());
	
	// Iterate through the contact details in the QContact
	const int detailCount(detailList.count());
	for(int i(0); i < detailCount; ++i) {
        // Now iterate through values in this detail
        QVariantMap fields = detailList[i].values();
        QList<QString> fieldKeys = fields.keys();
        QString detailType = detailList[i].definitionName(); //detailList[i].type();
        
        
        const int fieldCount(fieldKeys.count());
        for (int j(0); j < fieldCount; ++j) {
            
            // Look up the TUid identifier
            TUid fieldType(m_fieldMap.symbianOsUid(detailType, fieldKeys[j]));
            
            // Check that the field identifier was valid
            if (fieldType.iUid == 0) {
                continue;
            }
            
            // TODO: Add support for other field types
            // Make sure the value of this field is a text value
            QVariant fieldValue = fields[fieldKeys[j]];
            if (! fieldValue.canConvert(QVariant::String)) {
                continue;
            }
            
            // Create a text field for the Symbian Contact item
            CContactItemField* newField = CContactItemField::NewLC(KStorageTypeText, fieldType);
            TPtrC fieldText(reinterpret_cast<const TUint16*>(fieldValue.toString().utf16()));
            newField->TextStorage()->SetTextL(fieldText);
            
            // Add field to contact item
            symContact->AddFieldL(*newField);
            CleanupStack::Pop(newField);
        }
	}
	
	CleanupStack::Pop(symContact);
	return symContact;

}


