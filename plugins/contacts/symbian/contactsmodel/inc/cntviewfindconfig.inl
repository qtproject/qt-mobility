/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

inline CContactViewFindConfigInterface* CContactViewFindConfigInterface::NewL(TUid aImplementationUid)
/** Allocates and constructs an instance of the plug-in interface.

@param aImplementationUid The UID of the find configuration implementation 
to instantiate.
@return Pointer to the newly created implementation object. */
	{
		TAny* ptr = REComSession::CreateImplementationL(aImplementationUid,
					_FOFF(CContactViewFindConfigInterface,iDtor_ID_Key));
		return reinterpret_cast<CContactViewFindConfigInterface*>(ptr);
	}
inline CContactViewFindConfigInterface::~CContactViewFindConfigInterface()
/** Virtual destructor. */
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

