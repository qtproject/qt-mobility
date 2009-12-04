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
#include <mpbutil.h>

const int KOneSimContactBufferSize = 500;
const int KMaxSimContactsNumber = 500;
const TInt KDataClientBuf  = 128;
const TInt KEtsiTonPosition = 0x70;

CntSymbianSimEngine::CntSymbianSimEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
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
        etelStore.Open(etelPhone, KETelIccAdnPhoneBook);
        }
    if (err != KErrNone) {
        error = QContactManager::UnspecifiedError;
    }
    
    m_managerUri = QContactManager::buildUri(CNT_SYMBIANSIM_MANAGER_NAME, parameters);
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
    QList<QContactLocalId> contactIds; 

    // Get unsorted contacts
    QList<QContact> contacts;
    TRAPD(err, QT_TRYCATCH_LEAVING(contacts = fetchContactsL()));
    transformError(err, error);
    
    // Sort contacts
    if (err == KErrNone) {
        contactIds = QContactManagerEngine::sortContacts(contacts, sortOrders);
    }

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
    return *QScopedPointer<QContact>(contact);
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
    //read the contact from the Etel store
    TRequestStatus requestStatus;
    RBuf8 buffer;
    buffer.CreateL(KOneSimContactBufferSize*KMaxSimContactsNumber);
    CleanupClosePushL(buffer);
    etelStore.Read(requestStatus, 1, KMaxSimContactsNumber, buffer);
    User::WaitForRequest(requestStatus);
    if (requestStatus.Int() != KErrNone) {
        User::Leave(requestStatus.Int());
    }

    //process contact data
    QList<QContact> contacts = decodeSimContactsL(buffer);
   
    CleanupStack::PopAndDestroy(); //buffer
    return contacts;
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
    QList<QContact> fetchedContacts;
    QContact currentContact;

    TBuf16<KDataClientBuf> buffer;
    TPtrC16 bufPtr(buffer);

    TUint8 tagValue(0);
    CPhoneBookBuffer::TPhBkTagType dataType;

    bool isAdditionalNumber = false;
    QContactPhoneNumber* lastPhoneNumber = 0;

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
                break;
            }
            case RMobilePhoneBookStore::ETagPBTonNpi:
            {
                // Check number type, we are only interested
                // if it's international or not.
                TUint8  tonNpi;
                if ( pbBuffer->GetValue(tonNpi) == KErrNone) { 
                    if (lastPhoneNumber != 0) {
                        TUint8  intFlag = (tonNpi & KEtsiTonPosition) >> 4;
                        if (intFlag == 1) {
                            //international number format, append "+"
                            QString number = lastPhoneNumber->number();
                            number.insert(0, "+");
                            lastPhoneNumber->setNumber(number);
                        }
                        currentContact.saveDetail(lastPhoneNumber);
                    }
                }

                delete lastPhoneNumber;
                lastPhoneNumber = 0;
                 
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
                    if (lastPhoneNumber == 0) {
                        lastPhoneNumber = new QContactPhoneNumber();
                    }
                    QString number = QString::fromUtf16(bufPtr.Ptr(), bufPtr.Length());
                    lastPhoneNumber->setNumber(number);
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
    delete lastPhoneNumber;
    lastPhoneNumber = 0;
    return fetchedContacts;
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
