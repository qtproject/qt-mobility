/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class to hold the character to key mappings and provide
*   conversion of strings to their numerical mappings. 
*
*/

#ifndef __MLANGUAGESPECIFICKEYMAP_H__
#define __MLANGUAGESPECIFICKEYMAP_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class QString;


// CLASS DECLARATION
NONSHARABLE_CLASS(MLanguageSpecificKeymap)
	{
    public: // New pure virtual functions
		/**
		 * Maps the given string using a built-in Korean key map.
		 * aSource String to be converted
		 * returns Conversion result
		 */
		virtual QString GetMappedString(QString aSource) const = 0;
	};

#endif // __MLANGUAGESPECIFICKEYMAP_H__

// End of file
