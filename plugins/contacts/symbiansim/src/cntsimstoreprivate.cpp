/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cntsimstoreprivate.h"
#include "cntsymbiansimtransformerror.h"
#include "cntsimstore.h"
#include "cntsimstoreeventlistener.h"

#include <mmtsy_names.h>
#include <qtcontacts.h>
#include <qcontactchangeset.h>
#include <QDebug>

const TInt KOneSimContactBufferSize = 512;
const TInt KDataClientBuf  = 128;
const TInt KEtsiTonPosition = 0x70;

CntSimStorePrivate* CntSimStorePrivate::NewL(CntSymbianSimEngine &engine, CntSimStore &simStore, const QString &storeName)
{
    CntSimStorePrivate* self = new (ELeave) CntSimStorePrivate(engine, simStore, storeName);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CntSimStorePrivate::CntSimStorePrivate(CntSymbianSimEngine &engine, CntSimStore &simStore, const QString &storeName)
    :CActive(CActive::EPriorityStandard),
     m_state(InactiveState),
     m_engine(engine),
     m_simStore(simStore),
     m_storeName(storeName),
     m_readOnlyAccess(false),
     m_storeInfoPckg(m_storeInfo),
     m_listener(0)
{
    CActiveScheduler::Add(this);
    m_managerUri = engine.managerUri();
}

void CntSimStorePrivate::ConstructL()
{
    TBuf<RMobilePhoneBookStore::KMaxPBIDSize> storeName;
    convertStoreNameL(storeName);
    
    // SDN store is allways read only
    if (m_storeName == KParameterValueSimStoreNameSdn)
        m_readOnlyAccess = true;

    // Open etel server
    User::LeaveIfError(m_etelServer.Connect());
    User::LeaveIfError(m_etelServer.LoadPhoneModule(KMmTsyModuleName));

    // Open etel phone
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(m_etelServer.GetPhoneInfo(0, info));
    User::LeaveIfError(m_etelPhone.Open(m_etelServer, info.iName));
    
    // open Etel store
    User::LeaveIfError(m_etelStore.Open(m_etelPhone, storeName));
    
    //check what information can be saved to the Etel store
    TRequestStatus requestStatus;
    m_etelStore.GetInfo(requestStatus, (TDes8&)m_storeInfoPckg);
    User::WaitForRequest(requestStatus);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - GetInfo err = %d"),
        requestStatus.Int());
    User::LeaveIfError(requestStatus.Int());

    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - TotalEntries = %d"),
            m_storeInfo.iTotalEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - UsedEntries = %d"),
            m_storeInfo.iUsedEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - MaxNumLength = %d"),
            m_storeInfo.iMaxNumLength);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - MaxTextLength = %d"),
            m_storeInfo.iMaxTextLength);
    
    m_listener = new (ELeave) CntSimStoreEventListener(m_engine, m_etelStore);
    m_listener->start();
}

CntSimStorePrivate::~CntSimStorePrivate()
{
    Cancel();
    delete m_listener;
    m_etelStore.Close();
    m_etelPhone.Close();
    m_etelServer.Close();
}

void CntSimStorePrivate::convertStoreNameL(TDes &storeName)
{
    if(storeName.MaxLength() < RMobilePhoneBookStore::KMaxPBIDSize) {
        User::Leave(KErrArgument);
    }

    if (m_storeName.isEmpty()) {
        // Default to ADN store
        m_storeName = (QLatin1String) KParameterValueSimStoreNameAdn;
        storeName.Copy(KETelIccAdnPhoneBook);
    } else if (m_storeName == KParameterValueSimStoreNameFdn) {
        storeName.Copy(KETelIccFdnPhoneBook);
    } else if (m_storeName == KParameterValueSimStoreNameAdn) {
        storeName.Copy(KETelIccAdnPhoneBook);
    } else if (m_storeName == KParameterValueSimStoreNameSdn) {
        storeName.Copy(KETelIccSdnPhoneBook);
    }

    // Check that we got a valid store name
    if(storeName.Length() == 0) {
        User::Leave(KErrArgument);
    }
}

bool CntSimStorePrivate::read(int index, int numSlots, QContactManager::Error* error)
{
    if (IsActive()) {
        *error = QContactManager::LockedError;
        return false;
    }
    
    // start reading
    m_buffer.Zero();
    m_buffer.ReAlloc(KOneSimContactBufferSize*numSlots);
    m_etelStore.Read(iStatus, index, numSlots, m_buffer);
    SetActive();
    m_state = ReadState;
    
    *error = QContactManager::NoError;
    return true;
}

bool CntSimStorePrivate::write(const QContact &contact, QContactManager::Error* error)
{
    if (IsActive()) {
        *error = QContactManager::LockedError;
        return false;
    }
    
    // get index
    m_writeIndex = KErrNotFound;
    if (contact.id().managerUri() == m_managerUri &&
        contact.localId() > 0) {
        m_writeIndex = contact.localId();  
    }
    
    // encode
    m_buffer.Zero();
    m_buffer.ReAlloc(KOneSimContactBufferSize);
    TRAPD(err, m_convertedContact = encodeSimContactL(&contact, m_buffer));
    if (err != KErrNone) {
        CntSymbianSimTransformError::transformError(err, error);
        return false;
    }

    // start writing
    m_etelStore.Write(iStatus, m_buffer, m_writeIndex);
    SetActive();
    m_state = WriteState;
    
    *error = QContactManager::NoError;
    return true;
}

bool CntSimStorePrivate::remove(int index, QContactManager::Error* error)
{
    if (IsActive()) {
        *error = QContactManager::LockedError;
        return false;
    }
    
    // NOTE:
    // If index points to an empty slot and running in hardware the 
    // delete operation will not return any error.
    
    m_etelStore.Delete(iStatus, index);
    SetActive();
    m_state = DeleteState;
    
    *error = QContactManager::NoError;
    return true;
}

bool CntSimStorePrivate::getReservedSlots(QContactManager::Error* error)
{
    if (IsActive()) {
        *error = QContactManager::LockedError;
        return false;
    }
    
    // start reading
    m_buffer.Zero();
    m_buffer.ReAlloc(KOneSimContactBufferSize*m_storeInfo.iTotalEntries);
    m_etelStore.Read(iStatus, 1, m_storeInfo.iTotalEntries, m_buffer);
    SetActive();
    m_state = ReadReservedSlotsState;
    
    *error = QContactManager::NoError;
    return true;  
}

void CntSimStorePrivate::DoCancel()
{
    if (m_state == ReadState)
        m_etelStore.CancelAsyncRequest(EMobilePhoneStoreRead);
    if (m_state == WriteState)
        m_etelStore.CancelAsyncRequest(EMobilePhoneStoreWrite);
    if (m_state == DeleteState)
        m_etelStore.CancelAsyncRequest(EMobilePhoneStoreDelete);
    if (m_state == ReadReservedSlotsState)
        m_etelStore.CancelAsyncRequest(EMobilePhoneStoreRead);
    
    m_state = InactiveState;
}

void CntSimStorePrivate::RunL()
{
    //qDebug() << "CntSimStorePrivate::RunL()" << m_state << iStatus.Int();
    
    m_asyncError = iStatus.Int();
    User::LeaveIfError(iStatus.Int());
    
    // NOTE: It is assumed that emitting signals is queued
    
    switch (m_state)
    {
        case ReadState:
        {
            QList<QContact> contacts = decodeSimContactsL(m_buffer);
            emit m_simStore.readComplete(contacts, QContactManager::NoError);
        }
        break;
        
        case WriteState:
        {
            // save id
            QContactId contactId;
            contactId.setLocalId(m_writeIndex);
            contactId.setManagerUri(m_managerUri);
            m_convertedContact.setId(contactId);  
            
            // set sync target
            QContactSyncTarget syncTarget;
            syncTarget.setSyncTarget(KSimSyncTarget);
            m_convertedContact.saveDetail(&syncTarget);
            
            emit m_simStore.writeComplete(m_convertedContact, QContactManager::NoError);
        }
        break;
        
        case DeleteState:
        {
            emit m_simStore.removeComplete(QContactManager::NoError);
        }
        break;
        
        case ReadReservedSlotsState:
        {
            QList<int> reservedSlots = decodeReservedSlotsL(m_buffer);
            emit m_simStore.getReservedSlotsComplete(reservedSlots, QContactManager::NoError);
        }
        break;
        
        default:
        {
            User::Leave(KErrUnknown);
        }
        break;
    }
    m_state = InactiveState;
}

TInt CntSimStorePrivate::RunError(TInt aError)
{
    QContactManager::Error qtError = QContactManager::NoError;
    CntSymbianSimTransformError::transformError(aError, qtError);
    
    // NOTE: It is assumed that emitting signals is queued
        
    if (m_state == ReadState) 
        emit m_simStore.readComplete(QList<QContact>(), qtError);            
    else if (m_state == WriteState)
        emit m_simStore.writeComplete(m_convertedContact, qtError);  
     else if (m_state == DeleteState)
        emit m_simStore.removeComplete(qtError);
    else if (m_state == ReadReservedSlotsState)
        emit m_simStore.getReservedSlotsComplete(QList<int>(), qtError);
        
    m_state = InactiveState;
    
    return KErrNone;
}

/*! Parses SIM contacts in TLV format.
 *
 * \param rawData SIM contacts in TLV format.
 * \return List of contacts.
*/
QList<QContact> CntSimStorePrivate::decodeSimContactsL(TDes8& rawData) const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::decodeSimContactsL() - IN"));
    QList<QContact> fetchedContacts;
    QContact currentContact;

    TBuf16<KDataClientBuf> buffer;
    TPtrC16 bufPtr(buffer);

    TUint8 tagValue(0);
    CPhoneBookBuffer::TPhBkTagType dataType;

    bool isAdditionalNumber = false;

    CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);
    pbBuffer->Set(&rawData);
    pbBuffer->StartRead();

    while (pbBuffer->GetTagAndType(tagValue, dataType) == KErrNone) {
        switch (tagValue)
        {
            case RMobilePhoneBookStore::ETagPBAdnIndex:
            {
                //save contact's id (SIM card index) and manager's name
                TUint16  index;
                if (pbBuffer->GetValue(index) == KErrNone) {
                    QScopedPointer<QContactId> contactId(new QContactId());
                    contactId->setLocalId(index);
                    contactId->setManagerUri(m_managerUri);
                    currentContact.setId(*contactId);
                }
                isAdditionalNumber = false;
                break;
            }
            case RMobilePhoneBookStore::ETagPBTonNpi:
            {
                // Note, that TON info can be incorporated into the phone number by Etel
                // implementation (TSY). E.g. this is the case with Nokia TSY.
                // Here general case is implemented.

                // Check number type, we are only interested if it's international or not.
                // We assume here that ETagPBTonNpi always comes after ETagPBNumber, not before.
                TUint8  tonNpi;
                if (pbBuffer->GetValue(tonNpi) == KErrNone) {
                    TUint8  intFlag = (tonNpi & KEtsiTonPosition) >> 4;
                    if (intFlag == 1) {
                        //international number format, append "+" to the last
                        //saved number
                        QList<QContactDetail> phoneNumbers = currentContact.details(
                                QContactPhoneNumber::DefinitionName);
                        if (phoneNumbers.count() > 0) {
                            QContactPhoneNumber lastNumber = static_cast<QContactPhoneNumber>(
                                phoneNumbers.at(phoneNumbers.count() - 1));
                            QString number = lastNumber.number();
                            number.insert(0, "+");
                            lastNumber.setNumber(number);
                            if (m_readOnlyAccess)
                                m_engine.setReadOnlyAccessConstraint(&lastNumber);
                            currentContact.saveDetail(&lastNumber);
                        }
                    }
                }

                // We have rearched to the end of the number,
                // invalidate additional number flag.
                isAdditionalNumber = false;
                break;
            }
            case RMobilePhoneBookStore::ETagPBText:
            {
                if (pbBuffer->GetValue(bufPtr) == KErrNone) {
                    if (isAdditionalNumber) {
                        // For additional number bufPtr contains number alpha string,
                        // this is ignored currently
                    }
                    else {
                        // Contact name otherwise
                        QContactName name;
                        QString nameString = QString::fromUtf16(bufPtr.Ptr(), bufPtr.Length());
                        name.setCustomLabel(nameString);
                        if (m_readOnlyAccess)
                            m_engine.setReadOnlyAccessConstraint(&name);                        
                        currentContact.saveDetail(&name);
                        QContactManager::Error error(QContactManager::NoError);
                        currentContact = m_engine.setContactDisplayLabel(m_engine.synthesizedDisplayLabel(currentContact, error), currentContact);
                    }
                }
                break;
            }
            case RMobilePhoneBookStore::ETagPBSecondName:
            {
                if (pbBuffer->GetValue(bufPtr) == KErrNone) {
                    QContactNickname nickName;
                    QString name = QString::fromUtf16(bufPtr.Ptr(), bufPtr.Length());
                    nickName.setNickname(name);
                    if (m_readOnlyAccess)
                        m_engine.setReadOnlyAccessConstraint(&nickName);                    
                    currentContact.saveDetail(&nickName);
                }
                break;
            }
            case RMobilePhoneBookStore::ETagPBNumber:
            {
                if (pbBuffer->GetValue(bufPtr) == KErrNone) {
                    QContactPhoneNumber phoneNumber;
                    phoneNumber.setSubTypes( QContactPhoneNumber::SubTypeMobile );
                    QString number = QString::fromUtf16(bufPtr.Ptr(), bufPtr.Length());
                    phoneNumber.setNumber(number);
                    if (m_readOnlyAccess)
                        m_engine.setReadOnlyAccessConstraint(&phoneNumber);                    
                    currentContact.saveDetail(&phoneNumber);
                }
                break;
            }
            case RMobilePhoneBookStore::ETagPBAnrStart:
            {
                // This tag should precede every additional number entry
                isAdditionalNumber = true;
                break;
            }
            case RMobilePhoneBookStore::ETagPBEmailAddress:
            {
                if (pbBuffer->GetValue(bufPtr) == KErrNone) {
                    QContactEmailAddress email;
                    QString emailAddress = QString::fromUtf16(bufPtr.Ptr(), bufPtr.Length());
                    email.setEmailAddress(emailAddress);
                    if (m_readOnlyAccess)
                        m_engine.setReadOnlyAccessConstraint(&email);                    
                    currentContact.saveDetail(&email);
                }
                break;
            }
            case RMobilePhoneBookStore::ETagPBNewEntry:
            {
                // This signals the end of the entry and is a special case
                // which will be handled below.
                break;
            }
            default:
            {
                // An unsupported field type - just skip this value
                pbBuffer->SkipValue(dataType);
                break;
            }
        } //switch

        // save contact to the array of contact to be returned if the whole entry was extracted
        if ((tagValue == RMobilePhoneBookStore::ETagPBNewEntry && currentContact.localId() > 0) ||
            (pbBuffer->RemainingReadLength() == 0 && currentContact.localId() > 0)) {
            //QContactSyncTarget syncTarget;
            //syncTarget.setSyncTarget(KSimSyncTarget);
            //currentContact.saveDetail(&syncTarget);
            fetchedContacts.append(currentContact);
            //clear current contact
            currentContact.clearDetails();
            QScopedPointer<QContactId> contactId(new QContactId());
            contactId->setLocalId(0);
            contactId->setManagerUri(QString());
            currentContact.setId(*contactId);
        }
    } //while

    CleanupStack::PopAndDestroy(pbBuffer);
    PbkPrintToLog(_L("CntSymbianSimEngine::decodeSimContactsL() - OUT"));
    return fetchedContacts;
}

/*! Converts QContact to the TLV format which is used to save it to the Etel store.
 *
 * \param contact QContact to be converted
 * \param rawData Contact in TLV format on return.
 * \return QContact containing actually saved information.
*/
QContact CntSimStorePrivate::encodeSimContactL(const QContact* contact, TDes8& rawData) const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - IN"));
    QContact convertedContact;
    CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);
    pbBuffer->Set(&rawData);

    //add new enty tag
    User::LeaveIfError(pbBuffer->AddNewEntryTag());

    //add name
    QString name;
    QList<QContactDetail> nameDetails = contact->details(QContactName::DefinitionName);
    if (nameDetails.count() == 0) {
        // TODO: should we leave name empty?
        name.append("Unnamed");
    }
    else {
        QContactName nameDetail = static_cast<QContactName>(nameDetails.at(0));
        name.append(nameDetail.customLabel());
        if (name.isNull()) {
            // TODO: should we leave name empty?
            name.append("Unnamed)");
        }
    }
    name = name.left(m_storeInfo.iMaxTextLength); //trim to the max possible length
    TPtrC nameValue(reinterpret_cast<const TUint16*>(name.utf16()));
    User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, nameValue));

    QContactName convertedNameDetail;
    convertedNameDetail.setCustomLabel(name);
    convertedContact.saveDetail(&convertedNameDetail);

    //add nickname
#ifndef SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
    if (m_storeInfo.iMaxSecondNames > 0) {
        QString nickname;
        QList<QContactDetail> nicknameDetails = contact->details(QContactNickname::DefinitionName);
        if (nicknameDetails.count() > 0) {
            QContactNickname nicknameDetail = static_cast<QContactNickname>(nicknameDetails.at(0));
            nickname = nicknameDetail.nickname();
            nickname = nickname.left(m_storeInfo.iMaxTextLengthSecondName); //trim to the max possible length
            TPtrC nicknameValue(reinterpret_cast<const TUint16*>(nickname.utf16()));
            User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBSecondName, nicknameValue));

            QContactNickname convertedNicknameDetail;
            convertedNicknameDetail.setNickname(nickname);
            convertedContact.saveDetail(&convertedNicknameDetail);
        }
    }
#endif

    //add phone number
    QList<QContactDetail> phoneNumberDetails = contact->details(QContactPhoneNumber::DefinitionName);
    if (phoneNumberDetails.count() > 0) {
        PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - add phone number"));
        QContactPhoneNumber phoneNumberDetail = static_cast<QContactPhoneNumber>(phoneNumberDetails.at(0));
        QString number = phoneNumberDetail.number();
        foreach (const QChar character, number) {
            if(!character.isDigit()) {
                if(character != QChar('+')
                    && character != QChar('*')
                    && character != QChar('#')
                    && character != QChar('p')
                    && character != QChar('w')) {
                    User::Leave(KErrArgument);
                }
            }
        }
        PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - phone number length = %d"),
            phoneNumberDetail.number().length());
        if (phoneNumberDetail.number().length() > m_storeInfo.iMaxNumLength) {
            User::Leave(KErrTooBig);
        }
        TPtrC phoneNumberValue(reinterpret_cast<const TUint16*>(phoneNumberDetail.number().utf16()));
        PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - number = %S"), &phoneNumberValue);
        User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, phoneNumberValue));

        QContactPhoneNumber convertedPhoneNumberDetail;
        convertedPhoneNumberDetail.setNumber(phoneNumberDetail.number());
        convertedContact.saveDetail(&convertedPhoneNumberDetail);
    }

    //add additional numbers
#ifndef SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
    if (m_storeInfo.iMaxAdditionalNumbers > 0) {
        //one number is saved already
        for (int i = 1; i < phoneNumberDetails.count() && i-1 < m_storeInfo.iMaxAdditionalNumbers; ++i) {
            QContactPhoneNumber phoneNumberDetail = static_cast<QContactPhoneNumber>(phoneNumberDetails.at(i));
            if (phoneNumberDetail.number().length() > m_storeInfo.iMaxNumLengthAdditionalNumber) {
                User::Leave(KErrTooBig);
            }
            //mark the beginning of an additional number
            User::LeaveIfError(pbBuffer->AddNewNumberTag());
            //add number itself
            TPtrC phoneNumberValue(reinterpret_cast<const TUint16*>(phoneNumberDetail.number().utf16()));
            User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, phoneNumberValue));

            QContactPhoneNumber convertedPhoneNumberDetail;
            convertedPhoneNumberDetail.setNumber(phoneNumberDetail.number());
            convertedContact.saveDetail(&convertedPhoneNumberDetail);
        }
    }
#endif

    //add e-mails
#ifndef SYMBIANSIM_BACKEND_PHONEBOOKINFOV1
    if (m_storeInfo.iMaxEmailAddr > 0) {
        QList<QContactDetail> emailDetails = contact->details(QContactEmailAddress::DefinitionName);
        for (int i = 0; i < emailDetails.count() && i < m_storeInfo.iMaxEmailAddr; ++i) {
            QContactEmailAddress emailDetail = static_cast<QContactEmailAddress>(emailDetails.at(i));
            TPtrC emailValue(reinterpret_cast<const TUint16*>(emailDetail.emailAddress().utf16()));
            if (emailValue.Length() > m_storeInfo.iMaxTextLengthEmailAddr) {
                User::Leave(KErrTooBig);
            }
            User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBEmailAddress, emailValue));

            QContactEmailAddress convertedEmailDetail;
            convertedEmailDetail.setEmailAddress(emailDetail.emailAddress());
            convertedContact.saveDetail(&convertedEmailDetail);
        }
    }
#endif

    CleanupStack::PopAndDestroy(pbBuffer);
    PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - OUT"));
    return convertedContact;
}

QList<int> CntSimStorePrivate::decodeReservedSlotsL(TDes8& rawData) const
{
    QList<int> reservedSlots;

    TUint8 tagValue(0);
    CPhoneBookBuffer::TPhBkTagType dataType;

    CPhoneBookBuffer* pbBuffer = new (ELeave) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);
    pbBuffer->Set(&rawData);
    pbBuffer->StartRead();

    while (pbBuffer->GetTagAndType(tagValue, dataType) == KErrNone)
    {
        if (tagValue == RMobilePhoneBookStore::ETagPBAdnIndex) 
        {
            TUint16 index;
            if (pbBuffer->GetValue(index) == KErrNone)
                reservedSlots.append(index);
        } else
            pbBuffer->SkipValue(dataType);
    } //while
    
    CleanupStack::PopAndDestroy(pbBuffer);
    return reservedSlots;
}

