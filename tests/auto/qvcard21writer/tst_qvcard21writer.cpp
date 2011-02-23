/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

//TESTED_COMPONENT=src/versit

#include "tst_qvcard21writer.h"
#ifdef QT_BUILD_INTERNAL
#include "qvcard21writer_p.h"
#endif
#include "qversitproperty.h"
#include "qversitdocument.h"
#include <QtTest/QtTest>
#include <QByteArray>
#include <QVariant>

// This says "NOKIA" in Katakana
const QString KATAKANA_NOKIA(QString::fromUtf8("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2"));

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QVersitProperty)

// Because the QFETCH macro balks on the comma in QMultiHash<QString,QString>
typedef QMultiHash<QString,QString> StringHash;
Q_DECLARE_METATYPE(StringHash)
#ifdef QT_BUILD_INTERNAL
void tst_QVCard21Writer::init()
{
    mWriter = new QVCard21Writer(QVersitDocument::VCard21Type);
    mWriter->setCodec(QTextCodec::codecForName("ISO_8859-1"));
}

void tst_QVCard21Writer::cleanup()
{
    delete mWriter;
}

void tst_QVCard21Writer::testEncodeVersitProperty()
{
    QFETCH(QVersitProperty, property);
    QFETCH(QByteArray, expectedResult);
    QFETCH(QByteArray, codec);
    QTextCodec* textCodec = QTextCodec::codecForName(codec);
    QByteArray encodedProperty;
    QBuffer buffer(&encodedProperty);
    mWriter->setDevice(&buffer);
    mWriter->setCodec(textCodec);
    buffer.open(QIODevice::WriteOnly);

    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);
}

void tst_QVCard21Writer::testEncodeVersitProperty_data()
{
    QTest::addColumn<QVersitProperty>("property");
    QTest::addColumn<QByteArray>("expectedResult");
    QTest::addColumn<QByteArray>("codec");

    QVersitProperty property;
    QByteArray expectedResult;
    QByteArray codec("ISO-8859_1");

    // normal case
    property.setName(QString::fromAscii("FN"));
    property.setValue(QString::fromAscii("John Citizen"));
    property.setValueType(QVersitProperty::PlainType);
    expectedResult = "FN:John Citizen\r\n";
    QTest::newRow("No parameters") << property << expectedResult << codec;

    // Structured N - escaping should happen for semicolons, not for commas
    property.setName(QLatin1String("N"));
    property.setValue(QStringList()
                      << QLatin1String("La;st")    // needs to be backslash escaped
                      << QLatin1String("Fi,rst")
                      << QLatin1String("Mi:ddle")
                      << QLatin1String("Pr\\efix") // needs to be QP encoded
                      << QLatin1String("Suffix"));
    property.setValueType(QVersitProperty::CompoundType);
    expectedResult = "N;ENCODING=QUOTED-PRINTABLE:La\\;st;Fi,rst;Mi:ddle;Pr=5Cefix;Suffix\r\n";
    QTest::newRow("N property") << property << expectedResult << codec;

    // Structured CATEGORIES - escaping should happen for commas, not semicolons
    property.setName(QLatin1String("CATEGORIES"));
    property.setValue(QStringList()
                      << QLatin1String("re;d")
                      << QLatin1String("gr,een")
                      << QLatin1String("bl:ue"));
    property.setValueType(QVersitProperty::ListType);
    expectedResult = "CATEGORIES:re;d,gr\\,een,bl:ue\r\n";
    QTest::newRow("CATEGORIES property") << property << expectedResult << codec;

    // With parameter(s). No special characters in the value.
    // -> No need to Quoted-Printable encode the value.
    expectedResult = "TEL;HOME:123\r\n";
    property.setName(QString::fromAscii("TEL"));
    property.setValue(QString::fromAscii("123"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    QTest::newRow("With parameters, plain value") << property << expectedResult << codec;

    expectedResult = "EMAIL;HOME;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n";
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QString::fromAscii("john.citizen@example.com"));
    QTest::newRow("With parameters, special value") << property << expectedResult << codec;

    // AGENT property with parameter
    expectedResult =
"AGENT;X-PARAMETER=VALUE:\r\n\
BEGIN:VCARD\r\n\
VERSION:2.1\r\n\
FN:Secret Agent\r\n\
END:VCARD\r\n\
\r\n";
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("AGENT"));
    property.setValue(QString());
    property.insertParameter(QString::fromAscii("X-PARAMETER"),QString::fromAscii("VALUE"));
    QVersitDocument document(QVersitDocument::VCard21Type);
    document.setComponentType(QLatin1String("VCARD"));
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString(QString::fromAscii("FN")));
    embeddedProperty.setValue(QString::fromAscii("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setValue(QVariant::fromValue(document));
    QTest::newRow("AGENT property") << property << expectedResult << codec;

    // Value is base64 encoded.
    // Check that the extra folding and the line break are added
    QByteArray value("value");
    expectedResult = "Springfield.HOUSE.PHOTO;ENCODING=BASE64:\r\n " + value.toBase64() + "\r\n\r\n";
    QStringList groups(QString::fromAscii("Springfield"));
    groups.append(QString::fromAscii("HOUSE"));
    property.setGroups(groups);
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(value);
    QTest::newRow("base64 encoded") << property << expectedResult << codec;

    // Characters other than ASCII:
    // Note: KATAKANA_NOKIA is defined as: QString::fromUtf8("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2")
    // The expected behaviour is to convert to UTF8, then encode with quoted-printable
    expectedResult = "ORG;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:=E3=83=8E=E3=82=AD=E3=82=A2\r\n";

    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    QTest::newRow("non-ASCII") << property << expectedResult << codec;

    // In Shift-JIS codec.
    QTextCodec* jisCodec = QTextCodec::codecForName("Shift-JIS");
    expectedResult = jisCodec->fromUnicode(
            QLatin1String("ORG:") + KATAKANA_NOKIA + QLatin1String("\r\n"));
    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    QTest::newRow("JIS codec") << property << expectedResult << QByteArray("Shift-JIS");
}

void tst_QVCard21Writer::testEncodeParameters()
{
    QFETCH(StringHash, parameters);
    QFETCH(QByteArray, expected);

    QByteArray encodedParameters;
    QBuffer buffer(&encodedParameters);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);

    // No parameters
    mWriter->encodeParameters(parameters);
    if (encodedParameters != expected) {
        qDebug() << "Encoded: " << encodedParameters;
        qDebug() << "Expected: " << expected;
        QVERIFY(false);
    }
}

void tst_QVCard21Writer::testEncodeParameters_data()
{
    QTest::addColumn< QMultiHash<QString, QString> >("parameters");
    QTest::addColumn<QByteArray>("expected");

    QMultiHash<QString,QString> parameters;

    QTest::newRow("No parameters") << parameters << QByteArray("");

    parameters.insert(QLatin1String("TYPE"), QString::fromAscii("HOME"));
    QTest::newRow("One TYPE parameter") << parameters << QByteArray(";HOME");

    // HOME should appear before VOICE because it is more "important" and some vCard
    // parsers may ignore everything after the first TYPE
    parameters.insert(QLatin1String("TYPE"), QString::fromAscii("VOICE"));
    QTest::newRow("Two TYPE parameters") << parameters << QByteArray(";HOME;VOICE");

    parameters.clear();
    parameters.insert(QLatin1String("ENCODING"), QString::fromAscii("8BIT"));
    QTest::newRow("One ENCODING parameter") << parameters << QByteArray(";ENCODING=8BIT");

    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("VALUE"));
    QTest::newRow("Two parameters") << parameters << QByteArray(";X-PARAM=VALUE;ENCODING=8BIT");

    parameters.clear();
    parameters.insert(QLatin1String("TYPE"), QLatin1String("VOICE"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("CELL"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("MODEM"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("CAR"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("VIDEO"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("FAX"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("BBS"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("PAGER"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("HOME"));
    parameters.insert(QLatin1String("TYPE"), QLatin1String("WORK"));
    // Ensure CELL and FAX are at the front because they are "more important" and some vCard
    // parsers may ignore everything after the first TYPE
    // Ensure WORK and HOME come next.
    // Besides these conditions, there are no other ordering constraints.  The data here is simply
    // what the writer produces (as dictated by its internal data structures).
    QTest::newRow("TYPE parameters order") << parameters
        << QByteArray(";CELL;FAX;WORK;HOME;MODEM;CAR;VIDEO;BBS;PAGER;VOICE");
}

void tst_QVCard21Writer::testEncodeGroupsAndName()
{
    QVersitProperty property;
    QByteArray result;
    QBuffer buffer(&result);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);

    // No groups

    property.setName(QString::fromAscii("name"));
    QByteArray expected("NAME");
    mWriter->encodeGroupsAndName(property);
    QCOMPARE(result, expected);

    // One group
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    result.clear();
    buffer.open(QIODevice::WriteOnly);
    property.setGroups(QStringList(QString::fromAscii("group")));
    expected = "group.NAME";
    mWriter->encodeGroupsAndName(property);
    QCOMPARE(result, expected);

    // Two groups
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    result.clear();
    buffer.open(QIODevice::WriteOnly);
    QStringList groups(QString::fromAscii("group1"));
    groups.append(QString::fromAscii("group2"));
    property.setGroups(groups);
    expected = "group1.group2.NAME";
    mWriter->encodeGroupsAndName(property);
    QCOMPARE(result, expected);
}


void tst_QVCard21Writer::testQuotedPrintableEncode()
{
    QByteArray encodedBytes;

    // Nothing to encode
    QString nothingToEncode(QLatin1String("nothing to encode"));
    QVERIFY(!mWriter->quotedPrintableEncode(nothingToEncode));

    // Special characters
    QString inputOutput(QLatin1String("\n"));
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=0A"));
    inputOutput = QLatin1String("\r");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=0D"));
    inputOutput = QLatin1String("!");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=21"));
    inputOutput = QLatin1String("\"");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=22"));
    inputOutput = QLatin1String("#");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=23"));
    inputOutput = QLatin1String("$");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=24"));
    inputOutput = QLatin1String("=");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=3D"));
    inputOutput = QLatin1String("@");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=40"));
    inputOutput = QLatin1String("[");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5B"));
    inputOutput = QLatin1String("\\");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5C"));
    inputOutput = QLatin1String("]");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5D"));
    inputOutput = QLatin1String("^");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=5E"));
    inputOutput = QLatin1String("`");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=60"));
    inputOutput = QLatin1String("{");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7B"));
    inputOutput = QLatin1String("|");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7C"));
    inputOutput = QLatin1String("}");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7D"));
    inputOutput = QLatin1String("~");
    QVERIFY(mWriter->quotedPrintableEncode(inputOutput));
    QCOMPARE(inputOutput, QLatin1String("=7E"));
}
#endif
QTEST_MAIN(tst_QVCard21Writer)

