/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "versitutils.h"
#include <QMap>
#include <QRegExp>

/*!
 * Folds \a text by making all lines \a maxChars long.
 */
QByteArray VersitUtils::fold(QByteArray& text, int maxChars)
{
    char previousChar = 0;
    int charsSinceLastLineBreak = 0;
    for (int i=0; i<text.length(); i++) {
         char currentChar = text[i];
         if (previousChar == '\r' && currentChar == '\n') {
             charsSinceLastLineBreak = 0;
             previousChar = 0;
         } else {
             char nextChar = 0;
             if (i != text.length()-1)
                 nextChar == text[i+1];
             if (charsSinceLastLineBreak == maxChars &&
                 (currentChar != '\r' && nextChar != '\n')) {
                 text.insert(i,"\r\n ");
                 charsSinceLastLineBreak = 1; // space
                 // Skip the added CRLF, for-loop increment i++ skips the space:
                 i += 2;
                 previousChar = 0;
             } else {
                 charsSinceLastLineBreak++;
                 previousChar = currentChar;
             }
         }
    }
    return text;
}

/*!
 * Unfolds \a text by removing all the CRLFs followed immediately 
 * by any number of whitespaces.
 */
QByteArray VersitUtils::unfold(QByteArray& text)
{
    char previousChar = 0;
    char previousOfThePreviousChar = 0;
    for (int i=0; i<text.length(); i++) {
        char currentChar = text[i];
        if ((currentChar == ' ' || currentChar == '\t') && 
            previousChar == '\n' && 
            previousOfThePreviousChar == '\r') {
            
            int countOfCharsToBeRemoved = 2; // CRLF
            countOfCharsToBeRemoved += countLeadingWhiteSpaces(text,i);
            text.replace(i-2,countOfCharsToBeRemoved,QByteArray(" "));
            previousChar = 0;
            previousOfThePreviousChar = 0;
            i--;
        }
        else {
            previousOfThePreviousChar = previousChar;
            previousChar = currentChar;
        }
    }
    return text;
}

/*!
 * Returns the count of leading whitespaces in /a text
 * starting from /a pos.
 */
int VersitUtils::countLeadingWhiteSpaces(const QByteArray& text, int pos)
{
    int whiteSpaceCount = 0;
    bool nonWhiteSpaceFound = false;
    for (int i=pos; i<text.length() && !nonWhiteSpaceFound; i++) {
        char currentChar = text[i];
        if (currentChar == ' ' || 
            currentChar == '\t' || 
            currentChar == '\r' || 
            currentChar == '\n') {
            whiteSpaceCount++;
        }
        else {
            nonWhiteSpaceFound = true;
        }
    }
    return whiteSpaceCount;
}

/*!
 * Encodes special characters in /a text 
 * using Quoted-Printable encoding (RFC 1521).
 * Returns true if at least one character was encoded.
 */
bool VersitUtils::quotedPrintableEncode(QByteArray& text)
{    
    bool encoded = false;
    for (int i=0; i<text.length(); i++) {
        char currentChar = text[i];
        if (shouldBeQuotedPrintableEncoded(currentChar)) {
            QString encodedStr;
            encodedStr.sprintf("=%02X",currentChar);
            text.replace(i,1,encodedStr.toAscii());
            i += 2;
            encoded = true;
        }
    }
    return encoded;
}

/*!
 * Decodes Quoted-Printable encoded (RFC 1521) characters in /a text.
 */
void VersitUtils::decodeQuotedPrintable(QByteArray& text)
{
    // QString needed instead of QByteArray to use QRegExp
    QString textAsString(QString::fromAscii(text));
    
    // Replace soft line breaks
    QRegExp softLineBreaks(QString::fromAscii("(=\\r\\n)")); 
    textAsString.remove(softLineBreaks);

    // Other encoded characters
    QRegExp encodedChars(
        QString::fromAscii("(=\\d[A-F]|=\\d\\d)"),Qt::CaseInsensitive);
    int pos = 0;    
    while ((pos = encodedChars.indexIn(textAsString,pos)) >= 0) {
        QString encodedCharAsString(encodedChars.cap(1));
        bool decoded = false;
        QChar decodedChar(encodedCharAsString.mid(1).toInt(&decoded,16));
        if (decoded)
            textAsString.replace(encodedCharAsString,decodedChar);
    }
    text = textAsString.toAscii();
}

/*!
 * Finds the position of the first non-soft line break 
 * in a Quoted-Printable encoded string.
 */
int VersitUtils::findHardLineBreakInQuotedPrintable(const QByteArray& encoded)
{
    int crlfIndex = encoded.indexOf("\r\n");
    if (crlfIndex <= 0)
        return -1;
    while (crlfIndex > 0 && encoded[crlfIndex-1] == '=') {
        crlfIndex = encoded.indexOf("\r\n",crlfIndex+2);
    }
    return crlfIndex;
}

/*!
 * Extracts the name of the property.
 */
QString VersitUtils::extractPropertyName(const QByteArray& property)
{
    QString name;
    int nameLength = 0;
    char previousChar = 0;
    for (int i=0; i < property.length(); i++) {
        char currentChar = property[i];
        if ((currentChar == ';' && previousChar != '\\') || 
            currentChar == ':') {
            nameLength = i;
            break;
        }
        previousChar = currentChar;
    }
    if (nameLength > 0)
        name = QString::fromAscii(property.left(nameLength).trimmed());
    return name;
}

/*!
 * Extracts the value of the property.
 * Returns an empty string if the value was not found
 */
QByteArray VersitUtils::extractPropertyValue(const QByteArray& property)
{
    QByteArray value;
    int index = property.indexOf(':') + 1;
    if (index > 0 && property.length() > index)
        value = property.mid(index);
    return value;
}

/*!
 * Extracts the property parameters as a QMultiHash.
 * The parameters without names are added as "TYPE" parameters.
 */
QMultiHash<QString,QString> VersitUtils::extractPropertyParams(
    const QByteArray& property)
{
    QMultiHash<QString,QString> params;    
    int colonIndex = property.indexOf(':');
    if (colonIndex > 0) {
        QByteArray nameAndParamsString = property.left(colonIndex);
        int paramStartIndex = -1;
        char previousChar = 0;
        for (int i=0; i < nameAndParamsString.length(); i++) {
            char currentChar = nameAndParamsString[i];
            if (currentChar == ';' && previousChar != '\\') {
                int length = i-paramStartIndex;
                addParam(params,nameAndParamsString,paramStartIndex,length);
                paramStartIndex = i+1;
            }
            previousChar = currentChar;
        }
        // Add the last parameter before the colon
        addParam(params,nameAndParamsString,paramStartIndex);
    }
    return params;
}

/*!
 * Adds a parameter into /a params from a substring
 * limited by /a startPosition and /a length
 */
void VersitUtils::addParam(
    QMultiHash<QString,QString>& params,
    const QByteArray& originalString, 
    int startPosition, 
    int length)
{
    QByteArray paramString;
    if (startPosition >= 0)
        paramString = originalString.mid(startPosition,length).trimmed();
    if (paramString.length() > 0)
        params.insertMulti(paramName(paramString),paramValue(paramString));
}

/*!
 * Extracts the name of the parameter.
 * No name is interpreted as an implicit "TYPE".
 */
QString VersitUtils::paramName(const QByteArray& parameter)
{
     if (parameter.trimmed().length() == 0)
         return QString();
     int equalsIndex = parameter.indexOf('=');
     if (equalsIndex > 0)
         return QString::fromAscii(parameter.left(equalsIndex).trimmed());
     return QString::fromAscii("TYPE");
}

/*!
 * Extracts the value of the parameter
 */
QString VersitUtils::paramValue(const QByteArray& parameter)
{
    QString value(QString::fromAscii(parameter));
    int equalsIndex = parameter.indexOf('=');
    if (equalsIndex > 0) {
        if (equalsIndex == parameter.length()-1) {
            value = QString();
        }
        else {
            int valueLength = parameter.length() - (equalsIndex + 1);
            value = QString::fromAscii(parameter.right(valueLength).trimmed());
        }    
    }
    return value;
}

/*!
 * Checks whether the \a chr should be Quoted-Printable encoded (RFC 1521). 
 */
bool VersitUtils::shouldBeQuotedPrintableEncoded(char chr)
{
    return (chr < 32 || 
            chr == '!' || chr == '"' || chr == '#' || chr == '$' || 
            chr == '=' || chr == '@' || chr == '[' || chr == '\\' || 
            chr == ']' || chr == '^' || chr == '`' ||
            chr > 122 ); 
}
