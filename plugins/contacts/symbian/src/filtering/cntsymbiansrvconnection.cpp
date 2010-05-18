/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
//system includes
#include <e32base.h>
#include <s32mem.h>
#include <qcontact.h>
#include <qcontactname.h>
#include <qcontactorganization.h>

//user includes
#include "cntsymbiansrvconnection.h"
#include "cntsymbiantransformerror.h"

// Constants
// To be removed. Should be defined in a header file
#define KCntSearchResultList 99
#define KCntOpenDataBase 100 // = KCapabilityReadUserData

_LIT(KCntServerExe,"CNTSRV.EXE");   // Name of the exe for the Contacts server.
_LIT(KCntServerName,"CNTSRV");  // Name used to connect a session 
                                // to the Contacts server.

/** Maximum number of asynchronous IPC calls. */
const TInt KAsyncMessageSlots=6;

/* Contacts server version number. */ 
const TInt KCntServerMajorVersionNumber=1;
const TInt KCntServerMinorVersionNumber=1;
const TInt KCntServerBuildVersionNumber=1;

const TInt KGranularityRank = 8; //2^8 = 256 bytes
const TInt KDefaultPackagerSize = 3514; //Observed Techview Golden template size.

/*!
 * The constructor
 */
CntSymbianSrvConnection::CntSymbianSrvConnection(QContactManagerEngine* manager) :
    m_manager(manager),
    m_buffer(0),
    m_bufPtr(0,0,0),
    m_isInitialized(false)
{
}

/*!
 * Destructor 
 */
CntSymbianSrvConnection::~CntSymbianSrvConnection()
{
    delete m_buffer;
    RHandleBase::Close();
}

/*!
 * Query the SQL database
 * 
 * \a sqlQuery An SQL query
 * \a error On return, contains the possible error.
 * \return the list of matched contact ids
 */
QList<QContactLocalId> CntSymbianSrvConnection::searchContacts(const QString& sqlQuery, 
                                                       QContactManager::Error* error)
{
    QList<QContactLocalId> list;
    TPtrC queryPtr(reinterpret_cast<const TUint16*>(sqlQuery.utf16()));
    TRAPD(err, list = searchContactIdsL(queryPtr));
    CntSymbianTransformError::transformError(err, error);
    return list;
}

/*!
 * Fetches all contact names from the database. If there are more than 3000 contacts,
 * only the first (by id) 3000 contacts will be fetched due to RAM restrictions.
 * 
 * \a error On return, contains the possible error.
 * \return the list of contact names (stored in QContact objects)
 */
QList<QContact> CntSymbianSrvConnection::searchAllContactNames(QContactManager::Error* error)
{
    QList<QContact> list;
    TRAPD(err, list = searchContactNamesL(_L("SELECT contact_id, first_name, last_name, company_name FROM contact WHERE (type_flags>>24)=0")));
    CntSymbianTransformError::transformError(err, error);
    return list;
}

/*!
 * Query the SQL database
 * 
 * \a id Id of the contact whose name to search
 * \a error On return, contains the possible error.
 * \return the list of matched contact ids
 */
QContact CntSymbianSrvConnection::searchContactName(QContactLocalId id, 
                                                    QContactManager::Error* error)
{
    QList<QContact> list;

    // Fetch results from the server
    TBuf<100> sqlQuery;
    sqlQuery.Format(_L("SELECT contact_id, first_name, last_name, company_name FROM contact WHERE contact_id = %d"), id);
    TRAPD(err, list = searchContactNamesL(sqlQuery));
    CntSymbianTransformError::transformError(err, error);
    
    if (list.size() == 0) {
        *error = QContactManager::DoesNotExistError;
        return QContact();
    }

    return list.at(0);
}

/*!
 * The leaving function that queries the SQL database
 * 
 * \a aSqlQuery An SQL query
 * \return the list of matched contact ids
 */
QList<QContactLocalId> CntSymbianSrvConnection::searchContactIdsL(const TDesC& aSqlQuery)
{
    readContactsToBufferL(aSqlQuery);

    RBufReadStream readStream;
    QList<QContactLocalId> list;
    TInt item;
    
    readStream.Open(*m_buffer);
    while ((item = readStream.ReadInt32L()) != 0) {
        list << item;
    }

    return list;
}

/*!
 * The leaving function that queries the SQL database
 * 
 * \a aSqlQuery An SQL query
 * \return the list of matched contact ids
 */
QList<QContact> CntSymbianSrvConnection::searchContactNamesL(const TDesC& aSqlQuery)
{
    readContactsToBufferL(aSqlQuery);

    RBufReadStream readStream;
    QList<QContact> contacts;
    TInt id;
    TBuf<256> firstName;
    TBuf<256> lastName;
    TBuf<256> company;

    readStream.Open(*m_buffer);
    while ((id = readStream.ReadInt32L()) != 0) {
        readStream >> firstName;
        readStream >> lastName;
        readStream >> company;

        QContact contact, tempContact;

        QContactName name;
        name.setFirstName(QString::fromUtf16(firstName.Ptr(), firstName.Length()));
        name.setLastName(QString::fromUtf16(lastName.Ptr(), lastName.Length()));
        tempContact.saveDetail(&name);

        QContactOrganization organization;
        organization.setName(QString::fromUtf16(company.Ptr(), company.Length()));
        tempContact.saveDetail(&organization);

        QContactManager::Error error(QContactManager::NoError);
        QString label = m_manager->synthesizedDisplayLabel(tempContact, &error);
        if (error != QContactManager::NoError) {
            continue;
        }
        tempContact.clearDetails();

        m_manager->setContactDisplayLabel(&contact, label);

        QContactId contactId;
        contactId.setLocalId(id);
        contactId.setManagerUri(m_manager->managerUri());
        contact.setId(contactId);

        contacts << contact;
    }

    return contacts;
}

    
/*!
 * The leaving function that queries the SQL database
 * 
 * \a id database id of the contact
 * \return the list of matched contact names
 */
void CntSymbianSrvConnection::readContactsToBufferL(const TDesC& sqlQuery)
{
    // Initialize connection if it is not initialized yet.
    if (!m_isInitialized) {
        ConnectSrvL();
        OpenDatabaseL();
        m_isInitialized = true;
    }

    TIpcArgs args;
    args.Set(0, &GetReceivingBufferL());
    args.Set(1, &sqlQuery);
    TInt newBuffSize = SendReceive(KCntSearchResultList, args);
    User::LeaveIfError(newBuffSize);
    if (newBuffSize > 0) {
        args.Set(0, &GetReceivingBufferL(newBuffSize));
        args.Set(1, &sqlQuery);
        User::LeaveIfError(SendReceive(KCntSearchResultList, args));
    }
}

/*!
 * Connect to / create a cntsrv server session
 */
void CntSymbianSrvConnection::ConnectSrvL()
{
    // Assume the server is already running and attempt to create a session
    // with a maximum of KAsyncMessageSlots message slots.
    TInt err = CreateSession(KCntServerName,Version(),KAsyncMessageSlots);
    
    // Server is not running
    if(err == KErrNotFound) {
        // Use the RProcess API to start the server.
        RProcess server;
        User::LeaveIfError(server.Create(KCntServerExe,KNullDesC));
        
        //Enforce server to be at system default priority EPriorityForeground
        server.SetPriority(EPriorityForeground);
        
        // Synchronize with the server.
        TRequestStatus reqStatus;
        server.Rendezvous(reqStatus);
        server.Resume();
        
        // Server will call the reciprocal static synchronization call.
        User::WaitForRequest(reqStatus);
        server.Close();
        User::LeaveIfError(reqStatus.Int());
        
        // Create the server session.
        User::LeaveIfError(CreateSession(KCntServerName,Version(),KAsyncMessageSlots));
    } else {
        User::LeaveIfError(err);
    }
    
    // Create IPC buffer
    m_buffer = CBufFlat::NewL(1 << KGranularityRank);
    m_maxBufferSize = KDefaultPackagerSize;
            
}

/*!
 * Open database
 */
void CntSymbianSrvConnection::OpenDatabaseL()
{
    TIpcArgs args;
    args.Set(0, &KNullDesC);
    User::LeaveIfError(SendReceive(KCntOpenDataBase, args));
}

/*!
 * Version of cntsrv
 */
TVersion CntSymbianSrvConnection::Version() const
{
    return(TVersion(KCntServerMajorVersionNumber,
                    KCntServerMinorVersionNumber,
                    KCntServerBuildVersionNumber));
}

/*!
 * Get the buffer reference to be used for IPC
 * 
 * \a size size of the receiving buffer
 * \return a reference to the beginning of the buffer
 */
TDes8& CntSymbianSrvConnection::GetReceivingBufferL(int size)
{
    if(size > m_buffer->Size()) {
        // Find next divisable by granularity size value.
        (size >>= KGranularityRank)++;
        m_maxBufferSize = size <<= 8;
        m_buffer->ResizeL(m_maxBufferSize);
    
    } else if(!size && m_buffer->Size() < m_maxBufferSize) {
        // Use the stored default size.
        m_buffer->ResizeL(m_maxBufferSize);
    }
    // The location of the whole buffer may have changed, because reallocation
    // may have taken place. Update both buffer pointers.
    m_bufPtr.Set(m_buffer->Ptr(0));
    return m_bufPtr;
}
