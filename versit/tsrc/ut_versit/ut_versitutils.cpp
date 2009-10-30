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
#include "versitutils.h"
#include <QByteArray>
#include <QtTest/QtTest>

void UT_VersitUtils::testCountLeadingWhiteSpaces()
{
    // Empty string
    QByteArray text;
    QCOMPARE(VersitUtils::countLeadingWhiteSpaces(text), 0);
    
    // Only whitespaces
    text = " \r \n\t \t";
    QCOMPARE(VersitUtils::countLeadingWhiteSpaces(text), 7);

    // One whitespace with other characters
    text = " text";
    QCOMPARE(VersitUtils::countLeadingWhiteSpaces(text), 1);
    
    // Several whitespaces with other characters
    text = "\t\r\n\r\n text";
    QCOMPARE(VersitUtils::countLeadingWhiteSpaces(text), 6);    
    
    // No whitespaces
    text = "text";
    QCOMPARE(VersitUtils::countLeadingWhiteSpaces(text), 0);
    
    // Start counting whitespaces in the middle of the string
    text = "something \r\n\r\n";
    QCOMPARE(VersitUtils::countLeadingWhiteSpaces(text,9), 5);   
}

void UT_VersitUtils::testFindHardLineBreakInQuotedPrintable()
{
    // Empty string
    QCOMPARE(-1, VersitUtils::findHardLineBreakInQuotedPrintable(QByteArray()));
    
    // No line breaks
    QCOMPARE(-1, VersitUtils::findHardLineBreakInQuotedPrintable(QByteArray("just text")));
    
    // No soft line breaks
    QCOMPARE(4, VersitUtils::findHardLineBreakInQuotedPrintable(QByteArray("text\r\n")));
    
    // One soft line break
    QCOMPARE(7, VersitUtils::findHardLineBreakInQuotedPrintable(QByteArray("te=\r\nxt\r\n")));
    
    // Two soft line breaks
    QCOMPARE(10, VersitUtils::findHardLineBreakInQuotedPrintable(QByteArray("t=\r\ne=\r\nxt\r\n")));
}

void UT_VersitUtils::testParamName()
{
    // Empty value
    QByteArray param;
    QCOMPARE(VersitUtils::paramName(param),QByteArray());

    // Only value present
    param = "WORK";
    QCOMPARE(QString::fromAscii(VersitUtils::paramName(param)),
             QString::fromAscii("TYPE"));

    // Both name and value, spaces after the name
    param = "TYPE \t =WORK";
    QCOMPARE(QString::fromAscii(VersitUtils::paramName(param)),
             QString::fromAscii("TYPE"));

    // Both name and value, no spaces after the name
    param = "TYPE=WORK";
    QCOMPARE(QString::fromAscii(VersitUtils::paramName(param)),
             QString::fromAscii("TYPE"));
}

void UT_VersitUtils::testParamValue()
{
    // Empty value
    QByteArray param;
    QCOMPARE(VersitUtils::paramValue(param),QByteArray());
    
    // Only value present
    param = "WORK";
    QCOMPARE(QString::fromAscii(VersitUtils::paramValue(param)),
             QString::fromAscii("WORK"));
    
    // Name and equals sign, but no value
    param = "TYPE=";
    QCOMPARE(VersitUtils::paramValue(param),QByteArray());
    
    // Name and equals sign, but value has only spaces
    param = "TYPE=  \t ";
    QCOMPARE(VersitUtils::paramValue(param),QByteArray());
    
    // Both name and value, spaces before the value
    param = "TYPE= \t  WORK";
    QCOMPARE(QString::fromAscii(VersitUtils::paramValue(param)),
             QString::fromAscii("WORK"));
    
    // Both name and value, no spaces before the value
    param = "ENCODING=QUOTED-PRINTABLE";
    QCOMPARE(QString::fromAscii(VersitUtils::paramValue(param)),
             QString::fromAscii("QUOTED-PRINTABLE"));
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
    parts = VersitUtils::extractParts(text,';');
    QVERIFY(parts.isEmpty());

    // Only separator
    text = ";";
    parts = VersitUtils::extractParts(text,';');
    QVERIFY(parts.isEmpty());

    // One part
    text = "part";
    parts = VersitUtils::extractParts(text,';');
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // Separator in the beginning, one part
    text = ";part";
    parts = VersitUtils::extractParts(text,';');
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // Separator in the end, one part
    text = "part;";
    parts = VersitUtils::extractParts(text,';');
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part"));

    // One part that contains escaped separator
    text = "part\\;";
    parts = VersitUtils::extractParts(text,';');
    QCOMPARE(parts.count(),1);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part\\;"));

    // Two parts
    text = "part1;part2";
    parts = VersitUtils::extractParts(text,';');
    QCOMPARE(parts.count(),2);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("part1"));
    QCOMPARE(QString::fromAscii(parts[1]),QString::fromAscii("part2"));

    // Two parts that contain escaped separators
    text = "pa\\;rt1;par\\;t2";
    parts = VersitUtils::extractParts(text,';');
    QCOMPARE(parts.count(),2);
    QCOMPARE(QString::fromAscii(parts[0]),QString::fromAscii("pa\\;rt1"));
    QCOMPARE(QString::fromAscii(parts[1]),QString::fromAscii("par\\;t2"));
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

void UT_VersitUtils::testUnfold()
{
    // No folding
    QCOMPARE(QByteArray("no folding\r\n"), QByteArray("no folding\r\n"));
    
    // Single space
    QByteArray folded("unfol\r\n ded\r\n");
    QByteArray unfolded("unfolded\r\n");
    QCOMPARE(unfolded, VersitUtils::unfold(folded));

    // Multiple spaces
    folded = "unfol\r\n  ded\r\n";
    unfolded = "unfol ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));

    // Single tab
    folded = "unfol\r\n\tded\r\n";
    unfolded = "unfolded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));

    // Multiple tabs
    folded = "unfol\r\n\t\tded\r\n";
    unfolded = "unfol\tded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // One space and one tab
    folded = "unfol\r\n \tded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // One tab and one space
    folded = "unfol\r\n\t ded\r\n";
    unfolded = "unfol ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // Multiple spaces and tabs
    folded = "unfol\r\n\t \t  \t\t ded\r\n";
    unfolded = "unfol \t  \t\t ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // Two foldings
    folded = "un\r\n\tfol\r\n ded\r\n";
    unfolded = "unfolded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));

    // Three foldings
    folded = "un\r\n fol\r\n\tde\r\n d\r\n";
    unfolded = "unfolded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // Two actual lines containing foldings
    folded = "li\r\n\tne1\r\nl\r\n ine2\r\n";
    unfolded = "line1\r\nline2\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
}

void UT_VersitUtils::testQuotedPrintableEncode()
{
    QByteArray encodedBytes;
    
    // Nothing to encode
    QByteArray nothingToEncode("nothing to encode");
    QVERIFY(!VersitUtils::quotedPrintableEncode(nothingToEncode));
    
    // Special characters
    QByteArray inputOutput("\n");
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=0A"));
    inputOutput = "\r";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=0D"));
    inputOutput = "!";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=21"));
    inputOutput = "\"";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=22"));
    inputOutput = "#";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=23"));
    inputOutput = "$";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=24"));
    inputOutput = "=";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=3D"));
    inputOutput = "@";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=40"));
    inputOutput = "[";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=5B"));
    inputOutput = "\\";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=5C"));
    inputOutput = "]";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=5D"));
    inputOutput = "^";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=5E"));
    inputOutput = "`";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=60"));
    inputOutput = "{";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=7B"));
    inputOutput = "|";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=7C"));
    inputOutput = "}";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=7D"));
    inputOutput = "~";
    QVERIFY(VersitUtils::quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QByteArray("=7E"));
}

void UT_VersitUtils::testDecodeQuotedPrintable()
{
    // Soft line breaks
    QByteArray encoded("This=\r\n is =\r\none line.");
    QByteArray decoded("This is one line.");
    VersitUtils::decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
    
    // Characters recommended to be encoded according to RFC 1521:
    encoded = "To be decoded: =0A=0D=21=22=23=24=3D=40=5B=5C=5D=5E=60=7B=7C=7D=7E";
    decoded = "To be decoded: \n\r!\"#$=@[\\]^`{|}~";
    VersitUtils::decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
    
    // Other random characters encoded.
    // Some implementation may encode these too, as it is allowed.
    encoded = "=45=6E=63=6F=64=65=64 =64=61=74=61";
    decoded = "Encoded data";
    VersitUtils::decodeQuotedPrintable(encoded);
    QCOMPARE(encoded, decoded);
}

void UT_VersitUtils::testBackSlashEscape()
{
    // Empty string
    QByteArray input;
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(input,QByteArray());

    // Nothing to escape in the string
    input = "Nothing to escape";
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("Nothing to escape"));

    // Line break in the beginning
    input = "\r\n input";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("\\n input"));

    // Line break in the end
    input = "input\r\n";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("input\\n"));

    // Semicolon in the beginning
    input = ";input";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("\\;input"));

    // Semicolon in the end
    input = "input;";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("input\\;"));

    // Comma in the beginning
    input = ",input";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("\\,input"));

    // Comma in the end
    input = "input,";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("input\\,"));

    // Backslash in the beginning
    input = "\\input";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("\\\\input"));

    // Backslash in the end
    input = "input\\";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("input\\\\"));

    // Line break, semicolon, backslash and comma in the middle of the string
    input = "Escape these \r\n ; , \\ ";
    QVERIFY(VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),
             QString::fromAscii("Escape these \\n \\; \\, \\\\ "));

    // Escaping not done for an already escaped string
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),
             QString::fromAscii("Escape these \\n \\; \\, \\\\ "));

    // Don't escape special characters within quotes
    input = "Quoted \"\r\n ; , \\ \"";
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),
             QString::fromAscii("Quoted \"\r\n ; , \\ \""));

}

void UT_VersitUtils::testRemoveBackSlashEscaping()
{
    // Empty string
    QByteArray input;
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(input,QByteArray());

    // Nothing to escape in the string
    input = "Nothing to escape";
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(QString::fromAscii(input),QString::fromAscii("Nothing to escape"));

    // Line break, semicolon, backslash and comma in the string
    input = "These should be unescaped \\n \\N \\; \\, \\\\";
    VersitUtils::removeBackSlashEscaping(input);
    QCOMPARE(QString::fromAscii(input),
             QString::fromAscii("These should be unescaped \r\n \r\n ; , \\"));

    // Don't remove escaping within quotes
    input = "Quoted \"\\n \\N \\; \\,\"";
    QVERIFY(!VersitUtils::backSlashEscape(input));
    QCOMPARE(QString::fromAscii(input),
             QString::fromAscii("Quoted \"\\n \\N \\; \\,\""));
}

void UT_VersitUtils::testExtractPropertyGroupsAndName()
{
    QPair<QStringList,QString> groupsAndName;

    // Empty string
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(QByteArray());
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());
    
    // No value -> returns empty string and no groups
    QByteArray property("TEL");
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString());
    
    // Simple name and value
    property = "TEL:123";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // One whitespace before colon
    property = "TEL :123";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Several whitespaces before colon
    property = "TEL \t  :123";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Name contains a group
    property = "group1.TEL:1234";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),1);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));
    
    // Name contains more than one group
    property = "group1.group2.TEL:12345";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),2);
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group1"));
    QCOMPARE(groupsAndName.first.takeFirst(),QString::fromAscii("group2"));
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Property contains one parameter
    property = "TEL;WORK:123";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("TEL"));

    // Property contains several parameters
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("EMAIL"));
    
    // Name contains an escaped semicolon
    property = "X-proper\\;ty:value";
    groupsAndName = VersitUtils::extractPropertyGroupsAndName(property);
    QCOMPARE(groupsAndName.first.count(),0);
    QCOMPARE(groupsAndName.second,QString::fromAscii("X-proper\\;ty"));
}

void UT_VersitUtils::testExtractPropertyValue()
{    
    // Empty string
    QCOMPARE(VersitUtils::extractPropertyValue(QByteArray()), QByteArray());

    // No colon found
    QCOMPARE(VersitUtils::extractPropertyValue(QByteArray("TEL")), QByteArray());

    // Colon in the end
    QCOMPARE(VersitUtils::extractPropertyValue(QByteArray("TEL:")), QByteArray());

    // Simple name and value
    QByteArray property("TEL:123");
    QCOMPARE(VersitUtils::extractPropertyValue(property), QByteArray("123"));
    
    // Simple name and the value contains colons
    property = "X-property:12:3";
    QCOMPARE(VersitUtils::extractPropertyValue(property), QByteArray("12:3"));
}

void UT_VersitUtils::testExtractVCard21PropertyParams()
{
    // No parameters
    QByteArray property("TEL:123");
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(property).count(), 0);
    
    // "Empty" parameter
    property ="TEL;:123";
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(property).count(), 0);
    
    // Semicolon found, but no value for the property
    property = "X-property;TYPE=X-TYPE";
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(property).count(), 0);
    
    // The property name contains an escaped semicolon, no parameters
    property = "X-proper\\;ty:value";
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(property).count(), 0);

    // The property value contains a semicolon, no parameters
    property = "X-property:va;lue";
    QCOMPARE(VersitUtils::extractVCard21PropertyParams(property).count(), 0);
    
    // One parameter
    property = "TEL;HOME:123";
    QMultiHash<QString,QString> params =
        VersitUtils::extractVCard21PropertyParams(property);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
      
    // Two parameters of the same type
    property = "TEL;HOME;VOICE:123";
    params = VersitUtils::extractVCard21PropertyParams(property);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));
    
    // Two parameters, several empty parameters (extra semicolons)
    property = "TEL;;;;HOME;;;;;VOICE;;;:123";
    params = VersitUtils::extractVCard21PropertyParams(property);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));
    
    // Two parameters with different types
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    params.clear();
    params = VersitUtils::extractVCard21PropertyParams(property);
    QCOMPARE(2, params.count());
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(typeParams[0],QString::fromAscii("INTERNET"));
    QList<QString> encodingParams = params.values(QString::fromAscii("ENCODING"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(encodingParams[0],QString::fromAscii("QUOTED-PRINTABLE"));
}

void UT_VersitUtils::testExtractVCard30PropertyParams()
{
    // No parameters
    QByteArray property("TEL:123");
    QCOMPARE(VersitUtils::extractVCard30PropertyParams(property).count(), 0);

    // One parameter
    property = "TEL;TYPE=HOME:123";
    QMultiHash<QString,QString> params =
        VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],
             QString::fromAscii("HOME"));

    // One parameter with an escaped semicolon
    property = "X-property;para\\;meter:value";
    params = VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],
             QString::fromAscii("para;meter"));

    // One parameter with and escaped comma in the name and the value
    property = "TEL;X-PA\\,RAM=VAL\\,UE:123";
    params = VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 1);
    QCOMPARE(params.values(QString::fromAscii("X-PA,RAM")).count(), 1);
    QCOMPARE(params.values(QString::fromAscii("X-PA,RAM"))[0],
             QString::fromAscii("VAL,UE"));

    // Two parameters of the same type
    property = "TEL;TYPE=HOME,VOICE:123";
    params = VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));

    // Two parameters of the same type in separate name-values
    property = "TEL;TYPE=HOME;TYPE=VOICE:123";
    params = VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 2);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("VOICE"));

    // Three parameters of the same type
    property = "TEL;TYPE=PREF,HOME,VOICE:123";
    params = VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 3);
    QCOMPARE(params.values(QString::fromAscii("TYPE")).count(), 3);
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[0],QString::fromAscii("PREF"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[1],QString::fromAscii("HOME"));
    QCOMPARE(params.values(QString::fromAscii("TYPE"))[2],QString::fromAscii("VOICE"));

    // Two parameters with different types
    property = "ADR;TYPE=HOME;X-PARAM=X-VALUE:Home Street 1";
    params.clear();
    params = VersitUtils::extractVCard30PropertyParams(property);
    QCOMPARE(params.count(), 2);
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(typeParams.count(), 1);
    QCOMPARE(typeParams[0],QString::fromAscii("HOME"));
    QList<QString> encodingParams = params.values(QString::fromAscii("X-PARAM"));
    QCOMPARE(encodingParams.count(), 1);
    QCOMPARE(encodingParams[0],QString::fromAscii("X-VALUE"));
}
