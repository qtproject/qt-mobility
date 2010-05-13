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

#include "tst_qvcard30writer.h"
#ifdef QT_BUILD_INTERNAL
#include "qvcard30writer_p.h"
#endif
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>
#include <QVariant>

// This says "NOKIA" in Katakana encoded with UTF-8
const QString KATAKANA_NOKIA(QString::fromUtf8("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2"));

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QVersitProperty)
#ifdef QT_BUILD_INTERNAL
void tst_QVCard30Writer::init()
{
    mWriter = new QVCard30Writer;
    mWriter->setCodec(QTextCodec::codecForName("UTF-8"));
}

void tst_QVCard30Writer::cleanup()
{
    delete mWriter;
}

void tst_QVCard30Writer::testEncodeVersitProperty()
{
    QFETCH(QVersitProperty, property);
    QFETCH(QByteArray, expectedResult);
    QByteArray encodedProperty;
    QBuffer buffer(&encodedProperty);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);
    mWriter->encodeVersitProperty(property);
    QCOMPARE(encodedProperty, expectedResult);
}


void tst_QVCard30Writer::testEncodeVersitProperty_data()
{
    QTest::addColumn<QVersitProperty>("property");
    QTest::addColumn<QByteArray>("expectedResult");

    QVersitProperty property;
    QByteArray expectedResult;

    // No parameters
    expectedResult = "FN:John Citizen\r\n";
    property.setName(QString::fromAscii("FN"));
    property.setValue(QString::fromAscii("John Citizen"));
    QTest::newRow("No parameters") << property << expectedResult;

    // With parameter(s)
    expectedResult = "TEL;TYPE=HOME:123\r\n";
    property.setName(QString::fromAscii("TEL"));
    property.setValue(QString::fromAscii("123"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    QTest::newRow("With parameters, plain value") << property << expectedResult;

    // normal FN property is backslash escaped
    property.clear();
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String(";,:\\"));
    // semicolons, commas and backslashes are escaped (not colons, as per RFC2426)
    expectedResult = "FN:\\;\\,:\\\\\r\n";
    QTest::newRow("FN property") << property << expectedResult;

    // Structured N
    property.setName(QLatin1String("N"));
    property.setValue(QStringList()
                      << QLatin1String("La;st")    // needs to be backslash escaped
                      << QLatin1String("Fi,rst")
                      << QLatin1String("Mi:ddle")
                      << QLatin1String("Pr\\efix") // needs to be QP encoded
                      << QLatin1String("Suffix"));
    property.setValueType(QVersitProperty::CompoundType);
    expectedResult = "N:La\\;st;Fi\\,rst;Mi:ddle;Pr\\\\efix;Suffix\r\n";
    QTest::newRow("N property") << property << expectedResult;

    // Structured CATEGORIES
    property.setName(QLatin1String("CATEGORIES"));
    property.setValue(QStringList()
                      << QLatin1String("re;d")
                      << QLatin1String("gr,een")
                      << QLatin1String("bl:ue")
                      << QLatin1String("ye\\llow"));
    property.setValueType(QVersitProperty::ListType);
    expectedResult = "CATEGORIES:re\\;d,gr\\,een,bl:ue,ye\\\\llow\r\n";
    QTest::newRow("CATEGORIES property") << property << expectedResult;

    // Convert X-NICKNAME to NICKNAME
    expectedResult = "NICKNAME:Jack\r\n";
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("X-NICKNAME"));
    property.setValue(QString::fromAscii("Jack"));
    QTest::newRow("NICKNAME property") << property << expectedResult;

    // Convert X-IMPP to IMPP;
    expectedResult = "IMPP:msn:msn-address\r\n";
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("X-IMPP"));
    property.setValue(QString::fromAscii("msn:msn-address"));
    QTest::newRow("IMPP property") << property << expectedResult;

    // AGENT property
    expectedResult = "AGENT:BEGIN:VCARD\\nVERSION:3.0\\nFN:Secret Agent\\nEND:VCARD\\n\r\n";
    property.setName(QString::fromAscii("AGENT"));
    property.setValue(QString());
    QVersitDocument document;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString(QString::fromAscii("FN")));
    embeddedProperty.setValue(QString::fromAscii("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setValue(QVariant::fromValue(document));
    QTest::newRow("AGENT property") << property << expectedResult;

    // Value is base64 encoded.
    QByteArray value("value");
    expectedResult = "Springfield.HOUSE.PHOTO;ENCODING=b:" + value.toBase64() + "\r\n";
    QStringList groups(QString::fromAscii("Springfield"));
    groups.append(QString::fromAscii("HOUSE"));
    property.setGroups(groups);
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("PHOTO"));
    property.setValue(value);
    QTest::newRow("base64 encoded") << property << expectedResult;

    // Characters other than ASCII:
    expectedResult = "ORG:" + KATAKANA_NOKIA.toUtf8() + "\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    QTest::newRow("non-ASCII") << property << expectedResult;

    // No CHARSET and QUOTED-PRINTABLE parameters
    expectedResult = "EMAIL:john@" + KATAKANA_NOKIA.toUtf8() + ".com\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("EMAIL"));
    property.setValue(QString::fromAscii("john@%1.com").arg(KATAKANA_NOKIA));
    QTest::newRow("special chars") << property << expectedResult;
}

void tst_QVCard30Writer::testEncodeParameters()
{
    QByteArray encodedParameters;
    QBuffer buffer(&encodedParameters);
    mWriter->setDevice(&buffer);
    buffer.open(QIODevice::WriteOnly);

    QString typeParameterName(QString::fromAscii("TYPE"));
    QString encodingParameterName(QString::fromAscii("ENCODING"));

    // No parameters
    QMultiHash<QString,QString> parameters;
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(""));

    // One TYPE parameter
    parameters.insert(typeParameterName,QString::fromAscii("HOME"));
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";TYPE=HOME"));

    // Two TYPE parameters
    parameters.insert(typeParameterName,QString::fromAscii("VOICE"));
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";TYPE=VOICE,HOME"));

    // One ENCODING parameter
    parameters.clear();
    parameters.insert(encodingParameterName,QString::fromAscii("8BIT"));
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";ENCODING=8BIT"));

    // Two parameters
    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("VALUE"));
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";X-PARAM=VALUE;ENCODING=8BIT"));

    // Parameter with characters that require backslash escaping
    parameters.clear();
    parameters.insert(QString::fromAscii("X-P;ARAM"),QString::fromAscii("VA,LUE"));
    mWriter->writeCrlf(); // so it doesn't start folding
    buffer.close();
    encodedParameters.clear();
    buffer.open(QIODevice::WriteOnly);
    mWriter->encodeParameters(parameters);
    QCOMPARE(encodedParameters, QByteArray(";X-P\\;ARAM=VA\\,LUE"));
}

void tst_QVCard30Writer::testBackSlashEscape()
{
    // Empty string
    QString input;
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString());

    // Nothing to escape in the string
    input = QString::fromAscii("Nothing to escape");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("Nothing to escape"));

    // Line break in the beginning
    input = QString::fromAscii("\r\n input");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\n input"));

    // Line break in the end
    input = QString::fromAscii("input\r\n");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\n"));

    // Semicolon in the beginning
    input = QString::fromAscii(";input");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\;input"));

    // Semicolon in the end
    input = QString::fromAscii("input;");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\;"));

    // Comma in the beginning
    input = QString::fromAscii(",input");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\,input"));

    // Comma in the end
    input = QString::fromAscii("input,");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\,"));

    // Backslash in the beginning
    input = QString::fromAscii("\\input");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("\\\\input"));

    // Backslash in the end
    input = QString::fromAscii("input\\");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input,QString::fromAscii("input\\\\"));

    // Line break, semicolon, backslash and comma in the middle of the string
    input = QString::fromAscii("Escape these \r\n ; , \\ ");
    QVCard30Writer::backSlashEscape(input);
    QCOMPARE(input, QString::fromAscii("Escape these \\n \\; \\, \\\\ "));
}
#endif
QTEST_MAIN(tst_QVCard30Writer)

