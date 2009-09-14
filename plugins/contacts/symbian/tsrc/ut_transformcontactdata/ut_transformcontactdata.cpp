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

#include "ut_transformcontactdata.h"
#include "transformemail.h"
#include "transformname.h"
#include "transformnickname.h"
#include "transformphonenumber.h"

#include <QtTest/QtTest>


void TestTransformContactData::initTestCase()
{
}

void TestTransformContactData::cleanupTestCase()
{
}

void TestTransformContactData::executeTransformEmail()
{
    TransformContactData* transformEmail = new TransformEmail();
    QVERIFY(transformEmail != 0);
    QVERIFY(transformEmail->supportsField(KUidContactFieldEMail.iUid));
    QVERIFY(transformEmail->supportsDetail(QContactEmailAddress::DefinitionName));
    
    validateContexts(transformEmail);

    QContactEmailAddress email;
    email.setEmailAddress("dummyemail");
    QList<CContactItemField *> fields = transformEmail->transformDetailL(email);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldEMail));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(_L("dummyemail")), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldEMail);
    newField->TextStorage()->SetTextL(_L("dummyemail"));
    QContact contact;
    QContactDetail* contactDetail = transformEmail->transformItemField(*newField, contact);
    const QContactEmailAddress* emailAddress(static_cast<const QContactEmailAddress*>(contactDetail));
    QCOMPARE(emailAddress->emailAddress(), QString("dummyemail"));
        
    delete contactDetail;
    delete newField;
    delete transformEmail;
}

void TestTransformContactData::executeTransformName()
{
    TransformContactData* transformName = new TransformName();
    QVERIFY(transformName != 0);
    QVERIFY(transformName->supportsField(KUidContactFieldPrefixName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldGivenName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldAdditionalName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldFamilyName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldSuffixName.iUid));
    QVERIFY(transformName->supportsDetail(QContactName::DefinitionName));
    
    validateContexts(transformName);
    
    QContactName name;
    name.setPrefix("dummyprefix");
    name.setFirst("dummyfirst");
    name.setLast("dummylast");
    name.setMiddle("dummymiddle");
    name.setSuffix("dummysuffix");
    QList<CContactItemField *> fields = transformName->transformDetailL(name);
    QVERIFY(fields.count() == 5);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPrefixName));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(_L("dummyprefix")), 0);
    QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldGivenName));
    QCOMPARE(fields.at(1)->TextStorage()->Text().CompareF(_L("dummyfirst")), 0);
    QVERIFY(fields.at(2)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(2)->ContentType().ContainsFieldType(KUidContactFieldAdditionalName));
    QCOMPARE(fields.at(2)->TextStorage()->Text().CompareF(_L("dummymiddle")), 0);
    QVERIFY(fields.at(3)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(3)->ContentType().ContainsFieldType(KUidContactFieldFamilyName));
    QCOMPARE(fields.at(3)->TextStorage()->Text().CompareF(_L("dummylast")), 0);
    QVERIFY(fields.at(4)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(4)->ContentType().ContainsFieldType(KUidContactFieldSuffixName));
    QCOMPARE(fields.at(4)->TextStorage()->Text().CompareF(_L("dummysuffix")), 0);

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPrefixName);
    newField->TextStorage()->SetTextL(_L("dummyprefix"));
    QContact contact;
    QContactDetail* contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo1(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo1->prefix(), QString("dummyprefix"));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    newField->TextStorage()->SetTextL(_L("dummyfirst"));
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo2(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo2->first(), QString("dummyfirst"));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldAdditionalName);
    newField->TextStorage()->SetTextL(_L("dummymiddle"));
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo3(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo3->middle(), QString("dummymiddle"));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    newField->TextStorage()->SetTextL(_L("dummylast"));
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo4(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo4->last(), QString("dummylast"));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSuffixName);
    newField->TextStorage()->SetTextL(_L("dummysuffix"));
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo5(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo5->suffix(), QString("dummysuffix"));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    delete transformName;
}

void TestTransformContactData::executeTransformNickname()
{
    TransformContactData* transformNickname = new TransformNickname();
    QVERIFY(transformNickname != 0);
    QVERIFY(transformNickname->supportsField(KUidContactFieldSecondName.iUid));
    QVERIFY(transformNickname->supportsDetail(QContactNickname::DefinitionName));
    
    validateContexts(transformNickname);
    
    QContactNickname nickname;
    nickname.setNickname("dummynickname");
    QList<CContactItemField *> fields = transformNickname->transformDetailL(nickname);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSecondName));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(_L("dummynickname")), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSecondName);
    newField->TextStorage()->SetTextL(_L("dummynickname"));
    QContact contact;
    QContactDetail* contactDetail = transformNickname->transformItemField(*newField, contact);
    const QContactNickname* nicknameInfo(static_cast<const QContactNickname*>(contactDetail));
    QCOMPARE(nicknameInfo->nickname(), QString("dummynickname"));
        
    delete contactDetail;
    delete newField;
    delete transformNickname;
}

void TestTransformContactData::executeTransformPhonenumber()
{
    TransformContactData* transformPhoneNumber = new TransformPhoneNumber();
    QVERIFY(transformPhoneNumber != 0);
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldPhoneNumber.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldFax.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldVCardMapTEL.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldVCardMapCELL.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldVCardMapPAGER.iUid));
    QVERIFY(transformPhoneNumber->supportsDetail(QContactPhoneNumber::DefinitionName));
    
    validateContexts(transformPhoneNumber);
    
    QContactPhoneNumber phoneNumber;
    phoneNumber.setNumber("dummyphonenumber");
    QList<CContactItemField *> fields = transformPhoneNumber->transformDetailL(phoneNumber);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(_L("dummyphonenumber")), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(_L("dummyphonenumber"));
    QContact contact;
    QContactDetail* contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo->number(), QString("dummyphonenumber"));
        
    delete contactDetail;
    delete newField;
    delete transformPhoneNumber;
}

void TestTransformContactData::validateContexts(TransformContactData* transformContactData) const
{
    QContactDetail detail1;
    transformContactData->setContexts(KUidContactFieldVCardMapHOME, detail1);
    QVERIFY(detail1.contexts().count() == 1);
    QVERIFY(detail1.contexts().at(0) == QContactDetail::ContextHome);
    
    QContactDetail detail2;
    transformContactData->setContexts(KUidContactFieldVCardMapWORK, detail2);
    QVERIFY(detail2.contexts().count() == 1);
    QVERIFY(detail2.contexts().at(0) == QContactDetail::ContextWork);
}

QTEST_MAIN(TestTransformContactData);
