/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cviewcontactextension.h"


/** Allocates and constructs a CViewContactExtension object
 * @param aLength initial MaxLenght of a contact buffer.*/
CViewContactExtension* CViewContactExtension::NewL(TInt aLength)
	{
	CViewContactExtension* extension=new(ELeave)CViewContactExtension();
	CleanupStack::PushL(extension);
	extension->ConstructL(aLength);
	CleanupStack::Pop(extension);
	return extension;	
	}

/**
 * Second phase constructor.
 * @param aLength initial MaxLenght of a contact buffer.
 */
void CViewContactExtension::ConstructL(TInt aLength)
	{
	if (aLength > 0) 
		{
		iFieldTextBuf = HBufC::NewL(aLength);
		}
	}

/**
 * Default constructor
 */
CViewContactExtension::CViewContactExtension()
: iFieldTextBuf(NULL)
	{
	}

/**
 * Destructor
 */
CViewContactExtension::~CViewContactExtension()
	{
	delete iFieldTextBuf;
	}
	
/**
 * Append the specified field data to any existing field data
 *
 * @param aField Field data
 */
void CViewContactExtension::AppendToFieldTextL(const TDesC& aField)
	{
	const TInt fieldLength = aField.Length();

	if (fieldLength > 0)
		{
		if	(!iFieldTextBuf)
			{
			iFieldTextBuf = aField.AllocL();
			}
		else
			{
			const TInt length      = iFieldTextBuf->Length();
			const TInt maxLength   = iFieldTextBuf->Des().MaxLength();

			if (length + fieldLength > maxLength)
				{
				iFieldTextBuf = iFieldTextBuf->ReAllocL(length + fieldLength);
				}
			iFieldTextBuf->Des().Append(aField);
			}
		}
	}
