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

#include "cntsymbiansimengine.h"
#include <qtcontacts.h>
#include <mmtsy_names.h>

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <..\tsrc\ETelTestServer\Client\Inc\mpbutil_etel_test_server.h>
#else
#include <mpbutil.h>
#endif

const int KOneSimContactBufferSize = 512;
const TInt KDataClientBuf  = 128;
const TInt KEtsiTonPosition = 0x70;
Q_DEFINE_LATIN1_LITERAL(KSimSyncTarget, "SIM");

#include <flogger.h>
#include <f32file.h>
namespace {
    void PbkPrintToLog( TRefByValue<const TDesC> aFormat, ... )
    {
        _LIT( KLogDir, "Sim" );
        _LIT( KLogName, "sim.log" );

        VA_LIST args;
        VA_START( args, aFormat );
        RFileLogger::WriteFormat(KLogDir, KLogName, EFileLoggingModeAppend, aFormat, args);
        VA_END( args );
    }
}  // namespace

CntSymbianSimEngine::CntSymbianSimEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error) :
    etelInfoPckg( etelStoreInfo )
{
    error = QContactManager::NoError;

    int err = etelServer.Connect();
    if (err == KErrNone) {
        err = etelServer.LoadPhoneModule(KMmTsyModuleName);
    }
    if (err == KErrNone) {
        RTelServer::TPhoneInfo info;
        err = etelServer.GetPhoneInfo(0, info);
        if (err == KErrNone) {
            err = etelPhone.Open(etelServer, info.iName);
        }
    }
    if (err == KErrNone) {
        // open Etel store - TODO: check from parameters what Etel store to use
        err = etelStore.Open(etelPhone, KETelIccAdnPhoneBook);
        }
    if (err != KErrNone) {
        error = QContactManager::UnspecifiedError;
    }

    m_managerUri = QContactManager::buildUri(CNT_SYMBIANSIM_MANAGER_NAME, parameters);

    RFs fs;
    fs.Connect();
    fs.MkDir(_L("C:\\Logs\\"));
    fs.MkDir(_L("C:\\Logs\\Sim\\"));
    fs.Close();
    PbkPrintToLog(_L("CntSymbianSimEngine::CntSymbianSimEngine, err = %d"), err);
}

CntSymbianSimEngine::~CntSymbianSimEngine()
{
    etelStore.Close();
    etelPhone.Close();
    etelServer.Close();
}

void CntSymbianSimEngine::deref()
{
    delete this;
}

QString CntSymbianSimEngine::managerName() const
{
    return CNT_SYMBIANSIM_MANAGER_NAME;
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrders.
 * Any error that occurs will be stored in \a error. Uses the generic (slow) filtering of QContactManagerEngine.
 */
QList<QContactLocalId> CntSymbianSimEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::contacts"));

    QList<QContactLocalId> contactIds;

    // Get unsorted and not filtered contacts
    QList<QContact> contacts;
    TRAPD(err, QT_TRYCATCH_LEAVING(contacts = fetchContactsL()));
    transformError(err, error);

    // Filter and sort contacts
    if (err == KErrNone) {
        for (int i(0); i < contacts.count(); i++) {
            if (!QContactManagerEngine::testFilter(filter, contacts.at(i))) {
                contacts.removeAt(i);
                i--;
            }
        }
        contactIds = QContactManagerEngine::sortContacts(contacts, sortOrders);
    }

    return contactIds;
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter.
 * Any error that occurs will be stored in \a error.
 */
QList<QContactLocalId> CntSymbianSimEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::contacts"));

    QList<QContactLocalId> contactIds;

    // Get unsorted contacts
    QList<QContact> contacts;
    TRAPD(err, QT_TRYCATCH_LEAVING(contacts = fetchContactsL()));
    transformError(err, error);

    // Sort contacts
    if (err == KErrNone) {
        contactIds = QContactManagerEngine::sortContacts(contacts, sortOrders);
    }
    PbkPrintToLog(_L("CntSymbianSimEngine::contacts() - err = %d"), err);
    return contactIds;
}

/*!
 * Reads a contact from the Etel store.
 *
 * \param contactId The Id of the contact to be retrieved.
 * \param error Qt error code.
 * \return A QContact for the requested QContactLocalId value or 0 if the read
 *  operation was unsuccessful (e.g. contact not found).
 */
QContact CntSymbianSimEngine::contact(const QContactLocalId& contactId, QContactManager::Error& error) const
{
    QContact* contact = new QContact();
    TRAPD(err, QT_TRYCATCH_LEAVING(*contact = fetchContactL(contactId)));
    transformError(err, error);
    PbkPrintToLog(_L("CntSymbianSimEngine::contact() - err = %d"), err);
    return *QScopedPointer<QContact>(contact);
}

QString CntSymbianSimEngine::synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    Q_UNUSED(error);

    // TODO: localize unnamed
    QString label("Unnamed");
    QContactName name = contact.detail(QContactName::DefinitionName);
    if(!name.first().isEmpty()) {
        label = name.first();
    }
    return label;
}

/*!
 * Saves the contact to the Etel store. Only part of the contact's details
 * can be saved, and some fields may be trimmed to fit to the SIM card.
 *
 * \param contact Contact to be saved.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool CntSymbianSimEngine::saveContact(QContact* contact, QContactManager::Error& error)
{
    TRAPD(err, QT_TRYCATCH_LEAVING(saveContactL(contact)));
    transformError(err, error);
    PbkPrintToLog(_L("CntSymbianSimEngine::saveContact() - err = %d"), err);
    return (err == KErrNone);
}

/*!
 * Removes the specified contact object from the Etel store.
 *
 * \param contactId Id of the contact to be removed.
 * \param qtError Qt error code.
 * \return Error status.
 */
bool CntSymbianSimEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error& error)
{
    TRequestStatus requestStatus;
    etelStore.Delete(requestStatus, contactId);
    User::WaitForRequest(requestStatus);
    transformError(requestStatus.Int(), error);
    return (requestStatus.Int() == KErrNone);
}

/*!
 * Private leaving implementation for contact()
 */
QContact CntSymbianSimEngine::fetchContactL(const QContactLocalId &localId) const
{
    if(localId == 0) {
        User::Leave(KErrNotFound);
    }

    //read the contact from the Etel store
    TRequestStatus requestStatus;
    RBuf8 buffer;
    buffer.CreateL(KOneSimContactBufferSize);
    CleanupClosePushL(buffer);
    etelStore.Read(requestStatus, localId, 1, buffer);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone) {
        User::Leave(requestStatus.Int());
    }

    //process contact data
    QList<QContact> contacts = decodeSimContactsL(buffer);
    if (contacts.count() == 0) {
        User::Leave(KErrNotFound);
    }

    CleanupStack::PopAndDestroy(); //buffer
    //there should only 1 contact in the list
    return contacts.at(0);
}

/*!
 * Private leaving implementation for contacts()
 */
QList<QContact> CntSymbianSimEngine::fetchContactsL() const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::fetchContactsL() - IN"));

    TRequestStatus requestStatus;

    //check number of storage slots in the store
    etelStore.GetInfo(requestStatus, (TDes8&)etelInfoPckg);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone) {
        PbkPrintToLog(_L("CntSymbianSimEngine::fetchContactsL() - getInfo error = %d"),
                requestStatus.Int());
        User::Leave(requestStatus.Int());
    }

    PbkPrintToLog(_L("CntSymbianSimEngine::fetchContactsL() - totalEntries = %d"),
            etelStoreInfo.iTotalEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::fetchContactsL() - usedEntries = %d"),
            etelStoreInfo.iUsedEntries);

    QList<QContact> contacts;
    if(etelStoreInfo.iUsedEntries > 0) {
        //read the contacts from the Etel store
        RBuf8 buffer;
        buffer.CreateL(KOneSimContactBufferSize*etelStoreInfo.iUsedEntries);
        CleanupClosePushL(buffer);
        //contacts are fetched starting from index 1, all slots should be checked
        //since slots may be not filled in a sequence.
        etelStore.Read(requestStatus, 1, etelStoreInfo.iTotalEntries, buffer);
        User::WaitForRequest(requestStatus);
        if (requestStatus.Int() != KErrNone) {
            User::Leave(requestStatus.Int());
        }

        //process contact data
        contacts = decodeSimContactsL(buffer);

        CleanupStack::PopAndDestroy(); //buffer
    }

    PbkPrintToLog(_L("CntSymbianSimEngine::fetchContactsL() - OUT, count = %d"), contacts.count());
    return contacts;
}

/*!
 * Private leaving implementation for saveContact()
 */
void CntSymbianSimEngine::saveContactL(QContact* contact) const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - IN"));

    TRequestStatus requestStatus;

    //check what information can be saved to the Etel store
    etelStore.GetInfo(requestStatus, (TDes8&)etelInfoPckg);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone) {
        PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - getInfo error = %d"),
                requestStatus.Int());
        User::Leave(requestStatus.Int());
    }

    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - totalEntries = %d"),
            etelStoreInfo.iTotalEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - usedEntries = %d"),
            etelStoreInfo.iUsedEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - MaxNumLength = %d"),
                etelStoreInfo.iMaxNumLength);
    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - MaxTextLength = %d"),
                etelStoreInfo.iMaxTextLength);

    //encode contact
    RBuf8 buffer;
    buffer.CreateL(KOneSimContactBufferSize);
    CleanupClosePushL(buffer);
    QContact convertedContact = encodeSimContactL(contact, buffer);

    //write contact
    int index(KErrNotFound);
    if (contact->localId() > 0) {
        index = contact->localId();
    }
    etelStore.Write(requestStatus, buffer, index);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone) {
        PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - write error = %d"),
                    requestStatus.Int());
        User::Leave(requestStatus.Int());
    }
    CleanupStack::PopAndDestroy(); //buffer

    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - index = %d"), index);

    //update provided contact with the details what were really saved
    contact->clearDetails();
    QList<QContactDetail> details = convertedContact.details();
    foreach(QContactDetail detail, details) {
        contact->saveDetail(&detail);
    }
    QContactSyncTarget syncTarget;
    syncTarget.setSyncTarget(KSimSyncTarget);
    contact->saveDetail(&syncTarget);

    //save id, if new contact
    if (contact->localId() == 0) {
        QScopedPointer<QContactId> contactId(new QContactId());
        //index contains the location of saved contact
        contactId->setLocalId(index);
        contactId->setManagerUri(m_managerUri);
        contact->setId(*contactId);
    }
    PbkPrintToLog(_L("CntSymbianSimEngine::saveContactL() - OUT"));
}

/*! Transform a Symbian contact error id to QContactManager::Error.
 *
 * \param symbianError Symbian error.
 * \param QtError Qt error.
*/
void CntSymbianSimEngine::transformError(TInt symbianError, QContactManager::Error& qtError) const
{
    switch(symbianError)
    {
        case KErrNone:
        {
            qtError = QContactManager::NoError;
            break;
        }
        case KErrNotFound:
        {
            qtError = QContactManager::DoesNotExistError;
            break;
        }
        case KErrAlreadyExists:
        {
            qtError = QContactManager::AlreadyExistsError;
            break;
        }
        case KErrLocked:
        {
            qtError = QContactManager::LockedError;
            break;
        }
        case KErrAccessDenied:
        case KErrPermissionDenied:
        {
            qtError = QContactManager::PermissionsError;
            break;
        }
        case KErrNoMemory:
        {
            qtError = QContactManager::OutOfMemoryError;
            break;
        }
        case KErrNotSupported:
        {
            qtError = QContactManager::NotSupportedError;
            break;
        }
        case KErrArgument:
        {
            qtError = QContactManager::BadArgumentError;
            break;
        }
        default:
        {
            qtError = QContactManager::UnspecifiedError;
            break;
        }
    }
}

/*! Parses SIM contacts in TLV format.
 *
 * \param rawData SIM contacts in TLV format.
 * \return List of contacts.
*/
QList<QContact> CntSymbianSimEngine::decodeSimContactsL(TDes8& rawData) const
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
                        name.setFirst(nameString);
                        currentContact.saveDetail(&name);
QContactManager::Error error(QContactManager::NoError);
currentContact = setContactDisplayLabel(synthesizeDisplayLabel(currentContact, error), currentContact);
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
                    currentContact.saveDetail(&nickName);
                }
                break;
            }
            case RMobilePhoneBookStore::ETagPBNumber:
            {
                if (pbBuffer->GetValue(bufPtr) == KErrNone) {
                    QContactPhoneNumber phoneNumber;
                    QString number = QString::fromUtf16(bufPtr.Ptr(), bufPtr.Length());
                    phoneNumber.setNumber(number);
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
QContact CntSymbianSimEngine::encodeSimContactL(const QContact* contact, TDes8& rawData) const
{
    PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - IN"));
    QContact convertedContact;
    CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer();
    CleanupStack::PushL(pbBuffer);
    pbBuffer->Set(&rawData);

    //add new enty tag
    User::LeaveIfError(pbBuffer->AddNewEntryTag());

    //add first and last name
    QString name;
    QList<QContactDetail> nameDetails = contact->details(QContactName::DefinitionName);
    if (nameDetails.count() == 0) {
        name.append("(unnamed)");
    }
    else {
        QContactName nameDetail = static_cast<QContactName>(nameDetails.at(0));
        name.append(nameDetail.first());
        if (nameDetail.last().length() > 0) {
            if (nameDetail.first().length() > 0) {
                name.append(" "); //separator
            }
            name.append(nameDetail.last());
        }
        if (name.length() == 0) {
            name.append("(unnamed)");
        }
    }
    name = name.left(etelStoreInfo.iMaxTextLength); //trim to the max possible length
    TPtrC nameValue(reinterpret_cast<const TUint16*>(name.utf16()));
    User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, nameValue));

    QContactName convertedNameDetail;
    convertedNameDetail.setFirst(name);
    convertedContact.saveDetail(&convertedNameDetail);

    // TODO: not supported currently by etel test server
    //add nickname
    if (etelStoreInfo.iMaxSecondNames > 0) {
        QString nickname;
        QList<QContactDetail> nicknameDetails = contact->details(QContactNickname::DefinitionName);
        if (nicknameDetails.count() > 0) {
            QContactNickname nicknameDetail = static_cast<QContactNickname>(nicknameDetails.at(0));
            nickname = nicknameDetail.nickname();
            nickname = nickname.left(etelStoreInfo.iMaxTextLengthSecondName); //trim to the max possible length
            TPtrC nicknameValue(reinterpret_cast<const TUint16*>(nickname.utf16()));
            User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBSecondName, nicknameValue));

            QContactNickname convertedNicknameDetail;
            convertedNicknameDetail.setNickname(nickname);
            convertedContact.saveDetail(&convertedNicknameDetail);
        }
    }

    //add phone number
    QList<QContactDetail> phoneNumberDetails = contact->details(QContactPhoneNumber::DefinitionName);
    if (phoneNumberDetails.count() > 0) {
        PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - add phone number"));
        QContactPhoneNumber phoneNumberDetail = static_cast<QContactPhoneNumber>(phoneNumberDetails.at(0));
        QString number = phoneNumberDetail.number();
        PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - phone number length = %d"),
                phoneNumberDetail.number().length());
        if (phoneNumberDetail.number().length() > etelStoreInfo.iMaxNumLength) {
            // if number cannot fit, do no save a contact
            User::LeaveIfError(KErrTooBig);
        }
        TPtrC phoneNumberValue(reinterpret_cast<const TUint16*>(phoneNumberDetail.number().utf16()));
        PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - number = %S"), &phoneNumberValue);
        User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber, phoneNumberValue));

        QContactPhoneNumber convertedPhoneNumberDetail;
        convertedPhoneNumberDetail.setNumber(phoneNumberDetail.number());
        convertedContact.saveDetail(&convertedPhoneNumberDetail);
    }

    // TODO: not supported currently by etel test server
    //add additional numbers
    if (etelStoreInfo.iMaxAdditionalNumbers > 0) {
        //one number is saved already
        for (int i = 1; i < phoneNumberDetails.count() && i-1 < etelStoreInfo.iMaxAdditionalNumbers; ++i) {
            QContactPhoneNumber phoneNumberDetail = static_cast<QContactPhoneNumber>(phoneNumberDetails.at(i));
            if (phoneNumberDetail.number().length() <= etelStoreInfo.iMaxNumLengthAdditionalNumber) {
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
    }

    // TODO: not supported currently by etel test server
    //add e-mails
    if (etelStoreInfo.iMaxEmailAddr > 0) {
        QList<QContactDetail> emailDetails = contact->details(QContactEmailAddress::DefinitionName);
        for (int i = 0; i < emailDetails.count() && i < etelStoreInfo.iMaxEmailAddr; ++i) {
        QContactEmailAddress emailDetail = static_cast<QContactEmailAddress>(emailDetails.at(i));
            if (emailDetail.emailAddress().length() <= etelStoreInfo.iMaxTextLengthEmailAddr) {
                TPtrC emailValue(reinterpret_cast<const TUint16*>(emailDetail.emailAddress().utf16()));
                User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBEmailAddress, emailValue));

                QContactEmailAddress convertedEmailDetail;
                convertedEmailDetail.setEmailAddress(emailDetail.emailAddress());
                convertedContact.saveDetail(&convertedEmailDetail);
            }
        }
    }

    CleanupStack::PopAndDestroy(pbBuffer);
    PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - OUT"));
    return convertedContact;

}

QContactManagerEngine* CntSymbianSimFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new CntSymbianSimEngine(parameters, error);
}

QString CntSymbianSimFactory::managerName() const
{
    return CNT_SYMBIANSIM_MANAGER_NAME;
}

Q_EXPORT_PLUGIN2(mobapicontactspluginsymbiansim, CntSymbianSimFactory);
