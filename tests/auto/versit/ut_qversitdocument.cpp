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

#include "ut_qversitdocument.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include <QString>
#include <QtTest/QtTest>


void UT_QVersitDocument::init()
{
    mVersitDocument = new QVersitDocument();
    QVERIFY(mVersitDocument);
}

void UT_QVersitDocument::cleanup()
{
    delete mVersitDocument;
}


void UT_QVersitDocument::testConstructor()
{
    QCOMPARE(QVersitDocument::VCard21, mVersitDocument->versitType());
}

void UT_QVersitDocument::testVersitType()
{
    QCOMPARE(QVersitDocument::VCard21, mVersitDocument->versitType());

    mVersitDocument->setVersitType(QVersitDocument::VCard30);
    QCOMPARE(QVersitDocument::VCard30, mVersitDocument->versitType());
}

void UT_QVersitDocument::testAddProperty()
{
    QCOMPARE(0, mVersitDocument->properties().count());
    QVersitProperty property;
    mVersitDocument->addProperty(property);
    QCOMPARE(1, mVersitDocument->properties().count());
}

void UT_QVersitDocument::testRemoveProperties()
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
