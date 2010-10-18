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
* Description:
*
*/


#include "koreaninput.h"
#include <QStringList>


// -----------------------------------------------------------------------------
// KoreanInput::KoreanInput
// -----------------------------------------------------------------------------
//
 KoreanInput::KoreanInput()
{

}

 // -----------------------------------------------------------------------------
 // KoreanInput::~KoreanInput
 // -----------------------------------------------------------------------------
 //
KoreanInput::~KoreanInput()
{

}


// -----------------------------------------------------------------------------
// KoreanInput::Tokenize
// -----------------------------------------------------------------------------
//
QStringList KoreanInput::Tokenize( QString sourcestring )
{
    QStringList mTokenList;
    int endposition = 0;
    int substringLen = 0;
    int startposition = 0;

    while (startposition < sourcestring.length()) {

     QChar startchar = sourcestring[startposition];

         //Finding endchar for given startchar here
         //endchar is dependent of the startchar (keypad number)
         endposition = findEndchar(sourcestring, startchar, startposition);
         substringLen = endposition - startposition;
         if ((substringLen>0) && (mTokenList.count() <= 4) ) {
             mTokenList.append(sourcestring.mid(startposition,substringLen));
             startposition = startposition + substringLen;
         }
         else
         {
             startposition++;
         }
    }

    return mTokenList;
}


// -----------------------------------------------------------------------------
// KoreanInput::countConsecutive
// Initiates check if a given keychar e.g. '*' is entered multiple times after
// specified point.
// Returns the number amount for consecutive chars found in string.
// -----------------------------------------------------------------------------
//

int KoreanInput::countConsecutive( QString stringToCount, QChar keychar, int currentpos )
{
    int consecutive = 0;

    if (currentpos >= 0 ) {
        for (int x = currentpos; (x < stringToCount.length() && (stringToCount[x]==keychar)); x++) {
            consecutive++;
        }
    }

    return consecutive;
}



// -----------------------------------------------------------------------------
// KoreanInput::findEndchar
// find the end limit for a specified single consonant or vowel, using
// numbers '0'-'9' as cutting rule. Exceptions are present for
// vowels 0, 3 and 6 which can be variated with additional 3 and 9.
// returns point for the ending character in string.
// -----------------------------------------------------------------------------
//

int KoreanInput::findEndchar( QString sourcestring, QChar startchar, int startpoint )
{
    QChar followchar;
    int endpoint = 0;
    int amount = 0;
    int currentpos = 0;
    int x = 0;

    if (startchar == '3') {
        //handle possible 9's and *9's
        endpoint = startpoint+1;

        //endchar = first digit (0-9) character of next number

        /* detailed:
        take next char after '3' that begins a new number
        in this order:
        if 9 --> nextchar location = location of '9' + 1 --> end of handling
        if 3 --> continue to loop checking next char
        if any other digit than 3 or 9 --> = nextchar --> end of handling
        if '*' --> enter a new check :
         - count consecutive *'s
         - if odd number of *'s + next char = 9 --> endchar = next char
         - if odd number of *,s + next char = other digit than 9 -> = endchar
         - if even number of *'s --> continue looking next chars as from initial '3', startingpoint
            increases the amount of '*' s

         */


        currentpos = startpoint+1;
        amount = 0;

        while ( currentpos<=sourcestring.length()) {
            followchar = sourcestring[currentpos];
            if (followchar == '9') {
                return currentpos+1;
            }
            else if (followchar == '*') {

                amount = countConsecutive(sourcestring,'*',currentpos);
                if (amount % 2 != 0) {
                    //odd amount of '*'
                    if (sourcestring[currentpos+amount]=='9') {
                        return currentpos+amount+1;
                    }
                    else if (sourcestring[currentpos+amount].isDigit()) {
                        return currentpos+amount;
                    }
                    else {
                        currentpos = currentpos + amount;
                    }
                }
                else {
                    //even amount of '*'
                    //increase currentpos for the amount of '*'s
                    //minus 1 as while loop increases position with 1
                    currentpos = currentpos + amount - 1;
                }

            }
            else if ((followchar.isDigit()) && (followchar != '3')) {
                return currentpos;
            }

            currentpos++;
        }

        return currentpos;

    }

    else if (startchar == '6') {
        //handle possibly following 3's 39's  9's


        /* detailed:

        take next char after '6' that begins a new number
        in this order:
        if 9 --> nextchar location = location of '9' + 1 --> end of handling
        if 6 --> continue to loop checking next char
        if 3 --> check new nextchar:
          - if 9 --> nextchar = location of that '9' + 1 --> end of handling
          - if something else --> nextchar = location of '3' + 1 --> end of handling

        if any other digit than 9 or another 3 --> = nextchar --> end of handling
        if '*' --> enter a new check :
         - count consecutive *'s
         - if odd number of *'s + next char = any digit --> endchar --> end of handling
         - if even number of *'s --> continue looking next chars as from initial '6', startingpoint
            increases the amount of '*' s

         */


        currentpos = startpoint+1;
        amount = 0;
        endpoint = startpoint + 1; //assume ending to be next char by default

        while ( currentpos<=sourcestring.length()) {
            followchar = sourcestring[currentpos];
            if (followchar == '9') {
                return currentpos+1;
            }
            else if (followchar == '3') {
                if (sourcestring[currentpos+1]=='9') {
                    return currentpos+2;
                }
                else {
                return currentpos+1;
                }
            }
            else if (followchar == '*') {

                amount = countConsecutive(sourcestring,'*',currentpos);
                if (amount % 2 != 0) {
                    //odd amount of '*'
                    if (sourcestring[currentpos+amount]=='9') {
                        return currentpos+amount+1;
                    }
                    else if (sourcestring[currentpos+amount].isDigit()) {
                        return currentpos+amount;
                    }
                    else {
                        currentpos = currentpos + amount;
                    }
                }
                else {
                    //even amount of '*'
                    //increase currentpos for the amount of '*'s
                    //minus 1 as while loop increases position with 1
                    currentpos = currentpos + amount - 1;
                }

            }
            else if ((followchar.isDigit()) && (followchar != '6')) {
                return currentpos;
            }

            currentpos++;
        }

        return currentpos;


    }

    else if (startchar == '0') {
        //handle possibly following '9'
        if (sourcestring[startpoint+1]=='9') {
            endpoint = startpoint + 2;
        }
        else {
            endpoint = startpoint + 1;
        }
    }

    else
    {
        //handle rest of the numbers here , endpoint is any valid digit for them

        endpoint = startpoint+1; //assume ending to be next char by default
        x = startpoint;
        do {
            x++;

         if ( !sourcestring[x].isDigit() ) {
             endpoint=x;
         }
        }
        while (x<sourcestring.length() && !sourcestring[x].isDigit() );

        endpoint=x;
    }

    return endpoint;
}
// End of file
