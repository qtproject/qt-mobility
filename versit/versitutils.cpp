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
#include <QRegExp>

const char typeParameterName[] = "TYPE";

/*!
 * Unfolds \a text by removing all the CRLFs followed immediately 
 * by any number of whitespaces.
 */
QByteArray VersitUtils::unfold(QByteArray& text)
{
    QChar previousChar;
    QChar previousOfThePreviousChar;
    for (int i=0; i<text.length(); i++) {
        QChar currentChar(text[i]);
        if (currentChar.isSpace() 
            && previousChar == '\n' 
            && previousOfThePreviousChar == '\r') {
            
            int countOfCharsToBeRemoved = 2; // CRLF
            countOfCharsToBeRemoved += countLeadingWhiteSpaces(text,i);
            text.remove(i-2,countOfCharsToBeRemoved);
            previousChar = QChar();
            previousOfThePreviousChar = QChar();
            i -= 2;
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
        QChar currentChar(text[i]);
        if (currentChar.isSpace()) {
            whiteSpaceCount++;
        }
        else {
            nonWhiteSpaceFound = true;
        }
    }
    return whiteSpaceCount;
}

/*!
 * Checks whether /a text contains special characters
 * that may need to be encoded with Quoted-Printable encoding (RFC 1521).
 */
bool VersitUtils::containsSpecialChars(const QByteArray& text)
{
    for (int i=0; i<text.length(); i++) {
        QChar currentChar(text[i]);
        if (currentChar == '\n' || 
            currentChar == '\r' || 
            currentChar == '!' || 
            currentChar == '"' || 
            currentChar == '#' || 
            currentChar == '$' || 
            currentChar == '=' || 
            currentChar == '@' || 
            currentChar == '[' || 
            currentChar == '\\' || 
            currentChar == ']' || 
            currentChar == '^' || 
            currentChar == '`' || 
            currentChar == '{' ||
            currentChar == '|' ||
            currentChar == '}' || 
            currentChar == '~') {
            return true;
            }
    }
    return false;
}

/*!
 * Encodes special characters in /a text 
 * using Quoted-Printable encoding (RFC 1521).
 */
QByteArray VersitUtils::encodeQuotedPrintable(QByteArray& text)
{
    QMap<QChar,QByteArray> encodingMap;
    encodingMap.insert('\n', "=0A");
    encodingMap.insert('\r', "=0D");
    encodingMap.insert('!', "=21");
    encodingMap.insert('"', "=22");
    encodingMap.insert('#', "=23");
    encodingMap.insert('$', "=24");
    encodingMap.insert('=', "=3D");
    encodingMap.insert('@', "=40");
    encodingMap.insert('[', "=5B");
    encodingMap.insert('\\', "=5C");
    encodingMap.insert(']', "=5D");
    encodingMap.insert('^', "=5E");
    encodingMap.insert('`', "=60");
    encodingMap.insert('{', "=7B");
    encodingMap.insert('|', "=7C");
    encodingMap.insert('}', "=7D");
    encodingMap.insert('~', "=7E");
    
    for (int i=0; i<text.length(); i++) {
        QChar currentChar(text[i]);
        QByteArray mappedString = encodingMap.value(currentChar);
        if (mappedString.length() > 0) {
            text.replace(i,1,mappedString);
            i += 2;
        }
    }
    return text;
}

/*!
 * Decodes Quoted-Printable encoded (RFC 1521) characters in /a text.
 */
QByteArray VersitUtils::decodeQuotedPrintable(QByteArray& text)
{
    // QString needed instead of QByteArray to use QRegExp
    QString textAsString(text);
    
    // Replace soft line breaks
    QRegExp softLineBreaks("(=\\r\\n)"); 
    textAsString.replace(softLineBreaks,QByteArray());

    // Other encoded characters
    QRegExp encodedChars("(=\\d[A-F]|=\\d\\d)",Qt::CaseInsensitive); 
    int pos = 0;    
    while ((pos = encodedChars.indexIn(textAsString,pos)) >= 0) {
        QString encodedCharAsString(encodedChars.cap(1));
        bool decoded = false;
        QChar decodedChar(encodedCharAsString.mid(1).toInt(&decoded,16));
        if (decoded)
            textAsString.replace(encodedCharAsString,decodedChar);
    }
    
    text = textAsString.toAscii();
    return text;
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
QByteArray VersitUtils::extractPropertyName(const QByteArray& property)
{
    QByteArray name;
    int nameLength = 0;
    QChar previousChar;
    for (int i=0; i < property.length(); i++) {
        QChar currentChar = property[i];
        if ((currentChar == ';' && previousChar != '\\') || 
            currentChar == ':') {
            nameLength = i;
            break;
        }
        previousChar = currentChar;
    }
    if (nameLength > 0)
        name = property.left(nameLength).trimmed();
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
 * Extracts the property parameters as a QMultiMap.
 * The parameters without names are added as "TYPE" parameters.
 */
QMultiMap<QByteArray,QByteArray> VersitUtils::extractPropertyParams(
    const QByteArray& property)
{
    QMultiMap<QByteArray,QByteArray> params;    
    int colonIndex = property.indexOf(':');
    if (colonIndex > 0) {
        QByteArray nameAndParamsString = property.left(colonIndex);
        int paramStartIndex = -1;
        QChar previousChar;
        for (int i=0; i < nameAndParamsString.length(); i++) {
            QChar currentChar = nameAndParamsString[i];
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
    QMultiMap<QByteArray,QByteArray>& params,
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
QByteArray VersitUtils::paramName(const QByteArray& parameter)
{
     if (parameter.trimmed().length() == 0)
         return QByteArray();
     int equalsIndex = parameter.indexOf('=');
     if (equalsIndex > 0)
         return parameter.left(equalsIndex).trimmed();
     return QByteArray(typeParameterName);
}

/*!
 * Extracts the value of the parameter
 */
QByteArray VersitUtils::paramValue(const QByteArray& parameter)
{
    QByteArray value(parameter);
    int equalsIndex = parameter.indexOf('=');
    if (equalsIndex > 0) {
        if (equalsIndex == parameter.length()-1) {
            value = "";
        }
        else {
            int valueLength = parameter.length() - (equalsIndex + 1);
            value = parameter.right(valueLength).trimmed();
        }    
    }
    return value;
}
