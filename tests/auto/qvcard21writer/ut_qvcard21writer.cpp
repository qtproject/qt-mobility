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

#include "ut_qvcard21writer.h"
#include "qvcard21writer_p.h"
#include "qversitproperty.h"
#include "qversitdocument.h"
#include <QtTest/QtTest>
#include <QByteArray>
#include <QVariant>

// This says "NOKIA" in Katakana
const QString KATAKANA_NOKIA(QString::fromUtf8("\xe3\x83\x8e\xe3\x82\xad\xe3\x82\xa2"));

QTM_USE_NAMESPACE

void UT_QVCard21Writer::init()
{
    mWriter = new QVCard21Writer;
}

void UT_QVCard21Writer::cleanup()
{
    delete mWriter;
}

void UT_QVCard21Writer::testEncodeVersitProperty()
{
    // No parameters
    QByteArray expectedResult = "FN:John Citizen\r\n";
    QVersitProperty property;
    property.setName(QString::fromAscii("FN"));
    property.setValue(QString::fromAscii("John Citizen"));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);

    // With parameter(s). No special characters in the value.
    // -> No need to Quoted-Printable encode the value.
    expectedResult = "TEL;HOME:123\r\n";

    property.setName(QString::fromAscii("TEL"));
    property.setValue(QString::fromAscii("123"));
    property.insertParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    QByteArray encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), QString::fromAscii(expectedResult));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // With parameter(s). Special characters in the value.
    // -> The value needs to be Quoted-Printable encoded.
    expectedResult = "EMAIL;HOME;ENCODING=QUOTED-PRINTABLE:john.citizen=40example.com\r\n";
    property.setName(QString::fromAscii("EMAIL"));
    property.setValue(QString::fromAscii("john.citizen@example.com"));
    QCOMPARE(QString::fromAscii(mWriter->encodeVersitProperty(property)),
             QString::fromAscii(expectedResult));
    
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
    QVersitDocument document;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString(QString::fromAscii("FN")));
    embeddedProperty.setValue(QString::fromAscii("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setValue(QVariant::fromValue(document));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);

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
    QCOMPARE(QString::fromAscii(mWriter->encodeVersitProperty(property).data()),
             QString::fromAscii(expectedResult.data()));

    // Characters other than ASCII:
    expectedResult = "ORG;CHARSET=UTF-8:" + KATAKANA_NOKIA.toUtf8() + "\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);

    // In Shift-JIS codec.
    QTextCodec* jisCodec = QTextCodec::codecForName("Shift-JIS");
    expectedResult = "ORG;CHARSET=Shift_JIS:" + jisCodec->fromUnicode(KATAKANA_NOKIA) + "\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("ORG"));
    property.setValue(KATAKANA_NOKIA);
    QByteArray result = mWriter->encodeVersitProperty(property, jisCodec);
    QCOMPARE(result, expectedResult);

    // CHARSET and QUOTED-PRINTABLE
    expectedResult = "EMAIL;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:john=40"
                     + KATAKANA_NOKIA.toUtf8() + ".com\r\n";
    property = QVersitProperty();
    property.setName(QLatin1String("EMAIL"));
    property.setValue(QString::fromAscii("john@%1.com").arg(KATAKANA_NOKIA));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
}

void UT_QVCard21Writer::testEncodeParameters()
{
    QString typeParameterName(QString::fromAscii("TYPE"));
    QString encodingParameterName(QString::fromAscii("ENCODING"));

    // No parameters
    QMultiHash<QString,QString> parameters;
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString());

    // One TYPE parameter
    parameters.insert(typeParameterName,QString::fromAscii("HOME"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";HOME"));

    // Two TYPE parameters
    parameters.insert(typeParameterName,QString::fromAscii("VOICE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";VOICE;HOME"));

    // One ENCODING parameter
    parameters.clear();
    parameters.insert(encodingParameterName,QString::fromAscii("8BIT"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";ENCODING=8BIT"));

    // Two parameters
    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("VALUE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";X-PARAM=VALUE;ENCODING=8BIT"));
}

void UT_QVCard21Writer::testEncodeGroupsAndName()
{
    QVersitProperty property;

    // No groups
    property.setName(QString::fromAscii("name"));
    QByteArray result("NAME");
    QCOMPARE(mWriter->encodeGroupsAndName(property),result);

    // One group
    property.setGroups(QStringList(QString::fromAscii("group")));
    result = "group.NAME";
    QCOMPARE(mWriter->encodeGroupsAndName(property),result);

    // Two groups
    QStringList groups(QString::fromAscii("group1"));
    groups.append(QString::fromAscii("group2"));
    property.setGroups(groups);
    result = "group1.group2.NAME";
    QCOMPARE(mWriter->encodeGroupsAndName(property),result);
}


QTEST_MAIN(UT_QVCard21Writer)

