/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom/implementationproxy.h>
#include <cntdb.h>
#include "cntphone.h"

/*
This is the specification for the implementation of the phone number parser. 
This has been provided by Pekka Jokela (Nokia).

1.   If the first character of the number is '+',  the  '+' character is
left out, and the real phone number is considered to begin right after
that.

 * If there is a '(' as the first character and the plus sign follows then
  i.e. first two characters = '(+' then they must both be removed and the 
  real phone number is considered to begin after them

2.   If the number begins with a  '*' or '#' symbol, all characters to
the last '*' or '#' symbol are ignored, and the real phone number is
considered to begin right after that.

*    If there is '+' character right after the last '#' or '*', the  '+'
character is left out, and the real phone number is considered to
begin after that.

*    If there is '+' character within first five characters, all
characters to the '+' symbol are ignored, and the real phone number is
considered to begin right after that.

3.   If the number contains special characters 'p',' w' and '+' , all
characters after the first special character are ignored.

If the number contains '*' and '#' symbols, i.e. there is a
supplementary service code or a feature code within the number:
*    If the code is finished by '*' or by '#  the real phone number can
be separated easily (refer to the table below, example 1). Even
possible '+' symbol after the code can be left out (example 2).
*    If the code is finished by a digit, the real phone number can not be
separated from the code. In most cases, this is not a problem, because
only seven last digits are significant for comparison (example 3). If
there is '+' symbol between the code and the real phone number, the
'+' has to be within first five characters or otherwise it is
considered to start a DTMF function. (example 4)

These all match 1234567:
*#42# 0401234567 p123
*#42# +358401234567 p123
*61 0401234567
*61 +358401234567
+358401234567 +3
+358401234567 p123
(+358) 1234567

And these something else:
*#42# 0401234567#p123 -> empty
*12345+0401234567 -> 12345
*+123+456+++++++++++ ->123

The relevant ETSI specifications are:
ETSI 02.07 - MS Features. Section B.3.4. 
ETSI 03.14 - Defines what DTMF digits are.
ETSI 11.11 - Section 10.5.1. Defines the Separator digit.

*/


const TInt KPlusWithinChars = 5;
const TText KSymbolPause = 'p';
const TText KSymbolWait = 'w';
const TText KSymbolPlus = '+';
const TText KSymbolAsterisk = '*';
const TText KSymbolHash = '#';
//const symbol for checking the opening bracket before a plus
const TText KSymbolOpenBrace = '(';


/** 
 * Extracts the real phone number from a contacts phone number field.
 * This method strips away any DTMF strings or extended services. An empty 
 * descriptor is returned in aRawNumber if the field doesn't have a valid phone 
 * number.
 * 
 * @param aTextualNumber Descriptor containing a contacts model phone number field 
 * @param aRawNumber Descriptor to write the raw number to (loaned by caller)
 */
void CContactDefaultPhoneNumberParser::ExtractRawNumber(const TDesC& aTextualNumber, TDes& aRawNumber)
    {
	aRawNumber.Zero();

	TInt length = aTextualNumber.Length();
	if (length==0)
		{
		return;	
		}
		

	TPtrC numberPtr( aTextualNumber );
	TUint firstChar = numberPtr[0];
	
	//gobble spaces
	while (TChar(firstChar).IsSpace())
		{
		--length;
		if (length==0)
			{
			return;
			}
		
		numberPtr.Set(numberPtr.Right(length));
		firstChar = numberPtr[0];
		}

    // Get left hand side
    if ( firstChar == KSymbolAsterisk || firstChar == KSymbolHash )
        {
        //Check if there is plus on first five chars:
        TInt newStartPlace = numberPtr.Locate( KSymbolPlus );
        if ( newStartPlace>=KPlusWithinChars || newStartPlace==KErrNotFound )
            {
            // There is always star or hash...
            newStartPlace = Max( numberPtr.LocateReverse(KSymbolAsterisk ) ,numberPtr.LocateReverse( KSymbolHash) );
            }

        length = length - newStartPlace -1;
        if ( length <= 0 )
            {
            return;
            }
        numberPtr.Set( numberPtr.Right( length ) );
        firstChar = numberPtr[0];
        }

	//test condition to satisfy the removal of '(' the next if 
	//statement removes the '+' if needed
	if ( firstChar == KSymbolOpenBrace )
		{
		length--;
		numberPtr.Set( numberPtr.Right( length ) );
		// This may be the only character in the descriptor so only access if
		// 1 or more characters left.
		if (length > 0 )
			{
			firstChar = numberPtr[0];
			}        
		}

    if ( firstChar == KSymbolPlus )
        {
        length--;
        numberPtr.Set( numberPtr.Right( length ) );
        }

	if (length==0)
		{
		return;
		}		

    // Find right hand side
    TLex numberLexer( numberPtr );
    for ( ; ; )
        {
        TChar nextChar = numberLexer.Peek();
        if ( !nextChar )
            {
            break;
            }

        if ( nextChar.IsDigit() )
            {
            aRawNumber.Append( nextChar );
            numberLexer.Inc();
            }
        else if ( nextChar == KSymbolAsterisk || nextChar == KSymbolHash )
            {
            aRawNumber.Zero();
            return;
            }
        else
            {
            nextChar.LowerCase();
            if ( nextChar == KSymbolPause
                 || nextChar == KSymbolWait
                 || nextChar == KSymbolPlus)
                {
                break;
                }
            numberLexer.Inc();
            }
        }
    }




CContactDefaultPhoneNumberParser* CContactDefaultPhoneNumberParser::NewL()
	{
	CContactDefaultPhoneNumberParser* self = new (ELeave) CContactDefaultPhoneNumberParser();  // calls c'tor
	return self;
	}
		
// Export the implementation collection function
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(0x102035FC, 	CContactDefaultPhoneNumberParser::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

/**
 * Standard Epoc32 Dll Entry point
 */
