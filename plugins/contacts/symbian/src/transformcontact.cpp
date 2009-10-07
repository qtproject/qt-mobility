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
#include "transformurl.h"
#include "transformaddress.h"
#include "transformbirthday.h"
#include "transformonlineaccount.h"
#include "transformorganisation.h"
#include "transformavatar.h"
#include "transformsynctarget.h"
#include "transformgender.h"
#include "transformanniversary.h"

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
	m_transformContactData.insert(Address, new TransformAddress);
	m_transformContactData.insert(URL, new TransformUrl);
	m_transformContactData.insert(Birthday, new TransformBirthday);
	m_transformContactData.insert(OnlineAccount, new TransformOnlineAccount);
	m_transformContactData.insert(Organisation, new TransformOrganisation);
	m_transformContactData.insert(Avatar, new TransformAvatar);
	m_transformContactData.insert(SyncTarget, new TransformSyncTarget);
	m_transformContactData.insert(Gender, new TransformGender);
	m_transformContactData.insert(Anniversary, new TransformAnniversary);
}


QContact TransformContact::transformContactL(CContactItem &contact, CContactDatabase &contactDatabase) const
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
		
		// Add contact's UID
	    QContactDetail *detailUid = transformGuidItemFieldL(contact, contactDatabase);
        if(detailUid)
        {
	        newQtContact.saveDetail(detailUid);    
	    }

        // Add contact's timestamp
        QContactDetail *detailTimestamp = transformTimestampItemFieldL(contact, contactDatabase);
        if(detailTimestamp)
        {
            newQtContact.saveDetail(detailTimestamp);    
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
	//Create a new fieldSet
	CContactItemFieldSet *fieldSet = CContactItemFieldSet::NewLC();
	
	// Copy all fields to the Symbian contact.
	QList<QContactDetail> detailList(contact.details());
	
	// Iterate through the contact details in the QContact
	const int detailCount(detailList.count());
	
	for(int i(0); i < detailCount; ++i) 
	{
		QList<CContactItemField *> fieldList = transformDetailL( detailList.at(i) );
		int fieldCount = fieldList.count();
		
		for (int j = 0; j < fieldCount; j++)
        {
			//Add field to fieldSet
			fieldSet->AddL(*fieldList.at(j));
		}
		
		// Add contact's UID - just ignored silently since it's read only
		//if (detailList.at(i).definitionName() == QContactGuid::DefinitionName)
		//{
        //    transformGuidDetailL(detailList.at(i), contactItem);
		//}
		
	    // Add contact's timestamp - just ignored silently since it's read only
	    //if (detailList.at(i).definitionName() == QContactTimestamp::DefinitionName)
	    //{
        //    transformTimestampDetailL(detailList.at(i), contactItem);
	    //}
	}
	
	contactItem.UpdateFieldSet(fieldSet);
	CleanupStack::Pop(fieldSet);
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
	
QContactDetail* TransformContact::transformGuidItemFieldL(CContactItem &contactItem, CContactDatabase &contactDatabase) const
{
    QContactGuid *guidDetail = 0;
    QString guid = QString::fromUtf16(contactItem.UidStringL(contactDatabase.MachineId()).Ptr(),
        contactItem.UidStringL(contactDatabase.MachineId()).Length());
    if (guid.length() > 0)
    {
        guidDetail = new QContactGuid();
        guidDetail->setGuid(guid);
    }
    return guidDetail; 
}

void TransformContact::transformGuidDetailL(const QContactDetail &guidDetail, CContactItem &contactItem) const
{
    const QContactGuid &guid(static_cast<const QContactGuid&>(guidDetail));
    TPtrC guidString(reinterpret_cast<const TUint16*>(guid.guid().utf16()));
    if (guidString.Length() > 0 )
    {
        contactItem.SetUidStringL(guidString);
    }
}

QContactDetail* TransformContact::transformTimestampItemFieldL(CContactItem &contactItem, CContactDatabase &contactDatabase) const
{
    QContactTimestamp *timestampDetail = 0;
    
    // NOTE: In S60 3.1 we cannot use ContactGuid::GetCreationDate() because
    // it is not exported.
    // TODO: Make sure SYMBIAN_CNTMODEL_V2 is the right flag for this.
#ifdef SYMBIAN_CNTMODEL_V2
    HBufC* guidBuf = contactItem.UidStringL(contactDatabase.MachineId()).AllocLC();
    TPtr ptr = guidBuf->Des();
    if (ContactGuid::GetCreationDate(ptr, contactDatabase.MachineId()))
    {
        if (ptr.Length() > 0)
        {
            TLex lex(ptr);
            TInt64 timeValue;
            if (lex.Val(timeValue, EHex) == 0)
            {
                timestampDetail = new QContactTimestamp();
                
                //creation date
                TTime timeCreation(timeValue);
                TDateTime dateCreation = timeCreation.DateTime();
                QDate qDateCreation(dateCreation.Year(), dateCreation.Month() + 1, dateCreation.Day());
                QTime qTimeCreation(dateCreation.Hour(), dateCreation.Minute(), dateCreation.Second(), dateCreation.MicroSecond()/1000);
                QDateTime qDateTimeCreation(qDateCreation, qTimeCreation);
                timestampDetail->setCreated(qDateTimeCreation);
                
                //last modified date
                TTime timeModified = contactItem.LastModified();
                TDateTime dateModified = timeModified.DateTime();
                QDate qDateModified(dateModified.Year(), dateModified.Month() + 1, dateModified.Day());
                QTime qTimeModified(dateModified.Hour(), dateModified.Minute(), dateModified.Second(), dateModified.MicroSecond()/1000);
                QDateTime qDateTimeModified(qDateModified, qTimeModified);
                timestampDetail->setLastModified(qDateTimeModified);
            }
        }
    }
    CleanupStack::PopAndDestroy(guidBuf);
#endif
    return timestampDetail;      
}

void TransformContact::transformTimestampDetailL(const QContactDetail &guidDetail, CContactItem &contactItem) const
{
    Q_UNUSED(guidDetail);
    Q_UNUSED(contactItem);
    User::LeaveIfError(KErrNotSupported);
}
