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

#include "ut_qversitproperty.h"
#include "qversitproperty.h"
#include "qversitproperty_p.h"
#include "qversitdocument.h"
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

void UT_QVersitProperty::init()
{
    mVersitProperty = new QVersitProperty();
    QVERIFY(mVersitProperty);
}

void UT_QVersitProperty::cleanup()
{
   delete mVersitProperty;
}

void UT_QVersitProperty::testGroup()
{
    // One group
    QStringList group(QString::fromAscii("GROUP_NAME"));
    mVersitProperty->setGroups(group);
    QCOMPARE(mVersitProperty->groups(), group);

    // Several groups
    QStringList groupList;
    groupList.append(QString::fromAscii("GROUP1"));
    groupList.append(QString::fromAscii("Group2"));
    groupList.append(QString::fromAscii("group3"));
    mVersitProperty->setGroups(groupList);
    QCOMPARE(mVersitProperty->groups(), groupList);
}

void UT_QVersitProperty::testName()
{
    // Name in upper case
    QString name(QString::fromAscii("TEL"));
    mVersitProperty->setName(name);
    QCOMPARE(mVersitProperty->name(), name);
    
    // Name in lower case, converted automatically to upper case
    mVersitProperty->setName(QString::fromAscii("tel"));
    QCOMPARE(mVersitProperty->name(), name);    
}

void UT_QVersitProperty::testParameters()
{
    QString typeParameterName(QString::fromAscii("TYPE"));
    
    QString name(QString::fromAscii("type"));
    QString value1(QString::fromAscii("home"));
    mVersitProperty->addParameter(name,value1);
    QMultiHash<QString,QString> parameters = mVersitProperty->parameters();
    QCOMPARE(parameters.count(), 1);
    QVERIFY(parameters.contains(typeParameterName,QString::fromAscii("HOME")));
    
    QString value2(QString::fromAscii("voice"));
    mVersitProperty->addParameter(name,value2);
    parameters = mVersitProperty->parameters();
    QCOMPARE(parameters.count(), 2);
    QVERIFY(parameters.contains(typeParameterName,QString::fromAscii("HOME")));
    QVERIFY(parameters.contains(typeParameterName,QString::fromAscii("VOICE")));
    
    mVersitProperty->removeParameter(name,value1);
    QCOMPARE(mVersitProperty->parameters().count(), 1);
    QVERIFY(parameters.contains(typeParameterName,QString::fromAscii("HOME")));
    
    mVersitProperty->removeParameter(name,value2);
    QCOMPARE(mVersitProperty->parameters().count(), 0);    
}

void UT_QVersitProperty::testValue()
{
    QString value(QString::fromAscii("050484747"));
    mVersitProperty->setValue(value);
    QCOMPARE(mVersitProperty->valueString(), value);
}

void UT_QVersitProperty::testEmbeddedDocument()
{
    QVersitDocument document;
    QVersitProperty property;
    property.setName(QString::fromAscii("X-tension"));
    document.addProperty(property);
    mVersitProperty->setValue(QVariant::fromValue(document));
    QList<QVersitProperty> embeddedDocumentProperties = 
        mVersitProperty->value().value<QVersitDocument>().properties();
    QCOMPARE(embeddedDocumentProperties.count(),1);
    QCOMPARE(embeddedDocumentProperties[0].name(),QString::fromAscii("X-TENSION"));
}

QTEST_MAIN(UT_QVersitProperty)

