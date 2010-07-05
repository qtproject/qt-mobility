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


#ifndef CNTMETADATAOPERATION_H
#define CNTMETADATAOPERATION_H

// Forward declarations
class RColumboSession;
class CMetadataDocument;

/**
 * This class is a listener for the asynchronous operations used
 * to keep the metadata cache for contact items up to date.
 * 
 * Calling either the save or delete function will contact the
 * metadata service asynchronously and the object will be destroyed
 * on the completion of the operation. Therefore, do NOT maintain
 * references/pointers to CCntMetadataOperation objects as member
 * data.
 * 
 * Failed metadata operations are not considered fatal.
 */
class CCntMetadataOperation : public CActive
    {
public:
    ~CCntMetadataOperation();

    static CCntMetadataOperation* NewL(RColumboSession& aSession);
    static CCntMetadataOperation* NewLC(RColumboSession& aSession);

    void SaveContactLD(const CContactItem& aContact);
    void DeleteContactLD(TInt aContactId);
    
private:
    CCntMetadataOperation(RColumboSession& aSession);
    void ConstructL();
    
    CMetadataDocument* DocumentFromContactL(const CContactItem& aContact);
    HBufC* ContactMetadataKeyLC(TInt aContactId);
    
private:
    // From CActive
    void RunL();
    void DoCancel();

    TInt RunError(TInt aError);

private:
    RColumboSession& iSession;
    CMetadataDocument* iDocument;
    TInt iContactId;
    HBufC* iKey;
    };

#endif // CNTMETADATAOPERATION_H
