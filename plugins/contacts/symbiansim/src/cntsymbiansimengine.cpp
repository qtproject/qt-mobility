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

#include "cntsymbiansimengine.h"
#include <qtcontacts.h>
#include <mmtsy_names.h>

#include "cntsymbiansimtransformerror.h"
#include "cntsimstore.h"
#include "cntsimcontactfetchrequest.h"
#include "cntsimcontactlocalidfetchrequest.h"
#include "cntsimcontactsaverequest.h"
#include "cntsimcontactremoverequest.h"
#include "cntsimdetaildefinitionfetchrequest.h"

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <mpbutil_etel_test_server.h>
#else
#include <mpbutil.h>
#endif

#include <QEventLoop>
#include <QTimer>

const int KOneSimContactBufferSize = 512;
const TInt KDataClientBuf  = 128;
const TInt KEtsiTonPosition = 0x70;
Q_DEFINE_LATIN1_LITERAL(KSimSyncTarget, "SIM");
Q_DEFINE_LATIN1_LITERAL(KParameterKeySimStoreName, "store");
Q_DEFINE_LATIN1_LITERAL(KParameterValueSimStoreNameAdn, "ADN");
Q_DEFINE_LATIN1_LITERAL(KParameterValueSimStoreNameSdn, "SDN");
Q_DEFINE_LATIN1_LITERAL(KParameterValueSimStoreNameFdn, "FDN");

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

CntSymbianSimEngineData::CntSymbianSimEngineData()
    :m_etelStoreInfoPckg( m_etelStoreInfo ),
    m_simStore(0)
{
    
}

CntSymbianSimEngineData::~CntSymbianSimEngineData()
{
    if (ref == 0) {
        m_etelStore.Close();
        m_etelPhone.Close();
        m_etelServer.Close();
        
        // Remove all unfinished requests.
        // If the client has behaved itself then there should be none left.
        while (m_asyncRequests.size()) 
        {
            QMap<QContactAbstractRequest *, CntAbstractSimRequest *>::iterator itr = m_asyncRequests.begin();
            delete itr.value();
            m_asyncRequests.remove(itr.key());
            qWarning("Dangling async request!");
        }        
    }
}

CntSymbianSimEngine::CntSymbianSimEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
	d = new CntSymbianSimEngineData();
    error = QContactManager::NoError;
    TRAPD(err, initializeL(parameters));
    CntSymbianSimTransformError::transformError(err, error);
}

CntSymbianSimEngine::CntSymbianSimEngine(const CntSymbianSimEngine &other)
    :d(other.d)
{
    
}

void CntSymbianSimEngine::initializeL(const QMap<QString, QString> &parameters)
{
	
    TBuf<RMobilePhoneBookStore::KMaxPBIDSize> storeName;
    convertStoreNameL(parameters, storeName);

    // Open etel server
    User::LeaveIfError(d->m_etelServer.Connect());
    User::LeaveIfError(d->m_etelServer.LoadPhoneModule(KMmTsyModuleName));

    // Open etel phone
    RTelServer::TPhoneInfo info;
    User::LeaveIfError(d->m_etelServer.GetPhoneInfo(0, info));
    User::LeaveIfError(d->m_etelPhone.Open(d->m_etelServer, info.iName));
    
    // open Etel store
    User::LeaveIfError(d->m_etelStore.Open(d->m_etelPhone, storeName));
    
    d->m_managerUri = QContactManager::buildUri(managerName(), parameters);
    
    d->m_simStore = new CntSimStore(this);
    
    getEtelStoreInfoL();

    if(storeName.Match(KETelIccSdnPhoneBook) != KErrNotFound) {
        // In case of SDN store we need to check if any SDN contacts exist to
        // determine if the store is supported or not
        QContactManager::Error error(QContactManager::NoError);
        if(d->m_etelStoreInfo.iUsedEntries == 0) {
            User::Leave(KErrNotSupported);
        }
    }
	
}

void CntSymbianSimEngine::convertStoreNameL(const QMap<QString, QString> &parameters, TDes &storeName)
{
    if(storeName.MaxLength() < RMobilePhoneBookStore::KMaxPBIDSize) {
        User::Leave(KErrArgument);
    }

    // Parse parameters; the only allowed parameter key is "store"
    if (parameters.count() == 0) {
        // Default to ADN store
        storeName.Copy(KETelIccAdnPhoneBook);
    } else if(parameters.count() == 1) {
        if (parameters.contains(KParameterKeySimStoreName)) {
            if (parameters.value(KParameterKeySimStoreName) == KParameterValueSimStoreNameFdn) {
                storeName.Copy(KETelIccFdnPhoneBook);
            } else if (parameters.value(KParameterKeySimStoreName) == KParameterValueSimStoreNameAdn) {
                storeName.Copy(KETelIccAdnPhoneBook);
            } else if (parameters.value(KParameterKeySimStoreName) == KParameterValueSimStoreNameSdn) {
                storeName.Copy(KETelIccSdnPhoneBook);
            }
        }
    }

    // Check that we got a valid store name
    if(storeName.Length() == 0) {
        User::Leave(KErrArgument);
    }
}



CntSymbianSimEngine::~CntSymbianSimEngine()
{

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
 * Returns a list of the ids of contacts that match the supplied \a filter.
 * Any error that occurs will be stored in \a error.
 */
QList<QContactLocalId> CntSymbianSimEngine::contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QContactLocalIdFetchRequest req;
    req.setSorting(sortOrders);
    executeRequest(&req, error);
    return req.ids();
}

/*!
 * Returns a list of the ids of contacts that match the supplied \a filter, sorted according to the given \a sortOrders.
 * Any error that occurs will be stored in \a error. Uses the generic (slow) filtering of QContactManagerEngine.
 */
QList<QContactLocalId> CntSymbianSimEngine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QContactLocalIdFetchRequest req;
    req.setFilter(filter);
    req.setSorting(sortOrders);
    executeRequest(&req, error);
    return req.ids();
}

QList<QContact> CntSymbianSimEngine::contacts(const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error& error) const
{
    QContactFetchRequest req;
    req.setSorting(sortOrders);
    req.setDefinitionRestrictions(definitionRestrictions);
    executeRequest(&req, error);
    return req.contacts();
}

QList<QContact> CntSymbianSimEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QStringList& definitionRestrictions, QContactManager::Error& error) const
{
    QContactFetchRequest req;
    req.setFilter(filter);
    req.setSorting(sortOrders);
    req.setDefinitionRestrictions(definitionRestrictions);
    executeRequest(&req, error);
    return req.contacts();
}

/*!
 * Reads a contact from the Etel store.
 *
 * \param contactId The Id of the contact to be retrieved.
 * \param definitionRestrictions Definition restrictions.
 * \param error Qt error code.
 * \return A QContact for the requested QContactLocalId value or 0 if the read
 *  operation was unsuccessful (e.g. contact not found).
 */
QContact CntSymbianSimEngine::contact(const QContactLocalId& contactId, const QStringList& definitionRestrictions, QContactManager::Error& error) const
{
    QContactFetchRequest req;
    QContactLocalIdFilter filter;
    filter.setIds(QList<QContactLocalId>() << contactId);
    req.setFilter(filter);
    req.setDefinitionRestrictions(definitionRestrictions);
    executeRequest(&req, error);
    if (req.contacts().count() == 0)
        return QContact();
    return req.contacts().at(0); 
}

QString CntSymbianSimEngine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    Q_UNUSED(error);

    QContactName name = contact.detail(QContactName::DefinitionName);
    if(!name.customLabel().isEmpty()) {
        return name.customLabel();
    } else {
        // TODO: localize unnamed
        return QString("Unnamed");
    }
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
    if (!contact) {
        error = QContactManager::BadArgumentError;
        return false;
    }
    
    QContactSaveRequest req;
    req.setContacts(QList<QContact>() << *contact);
    executeRequest(&req, error);
    if (req.contacts().count())
        *contact = req.contacts().at(0);
    return (error == QContactManager::NoError);
}

bool CntSymbianSimEngine::saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error& error)
{
    if (!contacts) {
        error = QContactManager::BadArgumentError;
        return false;
    }
    
    QContactSaveRequest req;
    req.setContacts(*contacts);
    executeRequest(&req, error);
    if (errorMap)
        *errorMap = req.errorMap();
    return (error == QContactManager::NoError );    
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
    QContactRemoveRequest req;
    req.setContactIds(QList<QContactLocalId>() << contactId);
    return executeRequest(&req, error);
}

bool CntSymbianSimEngine::removeContacts(QList<QContactLocalId>* contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error& error)
{
    if (!contactIds) {
        error = QContactManager::BadArgumentError;
        return false;
    }    
    QContactRemoveRequest req;
    req.setContactIds(*contactIds);
    executeRequest(&req, error);
    if (errorMap)
        *errorMap = req.errorMap();    
    return (error == QContactManager::NoError);    
}

/*!
 * Returns a map of identifier to detail definition which are valid for contacts whose type is the given \a contactType
 * which are valid for the contacts in this store
 */
QMap<QString, QContactDetailDefinition> CntSymbianSimEngine::detailDefinitions(const QString& contactType, QContactManager::Error& error) const
{
    if (!supportedContactTypes().contains(contactType)) {
        // Should never happen
        error = QContactManager::NotSupportedError;
        return QMap<QString, QContactDetailDefinition>();
    }

    TRAPD(err, getEtelStoreInfoL());
    if(err != KErrNone) {
        CntSymbianSimTransformError::transformError(err, error);
        return QMap<QString, QContactDetailDefinition>();
    }

    // the map we will eventually return
    QMap<QString, QContactDetailDefinition> retn;

    // local variables for reuse
    QMap<QString, QContactDetailFieldDefinition> fields;
    QContactDetailFieldDefinition f;
    QContactDetailDefinition def;
    QVariantList subTypes;

    // sync target
    def.setName(QContactSyncTarget::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    subTypes.clear();
    subTypes << QString(QLatin1String(KSimSyncTarget));
    f.setAllowableValues(subTypes);
    fields.insert(QContactSyncTarget::FieldSyncTarget, f);
    def.setFields(fields);
    def.setUnique(true);
    retn.insert(def.name(), def);

    // type
    def.setName(QContactType::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    subTypes.clear();
    // groups are not supported
    subTypes << QString(QLatin1String(QContactType::TypeContact));
    f.setAllowableValues(subTypes);
    fields.insert(QContactType::FieldType, f); // note: NO CONTEXT!!
    def.setFields(fields);
    def.setUnique(true);
    retn.insert(def.name(), def);

/* TODO
    // guid
    def.setName(QContactGuid::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactGuid::FieldGuid, f);
    f.setDataType(QVariant::StringList);
    f.setAllowableValues(contexts);
    fields.insert(QContactDetail::FieldContext, f);
    def.setFields(fields);
    def.setUnique(false);
    def.setAccessConstraint(QContactDetailDefinition::CreateOnly);
    retn.insert(def.name(), def);
*/

    // display label
    def.setName(QContactDisplayLabel::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactDisplayLabel::FieldLabel, f);
    def.setFields(fields);
    def.setUnique(true);
    retn.insert(def.name(), def);

    // email support needs to be checked run-time, because it is SIM specific
    if (d->m_etelStoreInfo.iMaxEmailAddr > 0) {
        def.setName(QContactEmailAddress::DefinitionName);
        fields.clear();
        f.setDataType(QVariant::String);
        f.setAllowableValues(QVariantList());
        fields.insert(QContactEmailAddress::FieldEmailAddress, f);
        def.setFields(fields);
        def.setUnique(true);
        retn.insert(def.name(), def);
    }

    // phone number
    def.setName(QContactPhoneNumber::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactPhoneNumber::FieldNumber, f);
    // TODO: subtypes supported in case a sim contact can have multiple phone numbers?
    def.setFields(fields);
    if (d->m_etelStoreInfo.iMaxAdditionalNumbers > 0) {
        // multiple numbers supported
        def.setUnique(false);
    } else {
        // only one phone number allowed
        def.setUnique(true);
    }
    retn.insert(def.name(), def);

    // nickname support needs to be checked run-time, because it is SIM specific
    if (d->m_etelStoreInfo.iMaxSecondNames > 0) {
        def.setName(QContactNickname::DefinitionName);
        fields.clear();
        f.setDataType(QVariant::String);
        f.setAllowableValues(QVariantList());
        fields.insert(QContactNickname::FieldNickname, f);
        def.setFields(fields);
        def.setUnique(true);
        retn.insert(def.name(), def);
    }

    // name
    def.setName(QContactName::DefinitionName);
    fields.clear();
    f.setDataType(QVariant::String);
    f.setAllowableValues(QVariantList());
    fields.insert(QContactName::FieldCustomLabel, f);
    def.setFields(fields);
    def.setUnique(true);
    retn.insert(def.name(), def);

    return retn;
}

void CntSymbianSimEngine::requestDestroyed(QContactAbstractRequest* req)
{
    if (d->m_asyncRequests.contains(req)) {
        delete d->m_asyncRequests.take(req); 
    }
}

bool CntSymbianSimEngine::startRequest(QContactAbstractRequest* req)
{
    // Don't allow two async requests to be active at the same time.
    // The RMobilePhoneBookStore cannot handle it.
    foreach (QContactAbstractRequest* r, d->m_asyncRequests.keys()) {
        if (r->isActive()) {
            // TODO: Should we set the error for the request also?
            return false;
        }
    }
    
    // Check for existing request and start again
    if (d->m_asyncRequests.contains(req)) {
        return d->m_asyncRequests.value(req)->start();
    }
    
    // Existing request not found. Create a new one.
    CntAbstractSimRequest* simReq = 0;
    switch (req->type()) 
    {
        case QContactAbstractRequest::ContactFetchRequest:
        {
            QContactFetchRequest* r = static_cast<QContactFetchRequest*>(req);
            simReq = new CntSimContactFetchRequest(this, r);
        }
        break;

        case QContactAbstractRequest::ContactLocalIdFetchRequest:
        {
            QContactLocalIdFetchRequest* r = static_cast<QContactLocalIdFetchRequest*>(req);
            simReq = new CntSimContactLocalIdFetchRequest(this, r);
        }
        break;

        case QContactAbstractRequest::ContactSaveRequest:
        {
            QContactSaveRequest* r = static_cast<QContactSaveRequest*>(req);
            simReq = new CntSimContactSaveRequest(this, r);
        }
        break;

        case QContactAbstractRequest::ContactRemoveRequest:
        {
            QContactRemoveRequest* r = static_cast<QContactRemoveRequest*>(req);
            simReq = new CntSimContactRemoveRequest(this, r);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionFetchRequest:
        {
            QContactDetailDefinitionFetchRequest* r = static_cast<QContactDetailDefinitionFetchRequest*>(req);
            simReq = new CntSimDetailDefinitionFetchRequest(this, r);
        }
        break;

        case QContactAbstractRequest::DetailDefinitionSaveRequest:
        case QContactAbstractRequest::DetailDefinitionRemoveRequest:
        case QContactAbstractRequest::RelationshipFetchRequest:
        case QContactAbstractRequest::RelationshipSaveRequest:
        case QContactAbstractRequest::RelationshipRemoveRequest:
        // fall through.
        default: // unknown request type.
        break;
    }
    
    if (simReq) {
        d->m_asyncRequests.insert(req, simReq);
        return simReq->start();
    }
        
    return false;
}

bool CntSymbianSimEngine::cancelRequest(QContactAbstractRequest* req)
{
    if (d->m_asyncRequests.contains(req))
        return d->m_asyncRequests.value(req)->cancel();
    return false;
}

bool CntSymbianSimEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    if (!d->m_asyncRequests.contains(req)) 
        return false;
    
    if (req->state() != QContactAbstractRequest::ActiveState)
        return false;
    
    QEventLoop *loop = new QEventLoop(this);
    QObject::connect(req, SIGNAL(resultsAvailable()), loop, SLOT(quit()));

    // NOTE: zero means wait forever
    if (msecs > 0)
        QTimer::singleShot(msecs, loop, SLOT(quit()));

    loop->exec();
    loop->disconnect();
    loop->deleteLater();

    return (req->state() == QContactAbstractRequest::FinishedState);
}

/*!
 * Returns true if the given feature \a feature is supported by the manager,
 * for the specified type of contact \a contactType
 */
bool CntSymbianSimEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    Q_UNUSED(feature);
    Q_UNUSED(contactType);
    // We don't support anything in the ManagerFeature
    return false;
}

/*!
 * Returns the list of data types supported by the manager
 */
QStringList CntSymbianSimEngine::supportedContactTypes() const
{
    // TODO: groups supported by some USIM cards?
    return QStringList() << QContactType::TypeContact;
}

void CntSymbianSimEngine::getEtelStoreInfoL() const
{
    TRequestStatus requestStatus;

    //check what information can be saved to the Etel store
    d->m_etelStore.GetInfo(requestStatus, (TDes8&)d->m_etelStoreInfoPckg);
    User::WaitForRequest(requestStatus);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - GetInfo err = %d"),
        requestStatus.Int());
    User::LeaveIfError(requestStatus.Int());

    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - TotalEntries = %d"),
            d->m_etelStoreInfo.iTotalEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - UsedEntries = %d"),
            d->m_etelStoreInfo.iUsedEntries);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - MaxNumLength = %d"),
            d->m_etelStoreInfo.iMaxNumLength);
    PbkPrintToLog(_L("CntSymbianSimEngine::getEtelStoreInfoL() - MaxTextLength = %d"),
            d->m_etelStoreInfo.iMaxTextLength);
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
                    contactId->setManagerUri(d->m_managerUri);
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
                        name.setCustomLabel(nameString);
                        currentContact.saveDetail(&name);
                        QContactManager::Error error(QContactManager::NoError);
                        currentContact = setContactDisplayLabel(synthesizedDisplayLabel(currentContact, error), currentContact);
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
                    phoneNumber.setSubTypes( QContactPhoneNumber::SubTypeMobile );
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
    name = name.left(d->m_etelStoreInfo.iMaxTextLength); //trim to the max possible length
    TPtrC nameValue(reinterpret_cast<const TUint16*>(name.utf16()));
    User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, nameValue));

    QContactName convertedNameDetail;
    convertedNameDetail.setCustomLabel(name);
    convertedContact.saveDetail(&convertedNameDetail);

    //add nickname
    if (d->m_etelStoreInfo.iMaxSecondNames > 0) {
        QString nickname;
        QList<QContactDetail> nicknameDetails = contact->details(QContactNickname::DefinitionName);
        if (nicknameDetails.count() > 0) {
            QContactNickname nicknameDetail = static_cast<QContactNickname>(nicknameDetails.at(0));
            nickname = nicknameDetail.nickname();
            nickname = nickname.left(d->m_etelStoreInfo.iMaxTextLengthSecondName); //trim to the max possible length
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
        if (phoneNumberDetail.number().length() > d->m_etelStoreInfo.iMaxNumLength) {
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
    if (d->m_etelStoreInfo.iMaxAdditionalNumbers > 0) {
        //one number is saved already
        for (int i = 1; i < phoneNumberDetails.count() && i-1 < d->m_etelStoreInfo.iMaxAdditionalNumbers; ++i) {
            QContactPhoneNumber phoneNumberDetail = static_cast<QContactPhoneNumber>(phoneNumberDetails.at(i));
            if (phoneNumberDetail.number().length() > d->m_etelStoreInfo.iMaxNumLengthAdditionalNumber) {
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

    //add e-mails
    if (d->m_etelStoreInfo.iMaxEmailAddr > 0) {
        QList<QContactDetail> emailDetails = contact->details(QContactEmailAddress::DefinitionName);
        for (int i = 0; i < emailDetails.count() && i < d->m_etelStoreInfo.iMaxEmailAddr; ++i) {
        QContactEmailAddress emailDetail = static_cast<QContactEmailAddress>(emailDetails.at(i));
        TPtrC emailValue(reinterpret_cast<const TUint16*>(emailDetail.emailAddress().utf16()));
        if (emailValue.Length() > d->m_etelStoreInfo.iMaxTextLengthEmailAddr) {
            User::Leave(KErrTooBig);
        }
        User::LeaveIfError(pbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBEmailAddress, emailValue));

        QContactEmailAddress convertedEmailDetail;
        convertedEmailDetail.setEmailAddress(emailDetail.emailAddress());
        convertedContact.saveDetail(&convertedEmailDetail);
        }
    }

    CleanupStack::PopAndDestroy(pbBuffer);
    PbkPrintToLog(_L("CntSymbianSimEngine::encodeSimContactL() - OUT"));
    return convertedContact;
}

void CntSymbianSimEngine::updateDisplayLabel(QContact& contact) const
{
    QContactManager::Error error(QContactManager::NoError);
    QString label = synthesizedDisplayLabel(contact, error);
    if(error == QContactManager::NoError) {
        contact = setContactDisplayLabel(label, contact);
    }
}

bool CntSymbianSimEngine::executeRequest(QContactAbstractRequest *req, QContactManager::Error& qtError) const
{
    CntSymbianSimEngine engine(*this);
    if (engine.startRequest(req))
        engine.waitForRequestFinished(req, 0); // should we have a timeout?
    engine.requestDestroyed(req);
    qtError = req->error();
    return (qtError == QContactManager::NoError);
}

QContactManagerEngine* CntSymbianSimFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    CntSymbianSimEngine *engine = new CntSymbianSimEngine(parameters, error);
    if(error != QContactManager::NoError) {
        delete engine;
        return 0;
    }
    return engine;
}

QString CntSymbianSimFactory::managerName() const
{
    return CNT_SYMBIANSIM_MANAGER_NAME;
}

Q_EXPORT_PLUGIN2(mobapicontactspluginsymbiansim, CntSymbianSimFactory);
