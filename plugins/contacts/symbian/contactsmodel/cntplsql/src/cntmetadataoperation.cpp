/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

//#include <columbo.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "cntmetadataoperation.h"

CCntMetadataOperation::CCntMetadataOperation(RColumboSession& aSession) 
: CActive(EPriorityStandard), iSession(aSession), iContactId(-1)
    {
    }

/**
 * Construct a new metadata operation object. Note that this object will destroy
 * itself after either save or delete operations have been called.
 * 
 * The new object will be left on the cleanup stack.
 * 
 * @param aSession Session with the metadata service.
 * @return The new metadata operation instance.
 */
CCntMetadataOperation* CCntMetadataOperation::NewLC(RColumboSession& aSession)
    {
    CCntMetadataOperation* self = new (ELeave) CCntMetadataOperation(aSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

/**
 * Construct a new metadata operation object. Note that this object will destroy
 * itself after either save or delete operations have been called.
 * 
 * @param aSession Session with the metadata service.
 * @return The new metadata operation instance.
 */
CCntMetadataOperation* CCntMetadataOperation::NewL(RColumboSession& aSession)
    {
    CCntMetadataOperation* self = CCntMetadataOperation::NewLC(aSession);
    CleanupStack::Pop();
    return self;
    }

/**
 * Note: upon construction, the active object is set active and waiting
 * for a completion event to destroy itself.
 */
void CCntMetadataOperation::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

CCntMetadataOperation::~CCntMetadataOperation()
    {
    // (iSession is not owned)
    delete iDocument;
    delete iKey;
    }

/**
 * The DoCancel() function will only delete the object in production builds.
 * If __PROFILE_DEBUG__ is defined in the persistence layer MMP file,
 * error codes will be logged should they occur.
 */
void CCntMetadataOperation::DoCancel()
    {
    // Log that the operation is cancelled.
#ifdef __PROFILE_DEBUG__
    _LIT(KCancelFormat, "CCntMetadataOperation::DoCancel(): Operation Cancelled: Contact ID = %d");
    RDebug::Print(KCancelFormat(), iContactId);
#endif
    
    // Self destruct - operation cancelled.
    delete this;
    }

/**
 * The RunL() function will only delete the object in production builds.
 * If __PROFILE_DEBUG__ is defined in the persistence layer MMP file,
 * error codes will be logged should they occur.
 */
void CCntMetadataOperation::RunL()
    {
#ifdef __PROFILE_DEBUG__
    if (iStatus.Int() != KErrNone)
        {
        _LIT(KErrorFormat, "CCntMetadataOperation::RunL(): Operation failed [Contact ID = %d] Error = %d");
        RDebug::Print(KErrorFormat(), iContactId, iStatus.Int());
        }
    else
        {
        _LIT(KErrorFormat, "CCntMetadataOperation::RunL(): Operation complete [Contact ID = %d]");
        RDebug::Print(KErrorFormat(), iContactId);
        }
#endif // __PROFILE_DEBUG__
    
    // Self destruct - operation complete.
    delete this;
    }

TInt CCntMetadataOperation::RunError(TInt aError)
    {
    // RunL() doesn't really leave, this should never happen.
    // Log but do nothing.
#ifdef __PROFILE_DEBUG__
    _LIT(KRunErrorLog, "CCntMetadataOperation::RunError(): aError = %d");
    RDebug::Print(KRunErrorLog(), aError);
#endif // __PROFILE_DEBUG__
    return aError;
    }

/**
 * Save the contact @a aContact in the metadata service. If there is
 * already a contact entry with this ID, it will be updated. Otherwise,
 * a new contact metadata entry is added.
 * 
 * The metadata operation object will be destroyed when the operation
 * completes. Do not attempt to use this object after calling.
 * 
 * @param aContact The contact item to index for metadata.
 */
void CCntMetadataOperation::SaveContactLD(const CContactItem& aContact)
    {
    // For logging purposes
    iContactId = aContact.Id();
    
    // Create a new generic metadata object for this contact item.
    if (iDocument)
        {
        delete iDocument;
        iDocument = NULL;
        }
    iDocument = DocumentFromContactL(aContact);
    iDocument->Store(iSession, iStatus);
    SetActive();
    }

/**
 * Remove the contact described by @a aContactId from the metadata store.
 * 
 * The metadata operation object will be destroyed when the operation
 * completes. Do not attempt to use this object after calling.
 * 
 * @param aContactId The contact to be removed from the metadata service
 */
void CCntMetadataOperation::DeleteContactLD(TInt aContactId)
    {
    // For logging purposes
    iContactId = aContactId;
    
    // Get the key and delete the document
    if (iKey)
		{
		delete iKey;
		iKey = NULL;
		}
    HBufC* iKey = ContactMetadataKeyLC(aContactId);
    CleanupStack::Pop();
    
    iSession.DeleteDocument(*iKey, iStatus);
    SetActive();
    }

/**
 * Construct a new generic metadata object to represent a contact item.
 * 
 * @param aContact The contact item to encode as a generic metadata object.
 * @return The newly constructed metadata object instance.
 */
CMetadataDocument* CCntMetadataOperation::DocumentFromContactL(const CContactItem& aContact)
    {
    CMetadataDocument* document = CMetadataDocument::NewLC();
    
    // Assign a unique key for cntmodel purposes
    HBufC* key = ContactMetadataKeyLC(aContact.Id());
    document->SetUserKeyL(*key);
    CleanupStack::PopAndDestroy(key);
    
    // Iterate through fields, and produce a metadata string for this contact
    CContactItemFieldSet& fields = aContact.CardFields();
    const TInt KFieldCount(fields.Count());
    
    if (KFieldCount > 0)
        {
        // Find out how much space we need for the metadata buffer
        TInt bufferLength(0);
        for(TInt i(0); i < KFieldCount; ++i)
            {
            CContactItemField& field = fields[i];
            if (field.StorageType() != KStorageTypeText)
                {
                // Can only index text, no point if text is 0 length
                continue;
                }
            
            // No point in indexing 0 length data
            TInt fieldLength = field.TextStorage()->Text().Length();
            if (fieldLength == 0)
                {
                continue;
                }
            
            // Add field length plus the length of a space to
            // seperate it from the next field
            bufferLength += (fieldLength + 1);
            }
        
        // Allocate a buffer and add all the fields to it
        HBufC* metadata = HBufC::NewLC(bufferLength);
        TPtr bufferPtr = metadata->Des();
        
        for(TInt i(0); i < KFieldCount; ++i)
            {
            CContactItemField& field = fields[i];
            if (field.StorageType() != KStorageTypeText)
                {
                // Can only index text
                continue;
                }
            
            // No point in indexing 0 length data
            TInt fieldLength = field.TextStorage()->Text().Length();
            if (fieldLength == 0)
                {
                continue;
                }
            
            TPtrC fieldContent = field.TextStorage()->Text();
            bufferPtr.Append(fieldContent);

            // Add a space as a seperator
            _LIT(KSpace, " ");
            bufferPtr.Append(KSpace);
            }

        // Add the metadta to the generic document
        document->SetMetadataL(*metadata);
        CleanupStack::PopAndDestroy(metadata);
        }
    
    CleanupStack::Pop(document);
    return document;
    }

/**
 * Generate an RDNS style key to identify the metadata object for cntmodel purposes.
 * 
 * @param aContactId The contact model assigned contact ID.
 * @return A descriptor containing the new unique identifier for the document object.
 */
HBufC* CCntMetadataOperation::ContactMetadataKeyLC(TInt aContactId)
    {
    // Key format for cntmodel entries in the metadata service
    _LIT(KKeyFormat, "com.nokia.contact.id.%d");
    
    // Allocate a big enough buffer for just the key format with
    // an integer added to the end
    HBufC* key = HBufC::NewLC(KKeyFormat().Length() + 32);
    TPtr keyPtr = key->Des();
    keyPtr.AppendFormat(KKeyFormat(), aContactId);
    
    // Leaving key on the cleanup stack
    return key;
    }

