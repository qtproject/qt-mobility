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

#ifndef CNTIMAGERESCALEUTILITY_H_
#define CNTIMAGERESCALEUTILITY_H_

#include <f32file.h>

class CContactItem;
/**
 * This is a utility class used with CntImageRescale class. This class holds the information
 * about phonbooks private image folder and is able to update the contact fields.
 * 
 * When user assigns a image to contact, CntImageRescaler copies and rescales the image
 * and asks this class to store it to contact item. 
 */
NONSHARABLE_CLASS(TCntImageRescaleUtility)
{
public:
    /**
     * Returns the private image directory.
     * 
     * @return TPath Valid path to image directory
     * @leave If any error occures
     */
    IMPORT_C static TPath ImageDirectoryL();
    /**
     * Create a private image folder if it does not exists already.
     * 
     * @return TPath Full path to created image directory. 
     * @leave If any error occures
     */
    IMPORT_C static TPath CreateImageDirectoryL();
    /**
     * Delete the private image directory if it exists.
     * 
     * @leave If any error occures
     */
    IMPORT_C static void DeleteImageDirectoryL();
    /**
     * Delete the private image from file system, if one exists.
     */
    IMPORT_C static void DeleteImageL( const CContactItem& aItem );
    /**
     * Checks that if the private image filename does not contain an unique
     * identifier (GUID), adds it and renames the image. Updates also the contact
     * item to contain the renamed image path.
     * 
     * @param aItem Contact item which has a private image stored
     * @leave If any error occures 
     */
    IMPORT_C static void UpdateImageNameL( const CContactItem& aItem );
    /**
     * Stores the private image to given contacts field.
     * 
     * @param aItem CContactItem where the private image path should be stored
     * @param aPath New private image path 
     */
    IMPORT_C static void StoreImageFieldL( const CContactItem& aItem, const TPath aPath  );
    
private:
    /**
     * Creates a backup and restore file
     */
    static void CreateBackupAndRestoreFileL( RFs& aFs, const TPath& aDir );
};
#endif /* CNTIMAGERESCALEUTILITY_H_ */
