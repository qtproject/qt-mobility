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
#include "transformaddress.h"
#include "transformurl.h"
#include "transformbirthday.h"
#include "transformonlineaccount.h"
#include "cntmodelextuids.h"
#include "transformorganisation.h"

#include <QtTest/QtTest>


void TestTransformContactData::initTestCase()
{
}

void TestTransformContactData::cleanupTestCase()
{
}

void TestTransformContactData::executeTransformEmail()
{
    validateTransformEmail(_L("dummyemail"), QString("dummyemail"));
    validateTransformEmail(_L(""), QString(""));
}

void TestTransformContactData::executeTransformName()
{
    validateTransformName(_L("dummyprefix"), QString("dummyprefix"),
                          _L("dummyfirst"), QString("dummyfirst"),
                          _L("dummymiddle"), QString("dummymiddle"),
                          _L("dummylast"), QString("dummylast"),
                          _L("dummysuffix"), QString("dummysuffix"));
    validateTransformName(_L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""));
}

void TestTransformContactData::executeTransformNickname()
{
    validateTransformNickname(_L("dummynickname"), QString("dummynickname"));
    validateTransformNickname(_L(""), QString(""));
}

void TestTransformContactData::executeTransformPhonenumber()
{
    validateTransformPhonenumber(_L("dummyphonenumber"), QString("dummyphonenumber"));
    validateTransformPhonenumber(_L(""), QString(""));
}

void TestTransformContactData::executeTransformAddress()
{
    validateTransformAddress(_L("dummycountry"), QString("dummycountry"),
                             _L("dummypostcode"), QString("dummypostcode"),
                             _L("dummystreet"), QString("dummystreet"),
                             _L("dummylocality"), QString("dummylocality"),
                             _L("dummyregion"), QString("dummyregion"),
                             _L("dummypostofficebox"), QString("dummypostofficebox"));
    validateTransformAddress(_L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""));
}
void TestTransformContactData::executeTransformUrl()
{
    validateTransformUrl(_L("dummyurl"), QString("dummyurl"));
    validateTransformUrl(_L(""), QString(""));
}

void TestTransformContactData::executeTransformBithday()
{
    TDateTime dateTime(2009, ESeptember, 28, 0, 0, 0, 0);
    TTime field(dateTime);
    QDate detail(2009, 9, 28);
    
    validateTransformBirthday(field, detail);
}

void TestTransformContactData::executeTransformOnlineAccount()
{
    validateTransformOnlineAccount(_L("dummysip"), QString("dummysip"));
    validateTransformOnlineAccount(_L(""), QString(""));
}

void TestTransformContactData::executeTransformOrganisation()
{
    validateTransformOrganisation(_L("dummycompany"), QString("dummycompany"),
                                  _L("dummydepartment"), QString("dummydepartment"),
                                  _L("dummyjobtitle"), QString("dummyjobtitle"));
    validateTransformOrganisation(_L(""), QString(""),
                                  _L(""), QString(""),
                                  _L(""), QString(""));
}

void TestTransformContactData::validateTransformEmail(TPtrC16 field, QString detail)
{
    TransformContactData* transformEmail = new TransformEmail();
    QVERIFY(transformEmail != 0);
    QVERIFY(transformEmail->supportsField(KUidContactFieldEMail.iUid));
    QVERIFY(transformEmail->supportsDetail(QContactEmailAddress::DefinitionName));
    
    validateContexts(transformEmail);
    
    QContactEmailAddress email;
    email.setEmailAddress(detail);
    QList<CContactItemField *> fields = transformEmail->transformDetailL(email);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldEMail));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldEMail);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformEmail->transformItemField(*newField, contact);
    const QContactEmailAddress* emailAddress(static_cast<const QContactEmailAddress*>(contactDetail));
    QCOMPARE(emailAddress->emailAddress(), detail);
        
    delete contactDetail;
    delete newField;
    delete transformEmail;
}

void TestTransformContactData::validateTransformName(TPtrC16 prefixField, QString prefixDetail,
                           TPtrC16 firstnameField, QString firstnameDetail,
                           TPtrC16 middlenameField, QString middlenameDetail,
                           TPtrC16 lastnameField, QString lastnameDetail,
                           TPtrC16 suffixField, QString suffixDetail)
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
    name.setPrefix(prefixDetail);
    name.setFirst(firstnameDetail);
    name.setLast(lastnameDetail);
    name.setMiddle(middlenameDetail);
    name.setSuffix(suffixDetail);
    QList<CContactItemField *> fields = transformName->transformDetailL(name);
    QVERIFY(fields.count() == 5);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPrefixName));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(prefixField), 0);
    QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldGivenName));
    QCOMPARE(fields.at(1)->TextStorage()->Text().CompareF(firstnameField), 0);
    QVERIFY(fields.at(2)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(2)->ContentType().ContainsFieldType(KUidContactFieldAdditionalName));
    QCOMPARE(fields.at(2)->TextStorage()->Text().CompareF(middlenameField), 0);
    QVERIFY(fields.at(3)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(3)->ContentType().ContainsFieldType(KUidContactFieldFamilyName));
    QCOMPARE(fields.at(3)->TextStorage()->Text().CompareF(lastnameField), 0);
    QVERIFY(fields.at(4)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(4)->ContentType().ContainsFieldType(KUidContactFieldSuffixName));
    QCOMPARE(fields.at(4)->TextStorage()->Text().CompareF(suffixField), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPrefixName);
    newField->TextStorage()->SetTextL(prefixField);
    QContact contact;
    QContactDetail* contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo1(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo1->prefix(), prefixDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    newField->TextStorage()->SetTextL(firstnameField);
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo2(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo2->first(), firstnameDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldAdditionalName);
    newField->TextStorage()->SetTextL(middlenameField);
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo3(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo3->middle(), middlenameDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    newField->TextStorage()->SetTextL(lastnameField);
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo4(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo4->last(), lastnameDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSuffixName);
    newField->TextStorage()->SetTextL(suffixField);
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo5(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo5->suffix(), suffixDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    delete transformName;
}

void TestTransformContactData::validateTransformNickname(TPtrC16 field, QString detail)
{
    TransformContactData* transformNickname = new TransformNickname();
    QVERIFY(transformNickname != 0);
    QVERIFY(transformNickname->supportsField(KUidContactFieldSecondName.iUid));
    QVERIFY(transformNickname->supportsDetail(QContactNickname::DefinitionName));
    
    validateContexts(transformNickname);
    
    QContactNickname nickname;
    nickname.setNickname(detail);
    QList<CContactItemField *> fields = transformNickname->transformDetailL(nickname);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSecondName));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSecondName);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformNickname->transformItemField(*newField, contact);
    const QContactNickname* nicknameInfo(static_cast<const QContactNickname*>(contactDetail));
    QCOMPARE(nicknameInfo->nickname(), detail);
        
    delete contactDetail;
    delete newField;
    delete transformNickname;
}

void TestTransformContactData::validateTransformPhonenumber(TPtrC16 field, QString detail)
{
    TransformContactData* transformPhoneNumber = new TransformPhoneNumber();
    QVERIFY(transformPhoneNumber != 0);
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldPhoneNumber.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldFax.iUid));
    QVERIFY(transformPhoneNumber->supportsDetail(QContactPhoneNumber::DefinitionName));
    
    validateContexts(transformPhoneNumber);
    
    QContactPhoneNumber phoneNumber1;
    phoneNumber1.setNumber(detail);
    QList<CContactItemField *> fields = transformPhoneNumber->transformDetailL(phoneNumber1);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);

    QContactPhoneNumber phoneNumber2;
    phoneNumber2.setNumber(detail);
    phoneNumber2.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    fields = transformPhoneNumber->transformDetailL(phoneNumber2);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapVOICE));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber3;
    phoneNumber3.setNumber(detail);
    phoneNumber3.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    fields = transformPhoneNumber->transformDetailL(phoneNumber3);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapCELL));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber4;
    phoneNumber4.setNumber(detail);
    phoneNumber4.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
    fields = transformPhoneNumber->transformDetailL(phoneNumber4);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldFax));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapFAX));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber5;
    phoneNumber5.setNumber(detail);
    phoneNumber5.setSubTypes(QContactPhoneNumber::SubTypePager);
    fields = transformPhoneNumber->transformDetailL(phoneNumber5);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapPAGER));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber6;
    phoneNumber6.setNumber(detail);
    phoneNumber6.setSubTypes(QContactPhoneNumber::SubTypeBulletinBoardSystem);
    fields = transformPhoneNumber->transformDetailL(phoneNumber6);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapBBS));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber7;
    phoneNumber7.setNumber(detail);
    phoneNumber7.setSubTypes(QContactPhoneNumber::SubTypeCar);
    fields = transformPhoneNumber->transformDetailL(phoneNumber7);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapCAR));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber8;
    phoneNumber8.setNumber(detail);
    phoneNumber8.setSubTypes(QContactPhoneNumber::SubTypeDtmfMenu);
    fields = transformPhoneNumber->transformDetailL(phoneNumber8);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldDTMF));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    QContactPhoneNumber phoneNumber9;
    phoneNumber9.setNumber(detail);
    phoneNumber9.setSubTypes(QContactPhoneNumber::SubTypeAssistant);
    fields = transformPhoneNumber->transformDetailL(phoneNumber9);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapAssistantTel);
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo->number(), detail);
    QVERIFY(phoneNumberInfo->subTypes().count() == 0);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
   
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    newField->AddFieldTypeL(KUidContactFieldVCardMapVOICE);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo1(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo1->number(), detail);
    QVERIFY(phoneNumberInfo1->subTypes().contains(QContactPhoneNumber::SubTypeLandline));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    newField->AddFieldTypeL(KUidContactFieldVCardMapCELL);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo2(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo2->number(), detail);
    QVERIFY(phoneNumberInfo2->subTypes().contains(QContactPhoneNumber::SubTypeMobile));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    newField->AddFieldTypeL(KUidContactFieldVCardMapPAGER);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo3(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo3->number(), detail);
    QVERIFY(phoneNumberInfo3->subTypes().contains(QContactPhoneNumber::SubTypePager));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFax);
    newField->TextStorage()->SetTextL(field);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo4(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo4->number(), detail);
    QVERIFY(phoneNumberInfo4->subTypes().contains(QContactPhoneNumber::SubTypeFacsimile));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    newField->AddFieldTypeL(KUidContactFieldVCardMapBBS);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo5(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo5->number(), detail);
    QVERIFY(phoneNumberInfo5->subTypes().contains(QContactPhoneNumber::SubTypeBulletinBoardSystem));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    newField->AddFieldTypeL(KUidContactFieldVCardMapCAR);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo6(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo6->number(), detail);
    QVERIFY(phoneNumberInfo6->subTypes().contains(QContactPhoneNumber::SubTypeCar));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldDTMF);
    newField->TextStorage()->SetTextL(field);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo7(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo7->number(), detail);
    QVERIFY(phoneNumberInfo7->subTypes().contains(QContactPhoneNumber::SubTypeDtmfMenu));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPhoneNumber);
    newField->TextStorage()->SetTextL(field);
    newField->SetMapping(KUidContactFieldVCardMapAssistantTel);
    contactDetail = transformPhoneNumber->transformItemField(*newField, contact);
    const QContactPhoneNumber* phoneNumberInfo8(static_cast<const QContactPhoneNumber*>(contactDetail));
    QCOMPARE(phoneNumberInfo8->number(), detail);
    QVERIFY(phoneNumberInfo8->subTypes().contains(QContactPhoneNumber::SubTypeAssistant));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    delete transformPhoneNumber; 
}

void TestTransformContactData::validateTransformAddress(TPtrC16 countryField, QString countryDetail,
                              TPtrC16 postcodeField, QString postcodeDetail,
                              TPtrC16 streetField, QString streetDetail,
                              TPtrC16 localityField, QString localityDetail,
                              TPtrC16 regionField, QString regionDetail,
                              TPtrC16 postOfficeBoxField, QString postOfficeBoxDetail)
{
    TransformContactData* transformAddress = new TransformAddress();
    QVERIFY(transformAddress != 0);
    QVERIFY(transformAddress->supportsField(KUidContactFieldCountry.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldPostcode.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldAddress.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldLocality.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldRegion.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldPostOffice.iUid));
    QVERIFY(transformAddress->supportsDetail(QContactAddress::DefinitionName));
    
    validateContexts(transformAddress);
    
    QContactAddress address;
    address.setCountry(countryDetail);
    address.setPostcode(postcodeDetail);
    address.setStreet(streetDetail);
    address.setLocality(localityDetail);
    address.setRegion(regionDetail);
    address.setPostOfficeBox(postOfficeBoxDetail);
    QList<CContactItemField *> fields = transformAddress->transformDetailL(address);
    QVERIFY(fields.count() == 6);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldCountry));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(countryField), 0);
    QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldPostcode));
    QCOMPARE(fields.at(1)->TextStorage()->Text().CompareF(postcodeField), 0);
    QVERIFY(fields.at(2)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(2)->ContentType().ContainsFieldType(KUidContactFieldAddress));
    QCOMPARE(fields.at(2)->TextStorage()->Text().CompareF(streetField), 0);
    QVERIFY(fields.at(3)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(3)->ContentType().ContainsFieldType(KUidContactFieldLocality));
    QCOMPARE(fields.at(3)->TextStorage()->Text().CompareF(localityField), 0);
    QVERIFY(fields.at(4)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(4)->ContentType().ContainsFieldType(KUidContactFieldRegion));
    QCOMPARE(fields.at(4)->TextStorage()->Text().CompareF(regionField), 0);
    QVERIFY(fields.at(5)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(5)->ContentType().ContainsFieldType(KUidContactFieldPostOffice));
    QCOMPARE(fields.at(5)->TextStorage()->Text().CompareF(postOfficeBoxField), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldCountry);
    newField->TextStorage()->SetTextL(countryField);
    QContact contact;
    QContactDetail* contactDetail = transformAddress->transformItemField(*newField, contact);
    const QContactAddress* addressInfo1(static_cast<const QContactAddress*>(contactDetail));
    QCOMPARE(addressInfo1->country(), countryDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPostcode);
    newField->TextStorage()->SetTextL(postcodeField);
    contactDetail = transformAddress->transformItemField(*newField, contact);
    const QContactAddress* addressInfo2(static_cast<const QContactAddress*>(contactDetail));
    QCOMPARE(addressInfo2->postcode(), postcodeDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldAddress);
    newField->TextStorage()->SetTextL(streetField);
    contactDetail = transformAddress->transformItemField(*newField, contact);
    const QContactAddress* addressInfo3(static_cast<const QContactAddress*>(contactDetail));
    QCOMPARE(addressInfo3->street(), streetDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldLocality);
    newField->TextStorage()->SetTextL(localityField);
    contactDetail = transformAddress->transformItemField(*newField, contact);
    const QContactAddress* addressInfo4(static_cast<const QContactAddress*>(contactDetail));
    QCOMPARE(addressInfo4->locality(), localityDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldRegion);
    newField->TextStorage()->SetTextL(regionField);
    contactDetail = transformAddress->transformItemField(*newField, contact);
    const QContactAddress* addressInfo5(static_cast<const QContactAddress*>(contactDetail));
    QCOMPARE(addressInfo5->region(), regionDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldPostOffice);
    newField->TextStorage()->SetTextL(postOfficeBoxField);
    contactDetail = transformAddress->transformItemField(*newField, contact);
    const QContactAddress* addressInfo6(static_cast<const QContactAddress*>(contactDetail));
    QCOMPARE(addressInfo6->postOfficeBox(), postOfficeBoxDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    delete transformAddress;
}

void TestTransformContactData::validateTransformUrl(TPtrC16 field, QString detail)
{
    TransformContactData* transformUrl = new TransformUrl();
    QVERIFY(transformUrl != 0);
    QVERIFY(transformUrl->supportsField(KUidContactFieldUrl.iUid));
    QVERIFY(transformUrl->supportsDetail(QContactUrl::DefinitionName));
    
    validateContexts(transformUrl);
    
    QContactUrl url;
    url.setUrl(detail);
    url.setSubType(QContactUrl::SubTypeHomePage);
    QList<CContactItemField *> fields = transformUrl->transformDetailL(url);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldUrl));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldUrl);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformUrl->transformItemField(*newField, contact);
    const QContactUrl* urlAddress(static_cast<const QContactUrl*>(contactDetail));
    QCOMPARE(urlAddress->url(), detail);
        
    delete contactDetail;
    delete newField;
    delete transformUrl;
}

void TestTransformContactData::validateTransformBirthday(TTime field, QDate detail)
{
    TransformContactData* transformBirthday = new TransformBirthday();
    QVERIFY(transformBirthday != 0);
    QVERIFY(transformBirthday->supportsField(KUidContactFieldBirthday.iUid));
    QVERIFY(transformBirthday->supportsDetail(QContactBirthday::DefinitionName));
    
    validateContexts(transformBirthday);
    
    QContactBirthday birthday;
    birthday.setDate(detail);
    QList<CContactItemField *> fields = transformBirthday->transformDetailL(birthday);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeDateTime);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldBirthday));
    QCOMPARE(fields.at(0)->DateTimeStorage()->Time().DateTime().Year(), detail.year());
    QCOMPARE(fields.at(0)->DateTimeStorage()->Time().DateTime().Month() + 1, detail.month());
    QCOMPARE(fields.at(0)->DateTimeStorage()->Time().DateTime().Day(), detail.day());
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeDateTime, KUidContactFieldBirthday);
    newField->DateTimeStorage()->SetTime(field);
    QContact contact;
    QContactDetail* contactDetail = transformBirthday->transformItemField(*newField, contact);
    const QContactBirthday* birthdayInfo(static_cast<const QContactBirthday*>(contactDetail));
    QCOMPARE(birthdayInfo->date().year(), field.DateTime().Year());
    QCOMPARE(birthdayInfo->date().month(), field.DateTime().Month() + 1);
    QCOMPARE(birthdayInfo->date().day(), field.DateTime().Day());
        
    delete contactDetail;
    delete newField;
    delete transformBirthday;  
}

void TestTransformContactData::validateTransformOnlineAccount(TPtrC16 sipField, QString sipDetail)
{
    TransformContactData* transformOnlineAccount = new TransformOnlineAccount();
    QVERIFY(transformOnlineAccount != 0);
    QVERIFY(transformOnlineAccount->supportsField(KUidContactFieldSIPID.iUid));
    QVERIFY(transformOnlineAccount->supportsField(KUidContactFieldIMPP.iUid));
    QVERIFY(transformOnlineAccount->supportsDetail(QContactOnlineAccount::DefinitionName));
     
    validateContexts(transformOnlineAccount);
     
    QContactOnlineAccount onlineAccount1;
    onlineAccount1.setAccountUri(sipDetail);
    onlineAccount1.setSubTypes(QContactOnlineAccount::SubTypeInternet);
    QList<CContactItemField *> fields = transformOnlineAccount->transformDetailL(onlineAccount1);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSIPID));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapSIPID);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapVOIP));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
    
    QContactOnlineAccount onlineAccount2;
    onlineAccount2.setAccountUri(sipDetail);
    onlineAccount2.setSubTypes(QContactOnlineAccount::SubTypeShareVideo);
    fields = transformOnlineAccount->transformDetailL(onlineAccount2);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSIPID));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapSIPID);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapSWIS));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
 
    QContactOnlineAccount onlineAccount3;
    onlineAccount3.setAccountUri(sipDetail);
    onlineAccount3.setSubTypes(QContactOnlineAccount::SubTypeSip);
    fields = transformOnlineAccount->transformDetailL(onlineAccount3);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSIPID));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapSIPID);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapSIPID));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
    
    QContactOnlineAccount onlineAccount4;
    onlineAccount4.setAccountUri(sipDetail);
    onlineAccount4.setSubTypes(QContactOnlineAccount::SubTypeXmpp);
    fields = transformOnlineAccount->transformDetailL(onlineAccount4);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldIMPP));
    QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapUnknown);
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSIPID);
    newField->TextStorage()->SetTextL(sipField);
    newField->AddFieldTypeL(KUidContactFieldVCardMapVOIP);
    QContact contact;
    QContactDetail* contactDetail = transformOnlineAccount->transformItemField(*newField, contact);
    const QContactOnlineAccount* onlineAccountDetail1(static_cast<const QContactOnlineAccount*>(contactDetail));
    QCOMPARE(onlineAccountDetail1->accountUri(), sipDetail);
    QVERIFY(onlineAccountDetail1->subTypes().contains(QContactOnlineAccount::SubTypeInternet));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSIPID);
    newField->TextStorage()->SetTextL(sipField);
    newField->AddFieldTypeL(KUidContactFieldVCardMapSWIS);
    contactDetail = transformOnlineAccount->transformItemField(*newField, contact);
    const QContactOnlineAccount* onlineAccountDetail2(static_cast<const QContactOnlineAccount*>(contactDetail));
    QCOMPARE(onlineAccountDetail2->accountUri(), sipDetail);
    QVERIFY(onlineAccountDetail2->subTypes().contains(QContactOnlineAccount::SubTypeShareVideo));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSIPID);
    newField->TextStorage()->SetTextL(sipField);
    newField->AddFieldTypeL(KUidContactFieldVCardMapSIPID);
    contactDetail = transformOnlineAccount->transformItemField(*newField, contact);
    const QContactOnlineAccount* onlineAccountDetail3(static_cast<const QContactOnlineAccount*>(contactDetail));
    QCOMPARE(onlineAccountDetail3->accountUri(), sipDetail);
    QVERIFY(onlineAccountDetail3->subTypes().contains(QContactOnlineAccount::SubTypeSip));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldIMPP);
    newField->TextStorage()->SetTextL(sipField);
    contactDetail = transformOnlineAccount->transformItemField(*newField, contact);
    const QContactOnlineAccount* onlineAccountDetail4(static_cast<const QContactOnlineAccount*>(contactDetail));
    QCOMPARE(onlineAccountDetail4->accountUri(), sipDetail);
    QVERIFY(onlineAccountDetail4->subTypes().contains(QContactOnlineAccount::SubTypeXmpp));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    delete transformOnlineAccount;
}

void TestTransformContactData::validateTransformOrganisation(TPtrC16 companyField, QString companyDetail,
                                TPtrC16 departmentField, QString departmentDetail,
                                TPtrC16 jobtitleField, QString jobtitleDetail)
{
    TransformContactData* transformOrganisation = new TransformOrganisation();
    QVERIFY(transformOrganisation != 0);
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldCompanyName.iUid));
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldDepartmentName.iUid));
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldJobTitle.iUid));
    QVERIFY(transformOrganisation->supportsDetail(QContactOrganization::DefinitionName));
    
    validateContexts(transformOrganisation);
    
    QContactOrganization organisation;
    organisation.setName(companyDetail);
    organisation.setDepartment(departmentDetail);
    organisation.setTitle(jobtitleDetail);
    QList<CContactItemField *> fields = transformOrganisation->transformDetailL(organisation);
    QVERIFY(fields.count() == 3);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldCompanyName));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(companyField), 0);
    QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldDepartmentName));
    QCOMPARE(fields.at(1)->TextStorage()->Text().CompareF(departmentField), 0);
    QVERIFY(fields.at(2)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(2)->ContentType().ContainsFieldType(KUidContactFieldJobTitle));
    QCOMPARE(fields.at(2)->TextStorage()->Text().CompareF(jobtitleField), 0);
    
    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldCompanyName);
    newField->TextStorage()->SetTextL(companyField);
    QContact contact;
    QContactDetail* contactDetail = transformOrganisation->transformItemField(*newField, contact);
    const QContactOrganization* organisationInfo1(static_cast<const QContactOrganization*>(contactDetail));
    QCOMPARE(organisationInfo1->name(), companyDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldDepartmentName);
    newField->TextStorage()->SetTextL(departmentField);
    contactDetail = transformOrganisation->transformItemField(*newField, contact);
    const QContactOrganization* organisationInfo2(static_cast<const QContactOrganization*>(contactDetail));
    QCOMPARE(organisationInfo2->department(), departmentDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldJobTitle);
    newField->TextStorage()->SetTextL(jobtitleField);
    contactDetail = transformOrganisation->transformItemField(*newField, contact);
    const QContactOrganization* organisationInfo3(static_cast<const QContactOrganization*>(contactDetail));
    QCOMPARE(organisationInfo3->title(), jobtitleDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;
    
    delete transformOrganisation;
}

void TestTransformContactData::validateContexts(TransformContactData* transformContactData) const
{
    QContactDetail detail1;
    transformContactData->setContexts(KUidContactFieldVCardMapHOME, detail1);
    QVERIFY(detail1.contexts().count() == 1);
    QVERIFY(detail1.contexts().at(0) == QContactDetail::ContextHome);
    
    CContactItemField* itemField = CContactItemField::NewL(KStorageTypeText);
    transformContactData->setContextsL(detail1, *itemField);
    QVERIFY(itemField->ContentType().ContainsFieldType(KUidContactFieldVCardMapHOME));
    delete itemField;
    itemField = 0;
    
    QContactDetail detail2;
    transformContactData->setContexts(KUidContactFieldVCardMapWORK, detail2);
    QVERIFY(detail2.contexts().count() == 1);
    QVERIFY(detail2.contexts().at(0) == QContactDetail::ContextWork);
    
    itemField = CContactItemField::NewL(KStorageTypeText);
    transformContactData->setContextsL(detail2, *itemField);
    QVERIFY(itemField->ContentType().ContainsFieldType(KUidContactFieldVCardMapWORK));
    delete itemField;
    itemField = 0;
}

QTEST_MAIN(TestTransformContactData);
