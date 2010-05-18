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
#include "cnttransformonlineaccountsimple.h"
#include "cnttransformorganisation.h"
#include "cnttransformpresence.h"
#include "cnttransformavatar.h"
#include "cnttransformringtone.h"
#include "cnttransformthumbnail.h"
#include "cnttransformsynctarget.h"
#include "cnttransformgender.h"
#include "cnttransformanniversary.h"
#include "cnttransformanniversarysimple.h"
#include "cnttransformgeolocation.h"
#include "cnttransformnote.h"
#include "cnttransformfamily.h"
#include "cnttransformempty.h"
#include "cntsymbiantransformerror.h"

#include <qtcontacts.h>
#include <cntfldst.h>
#include <cntdb.h>
#include <cntdbobs.h>
#include <cntitem.h>
#include <cntdef.hrh> // explicitly included because of KUidContactFieldGEOValue

#include <QDebug>

//UIDs for preferred (default) fields
const int KDefaultFieldForCall = 0x10003E70;
const int KDefaultFieldForVideoCall = 0x101F85A6;
const int KDefaultFieldForEmail = 0x101F85A7;
const int KDefaultFieldForMessage = 0x101f4cf1;

CntTransformContact::CntTransformContact() :
    m_tzConverter(0)
{
	initializeCntTransformContactData();
}

CntTransformContact::~CntTransformContact()
{
    delete m_tzConverter;
    m_tzoneServer.Close();

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
	m_transformContactData.insert(SyncTarget, new CntTransformSyncTarget);
	m_transformContactData.insert(Note, new CntTransformNote);
	m_transformContactData.insert(Family, new CntTransformFamily);
    m_transformContactData.insert(Ringtone, new CntTransformRingtone);
    m_transformContactData.insert(Avatar, new CntTransformAvatar);

#ifdef SYMBIAN_BACKEND_USE_SQLITE
	// variated transform classes
    m_transformContactData.insert(Anniversary, new CntTransformAnniversary);

    // not supported on pre-10.1
	m_transformContactData.insert(Geolocation, new CntTransformGeolocation);
    m_transformContactData.insert(Gender, new CntTransformGender);

    // Causes a "CPbk2ContactEdit.. 2" panic in Phonebook2 contact editor
    // Although IMPP field is supported on some pre 10.1 platforms (newer
    // 3.2.3 and 5.0 releases), it may be safer not to include online account
    // at all.
    m_transformContactData.insert(OnlineAccount, new CntTransformOnlineAccount);
    
    // not supported on pre-10.1
    m_transformContactData.insert(Presence, new CntTransformPresence);

#else
    // Empty transform class for removing unsupported detail definitions
    m_transformContactData.insert(Empty, new CntTransformEmpty);

    m_transformContactData.insert(Thumbnail, new CntTransformThumbnail);

    // variated transform classes
    m_transformContactData.insert(Anniversary, new CntTransformAnniversarySimple);
    m_transformContactData.insert(OnlineAccount, new CntTransformOnlineAccount);
#endif
}

/*!
 * Converts Symbian contact item to QContact. Note that the contact is not
 * saved into contacts database so the details that require contact to exist
 * in the database are not transformed. Use transformPostSaveDetailsL to
 * transform those details after the contact item has been saved.
 * \param contact A reference to a symbian contact item to be converted.
 * \return Qt Contact
 */
QContact CntTransformContact::transformContactL(CContactItem &contact)
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
            transformPreferredDetail(fields[i], *detail, newQtContact);
            delete detail;
            detail = 0;
        }
    }

    return newQtContact;
}

/*!
 * Transforms details that are not available until the CContactItem has been
 * saved into contacts database.
 */
void CntTransformContact::transformPostSaveDetailsL(
        const CContactItem& contactItem,
        QContact& contact,
        const CContactDatabase &contactDatabase,
        QString managerUri)
{
    // Id
    QContactId contactId;
    contactId.setLocalId(contactItem.Id());
    contactId.setManagerUri(managerUri);
    contact.setId(contactId);

    // GUID
    QContactDetail *detailUid = transformGuidItemFieldL(contactItem, contactDatabase);
    if(detailUid)
    {
        // replace detail
        QList<QContactDetail> guids = contact.details(QContactGuid::DefinitionName);
        for(int i(0); i < guids.count(); i++) {
            QContactGuid guidDetail = guids[i];
            contact.removeDetail(&guidDetail);
        }
        contact.saveDetail(detailUid);
        delete detailUid;
        detailUid = 0;
    }

    // Timestamp
    QContactDetail *detailTimestamp = transformTimestampItemFieldL(contactItem, contactDatabase);
    if(detailTimestamp)
    {
        // replace detail
        QList<QContactDetail> timestamps = contact.details(QContactTimestamp::DefinitionName);
        for(int i(0); i < timestamps.count(); i++) {
            QContactTimestamp timestampDetail = timestamps[i];
            contact.removeDetail(&timestampDetail);
        }
        contact.saveDetail(detailTimestamp);
        delete detailTimestamp;
        detailTimestamp = 0;
    }
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
            QString detailName = detail->definitionName();
            QList<CContactItemField *> fieldList = transformDetailL(*detail);
            int fieldCount = fieldList.count();
            
            // save preferred detail
            transformPreferredDetailL(contact, detailList.at(i), fieldList);            
            
            for (int j = 0; j < fieldCount; j++)
            {
                //Add field to fieldSet
                fieldSet->AddL(*fieldList.at(j));
            }
	    }
	    else {
	        // remove empty details
	        contact.removeDetail(detail.data());
	    }
	}

	resetTransformObjects();
	
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

QList<TUid> CntTransformContact::itemFieldUidsL(const QString detailDefinitionName) const
{
    QList<TUid> fieldUids;
    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();

    while (i != m_transformContactData.constEnd()) {
        if (i.value()->supportsDetail(detailDefinitionName)) {
            // The leaf class supports this detail, so check which field type
            // uids it supports, use empty field name to get all the supported uids
            fieldUids << i.value()->supportedFields();

            // Assume there are no more leaf classes for this detail
            break;
        }
        i++;
    }
    return fieldUids;
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

void CntTransformContact::detailDefinitions(
        QMap<QString, QContactDetailDefinition>& defaultSchema,
        const QString& contactType,
        QContactManager::Error* error) const
{
    Q_UNUSED(error);

    // Ask leaf classes to do the modifications required to the default schema
    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
    while (i != m_transformContactData.constEnd()) {
        i.value()->detailDefinitions(defaultSchema, contactType);
        i++;
    }
    
#ifndef SYMBIAN_CNTMODEL_V2
    // Cannot support timestamp
    defaultSchema.remove(QContactTimestamp::DefinitionName);
#endif
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

	if(field.ContentType().FieldTypeCount()) {
	    TUint32 fieldType(field.ContentType().FieldType(0).iUid);

        QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
        while (i != m_transformContactData.constEnd()) {
            if (i.value()->supportsField(fieldType)) {
                detail = i.value()->transformItemField(field, contact);
                break;
            }
            ++i;
        }
	}

	return detail;
}

QContactDetail* CntTransformContact::transformGuidItemFieldL(const CContactItem &contactItem, const CContactDatabase &contactDatabase) const
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

QContactDetail* CntTransformContact::transformTimestampItemFieldL(const CContactItem &contactItem, const CContactDatabase &contactDatabase)
{
#ifdef SYMBIAN_CNTMODEL_V2

    // Time zone conversion is needed because contact model uses GMT time stamps
    if (!m_tzConverter) {
        User::LeaveIfError(m_tzoneServer.Connect()); 
        m_tzConverter = CTzConverter::NewL(m_tzoneServer);
    }

    QContactTimestamp *timestampDetail = 0;
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
                const TInt formattedDateLength(14);
                _LIT(KDateFormat, "%F%Y%M%D%H%T%S");
                QString DateFormatQt = QString("yyyyMMddHHmmss");

                // creation date
                TTime timeCreation(timeValue);
                User::LeaveIfError(m_tzConverter->ConvertToLocalTime(timeCreation));
                TBuf<formattedDateLength> createdBuf;
                timeCreation.FormatL(createdBuf, KDateFormat);
                QString createdString = QString::fromUtf16(createdBuf.Ptr(), createdBuf.Length());
                timestampDetail->setCreated(QDateTime::fromString(createdString, DateFormatQt));

                // last modified date
                TTime timeModified = contactItem.LastModified();
                User::LeaveIfError(m_tzConverter->ConvertToLocalTime(timeModified));
                TBuf<formattedDateLength> modifiedBuf;
                timeModified.FormatL(modifiedBuf, KDateFormat);
                QString modifiedString = QString::fromUtf16(modifiedBuf.Ptr(), modifiedBuf.Length());
                timestampDetail->setLastModified(QDateTime::fromString(modifiedString, DateFormatQt));
            }
        }
    }
    CleanupStack::PopAndDestroy(guidBuf);
    return timestampDetail;
#else
    // NOTE: In S60 3.1 we cannot use ContactGuid::GetCreationDate() because
    // it is not exported.
    // TODO: Make sure SYMBIAN_CNTMODEL_V2 is the right flag for this.
    Q_UNUSED(contactItem);
    Q_UNUSED(contactDatabase)
    return 0;
#endif
}

void CntTransformContact::transformPreferredDetailL(const QContact& contact,
        const QContactDetail& detail, QList<CContactItemField*> &fieldList) const
{
    if (fieldList.count() == 0) {
        return;
    }

    if (contact.isPreferredDetail("call", detail)) {
        fieldList.at(0)->AddFieldTypeL(TFieldType::Uid(KDefaultFieldForCall));
    }
    if (contact.isPreferredDetail("email", detail)) {
        fieldList.at(0)->AddFieldTypeL(TFieldType::Uid(KDefaultFieldForEmail));
    }
    if (contact.isPreferredDetail("videocall", detail)) {
        fieldList.at(0)->AddFieldTypeL(TFieldType::Uid(KDefaultFieldForVideoCall));
    }
    if (contact.isPreferredDetail("message", detail)) {
        fieldList.at(0)->AddFieldTypeL(TFieldType::Uid(KDefaultFieldForMessage));
    }
}

void CntTransformContact::transformPreferredDetail(const CContactItemField& field,
        const QContactDetail& detail, QContact& contact) const
{
    if (field.ContentType().ContainsFieldType(TFieldType::Uid(KDefaultFieldForCall))) {
        contact.setPreferredDetail("call", detail);
    }
    if (field.ContentType().ContainsFieldType(TFieldType::Uid(KDefaultFieldForEmail))) {
        contact.setPreferredDetail("email", detail);
    }
    if (field.ContentType().ContainsFieldType(TFieldType::Uid(KDefaultFieldForVideoCall))) {
        contact.setPreferredDetail("videocall", detail);
    }
    if (field.ContentType().ContainsFieldType(TFieldType::Uid(KDefaultFieldForMessage))) {
        contact.setPreferredDetail("message", detail);
    }
}

void CntTransformContact::resetTransformObjects() const
{
    QMap<ContactData, CntTransformContactData*>::const_iterator i = m_transformContactData.constBegin();
    while (i != m_transformContactData.constEnd()) {
        i.value()->reset();
        ++i;
    }
}
