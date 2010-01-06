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
#include <QTextCodec>

QTM_BEGIN_NAMESPACE

QTextCodec* VersitUtils::m_previousCodec = 0;
QList<QByteArray>* VersitUtils::m_newlineList = 0;
QByteArray VersitUtils::m_encodingMap[256];

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
bool VersitUtils::backSlashEscape(QString& text)
{
    bool escaped = false;
    bool withinQuotes = false;
    ushort previous = 0;
    for (int i=0; i < text.length(); i++) {
        ushort current = text.at(i).unicode();
        if (previous != '\\' && !withinQuotes) {
            int next = 0;
            if (i != text.length()-1)
                 next = text.at(i+1).unicode();
            if (current == ';' || current == ',' ||
                (current == '\\' &&
                 next != '\\' && next != ';' && next != ',' && next != 'n')) {
                text.insert(i,QChar::fromAscii('\\'));
                i++;
                escaped = true;
            } else if (previous == '\r' && current == '\n') {
                text.replace(i-1,2,QString::fromAscii("\\n"));
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
void VersitUtils::removeBackSlashEscaping(QString& text)
{
    // XXX build up a list of escapes and do them all at once?
    ushort previous = 0;
    bool withinQuotes = false;
    for (int i=0; i < text.length(); i++) {
        ushort current = text.at(i).unicode();
        if (previous == '\\' && !withinQuotes) {
            if (current == ';' || current == ',' || current == '\\' || current == ':') {
                text.remove(i-1,1);
            } else if (current == 'n' || current == 'N') {
                text.replace(i-1,2,QString::fromAscii("\r\n"));
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
 * Extracts the groups and the name of the property using \a codec to determine the delimeters
 *
 * On entry, \a line should select a whole line.
 * On exit, \a line will be updated to point after the groups and name.
 */
QPair<QStringList,QString> VersitUtils::extractPropertyGroupsAndName(VersitCursor& line,
                                                                     QTextCodec *codec)
{
    const QByteArray semicolon = encode(';', codec);
    const QByteArray colon = encode(':', codec);
    const QByteArray backslash = encode('\\', codec);
    QPair<QStringList,QString> groupsAndName;
    int length = 0;
    Q_ASSERT(line.data.size() > line.position);
    
    int separatorLength = semicolon.length();
    for (int i = line.position; i < line.selection - separatorLength + 1; i++) {
        if ((containsAt(line.data, semicolon, i)
                && !containsAt(line.data, backslash, i-separatorLength))
            || containsAt(line.data, colon, i)) {
            length = i - line.position;
            break;
        }
    }
    if (length > 0) {
        QString trimmedGroupsAndName =
                codec->toUnicode(line.data.mid(line.position, length)).trimmed();
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
 * Extracts the property parameters as a QMultiHash using \a codec to determine the delimeters.
 * The parameters without names are added as "TYPE" parameters.
 *
 * On entry \a line should contain the entire line.
 * On exit, line will be updated to point to the start of the value.
 */
QMultiHash<QString,QString> VersitUtils::extractVCard21PropertyParams(VersitCursor& line,
                                                                      QTextCodec *codec)
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line, codec);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name = paramName(param, codec);
        QString value = paramValue(param, codec);
        result.insert(name,value);
    }

    return result;
}

/*!
 * Extracts the property parameters as a QMultiHash using \a codec to determine the delimeters.
 * The parameters without names are added as "TYPE" parameters.
 */
QMultiHash<QString,QString> VersitUtils::extractVCard30PropertyParams(VersitCursor& line,
                                                                      QTextCodec *codec)
{
    QMultiHash<QString,QString> result;
    QList<QByteArray> paramList = extractParams(line, codec);
    while (!paramList.isEmpty()) {
        QByteArray param = paramList.takeLast();
        QString name(paramName(param, codec));
        removeBackSlashEscaping(name);
        QString values = paramValue(param, codec);
        QList<QString> valueList = values.split(QString::fromAscii(","), QString::SkipEmptyParts);
        QString buffer; // for any part ending in a backslash, join it to the next.
        foreach (QString value, valueList) {
            if (value.endsWith(QChar::fromAscii('\\'))) {
                value.chop(1);
                buffer.append(value);
                buffer.append(QString::fromAscii(",")); // because the comma got nuked by split()
            }
            else {
                buffer.append(value);
                removeBackSlashEscaping(buffer);
                result.insert(name, buffer);
                buffer.clear();
            }
        }
    }

    return result;
}

/*!
 * Get the next line of input to parse using \a codec to determine the line delimeters.
 *
 * On entry, \a line should contain the current data buffer and offset.
 * On exit, \a line will have the updated selection corresponding to the
 * total line (to the point just before the newline).
 *
 * Returns true if a full line was found, false otherwise.
 */
bool VersitUtils::getNextLine(VersitCursor &line, QTextCodec* codec)
{
    int crlfPos;

    /* See if we can find a newline */
    QList<QByteArray>* newlines = newlineList(codec);
    QByteArray space = encode(' ', codec);
    QByteArray tab = encode('\t', codec);
    forever {
        foreach(QByteArray newline, *newlines) {
            int newlineLength = newline.length();
            crlfPos = line.data.indexOf(newline, line.position);
            if (crlfPos == line.position) {
                // Newline at start of line.  Set position to directly after it
                line.position += newlineLength;
                break;
            } else if (crlfPos > line.position) {
                // Found the newline.  Check that it's not followed by a whitespace.
                if (containsAt(line.data, space, crlfPos + newlineLength)
                    || containsAt(line.data, tab, crlfPos + newlineLength)) {
                    line.data.remove(crlfPos, newlineLength + space.length());
                    break;
                } else {
                    line.selection = crlfPos;
                    return true;
                }
            }
        }
        if (crlfPos == -1) {
            // No crlf - return false
            return false;
        }
    }
}


/*!
 * Extracts the parameters as delimited by semicolons using \a codec to determine the delimeters.
 *
 * On entry \a line should point to the start of the parameter section (past the name).
 * On exit, \a line will be updated to point to the start of the value.
 */
QList<QByteArray> VersitUtils::extractParams(VersitCursor& line, QTextCodec *codec)
{
    const QByteArray colon = encode(':', codec);
    QList<QByteArray> params;

    /* find the end of the name&params */
    int colonIndex = line.data.indexOf(colon, line.position);
    if (colonIndex > line.position && colonIndex < line.selection) {
        QByteArray nameAndParamsString = line.data.mid(line.position, colonIndex - line.position);
        params = extractParts(nameAndParamsString, encode(';', codec), codec);

        /* Update line */
        line.setPosition(colonIndex + colon.length());
    } else if (colonIndex == line.position) {
        // No parameters.. advance past it
        line.setPosition(line.position + colon.length());
    }

    return params;
}

/*!
 * Extracts the parts separated by separator discarding the separators escaped with a backslash
 * encoded with \a codec
 */
QList<QByteArray> VersitUtils::extractParts(const QByteArray& text, const QByteArray& separator,
                                            QTextCodec* codec)
{
    QList<QByteArray> parts;
    int partStartIndex = 0;
    int textLength = text.length();
    int separatorLength = separator.length();
    QByteArray backslash = encode('\\', codec);
    int backslashLength = backslash.length();

    for (int i=0; i < textLength-separatorLength+1; i++) {
        if (containsAt(text, separator, i)
            && (i < backslashLength
                || !containsAt(text, backslash, i-backslashLength))) {
            int length = i-partStartIndex;
            QByteArray part = extractPart(text,partStartIndex,length);
            if (part.length() > 0)
                parts.append(part);
            partStartIndex = i+separatorLength;
        }
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
 * Extracts the name of the parameter using \a codec to determine the delimeters.
 * No name is interpreted as an implicit "TYPE".
 */
QString VersitUtils::paramName(const QByteArray& parameter, QTextCodec* codec)
{
     if (parameter.trimmed().length() == 0)
         return QString();
     QByteArray equals = encode('=', codec);
     int equalsIndex = parameter.indexOf(equals);
     if (equalsIndex > 0) {
         return codec->toUnicode(parameter.left(equalsIndex)).trimmed();
     }

     return QString::fromAscii("TYPE");
}

/*!
 * Extracts the value of the parameter using \a codec to determine the delimeters
 */
QString VersitUtils::paramValue(const QByteArray& parameter, QTextCodec* codec)
{
    QByteArray value(parameter);
    QByteArray equals = encode('=', codec);
    int equalsIndex = parameter.indexOf(equals);
    if (equalsIndex > 0) {
        int valueLength = parameter.length() - (equalsIndex + equals.length());
        value = parameter.right(valueLength).trimmed();
    }

    return codec->toUnicode(value);
}

/*!
 * Returns true if and only if \a text contains \a ba at \a index
 *
 * On entry, index must be >= 0
 */
bool VersitUtils::containsAt(const QByteArray& text, const QByteArray& match, int index)
{
    int n = match.length();
    if (text.length() - index < n)
        return false;
    const char* textData = text.data();
    const char* matchData = match.data();
    for (int i = n-1; i >= 0; i--) {
        if (textData[index+i] != matchData[i])
            return false;
    }
    return true;
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

/*!
 * Encode \a ch with \a codec, without adding an byte-order mark
 */
QByteArray VersitUtils::encode(char ch, QTextCodec* codec)
{
    if (codec != m_previousCodec) {
        changeCodec(codec);
    }
    return m_encodingMap[(int)ch];
}

/*!
 * Encode \a ba with \a codec, without adding an byte-order mark.  \a ba is interpreted as ASCII
 */
QByteArray VersitUtils::encode(const QByteArray& ba, QTextCodec* codec)
{
    QTextCodec::ConverterState state(QTextCodec::IgnoreHeader);
    return codec->fromUnicode(QString::fromAscii(ba.data()).data(), ba.length(), &state);
}

/*!
 * Returns the list of DOS, UNIX and Mac newline characters for \a codec.
 */
QList<QByteArray>* VersitUtils::newlineList(QTextCodec* codec)
{
    if (m_newlineList != 0 && codec == m_previousCodec) {
        return m_newlineList;
    }
    changeCodec(codec);
    return m_newlineList;
}

/*!
 * Update the cached tables of pregenerated encoded text with \a codec.
 */
void VersitUtils::changeCodec(QTextCodec* codec) {
    // Build m_encodingMap
    QChar qch;
    QTextCodec::ConverterState state(QTextCodec::IgnoreHeader);
    for (int c = 0; c < 256; c++) {
        qch = QChar::fromAscii(c);
        m_encodingMap[c] = codec->fromUnicode(&qch, 1, &state);
    }

    // Build m_newlineList
    if (m_newlineList != 0)
        delete m_newlineList;
    m_newlineList = new QList<QByteArray>;
    m_newlineList->append(encode("\r\n", codec));
    m_newlineList->append(encode("\n", codec));
    m_newlineList->append(encode("\r", codec));

    m_previousCodec = codec;
}

QTM_END_NAMESPACE
