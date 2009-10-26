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
    QCOMPARE(QString(), VersitUtils::paramName(QByteArray()));

    // Only value present
    QByteArray param("WORK");
    QCOMPARE(QString::fromAscii("TYPE"), VersitUtils::paramName(param));

    // Both name and value, spaces after the name
    param = "TYPE \t =WORK";
    QCOMPARE(QString::fromAscii("TYPE"), VersitUtils::paramName(param));    

    // Both name and value, no spaces after the name
    param = "TYPE=WORK";
    QCOMPARE(QString::fromAscii("TYPE"), VersitUtils::paramName(param));    
}

void UT_VersitUtils::testParamValue()
{
    // Empty value
    QCOMPARE(QString(), VersitUtils::paramValue(QByteArray()));
    
    // Only value present
    QByteArray param("WORK");
    QCOMPARE(QString::fromAscii("WORK"), VersitUtils::paramValue(param));
    
    // Name and equals sign, but no value
    param = "TYPE=";
    QCOMPARE(QString(), VersitUtils::paramValue(param));     
    
    // Name and equals sign, but value has only spaces
    param = "TYPE=  \t ";
    QCOMPARE(QString(), VersitUtils::paramValue(param));    
    
    // Both name and value, spaces before the value
    param = "TYPE= \t  WORK";
    QCOMPARE(QString::fromAscii("WORK"), VersitUtils::paramValue(param));    
    
    // Both name and value, no spaces before the value
    param = "ENCODING=QUOTED-PRINTABLE";
    QCOMPARE(QString::fromAscii("QUOTED-PRINTABLE"), VersitUtils::paramValue(param));
}

void UT_VersitUtils::testAddParam()
{
    QMultiHash<QString,QString> params;
    QByteArray originalStr;

    // Negative starting position
    VersitUtils::addParam(params,originalStr,-1,1);
    QCOMPARE(0, params.count());

    // Empty original string
    VersitUtils::addParam(params,originalStr,0,1);
    QCOMPARE(0, params.count());

    // Trimmed substring empty
    originalStr = " \t \t";
    VersitUtils::addParam(params,originalStr,0,4);
    QCOMPARE(0, params.count());

    // The given substring length is greater than the original string length
    originalStr = "ENCODING=7BIT";
    VersitUtils::addParam(params,originalStr,0,100);
    QCOMPARE(1, params.count());
    QCOMPARE(QString::fromAscii("7BIT"), params.value("ENCODING"));

    // Non-empty substring, from the beginning
    originalStr = " TYPE=WORK ; VOICE; ENCODING=8BIT";
    params.clear();
    VersitUtils::addParam(params,originalStr,0,11);
    QCOMPARE(1, params.count());
    QCOMPARE(QString::fromAscii("WORK"), params.value("TYPE"));
    
    // Non-empty substring, from the middle
    params.clear();
    VersitUtils::addParam(params,originalStr,12,6);
    QCOMPARE(1, params.count());
    QCOMPARE(QString::fromAscii("VOICE"), params.value("TYPE"));
    
    // Non-empty substring, from the middle to the end
    params.clear();
    VersitUtils::addParam(params,originalStr,20);
    QCOMPARE(1, params.count());
    QCOMPARE(QString::fromAscii("8BIT"), params.value("ENCODING"));
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

void UT_VersitUtils::testExtractPropertyName()
{
    QByteArray property("TEL");
    QString propertyName("TEL");

    // Empty string
    QCOMPARE(QString(), VersitUtils::extractPropertyName(QByteArray()));
    
    // No value -> returns empty string
    QCOMPARE(QString(), VersitUtils::extractPropertyName(property));
    
    // Simple name and value
    property = "TEL:123";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));

    // One whitespace before colon
    property = "TEL :123";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));

    // Several whitespaces before colon
    property = "TEL \t  :123";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));    

    // Name contains a group
    property = "group1.X-property:x";
    propertyName = QString::fromAscii("group1.X-property");
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));     
    
    // Name contains more than one group
    property = "group1.group2.X-property:x";
    propertyName = QString::fromAscii("group1.group2.X-property");
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));     

    // Property contains one parameter
    property = "TEL;WORK:123";
    propertyName = QString::fromAscii("TEL");
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));    

    // Property contains several parameters
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    propertyName = QString::fromAscii("EMAIL");
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));
    
    // Name contains an escaped semicolon
    property = "X-proper\\;ty:value";
    propertyName = QString::fromAscii("X-proper\\;ty");
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));     
}

void UT_VersitUtils::testExtractPropertyValue()
{    
    // Empty string
    QCOMPARE(QByteArray(), VersitUtils::extractPropertyValue(QByteArray()));

    // No colon found
    QCOMPARE(QByteArray(), VersitUtils::extractPropertyValue(QByteArray("TEL")));

    // Colon in the end
    QCOMPARE(QByteArray(), VersitUtils::extractPropertyValue(QByteArray("TEL:")));

    // Simple name and value
    QByteArray property("TEL:123");
    QCOMPARE(QByteArray("123"), VersitUtils::extractPropertyValue(property));
    
    // Simple name and the value contains colons
    property = "X-property:12:3";
    QCOMPARE(QByteArray("12:3"), VersitUtils::extractPropertyValue(property));    
}

void UT_VersitUtils::testExtractPropertyParams()
{
    // No parameters
    QByteArray property("TEL:123");
    QCOMPARE(0, VersitUtils::extractPropertyParams(property).count());
    
    // "Empty" parameter
    property ="TEL;:123";
    QCOMPARE(0, VersitUtils::extractPropertyParams(property).count());    
    
    // Semicolon found, but no value for the property
    property = "X-property;TYPE=X-TYPE";
    QCOMPARE(0, VersitUtils::extractPropertyParams(property).count());
    
    // The property name contains an escaped semicolon, no parameters
    property = "X-proper\\;ty:value";
    QCOMPARE(0, VersitUtils::extractPropertyParams(property).count());    

    // The property value contains a semicolon, no parameters
    property = "X-property:va;lue";
    QCOMPARE(0, VersitUtils::extractPropertyParams(property).count());    
    
    // One parameter
    property = "TEL;HOME:123";
    QMultiHash<QString,QString> params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(QString::fromAscii("HOME"), params.values(QString::fromAscii("TYPE"))[0]);      
    
    // One parameter with an escaped semicolon
    property = "X-property;para\\;meter:value";
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(QString::fromAscii("para\\;meter"), params.values(QString::fromAscii("TYPE"))[0]);
    
    // Two parameters of the same type
    property = "TEL;HOME;VOICE:123";
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(QString::fromAscii("VOICE"), params.values(QString::fromAscii("TYPE"))[0]);
    QCOMPARE(QString::fromAscii("HOME"), params.values(QString::fromAscii("TYPE"))[1]);
    
    // Two parameters, several empty parameters (extra semicolons)
    property = "TEL;;;;HOME;;;;;VOICE;;;:123";
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString::fromAscii("TYPE")).count());
    QCOMPARE(QString::fromAscii("VOICE"), params.values(QString::fromAscii("TYPE"))[0]);
    QCOMPARE(QString::fromAscii("HOME"), params.values(QString::fromAscii("TYPE"))[1]);    
    
    // Two parameters with different types
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    params.clear();
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(2, params.count());
    QList<QString> typeParams = params.values(QString::fromAscii("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(QString::fromAscii("INTERNET"), typeParams[0]);
    QList<QString> encodingParams = params.values(QString::fromAscii("ENCODING"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(QString::fromAscii("QUOTED-PRINTABLE"), encodingParams[0]);
}
