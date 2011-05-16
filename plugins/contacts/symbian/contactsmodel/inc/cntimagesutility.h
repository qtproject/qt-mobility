/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CNTIMAGESUTILITY_H_
#define CNTIMAGESUTILITY_H_

#include <e32base.h>

class CImageRescaler;

/*
 * Interface for listening to the operations of contacts image utility class
 */
class MContactImageUtilityObserver
{
public:
    
    /**
     * Call back function used to inform the client of completion or error status
     * 
     * @param aDestPath A valid path to the new image, or empty.
     * @param aError KErrNone The operation was successful
     *               KErrNotFound The images directory does not exist
     *               KErrCancel The operation was cancelled while going on
     *               KErrGeneral Any other error while decoding
     *               KErrArgument Source image does not exist
     *               Any other system wide errors
     */
    virtual void UtilityOperationComplete(const TPath& aDestPath, TInt aError) = 0;
};

/**
 * This is a utility class used for operations on the private images folder that is used
 * to store scaled images assigned to contacts on the contacts database.
 * 
 * When user assigns an image to a contact, the contacts database needs to take ownership
 * of the image. Because of this, a copy of the image is stored in an separate directory.
 * If need be, the image is rescaled to a smaller size before being copied. Users of CContactDatabase 
 * API can scale down and store the images in this directory using this utility class. This will avoid
 * any rescaling when saving the contact increasing performance of the save operation.
 */
NONSHARABLE_CLASS(CContactImagesUtility) : public CActive
{
public:
    /**
     * Factory method to create a CContactImagesUtility object
     */
    IMPORT_C static CContactImagesUtility* NewL();
    
    /*
     * Desctuctor
     */
    IMPORT_C ~CContactImagesUtility();
    
    /**
     * Scales down the image, if need be, and store it in the contact images directory
     * 
     * @param aObserver Observer to the events of image processing
     *        
     * @param aSrcPath A valid path to the source image.
     * 
     * @pre aObserver should not be NULL
     */
    IMPORT_C void ScaleAndStoreImage(MContactImageUtilityObserver* aObserver, const TDesC& aSrcPath);
    
    /**
     * Ensures that the image is deleted from the filesystem, if it is owned by 
     * contacts (is in the contacts images directory).
     * 
     * @param aImagePath File path to the image file. 
     */
    IMPORT_C void EnsureImageRemoved(const TDesC& aImagePath);
    
private: // CActive
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);  
  
private:
    CContactImagesUtility();
    void ConstructL();
    
private:
    CImageRescaler* iRescaler;
    MContactImageUtilityObserver* iObserver;
};
#endif /* CNTIMAGESUTILITY_H_ */
