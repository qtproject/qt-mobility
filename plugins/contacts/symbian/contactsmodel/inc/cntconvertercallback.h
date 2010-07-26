// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CNTCOVERTERCALLBACK_H__
#define __CNTCOVERTERCALLBACK_H__

class MConverterCallBack
/**
Interface class for providing Intra-Contact Properties during a PBAP specific contacts export.
@released
@internalTechnology
*/
{
	public:
	/**
	PBAP clients are supposed to provide any information regarding the contact item represented by aContactId.
	This information should be in form of standard vCard property, all such properties should be appended to the array aPropertyList.
	*/
	virtual	void AddIntraContactPropertiesL(const TContactItemId& aContactId, CArrayPtr<CParserProperty>* aPropertyList) = 0;
};

#endif //__CNTCOVERTERCALLBACK_H__