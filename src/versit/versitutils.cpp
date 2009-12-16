/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "versitutils_p.h"
#include "qmobilityglobal.h"

#include <QMap>
#include <QRegExp>
QTM_BEGIN_NAMESPACE

/*!
 * Folds \a text by making all lines \a maxChars long.
 */
QByteArray VersitUtils::fold(QByteArray& text, int maxChars)
{
    char previous = 0;
    int charsSinceLastLineBreak = 0;
    for (int i=0; i<text.length(); i++) {
         char current = text.at(i);
         if (previous == '\r' && current == '\n') {
             charsSinceLastLineBreak = 0;
             previous = 0;
         } else {
             char next = 0;
             if (i != text.length()-1)
                 next = text.at(i+1);
             if (charsSinceLastLineBreak == maxChars &&
                 (current != '\r' && next != '\n')) {
                 text.insert(i,"\r\n ");
                 charsSinceLastLineBreak = 1; // space
                 // Skip the added CRLF, for-loop increment i++ skips the space:
                 i += 2;
                 previous = 0;
             } else {
                 charsSinceLastLineBreak++;
                 previous = current;
             }
         }
    }

    return text;
}

/*!
 * Unfolds \a text by removing all the CRLFs
 *followed immediately by a linear whitespace (SPACE or TAB).
 */
QByteArray VersitUtils::unfold(QByteArray& text)
{
    // XXX build up a list of moves and do them all at once.
    char previous = 0;
    char previousOfTheprevious = 0;
    for (int i=0; i<text.length(); i++) {
        char current = text.at(i);
        if ((current == ' ' || current == '\t') &&
             previous == '\n' &&
             previousOfTheprevious == '\r') {
            text.replace(i-2,3,QByteArray());
            previous = 0;
            previousOfTheprevious = 0;
            i--;
        } else {
            previousOfTheprevious = previous;
            previous = current;
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
    // XXX change to skipLeadingWhiteSpaces and take a cursor
    int whiteSpaceCount = 0;
    bool nonWhiteSpaceFound = false;
    for (int i=pos; i<text.length() && !nonWhiteSpaceFound; i++) {
        char current = text.at(i);
        if (current == ' ' ||
            current == '\t' ||
            current == '\r' ||
            current == '\n') {
            whiteSpaceCount++;
        } else {
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
        char current = text.at(i);
        if (shouldBeQuotedPrintableEncoded(current)) {
            QString encodedStr;
            encodedStr.sprintf("=%02X",current);
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
    // XXX in theory we won't get newlines in this any more
    for (int i=0; i < text.length(); i++) {
        char current = text.at(i);
        if (current == '=' && i+2 < text.length()) {
            char next = text.at(i+1);
            char nextAfterNext = text.at(i+2);
            if (next == '\r' && nextAfterNext == '\n') {
                text.remove(i,3);
            }
            if (((next >= 'a' && next <= 'f') ||
                 (next >= 'A' && next <= 'F') ||
                 (next >= '0' && next <= '9')) &&
                ((nextAfterNext >= 'a' && nextAfterNext <= 'f') ||
                 (nextAfterNext >= 'A' && nextAfterNext <= 'F') ||
                 (nextAfterNext >= '0' && nextAfterNext <= '9'))) {
                QByteArray hexEncodedChar(text.mid(i+1,2));
                bool decoded = false; 
                char decodedChar = hexEncodedChar.toInt(&decoded,16);
                QByteArray decodedCharAsByteArray;
                decodedCharAsByteArray.append(decodedChar);
                if (decoded) {
                    text.replace(i,3,decodedCharAsByteArray);
                }
            }
        }
    }
}

/*!
 * Performs backslash escaping for line breaks (CRLFs),
 * semicolons, backslashes and commas according to RFC 2426.
 */
bool VersitUtils::backSlashEscape(QByteArray& text)
{
    bool escaped = false;
    bool withinQuotes = false;
    char previous = 0;
    for (int i=0; i < text.length(); i++) {
        char current = text.at(i);
        if (previous != '\\' && !withinQuotes) {
            char next = 0;
            if (i != text.length()-1)
                 next = text.at(i+1);
            if (current == ';' || current == ',' ||
                (current == '\\' &&
                 next != '\\' && next != ';' && next != ',' && next != 'n')) {
                text.insert(i,'\\');
                i++;
                escaped = true;
            } else if (previous == '\r' && current == '\n') {
                text.replace(i-1,2,"\\n");
                escaped = true;
            } else {
                // NOP
            }
        }
        if (current == '"')
            withinQuotes = !withinQuotes;
        previous = current;
    }
    return escaped;
}

/*!
 * Removes backslash escaping for line breaks (CRLFs),
 * semicolons, backslashes and commas according to RFC 2426.
 */
void VersitUtils::removeBackSlashEscaping(QByteArray& text)
{
    // XXX build up a list of escapes and do them all at once?
    char previous = 0;
    bool withinQuotes = false;
    for (int i=0; i < text.length(); i++) {
        char current = text.at(i);
        if (previous == '\\' && !withinQuotes) {
            if (current == ';' || current == ',' || current == '\\') {
                text.remove(i-1,1);
            } else if (current == 'n' || current == 'N') {
                text.replace(i-1,2,"\r\n");
            } else {
                // NOP
            }
        }
        if (current == '"')
            withinQuotes = !withinQuotes;
        previous = current;
    }
}

/*!
 * Extracts the groups and the name of the property.
 *
 * On entry, \a line should select a whole line.
 * On exit, \a line will be updated to point after the groups and name.
 */
QPair<QStringList,QString> VersitUtils::extractPropertyGroupsAndName(VersitCursor& line)
{
    QPair<QStringList,QString> groupsAndName;
    int length = 0;
    char previous = 0;
    Q_ASSERT(line.data.size() > line.position);
    for (int i=line.position; i < line.selection; i++) {
        char current = line.data.at(i);
        if ((current == ';' && previous != '\\')
            || current == ':') {
            length = i - line.position;
            break;
        }
        previous = current;
    }
    if (length > 0) {
        QString trimmedGroupsAndName = QString::fromAscii(line.data.mid(line.position, length).trimmed());
        QStringList parts = trimmedGroupsAndName.split(QString::fromAscii("."));
        if (parts.count() > 1) {
            groupsAndName.second = parts.takeLast();
            groupsAndName.first = parts;
        } else {
            groupsAndName.second = trimmedGroupsAndName;
        }
        line.setPosition(length + line.position);
    }

    return groupsAndName;
}

/*!
 * Extracts the value of the property.
 * Returns an empty string if the value was not found.
 *
 * On entry \a line should point to the value anyway.
 * On exit \a line should point to newline after the value
 */
QByteArray VersitUtils::extractPropertyValue(VersitCursor& line)
{
    QByteArray value = line.data.mid(line.position, line.selection - line.position);

    /* Now advance the cursor in all cases (TODO in state based should go to pending).. */
    line.position = line.selection;
    return value;
}

/*!
 * Extracts the property parameters as a QMultiHash.
 * The parameters without names are added as "TYPE" parameters.
 *
 * On entry \a line should contain the entire line.
 * On exit, line will be updated to point to the start of the value.
 */
QMultiHash<QString,QString> VersitUtils::extractVCard21PropertyParams(VersitCursor& line)
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name = QString::fromAscii(paramName(param));
        QString value = QString::fromAscii(paramValue(param));
        result.insert(name,value);
    }

    return result;
}

/*!
 * Extracts the property parameters as a QMultiHash.
 * The parameters without names are added as "TYPE" parameters.
 */
QMultiHash<QString,QString> VersitUtils::extractVCard30PropertyParams(VersitCursor& line)
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QByteArray name = paramName(param);
        removeBackSlashEscaping(name);
        QByteArray values = paramValue(param);
        QList<QByteArray> valueList = extractParts(values,',');
        while (!valueList.isEmpty()) {
            QByteArray value(valueList.takeLast());
            removeBackSlashEscaping(value);
            result.insert(QString::fromAscii(name),QString::fromAscii(value));
        }
    }

    return result;
}


/*!
 * Get the next line of input to parse.
 *
 * On entry, \a line should contain the current data buffer and offset.
 * On exit, \a line will have the updated selection corresponding to the
 * total line (to the point just before the newline).
 *
 * Returns true if a full line was found, false otherwise.
 */
bool VersitUtils::getNextLine(VersitCursor &line)
{
    int crlfPos;

    /* See if we can find a newline */
    forever {
        crlfPos = line.data.indexOf("\r\n", line.position);
        if (crlfPos == -1)
            crlfPos = line.data.indexOf("\n", line.position);
        if (crlfPos == -1)
            crlfPos = line.data.indexOf("\r", line.position);
        if (crlfPos == line.position) {
            /* initial newline, repeat - might need to advance two chars */
            if ((line.position < line.data.size() - 1)
                    && line.data.at(line.position) == '\r'
                    && line.data.at(line.position + 1) == '\n')
                line.setPosition(line.position + 2);
            else
                line.setPosition(line.position + 1);
            continue;
        } else if (crlfPos > line.position) {
            line.selection = crlfPos;
            return true;
        } else {
            // No crlf - return false
            return false;
        }
    }
}


/*!
 * Extracts the parameters as delimited by semicolons.
 *
 * On entry \a line should point to the start of the parameter section (past the name).
 * On exit, \a line will be updated to point to the start of the value.
 */
QList<QByteArray> VersitUtils::extractParams(VersitCursor& line)
{
    QList<QByteArray> params;

    /* find the end of the name&params */
    int colonIndex = line.data.indexOf(':', line.position);
    if (colonIndex > line.position && colonIndex < line.selection) {
        QByteArray nameAndParamsString = line.data.mid(line.position, colonIndex - line.position);
        params = extractParts(nameAndParamsString,';');

        /* Update line */
        line.setPosition(colonIndex + 1);
    } else if (colonIndex == line.position) {
        // No parameters.. advance past it
        line.setPosition(line.position + 1);
    }

    return params;
}

/*!
 * Extracts the parts separated by separator
 * discarding the separators escaped with a backslash
 */
QList<QByteArray> VersitUtils::extractParts(const QByteArray& text, char separator)
{
    QList<QByteArray> parts;
    int partStartIndex = 0;
    char previous = 0;
    for (int i=0; i<text.length(); i++) {
        char current = text.at(i);
        if (current == separator && previous != '\\') {
            int length = i-partStartIndex;
            QByteArray part = extractPart(text,partStartIndex,length);
            if (part.length() > 0)
                parts.append(part);
            partStartIndex = i+1;
        }
        previous = current;
    }

    // Add the last or only part
    QByteArray part = extractPart(text,partStartIndex);
    if (part.length() > 0)
        parts.append(part);
    return parts;
}

/*!
 * Extracts a substring limited by /a startPosition and /a length.
 */
QByteArray VersitUtils::extractPart(const QByteArray& text, int startPosition, int length)
{
    QByteArray part;
    if (startPosition >= 0)
        part = text.mid(startPosition,length).trimmed();
    return part;
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
     if (equalsIndex > 0) {
         return parameter.left(equalsIndex).trimmed();
     }

     return QByteArray("TYPE");
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
            value = QByteArray();
        } else {
            int valueLength = parameter.length() - (equalsIndex + 1);
            value = parameter.right(valueLength).trimmed();
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

QTM_END_NAMESPACE
