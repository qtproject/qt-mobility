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

#include "ut_qvcard30writer.h"
#include "qvcard30writer_p.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>
#include <QVariant>

QTM_USE_NAMESPACE

void UT_QVCard30Writer::init()
{
    mWriter = new QVCard30Writer;
}

void UT_QVCard30Writer::cleanup()
{
    delete mWriter;
}

void UT_QVCard30Writer::testEncodeVersitProperty()
{
    // No parameters
    QString expectedResult = QString::fromAscii("FN:John Citizen\r\n");
    QVersitProperty property;
    property.setName(QString::fromAscii("FN"));
    property.setValue(QString::fromAscii("John Citizen"));
    QByteArray encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), expectedResult);
    
    // With parameter(s)
    expectedResult = QString::fromAscii("TEL;TYPE=HOME:123\r\n");
    property.setName(QString::fromAscii("TEL"));
    property.setValue(QString::fromAscii("123"));
    property.addParameter(QString::fromAscii("TYPE"),QString::fromAscii("HOME"));
    encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), expectedResult);
    
    // Convert X-NICKNAME to NICKNAME
    expectedResult = QString::fromAscii("NICKNAME:Jack\r\n");
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("X-NICKNAME"));
    property.setValue(QString::fromAscii("Jack"));
    encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), expectedResult);

    // Convert X-IMPP to IMPP
    expectedResult = QString::fromAscii("IMPP:msn:msn-address\r\n");
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("X-IMPP"));
    property.setValue(QString::fromAscii("msn:msn-address"));
    encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), expectedResult);

    // AGENT property
    expectedResult =
        QString::fromAscii(
            "AGENT:BEGIN:VCARD\\nVERSION:3.0\\nFN:Secret Agent\\nEND:VCARD\\n\r\n");
    property.setName(QString::fromAscii("AGENT"));
    property.setValue(QString());
    QVersitDocument document;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString(QString::fromAscii("FN")));
    embeddedProperty.setValue(QString::fromAscii("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setValue(QVariant::fromValue(document));
    encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), expectedResult);
}

void UT_QVCard30Writer::testEncodeParameters()
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
             QString::fromAscii(";TYPE=HOME"));

    // Two TYPE parameters
    parameters.insert(typeParameterName,QString::fromAscii("VOICE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";TYPE=VOICE,HOME"));

    // One ENCODING parameter
    parameters.clear();
    parameters.insert(encodingParameterName,QString::fromAscii("8BIT"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";ENCODING=8BIT"));

    // Two parameters
    parameters.insert(QString::fromAscii("X-PARAM"),QString::fromAscii("VALUE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";X-PARAM=VALUE;ENCODING=8BIT"));

    // Parameter with characters that require backslash escaping
    parameters.clear();
    parameters.insert(QString::fromAscii("X-P;ARAM"),QString::fromAscii("VA,LUE"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";X-P\\;ARAM=VA\\,LUE"));

    // ENCODING=BASE64 converted to ENCODING=B
    parameters.clear();
    parameters.insert(encodingParameterName,QString::fromAscii("BASE64"));
    QCOMPARE(QString::fromAscii(mWriter->encodeParameters(parameters)),
             QString::fromAscii(";ENCODING=B"));
}

QTEST_MAIN(UT_QVCard30Writer)

