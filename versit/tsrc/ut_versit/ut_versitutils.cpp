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

#include "UT_VersitUtils.h"
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
    QCOMPARE(QString("TYPE"), VersitUtils::paramName(param));

    // Both name and value, spaces after the name
    param = "TYPE \t =WORK";
    QCOMPARE(QString("TYPE"), VersitUtils::paramName(param));    

    // Both name and value, no spaces after the name
    param = "TYPE=WORK";
    QCOMPARE(QString("TYPE"), VersitUtils::paramName(param));    
}

void UT_VersitUtils::testParamValue()
{
    // Empty value
    QCOMPARE(QString(), VersitUtils::paramValue(QByteArray()));
    
    // Only value present
    QByteArray param("WORK");
    QCOMPARE(QString("WORK"), VersitUtils::paramValue(param));
    
    // Name and equals sign, but no value
    param = "TYPE=";
    QCOMPARE(QString(), VersitUtils::paramValue(param));     
    
    // Name and equals sign, but value has only spaces
    param = "TYPE=  \t ";
    QCOMPARE(QString(), VersitUtils::paramValue(param));    
    
    // Both name and value, spaces before the value
    param = "TYPE= \t  WORK";
    QCOMPARE(QString("WORK"), VersitUtils::paramValue(param));    
    
    // Both name and value, no spaces before the value
    param = "ENCODING=QUOTED-PRINTABLE";
    QCOMPARE(QString("QUOTED-PRINTABLE"), VersitUtils::paramValue(param));
}

void UT_VersitUtils::testAddParam()
{
    QMultiMap<QString,QString> params;
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
    QCOMPARE(QString("7BIT"), params.value("ENCODING"));

    // Non-empty substring, from the beginning
    originalStr = " TYPE=WORK ; VOICE; ENCODING=8BIT";
    params.clear();
    VersitUtils::addParam(params,originalStr,0,11);
    QCOMPARE(1, params.count());
    QCOMPARE(QString("WORK"), params.value("TYPE"));
    
    // Non-empty substring, from the middle
    params.clear();
    VersitUtils::addParam(params,originalStr,12,6);
    QCOMPARE(1, params.count());
    QCOMPARE(QString("VOICE"), params.value("TYPE"));
    
    // Non-empty substring, from the middle to the end
    params.clear();
    VersitUtils::addParam(params,originalStr,20);
    QCOMPARE(1, params.count());
    QCOMPARE(QString("8BIT"), params.value("ENCODING"));
}

void UT_VersitUtils::testUnfold()
{
    // No folding
    QCOMPARE(QByteArray("no folding\r\n"), QByteArray("no folding\r\n"));
    
    // Single space
    QByteArray folded("unfol\r\n ded\r\n");
    QByteArray unfolded("unfol ded\r\n");
    QCOMPARE(unfolded, VersitUtils::unfold(folded));

    // Multiple spaces
    folded = "unfol\r\n   ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));    

    // Single tab
    folded = "unfol\r\n\tded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));     

    // Multiple tabs
    folded = "unfol\r\n\t\tded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));    
    
    // One space and one tab
    folded = "unfol\r\n \tded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // One tab and one space
    folded = "unfol\r\n\t ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // Multiple spaces and tabs
    folded = "unfol\r\n\t \t  \t\t ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));    
    
    // Two foldings
    folded = "un\r\n\tfol\r\n ded\r\n";
    unfolded = "un fol ded\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));

    // Three foldings
    folded = "un\r\n fol\r\n\tde\r\n \t d\r\n";
    unfolded = "un fol de d\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));
    
    // Two actual lines containing foldings
    folded = "li\r\n\tne1\r\nl\r\n ine2\r\n";
    unfolded = "li ne1\r\nl ine2\r\n";
    QCOMPARE(unfolded, VersitUtils::unfold(folded));     
}

void UT_VersitUtils::testContainsSpecialChars()
{
    QByteArray basicText("sometext");
    
    // No special characters
    QVERIFY(!VersitUtils::containsSpecialChars(basicText));
    
    // Special characters
    QByteArray textWithSpecialChars = basicText + '\n'; 
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '\r'; 
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
            
    textWithSpecialChars = basicText + '!';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '"';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '#';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '$';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '=';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '@';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));    
    
    textWithSpecialChars = basicText + '[';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars)); 
    
    textWithSpecialChars = basicText + '\\';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + ']';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '^';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '`';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '{';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));    

    textWithSpecialChars = basicText + '|';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
    
    textWithSpecialChars = basicText + '}';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));    
    
    textWithSpecialChars = basicText + '~';
    QVERIFY(VersitUtils::containsSpecialChars(textWithSpecialChars));
}

void UT_VersitUtils::testEncodeQuotedPrintable()
{
    // Nothing to encode
    QByteArray nothingToEncode("nothing to encode");
    QCOMPARE(nothingToEncode, VersitUtils::encodeQuotedPrintable(nothingToEncode));
    
    // Special characters
    QByteArray specialChar("\n");
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=0A"));
    specialChar = "\r";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=0D"));
    specialChar = "!";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=21"));
    specialChar = "\"";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=22"));
    specialChar = "#";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=23"));
    specialChar = "$";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=24"));
    specialChar = "=";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=3D"));
    specialChar = "@";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=40"));
    specialChar = "[";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=5B"));
    specialChar = "\\";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=5C"));
    specialChar = "]";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=5D"));
    specialChar = "^";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=5E"));
    specialChar = "`";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=60"));
    specialChar = "{";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=7B"));
    specialChar = "|";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=7C"));
    specialChar = "}";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=7D"));
    specialChar = "~";
    QCOMPARE(VersitUtils::encodeQuotedPrintable(specialChar), QByteArray("=7E"));
}

void UT_VersitUtils::testDecodeQuotedPrintable()
{
    // Soft line breaks
    QByteArray encoded("This=\r\n is =\r\none line.");
    QByteArray decoded("This is one line.");
    QCOMPARE(VersitUtils::decodeQuotedPrintable(encoded), decoded);
    
    // Characters recommended to be encoded according to RFC 1521:
    encoded = "To be decoded: =0A=0D=21=22=23=24=3D=40=5B=5C=5D=5E=60=7B=7C=7D=7E";
    decoded = "To be decoded: \n\r!\"#$=@[\\]^`{|}~";
    QCOMPARE(VersitUtils::decodeQuotedPrintable(encoded), decoded);
    
    // Other random characters encoded.
    // Some implementation may encode these too, as it is allowed.
    encoded = "=45=6E=63=6F=64=65=64 =64=61=74=61";
    decoded = "Encoded data";
    QCOMPARE(VersitUtils::decodeQuotedPrintable(encoded), decoded);
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
    propertyName = "group1.X-property";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));     
    
    // Name contains more than one group
    property = "group1.group2.X-property:x";
    propertyName = "group1.group2.X-property";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));     

    // Property contains one parameter
    property = "TEL;WORK:123";
    propertyName = "TEL";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));    

    // Property contains several parameters
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    propertyName = "EMAIL";
    QCOMPARE(propertyName, VersitUtils::extractPropertyName(property));
    
    // Name contains an escaped semicolon
    property = "X-proper\\;ty:value";
    propertyName = "X-proper\\;ty";
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
    QMultiMap<QString,QString> params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString("TYPE")).count());
    QCOMPARE(QString("HOME"), params.values(QString("TYPE"))[0]);      
    
    // One parameter with an escaped semicolon
    property = "X-property;para\\;meter:value";
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(1, params.count());
    QCOMPARE(1, params.values(QString("TYPE")).count());
    QCOMPARE(QString("para\\;meter"), params.values(QString("TYPE"))[0]);
    
    // Two parameters of the same type
    property = "TEL;HOME;VOICE:123";
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString("TYPE")).count());
    QCOMPARE(QString("VOICE"), params.values(QString("TYPE"))[0]);
    QCOMPARE(QString("HOME"), params.values(QString("TYPE"))[1]);
    
    // Two parameters, several empty parameters (extra semicolons)
    property = "TEL;;;;HOME;;;;;VOICE;;;:123";
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(2, params.count());
    QCOMPARE(2, params.values(QString("TYPE")).count());
    QCOMPARE(QString("VOICE"), params.values(QString("TYPE"))[0]);
    QCOMPARE(QString("HOME"), params.values(QString("TYPE"))[1]);    
    
    // Two parameters with different types
    property = "EMAIL;INTERNET;ENCODING=QUOTED-PRINTABLE:user=40ovi.com";
    params.clear();
    params = VersitUtils::extractPropertyParams(property);
    QCOMPARE(2, params.count());
    QList<QString> typeParams = params.values(QString("TYPE"));
    QCOMPARE(1, typeParams.count());
    QCOMPARE(QString("INTERNET"), typeParams[0]);
    QList<QString> encodingParams = params.values(QString("ENCODING"));
    QCOMPARE(1, encodingParams.count());
    QCOMPARE(QString("QUOTED-PRINTABLE"), encodingParams[0]);
}
