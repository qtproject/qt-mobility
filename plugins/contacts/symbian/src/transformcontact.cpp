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

#include "transformname.h"
#include "transformnickname.h"
#include "transformphonenumber.h"
#include "transformemail.h"

#include <qtcontacts.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>

#include <QDebug>

TransformContact::TransformContact()
{
	initializeTransformContactData();
}

TransformContact::~TransformContact()
{
}

void TransformContact::initializeTransformContactData()
{
	//These can be added to normal list, if we loop through it.
	m_transformContactData.insert(Name, new TransformName);
	m_transformContactData.insert(Nickname, new TransformNickname);
	m_transformContactData.insert(PhoneNumber, new TransformPhoneNumber);
	m_transformContactData.insert(EmailAddress, new TransformEmail);
}


QContact TransformContact::transformContact(CContactItem &contact) const
{
		// Create a new QContact
		QContact newQtContact;
		newQtContact.setId(contact.Id());
		
		// Iterate through the CContactItemFieldSet, creating
		// new fields for the QContact
		CContactItemFieldSet& fields(contact.CardFields());
		
		const int numFields(fields.Count());
		
		for(int i(0); i < numFields; ++i) 
		{
			QContactDetail *detail = transformItemField( fields[i], newQtContact );
			
			if(detail)
			{
				newQtContact.saveDetail(detail);	
			}
		}

		return newQtContact;
}

/*! Transform a QContact into a Symbian CContactItem.
 *  This will convert all supported fields to the CContactItem format.
 *
 * \param contact A reference to a QContact to be converted.
 * \param contactItem A reference to the CContactItem to add the fields to.
*/
void TransformContact::transformContactL(
        QContact &contact,
        CContactItem &contactItem) const
{
	// Copy all fields to the Symbian contact.
	QList<QContactDetail> detailList(contact.details());
	
	// Iterate through the contact details in the QContact
	const int detailCount(detailList.count());
	
	for(int i(0); i < detailCount; ++i) 
	{
		QList<CContactItemField *> fieldList = transformDetailL( detailList.at(i) );
		int fieldCount = fieldList.count();
		
		for (int i = 0; i < fieldCount; i++)
        {
			//transform ownership of field
		    contactItem.AddFieldL(*fieldList.at(i));
		}
	}
}

QList<CContactItemField *> TransformContact::transformDetailL(const QContactDetail &detail) const
{
	QList<CContactItemField *> itemFieldList;
	QString detailName(detail.definitionName());
	
	QMap<ContactData, TransformContactData*>::const_iterator i = m_transformContactData.constBegin();
	while (i != m_transformContactData.constEnd()) {
        if (i.value()->supportsDetail(detailName)) { 
            itemFieldList = i.value()->transformDetailL(detail);
            break;
	    }
        ++i;
    }
	
	return itemFieldList;
}

QContactDetail *TransformContact::transformItemField(const CContactItemField& field, const QContact &contact) const
{
	QContactDetail *detail(0);
	TUint32 fieldType(field.ContentType().FieldType(0).iUid);
	
	QMap<ContactData, TransformContactData*>::const_iterator i = m_transformContactData.constBegin();
	while (i != m_transformContactData.constEnd()) {
        if (i.value()->supportsField(fieldType)) { 
            detail = i.value()->transformItemField(field, contact);
            break;
        }
        ++i;
	 }

	return detail;
}
	





