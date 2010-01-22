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

#include "ut_versitutils.h"
#include <QByteArray>
#include <QtTest/QtTest>
#include <QTextCodec>

#include "versitutils_p.h"

QTM_USE_NAMESPACE

void UT_VersitUtils::init()
{
    m_asciiCodec = QTextCodec::codecForName("ISO 8859-1");
}

void UT_VersitUtils::cleanup()
{
}

void UT_VersitUtils::testSkipLeadingWhiteSpaces()
{
    VersitCursor line;
    // Empty string
    line.setPosition(0);
    VersitUtils::skipLeadingWhiteSpaces(line, m_asciiCodec);
    QCOMPARE(line.position, 0);
    
    // Only whitespaces
    line.setData(" \t \t");
    line.setPosition(0);
    VersitUtils::skipLeadingWhiteSpaces(line, m_asciiCodec);
    QCOMPARE(line.position, 4);

    // One whitespace with other characters
    line.setData(" text");
    line.setPosition(0);
    VersitUtils::skipLeadingWhiteSpaces(line, m_asciiCodec);
    QCOMPARE(line.position, 1);
    
    // Several whitespaces with other characters
    line.setData(" \t \ttext");
    line.setPosition(0);
    VersitUtils::skipLeadingWhiteSpaces(line, m_asciiCodec);
    QCOMPARE(line.position, 4);
    
    // No whitespaces
    line.setData("text");
    line.setPosition(0);
    VersitUtils::skipLeadingWhiteSpaces(line, m_asciiCodec);
    QCOMPARE(line.position, 0);
    
    // Start counting whitespaces in the middle of the string
    line.setData("some text");
    line.setPosition(4);
    VersitUtils::skipLeadingWhiteSpaces(line, m_asciiCodec);
    QCOMPARE(line.position, 5);
}

void UT_VersitUtils::testParamName()
{
    // Empty value
    QByteArray param;
    QCOMPARE(VersitUtils::paramName(param, m_asciiCodec),QString());

    // Only value present
    param = "WORK";
    QCOMPARE(VersitUtils::paramName(param, m_asciiCodec),
             QString::fromAscii("TYPE"));

    // The below tests intentionally use the misspelling TIPE to avoid the default behaviour of
    // returning TYPE when the name can't be parsed.
    // Both name and value, spaces after the name
    param = "TIPE \t =WORK";
    QCOMPARE(VersitUtils::paramName(param, m_asciiCodec),
             QString::fromAscii("TIPE"));

    // Both name and value, no spaces after the name
    param = "TIPE=WORK";
    QCOMPARE(VersitUtils::paramName(param, m_asciiCodec),
             QString::fromAscii("TIPE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    param = codec->fromUnicode(QString::fromAscii("TIPE=WORK"));
    QCOMPARE(VersitUtils::paramName(param, codec),
             QString::fromAscii("TIPE"));

}

void UT_VersitUtils::testParamValue()
{
    // Empty value
    QByteArray param;
    QCOMPARE(VersitUtils::paramValue(param, m_asciiCodec),QString());
    
    // Only value present
    param = "WORK";
    QCOMPARE(VersitUtils::paramValue(param, m_asciiCodec),
             QString::fromAscii("WORK"));
    
    // Name and equals sign, but no value
    param = "TYPE=";
    QCOMPARE(VersitUtils::paramValue(param, m_asciiCodec),QString());
    
    // Name and equals sign, but value has only spaces
    param = "TYPE=  \t ";
    QCOMPARE(VersitUtils::paramValue(param, m_asciiCodec),QString());
    
    // Both name and value, spaces before the value
    param = "TYPE= \t  WORK";
    QCOMPARE(VersitUtils::paramValue(param, m_asciiCodec),
             QString::fromAscii("WORK"));
    
    // Both name and value, no spaces before the value
    param = "ENCODING=QUOTED-PRINTABLE";
    QCOMPARE(VersitUtils::paramValue(param, m_asciiCodec),
             QString::fromAscii("QUOTED-PRINTABLE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    param = codec->fromUnicode(QString::fromAscii("TYPE=WORK"));
    QCOMPARE(VersitUtils::paramValue(param, codec),
             QString::fromAscii("WORK"));
}

void UT_VersitUtils::testExtractPart()
{
    QByteArray originalStr;

    // Negative starting position
    QCOMPARE(VersitUtils::extractPart(originalStr,-1,1), QByteArray());

    // Empty original string
    QCOMPARE(VersitUtils::extractPart(originalStr,0,1), QByteArray());

    // Trimmed substring empty
    originalStr = " \t \t";
    QCOMPARE(VersitUtils::extractPart(originalStr,0,4), QByteArray());

    // The given substring length is greater than the original string length
    originalStr = "ENCODING=7BIT";
    QCOMPARE(VersitUtils::extractPart(originalStr,0,100), originalStr);

    // Non-empty substring, from the beginning
    originalStr = " TYPE=WORK ; X-PARAM=X-VALUE; ENCODING=8BIT";
    QCOMPARE(VersitUtils::extractPart(originalStr,0,11),
             QByteArray("TYPE=WORK"));
    
    // Non-empty substring, from the middle
    QCOMPARE(VersitUtils::extractPart(originalStr,12,16),
             QByteArray("X-PARAM=X-VALUE"));
    
    // Non-empty substring, from the middle to the end
    QCOMPARE(VersitUtils::extractPart(originalStr,29),
             QByteArray("ENCODING=8BIT"));
}

void UT_VersitUtils::testExtractParts()
{
    QList<QByteArray> parts;

    // Empty value
    QByteArray text;
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QVERIFY(parts.isEmpty());

    // Only separator
    text = ";";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QVERIFY(parts.isEmpty());

    // One part
    text = "part";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // Separator in the beginning, one part
    text = ";part";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // Separator in the end, one part
    text = "part;";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // One part that contains escaped separator
    text = "part\\;";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part\\;"));

    // Two parts
    text = "part1;part2";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part1"));
    QCOMPARE(QString::fromAscii(parts[1]),QString::fromAscii("part2"));

    // Two parts that contain escaped separators
    text = "pa\\;rt1;par\\;t2";
    parts = VersitUtils::extractParts(text,";", m_asciiCodec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("pa\\;rt1"));
    QCOMPARE(QString::fromAscii(parts[1]),QString::fromAscii("par\\;t2"));

    // Test wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    text = codec->fromUnicode(QString::fromAscii("part1;part2"));
    parts = VersitUtils::extractParts(text,";", codec);
    QCOMPARE(parts.count(),2);
    QCOMPARE(codec->toUnicode(parts[0]),QString::fromAscii("part1"));
    QCOMPARE(codec->toUnicode(parts[1]),QString::fromAscii("part2"));
}

void UT_VersitUtils::testFold()
{
    int maxCharsForLine = 3;

    // Empty string
    QByteArray unfolded;
    QByteArray folded;
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // String that needs one folding
    unfolded = "12345";
    folded = "123\r\n 45";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // String that needs multiple foldings
    unfolded = "1234567";
    folded = "123\r\n 45\r\n 67";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // String that has line breaks and needs foldings
    unfolded = "12\r\n3456789";
    folded = "12\r\n345\r\n 67\r\n 89";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // Line break in the beginning of the string
    unfolded = "\r\n12345";
    folded = "\r\n123\r\n 45";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // Input contains the amount of characters after which a folding
    // would normally be added -> Folding not added
    unfolded = "123";
    folded = "123";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // CR in the end of the string -> Folding not added
    unfolded = "123\r";
    folded = "123\r";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));

    // CRLF in the end of the string -> Folding not added
    unfolded = "123\r\n";
    folded = "123\r\n";
    QCOMPARE(
        QString::fromAscii(VersitUtils::fold(unfolded,maxCharsForLine)),
        QString::fromAscii(folded));
}

void UT_VersitUtils::testQuotedPrintableEncode()
{
    QByteArray encodedBytes;
    
    // Nothing to encode
    QString nothingToEncode(QLatin1String("nothing to encode"));
    QVERIFY(!VersitUtils::quotedPrintableEncode(nothingToEncode));
    
    // Special characters
    QString inputOutput(QLatin1String("\n"));
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=0A"));
    inputOutput = QLatin1String("\r");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=0D"));
    inputOutput = QLatin1String("!");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=21"));
    inputOutput = QLatin1String("\"");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=22"));
    inputOutput = QLatin1String("#");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=23"));
    inputOutput = QLatin1String("$");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=24"));
    inputOutput = QLatin1String("=");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=3D"));
    inputOutput = QLatin1String("@");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=40"));
    inputOutput = QLatin1String("[");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5B"));
    inputOutput = QLatin1String("\\");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5C"));
    inputOutput = QLatin1String("]");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5D"));
    inputOutput = QLatin1String("^");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5E"));
    inputOutput = QLatin1String("`");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=60"));
    inputOutput = QLatin1String("{");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7B"));
    inputOutput = QLatin1String("|");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7C"));
    inputOutput = QLatin1String("}");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7D"));
    inputOutput = QLatin1String("~");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7E"));
}

void UT_VersitUtils::testDecodeQuotedPrintable()
{
    // Soft line breaks
    QString encoded(QLatin1String("This=\r\n is =\r\none line."));
    QString decoded(QLatin1String("This is one line."));
    VersitUtils::decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
    
    // Characters recommended to be encoded according to RFC 1521:
    encoded = QLatin1String("To be decoded: =0A=0D=21=22=23=24=3D=40=5B=5C=5D=5E=60=7B=7C=7D=7E");
    decoded = QLatin1String("To be decoded: \n\r!\"#$=@[\\]^`{|}~");
    VersitUtils::decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
    
    // Other random characters encoded.
    // Some implementation may encode these too, as it is allowed.
    encoded = QLatin1String("=45=6E=63=6F=64=65=64 =64=61=74=61");
    decoded = QLatin1String("Encoded data");
    VersitUtils::decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
}

void UT_VersitUtils::testBackSlashEscape()
{
    // Empty string
    QString input;
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QString::fromAscii("Nothing to escape");
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("Nothing to escape"));

    // Line break in the beginning
    input = QString::fromAscii("\r\n input");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("\\n input"));

    // Line break in the end
    input = QString::fromAscii("input\r\n");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("input\\n"));

    // Semicolon in the beginning
    input = QString::fromAscii(";input");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("\\;input"));

    // Semicolon in the end
    input = QString::fromAscii("input;");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("input\\;"));

    // Comma in the beginning
    input = QString::fromAscii(",input");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("\\,input"));

    // Comma in the end
    input = QString::fromAscii("input,");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("input\\,"));

    // Backslash in the beginning
    input = QString::fromAscii("\\input");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("\\\\input"));

    // Backslash in the end
    input = QString::fromAscii("input\\");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QString::fromAscii("input\\\\"));

    // Line break, semicolon, backslash and comma in the middle of the string
    input = QString::fromAscii("Escape these \r\n ; , \\ ");
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(input, QString::fromAscii("Escape these \\n \\; \\, \\\\ "));

    // Escaping not done for an already escaped string
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(input, QString::fromAscii("Escape these \\n \\; \\, \\\\ "));

    // Don't escape special characters within quotes
    input = QString::fromAscii("Quoted \"\r\n ; , \\ \"");
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(input, QString::fromAscii("Quoted \"\r\n ; , \\ \""));

}

void UT_VersitUtils::testRemoveBackSlashEscaping()
{
    // Empty string
    QString input;
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QString::fromAscii("Nothing to escape");
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input,QString::fromAscii("Nothing to escape"));

    // Line break, semicolon, backslash and comma in the string
    input = QString::fromAscii("These should be unescaped \\n \\N \\; \\, \\\\");
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input, QString::fromAscii("These should be unescaped \r\n \r\n ; , \\"));

    // Don't remove escaping within quotes
    input = QString::fromAscii("Quoted \"\\n \\N \\; \\,\"");
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(input, QString::fromAscii("Quoted \"\\n \\N \\; \\,\""));
}

void UT_VersitUtils::testExtractPropertyGroupsAndName()
{
    QPair<QStringList,QString> groupsAndName;

    // Empty string
    VersitCursor cursor(QByteArray(" "));
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());
    
    // No value -> returns empty string and no groups
    QByteArray property("TEL");
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());
    
    // Simple name and value
    property = "TEL:123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // One whitespace before colon
    property = "TEL :123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Several whitespaces before colon
    property = "TEL \t  :123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Name contains a group
    property = "group1.TEL:1234";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),1);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));
    
    // Name contains more than one group
    property = "group1.group2.TEL:12345";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group2"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));
    QCOMPARE(cursor.position, 17);

    // Property contains one parameter
    property = "TEL;WORK:123";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Property contains several parameters
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("EMAIL"));
    
    // Name contains an escaped semicolon
    property = "X-proper\\;ty:value";
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, m_asciiCodec);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("X-proper\\;ty"));

    // Test wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    property = codec->fromUnicode(QString::fromAscii("group1.group2.TEL;WORK:123"));
    cursor.setData(property);
    cursor.selection = property.size();
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(cursor, codec);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group2"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));
    QCOMPARE(cursor.position, 36); // 2 bytes * 17 characters + 2 byte BOM.

}

void UT_VersitUtils::testExtractVCard21PropertyParams()
{
    // No parameters
    VersitCursor cursor(QByteArray(":123"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec).count(), 0);
    
    // "Empty" parameter
    cursor.setData(QByteArray(";:123"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec).count(), 0);
    
    // Semicolon found, but no value for the property
    cursor.setData(QByteArray(";TYPE=X-TYPE"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec).count(), 0);
    
    // The property name contains an escaped semicolon, no parameters
    cursor.setData(QByteArray(":value"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec).count(), 0);

    // The property value contains a semicolon, no parameters
    cursor.setData(QByteArray(":va;lue"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec).count(), 0);
    
    // One parameter
    cursor.setData(QByteArray(";HOME:123"));
    cursor.setSelection(cursor.data.size());
    QMultiHash<QString,QString> params = VersitUtils::extractVCard21PropertyParams(cursor,
                                                                                   m_asciiCodec);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
      
    // Two parameters of the same type
    cursor.setData(QByteArray(";HOME;VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));
    
    // Two parameters, several empty parameters (extra semicolons)
    cursor.setData(QByteArray(";;;;HOME;;;;;VOICE;;;:123"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));
    
    // Two parameters with different types
    cursor.setData(QByteArray(";INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com"));
    cursor.setSelection(cursor.data.size());
    params.clear();
    params = VersitUtils::extractVCard21PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(2, params.count());
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QString::fromAscii("INTERNET"));
    QList<QString> encodingParams = params.values(QString::fromAscii("ENCODING"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("QUOTED-PRINTABLE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode(";HOME;CHARSET=UTF-16:123", codec);
    cursor.setData(data);
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard21PropertyParams(cursor, codec);
    QCOMPARE(2, params.count());
    typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QString::fromAscii("HOME"));
    encodingParams = params.values(QString::fromAscii("CHARSET"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("UTF-16"));
}

void UT_VersitUtils::testExtractVCard30PropertyParams()
{
    // No parameters
    VersitCursor cursor(QByteArray(":123"));
    cursor.setSelection(cursor.data.size());
    QCOMPARE(VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec).count(), 0);

    // One parameter
    cursor.setData(QByteArray(";TYPE=HOME:123"));
    cursor.setSelection(cursor.data.size());
    QMultiHash<QString,QString> params = VersitUtils::extractVCard30PropertyParams(cursor,
                                                                                   m_asciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0], QString::fromAscii("HOME"));

    // One parameter with an escaped semicolon
    cursor.setData(QByteArray(";para\\;meter:value"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0], QString::fromAscii("para;meter"));

    // One parameter with and escaped comma in the name and the value
    cursor.setData(QByteArray(";X-PA\\,RAM=VAL\\,UE:123"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("X-PA,RAM")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("X-PA,RAM"))[0], QString::fromAscii("VAL,UE"));

    // Two parameters of the same type
    cursor.setData(QByteArray(";TYPE=HOME,VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 2);
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("VOICE")));

    // Two parameters of the same type in separate name-values
    cursor.setData(QByteArray(";TYPE=HOME;TYPE=VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 2);
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("VOICE")));

    // Three parameters of the same type
    cursor.setData(QByteArray(";TYPE=PREF,HOME,VOICE:123"));
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(params.count(), 3);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 3);
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("PREF")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TYPE")).contains(QString::fromAscii("VOICE")));

    // Two parameters with different types
    cursor.setData(QByteArray(";TYPE=HOME;X-PARAM=X-VALUE:Home Street 1"));
    cursor.setSelection(cursor.data.size());
    params.clear();
    params = VersitUtils::extractVCard30PropertyParams(cursor, m_asciiCodec);
    QCOMPARE(params.count(), 2);
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(typeParams.count(), 1);
    QCOMPARE(typeParams[0],QString::fromAscii("HOME"));
    QList<QString> encodingParams = params.values(QString::fromAscii("X-PARAM"));
    QCOMPARE(encodingParams.count(), 1);
    QCOMPARE(encodingParams[0],QString::fromAscii("X-VALUE"));

    // Test wide character support.
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode(";TIPE=HOME,VOICE;CHARSET=UTF-16:123", codec);
    cursor.setData(data);
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractVCard30PropertyParams(cursor, codec);
    QCOMPARE(params.count(), 3);
    typeParams = params.values(QString::fromAscii("TIPE"));
    QCOMPARE(params.values(QString::fromAscii("TIPE")).count(), 2);
    QVERIFY(params.values(QString::fromAscii("TIPE")).contains(QString::fromAscii("HOME")));
    QVERIFY(params.values(QString::fromAscii("TIPE")).contains(QString::fromAscii("VOICE")));
    encodingParams = params.values(QString::fromAscii("CHARSET"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("UTF-16"));
}

void UT_VersitUtils::testGetNextLine()
{
    VersitCursor cursor("");
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    QVERIFY(!VersitUtils::getNextLine(cursor, m_asciiCodec));

    // Extract the "a"
    cursor.setData("a\r\nb\r\n");
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 1);

    // Extract the "b", starting from just after the "a"
    cursor.setPosition(1);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.position, 3);
    QCOMPARE(cursor.selection, 4);

    // Ensure it works with \r newlines
    cursor.setData("a\rb\r");
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 1);
    cursor.setPosition(1);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.position, 2);
    QCOMPARE(cursor.selection, 3);

    // Ensure it works with \n newlines
    cursor.setData("a\nb\n");
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 1);
    cursor.setPosition(1);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.position, 2);
    QCOMPARE(cursor.selection, 3);

    // Basic unfolding
    cursor.setData("eno\r\n rmous\r\nelephant\r\n");
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.data, QByteArray("enormous\r\nelephant\r\n"));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 8);

    // Unfold tabs
    cursor.setData("eno\r\n\trmous\r\nelephant\r\n");
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.data, QByteArray("enormous\r\nelephant\r\n"));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 8);

    // Unfold multiple times
    cursor.setData("un\r\n fol\r\n\tde\r\n d\r\nline");
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, m_asciiCodec));
    QCOMPARE(cursor.data, QByteArray("unfolded\r\nline"));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 8);

    // Wide character support
    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    QByteArray data = VersitUtils::encode("a\r\nb\r\n", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 2);
    cursor.setPosition(2);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.position, 6);
    QCOMPARE(cursor.selection, 8);

    // Wide character, \r newlines
    data = VersitUtils::encode("a\rb\r", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 2);
    cursor.setPosition(2);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.position, 4);
    QCOMPARE(cursor.selection, 6);

    // Wide character, \n newlines
    data = VersitUtils::encode("a\nb\n", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 2);
    cursor.setPosition(2);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.position, 4);
    QCOMPARE(cursor.selection, 6);

    // Unfold multiple times
    data = VersitUtils::encode("un\r\n fol\r\n\tde\r\n d\r\nline", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    QVERIFY(VersitUtils::getNextLine(cursor, codec));
    QCOMPARE(cursor.data, VersitUtils::encode("unfolded\r\nline", codec));
    QCOMPARE(cursor.position, 0);
    QCOMPARE(cursor.selection, 16);
}

void UT_VersitUtils::testExtractParams()
{
    VersitCursor cursor;
    QByteArray data = ":123";
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    QList<QByteArray> params = VersitUtils::extractParams(cursor, m_asciiCodec);
    QCOMPARE(params.size(), 0);
    QCOMPARE(cursor.position, 1);

    data = "a;b:123";
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractParams(cursor, m_asciiCodec);
    QCOMPARE(params.size(), 2);
    QCOMPARE(cursor.position, 4);
    QCOMPARE(params.at(0), QByteArray("a"));
    QCOMPARE(params.at(1), QByteArray("b"));

    QTextCodec* codec = QTextCodec::codecForName("UTF-16BE");
    data = VersitUtils::encode(":123", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractParams(cursor, codec);
    QCOMPARE(params.size(), 0);
    QCOMPARE(cursor.position, 2);

    data = VersitUtils::encode("a;b:123", codec);
    cursor.setData(data);
    cursor.setPosition(0);
    cursor.setSelection(cursor.data.size());
    params = VersitUtils::extractParams(cursor, codec);
    QCOMPARE(params.size(), 2);
    QCOMPARE(cursor.position, 8);

}

QTEST_MAIN(UT_VersitUtils)

