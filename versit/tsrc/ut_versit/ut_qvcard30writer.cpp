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
#include "qvcard30writer.h"
#include "qversitproperty.h"
#include <QtTest/QtTest>
#include <QByteArray>

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
    QByteArray expectedResult = "FN:Homer Simpson\r\n";
    QVersitProperty property;
    property.setName(QString::fromAscii("FN"));
    property.setValue(QByteArray("Homer Simpson"));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // With parameter(s)
    expectedResult = "TEL;TYPE=HOME:123\r\n";
    property.setName(QString::fromAscii("TEL"));
    property.setValue(QByteArray("123"));
    property.addParameter(QByteArray("TYPE"),QByteArray("HOME"));
    QByteArray encodedProperty = mWriter->encodeVersitProperty(property);
    QCOMPARE(QString::fromAscii(encodedProperty), QString::fromAscii(expectedResult));
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
    
    // AGENT property
    expectedResult =
        "AGENT:BEGIN\\:VCARD\\nVERSION\\:3.0\\nFN\\:Secret Agent\\nEND\\:VCARD\\n\r\n";
    property.setParameters(QMultiHash<QString,QString>());
    property.setName(QString::fromAscii("AGENT"));
    property.setValue(QByteArray());
    QVersitDocument document;
    QVersitProperty embeddedProperty;
    embeddedProperty.setName(QString("FN"));
    embeddedProperty.setValue(QByteArray("Secret Agent"));
    document.addProperty(embeddedProperty);
    property.setEmbeddedDocument(document);
    QCOMPARE(mWriter->encodeVersitProperty(property), expectedResult);
}

void UT_QVCard30Writer::testEncodeParameter()
{
    // Empty name and value
    QString name;
    QString value;
    QByteArray result;
    QCOMPARE(
        QString::fromAscii(mWriter->encodeParameter(name,value)),
        QString::fromAscii(result));

    // Normal parameter
    name = QString::fromAscii("TYPE");
    value = QString::fromAscii("HOME");
    result = "TYPE=HOME";
    QCOMPARE(
        QString::fromAscii(mWriter->encodeParameter(name,value)),
        QString::fromAscii(result));

    // Parameter with characters that require backslash escaping
    name = QString::fromAscii("X-A;B");
    value = QString::fromAscii("C:D");
    result = "X-A\\;B=C\\:D";
    QCOMPARE(
        QString::fromAscii(mWriter->encodeParameter(name,value)),
        QString::fromAscii(result));
}

