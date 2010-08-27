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

#include "tst_qversitdocument.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QString>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

void tst_QVersitDocument::init()
{
    mVersitDocument = new QVersitDocument();
    QVERIFY(mVersitDocument);
}

void tst_QVersitDocument::cleanup()
{
    delete mVersitDocument;
}


void tst_QVersitDocument::testConstructor()
{
    QCOMPARE(QVersitDocument::InvalidType, mVersitDocument->type());
}

void tst_QVersitDocument::testType()
{
    mVersitDocument->setType(QVersitDocument::VCard21Type);
    QCOMPARE(QVersitDocument::VCard21Type, mVersitDocument->type());

    mVersitDocument->setType(QVersitDocument::VCard30Type);
    QCOMPARE(QVersitDocument::VCard30Type, mVersitDocument->type());
}

void tst_QVersitDocument::testAddProperty()
{
    QCOMPARE(0, mVersitDocument->properties().count());
    QVersitProperty property;
    mVersitDocument->addProperty(property);
    QCOMPARE(1, mVersitDocument->properties().count());
}

void tst_QVersitDocument::testRemoveProperty()
{
    // Remove an empty property.
    QCOMPARE(mVersitDocument->properties().count(), 0);
    QVersitProperty property;
    mVersitDocument->addProperty(property);
    mVersitDocument->removeProperty(property);
    QCOMPARE(mVersitDocument->properties().count(), 0);

    // A full property.
    property.setName(QLatin1String("TEL"));
    property.setGroups(QStringList(QLatin1String("HOME")));
    QMultiHash<QString, QString> params;
    params.insert(QLatin1String("TYPE"), QLatin1String("HOME"));
    property.setParameters(params);
    property.setValue(QLatin1String("123"));
    mVersitDocument->addProperty(property);
    QCOMPARE(mVersitDocument->properties().count(), 1);
    QVersitProperty property2;
    property2.setName(QLatin1String("TEL"));
    // Remove with a partial property fails.
    mVersitDocument->removeProperty(property2);
    QCOMPARE(mVersitDocument->properties().count(), 1);
    property2.setGroups(QStringList(QLatin1String("HOME")));
    property2.setParameters(params);
    property2.setValue(QLatin1String("123"));
    // Remove with a fully specified property succeeds.
    mVersitDocument->removeProperty(property2);
    QCOMPARE(mVersitDocument->properties().count(), 0);
}

void tst_QVersitDocument::testRemoveAllProperties()
{
    QString name(QString::fromAscii("FN"));

    // Try to remove from an empty document
    QCOMPARE(0, mVersitDocument->properties().count());
    mVersitDocument->removeProperties(name);
    QCOMPARE(0, mVersitDocument->properties().count());

    // Try to remove from a non-empty document, name does not match
    QVersitProperty property;
    property.setName(QString::fromAscii("TEL"));
    mVersitDocument->addProperty(property);
    QCOMPARE(1, mVersitDocument->properties().count());
    mVersitDocument->removeProperties(name);
    QCOMPARE(1, mVersitDocument->properties().count());

    // Remove from a non-empty document, name matches
    mVersitDocument->removeProperties(QString::fromAscii("TEL"));
    QCOMPARE(0, mVersitDocument->properties().count());

    // Remove from a document with two properties, first matches
    property.setName(name);
    mVersitDocument->addProperty(property);
    property.setName(QString::fromAscii("TEL"));
    mVersitDocument->addProperty(property);
    QCOMPARE(2, mVersitDocument->properties().count());
    mVersitDocument->removeProperties(name);
    QCOMPARE(1, mVersitDocument->properties().count());

    // Remove from a document with two properties, second matches
    property.setName(name);
    mVersitDocument->addProperty(property);
    QCOMPARE(2, mVersitDocument->properties().count());
    mVersitDocument->removeProperties(name);
    QCOMPARE(1, mVersitDocument->properties().count());
}

void tst_QVersitDocument::testEquality()
{
    QVersitDocument document1;
    QVersitDocument document2;
    QVERIFY(document1.isEmpty());
    QVERIFY(document1 == document2);
    QVERIFY(!(document1 != document2));
    QVersitProperty property;
    property.setName(QLatin1String("FN"));
    property.setValue(QLatin1String("John Citizen"));
    document2.addProperty(property);
    QVERIFY(!(document1 == document2));
    QVERIFY(document1 != document2);
    QVERIFY(!document2.isEmpty());

    document1.addProperty(property);
    QVERIFY(document1 == document2);
    QVERIFY(!(document1 != document2));

    document2.clear();
    QVERIFY(document2.isEmpty());

    document1.clear();
    QVERIFY(document1 == document2);
    QVERIFY(!(document1 != document2));

    document2.setType(QVersitDocument::VCard21Type);
    QVERIFY(!(document1 == document2));
    QVERIFY(document1 != document2);
    QVERIFY(!document2.isEmpty());
}

void tst_QVersitDocument::testHash()
{
    QVersitDocument document1;
    document1.setType(QVersitDocument::VCard30Type);
    QVersitProperty property1;
    property1.setName(QLatin1String("name"));
    property1.setValue(QLatin1String("value"));
    document1.addProperty(property1);
    QVersitDocument document2;
    document2.setType(QVersitDocument::VCard30Type);
    document2.addProperty(property1);
    QVersitDocument document3;
    document3.setType(QVersitDocument::VCard30Type);
    QVersitProperty property3;
    property3.setName(QLatin1String("name"));
    property3.setValue(QLatin1String("another value"));
    document3.addProperty(property3);
    QVersitDocument document4; // no properties
    document4.setType(QVersitDocument::VCard30Type);

    QVERIFY(qHash(document1) == qHash(document2));
    QVERIFY(qHash(document1) != qHash(document3));
    QVERIFY(qHash(document1) != qHash(document4));
}

QTEST_MAIN(tst_QVersitDocument)

