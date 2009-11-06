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

#include "cnttransformcontact.h"

#include "cnttransformname.h"
#include "cnttransformnickname.h"
#include "cnttransformphonenumber.h"
#include "cnttransformemail.h"
#include "cnttransformurl.h"
#include "cnttransformaddress.h"
#include "cnttransformbirthday.h"
#include "cnttransformonlineaccount.h"
#include "cnttransformorganisation.h"
#include "cnttransformavatar.h"
#include "cnttransformsynctarget.h"
#include "cnttransformgender.h"
#include "cnttransformanniversary.h"
#include "cnttransformanniversarysimple.h"
#include "cnttransformgeolocation.h"
#include "cnttransformnote.h"
#include "cnttransformfamily.h"
#include "cntsymbiantransformerror.h"

#include <qtcontacts.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>
#include <cntdef.hrh> // explicitly included because of KUidContactFieldGEOValue

#include <QDebug>

CntTransformContact::CntTransformContact()
{
	initializeCntTransformContactData();
}

CntTransformContact::~CntTransformContact()
{
    QMap<ContactData, CntTransformContactData*>::iterator itr;

    for (itr = m_transformContactData.begin(); itr != m_transformContactData.end(); ++itr)
    {
        CntTransformContactData* value = itr.value();
        delete value;
        value = 0;
    }
}

void CntTransformContact::initializeCntTransformContactData()
{
	//These can be added to normal list, if we loop through it.
	m_transformContactData.insert(Name, new CntTransformName);
	m_transformContactData.insert(Nickname, new CntTransformNickname);
	m_transformContactData.insert(PhoneNumber, new CntTransformPhoneNumber);
	m_transformContactData.insert(EmailAddress, new CntTransformEmail);
	m_transformContactData.insert(Address, new CntTransformAddress);
	m_transformContactData.insert(URL, new CntTransformUrl);
	m_transformContactData.insert(Birthday, new CntTransformBirthday);
	m_transformContactData.insert(Organisation, new CntTransformOrganisation);
	m_transformContactData.insert(Note, new CntTransformNote);
	m_transformContactData.insert(Family, new CntTransformFamily);

#ifdef USE_CUSTOM_CNT_MODEL_FIELDS
	// These are not supported on pre-10.1
    m_transformContactData.insert(Anniversary, new CntTransformAnniversary);
	m_transformContactData.insert(Geolocation, new CntTransformGeolocation);

    // Causes a "CPbk2ContactEdit.. 2" panic in Phonebook2 contact editor
    // It is probably ok to use this only in 10.1 and newer
    m_transformContactData.insert(Gender, new CntTransformGender);

    // Causes a "CPbk2ContactEdit.. 2" panic in Phonebook2 contact editor
    // Although IMPP field is supported on some pre 10.1 platforms (newer
    // 3.2.3 and 5.0 releases), it may be safer not to include online account
    // at all.
    m_transformContactData.insert(OnlineAccount, new CntTransformOnlineAccount);

    // TODO: The following transform classes should be checked. What do we
	// need to change to make them compatible with Virtual Phonebook and
	// Phonebook2?

    // Causes a "CPbk2ContactEdit.. 2" panic in Phonebook2 contact editor.
    // Avatar is probably not correctly mapped to image fields of a contact item
    m_transformContactData.insert(Avatar, new CntTransformAvatar);
    // Causes a "CPbk2ContactEdit.. 2" panic in Phonebook2 contact editor
    m_transformContactData.insert(SyncTarget, new CntTransformSyncTarget);
#else
    m_transformContactData.insert(Anniversary, new CntTransformAnniversarySimple);
#endif
}

/*!
 * Converts Symbian contact item to QContact. Note that the contact id is not
 * converted to QContactId so the caller is responsible of setting the contact
 * id if needed.
 * \param contact A reference to a symbian contact item to be converted.
 * \param contactDatabase Reference to an opened Symbian contact database instance.
 * \return Qt Contact
 */
QContact CntTransformContact::transformContactL(CContactItem &contact, CContactDatabase &contactDatabase) const
{
    // Create a new QContact
    QContact newQtContact;

    // set the corect type
    if (contact.Type() == KUidContactGroup)
    {
        newQtContact.setType(QContactType::TypeGroup);
    }
    else
    {
        newQtContact.setType(QContactType::TypeContact);
    }

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
            delete detail;
            detail = 0;
        }
    }

    // Add contact's GUID
    QContactDetail *detailUid = transformGuidItemFieldL(contact, contactDatabase);
    if(detailUid)
    {
        newQtContact.saveDetail(detailUid);
        delete detailUid;
        detailUid = 0;
    }

    // Add contact's timestamp
    QContactDetail *detailTimestamp = transformTimestampItemFieldL(contact, contactDatabase);
    if(detailTimestamp)
    {
        newQtContact.saveDetail(detailTimestamp);
        delete detailTimestamp;
        detailTimestamp = 0;
    }

    return newQtContact;
}

/*! CntTransform a QContact into a Symbian CContactItem.
 *  This will convert all supported fields to the CContactItem format.
 *
 * \param contact A reference to a QContact to be converted.
 * \param contactItem A reference to the CContactItem to add the fields to.
*/
void CntTransformContact::transformContactL(
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
	    QScopedPointer<QContactDetail> detail(new QContactDetail(detailList.at(i)));
	    if (!detail->isEmpty()) {
            QList<CContactItemField *> fieldList = transformDetailL(*detail);
            int fieldCount = fieldList.count();
            
            // check if the contact has any unsupported details
            if(fieldCount == 0) {
                if (detail->definitionName() != QContactDisplayLabel::DefinitionName
                    && detail->definitionName() != QContactType::DefinitionName
                    && detail->definitionName() != QContactGuid::DefinitionName
                    && detail->definitionName() != QContactTimestamp::DefinitionName) {
                User::Leave(KErrInvalidContactDetail);
                }
            }
    
            for (int j = 0; j < fieldCount; j++)
            {
                //Add field to fieldSet
                fieldSet->AddL(*fieldList.at(j));
            }
	    }
	}

	contactItem.UpdateFieldSet(fieldSet);
	CleanupStack::Pop(fieldSet);
}

QList<TUid> CntTransformContact::supportedSortingFieldTypes( QString detailDefinitionName, QString detailFieldName )
{
    QList<TUid> uids;
    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
    while (i != m_transformContactData.constEnd()) {
        if (i.value()->supportsDetail(detailDefinitionName)) {
            uids = i.value()->supportedSortingFieldTypes(detailFieldName);
            if( uids.count() )
                break;
        }
        ++i;
    }
    return uids;
}

TUint32 CntTransformContact::GetIdForDetailL(const QContactDetailFilter& detailFilter, bool& isSubtype) const
    {
    isSubtype = false;
    QString defnitionName = detailFilter.detailDefinitionName();
    QString fieldName = detailFilter.detailFieldName();
    QString fieldValue = detailFilter.value().toString();
    quint32 fieldId = 0;

    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
    while (i != m_transformContactData.constEnd()) {

        if (i.value()->supportsDetail(defnitionName)){
            // This definition is supported,
            // so check if there is a subtype defined

            if (i.value()->supportsSubType(fieldName)){

               // subtype supported, so value contains the field to be checked
                fieldId  = i.value()->getIdForField(fieldValue);
                isSubtype = true;

            } else {
                // subtype not supported, so field itself should be passed
                fieldId  = i.value()->getIdForField(fieldName);
                isSubtype = false;
            }

            break;
        }
        ++i;
    }
    return fieldId;

    }

QMap<QString, QContactDetailDefinition> CntTransformContact::detailDefinitions(QContactManager::Error& error) const
{
    Q_UNUSED(error);
    QMap<QString, QContactDetailDefinition> defMap;
    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
    while (i != m_transformContactData.constEnd()) {
        i.value()->detailDefinitions(defMap);
        i++;
    }
    return defMap;
}

QList<CContactItemField *> CntTransformContact::transformDetailL(const QContactDetail &detail) const
{
	QList<CContactItemField *> itemFieldList;
    QScopedPointer<QString> detailName(new QString(detail.definitionName()));

    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
    while (i != m_transformContactData.constEnd()) {
        if (i.value()->supportsDetail(*detailName)) {
            itemFieldList = i.value()->transformDetailL(detail);
            break;
        }
        ++i;
	}
	return itemFieldList;
}

QContactDetail *CntTransformContact::transformItemField(const CContactItemField& field, const QContact &contact) const
{
	QContactDetail *detail(0);
	TUint32 fieldType(field.ContentType().FieldType(0).iUid);

	QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
	while (i != m_transformContactData.constEnd()) {
        if (i.value()->supportsField(fieldType)) {
            detail = i.value()->transformItemField(field, contact);
            break;
        }
        ++i;
	 }

	return detail;
}

QContactDetail* CntTransformContact::transformGuidItemFieldL(CContactItem &contactItem, CContactDatabase &contactDatabase) const
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

QContactDetail* CntTransformContact::transformTimestampItemFieldL(CContactItem &contactItem, CContactDatabase &contactDatabase) const
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
                QDate qDateCreation(dateCreation.Year(), dateCreation.Month() + 1, dateCreation.Day() + 1);
                QTime qTimeCreation(dateCreation.Hour(), dateCreation.Minute(), dateCreation.Second(), dateCreation.MicroSecond()/1000);
                QDateTime qDateTimeCreation(qDateCreation, qTimeCreation);
                timestampDetail->setCreated(qDateTimeCreation);

                //last modified date
                TTime timeModified = contactItem.LastModified();
                TDateTime dateModified = timeModified.DateTime();
                QDate qDateModified(dateModified.Year(), dateModified.Month() + 1, dateModified.Day() + 1);
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
