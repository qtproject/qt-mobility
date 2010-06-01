/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "ut_transformcontactdata.h"

#include "cnttransformemail.h"
#include "cnttransformname.h"
#include "cnttransformnickname.h"
#include "cnttransformphonenumber.h"
#include "cnttransformaddress.h"
#include "cnttransformurl.h"
#include "cnttransformbirthday.h"
#include "cnttransformonlineaccount.h"
#include "cnttransformorganisation.h"
#include "cnttransformavatar.h"
#include "cnttransformsynctarget.h"
#include "cnttransformgender.h"
#include "cnttransformanniversary.h"
#include "cnttransformgeolocation.h"
#include "cnttransformnote.h"
#include "cnttransformfamily.h"

#include "cntmodelextuids.h"

#include <QtTest/QtTest>
#include <qglobal.h>


void TestCntTransformContactData::initTestCase()
{
}

void TestCntTransformContactData::cleanupTestCase()
{
}

void TestCntTransformContactData::executeCntTransformEmail()
{
    TRAPD(err, validateCntTransformEmailL(_L("dummyemail"), QString("dummyemail"));
        validateCntTransformEmailL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformName()
{
    TRAPD(err, validateCntTransformNameL(_L("dummyprefix"), QString("dummyprefix"),
                          _L("dummyfirst"), QString("dummyfirst"),
                          _L("dummymiddle"), QString("dummymiddle"),
                          _L("dummylast"), QString("dummylast"),
                          _L("dummysuffix"), QString("dummysuffix"),
                          _L("dummylabel"), QString("dummylabel"));
        validateCntTransformNameL(_L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""),
                          _L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformNickname()
{
    TRAPD(err, validateCntTransformNicknameL(_L("dummynickname"), QString("dummynickname"));
        validateCntTransformNicknameL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformPhonenumber()
{
    TRAPD(err, validateCntTransformPhonenumberL(_L("dummyphonenumber"), QString("dummyphonenumber"));
        validateCntTransformPhonenumberL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformAddress()
{
    TRAPD(err, validateCntTransformAddressL(_L("dummycountry"), QString("dummycountry"),
                             _L("dummypostcode"), QString("dummypostcode"),
                             _L("dummystreet"), QString("dummystreet"),
                             _L("dummylocality"), QString("dummylocality"),
                             _L("dummyregion"), QString("dummyregion"),
                             _L("dummypostofficebox"), QString("dummypostofficebox"));
        validateCntTransformAddressL(_L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""),
                             _L(""), QString(""));
        );
    QVERIFY(err == 0);
}
void TestCntTransformContactData::executeCntTransformUrl()
{
    TRAPD(err, validateCntTransformUrlL(_L("dummyurl"), QString("dummyurl"));
        validateCntTransformUrlL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformBithday()
{
    TDateTime dateTime(2009, ESeptember, 27, 0, 0, 0, 0);
    TTime field(dateTime);
    QDate detail(2009, 9, 28);

    TRAPD(err, validateCntTransformBirthdayL(field, detail););
    QVERIFY(err == 0);
}

#ifdef SYMBIAN_BACKEND_USE_SQLITE

void TestCntTransformContactData::executeCntTransformOnlineAccount()
{
    TRAPD(err, validateCntTransformOnlineAccountL(_L("dummysip"), QString("dummysip"),
                                                  _L("nokia"), QString("nokia"),
                                                  _L("Available"), QString("Available"),
                                                  _L("Sleeping.."), QString("Sleeping.."));
    
        validateCntTransformOnlineAccountL(_L(""), QString(""),
                                           _L(""), QString(""),
                                           _L(""), QString(""),
                                           _L(""), QString(""));
        );
    QVERIFY(err == 0);
}

#endif // SYMBIAN_BACKEND_USE_SQLITE

void TestCntTransformContactData::executeCntTransformOrganisation()
{
    TRAPD(err, validateCntTransformOrganisationL(_L("dummycompany"), QString("dummycompany"),
                                  _L("dummydepartment"), QStringList("dummydepartment"),
                                  _L("dummyjobtitle"), QString("dummyjobtitle"),
                                  _L("dummyassistant"), QString("dummyassistant"));
        validateCntTransformOrganisationL(_L(""), QString(""),
                                  _L(""), QStringList(""),
                                  _L(""), QString(""),
                                  _L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformAvatar()
{
    TRAPD(err, validateCntTransformAvatarL(_L("dummyavatar"), QString("dummyavatar"));
        validateCntTransformAvatarL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformSyncTarget()
{
    TRAPD(err, validateCntTransformSyncTargetL(_L("dummysynctarget"), QString("dummysynctarget"));
        validateCntTransformSyncTargetL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformGender()
{
    TRAPD(err, validateCntTransformGenderL(_L("dummygender"), QString("dummygender"));
        validateCntTransformGenderL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformAnniversary()
{
    QDate dateDetail(2009, 9, 28);
    TRAPD(err, validateCntTransformAnniversaryL(_L("2009-09-28,dummyevent"), dateDetail, QString("dummyevent"));
        validateCntTransformAnniversaryL(_L("dummyevent"), QDate(), QString("dummyevent"));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformGeolocation()
{
    TRAPD(err, validateCntTransformGeolocationL(_L("123.45,765.88"), 123.45, 765.88);
        validateCntTransformGeolocationL(_L("123.45,"), 123.45, -1);
        validateCntTransformGeolocationL(_L(",765.88"), -1, 765.88);
        validateCntTransformGeolocationL(_L(""), -1, -1);
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformNote()
{
    TRAPD(err, validateCntTransformNoteL(_L("dummynote"), QString("dummynote"));
        validateCntTransformNoteL(_L(""), QString(""));
        );
    QVERIFY(err == 0);
}

void TestCntTransformContactData::executeCntTransformFamily()
{
    TRAPD(err, validateCntTransformFamilyL(_L("dummyspouse"), QString("dummyspouse"),
                            _L("dummychild"), QString("dummychild"));
        validateCntTransformFamilyL(_L(""), QString(""),
                            _L(""), QString(""));
        );
    QVERIFY(err == 0);
}


void TestCntTransformContactData::validateCntTransformEmailL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformEmail = new CntTransformEmail();
    QVERIFY(transformEmail != 0);
    QVERIFY(transformEmail->supportsField(KUidContactFieldEMail.iUid));
    QVERIFY(!(transformEmail->supportsField(0))); //Test for Wrong value
    QVERIFY(transformEmail->supportsDetail(QContactEmailAddress::DefinitionName));
    QVERIFY(!(transformEmail->supportsDetail("WrongValue")));
    //Test getIdForField
    validateGetIdForField(*transformEmail, QContactEmailAddress::FieldEmailAddress,
                           KUidContactFieldEMail.iUid);
    validateGetIdForField(*transformEmail, "WrongValue",0);
    QVERIFY( !(transformEmail->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldEMail;
    validateSupportedSortingFieldTypes(*transformEmail,QContactEmailAddress::FieldEmailAddress,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformEmail,"WrongValue",uidsToVerify);

    validateContextsL(transformEmail);

    QContactEmailAddress email;
    email.setEmailAddress(detail);
    QList<CContactItemField *> fields = transformEmail->transformDetailL(email);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldEMail));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

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

void TestCntTransformContactData::validateCntTransformNameL(TPtrC16 prefixField, QString prefixDetail,
                           TPtrC16 firstnameField, QString firstnameDetail,
                           TPtrC16 middlenameField, QString middlenameDetail,
                           TPtrC16 lastnameField, QString lastnameDetail,
                           TPtrC16 suffixField, QString suffixDetail,
                           TPtrC16 customLabelField, QString customLabelDetail)
{
    CntTransformContactData* transformName = new CntTransformName();
    QVERIFY(transformName != 0);
    QVERIFY(transformName->supportsField(KUidContactFieldPrefixName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldGivenName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldAdditionalName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldFamilyName.iUid));
    QVERIFY(transformName->supportsField(KUidContactFieldSuffixName.iUid));
    QVERIFY(!(transformName->supportsField(0))); //Test for Wrong value
    QVERIFY(transformName->supportsDetail(QContactName::DefinitionName));
    QVERIFY(!(transformName->supportsDetail("WrongValue")));
    validateGetIdForField(*transformName, QContactName::FieldPrefix,KUidContactFieldPrefixName.iUid);
    validateGetIdForField(*transformName, QContactName::FieldFirstName, KUidContactFieldGivenName.iUid);
    validateGetIdForField(*transformName, QContactName::FieldMiddleName,KUidContactFieldAdditionalName.iUid);
    validateGetIdForField(*transformName, QContactName::FieldLastName,KUidContactFieldFamilyName.iUid);
    validateGetIdForField(*transformName, QContactName::FieldSuffix,KUidContactFieldSuffixName.iUid);
    validateGetIdForField(*transformName, "WrongValue", 0);
    QVERIFY( !(transformName->supportsSubType("WrongValue")));
    //Test supportedSortingFieldTypes
    //supportedSortingFieldTypes - FieldPrefix
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldPrefixName;
    validateSupportedSortingFieldTypes(*transformName,QContactName::FieldPrefix,uidsToVerify);

    //supportedSortingFieldTypes - FieldFirst
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldGivenName;
    validateSupportedSortingFieldTypes(*transformName,QContactName::FieldFirstName,uidsToVerify);

    //supportedSortingFieldTypes - FieldMiddle
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldAdditionalName;
    validateSupportedSortingFieldTypes(*transformName,QContactName::FieldMiddleName,uidsToVerify);

    //supportedSortingFieldTypes - FieldLast
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldFamilyName;
    validateSupportedSortingFieldTypes(*transformName,QContactName::FieldLastName,uidsToVerify);

    //supportedSortingFieldTypes - FieldSuffix
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldSuffixName;
    validateSupportedSortingFieldTypes(*transformName,QContactName::FieldSuffix,uidsToVerify);

    //supportedSortingFieldTypes - Wrongfieldvalue
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformName,"WrongValue",uidsToVerify);



    validateContextsL(transformName);

    QContactName name;
    name.setPrefix(prefixDetail);
    name.setFirstName(firstnameDetail);
    name.setLastName(lastnameDetail);
    name.setMiddleName(middlenameDetail);
    name.setSuffix(suffixDetail);
    name.setCustomLabel(customLabelDetail);
    QList<CContactItemField *> fields = transformName->transformDetailL(name);
    // Assume zero or 6 details
    if(prefixDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 6);
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
        QVERIFY(fields.at(5)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(5)->ContentType().ContainsFieldType(KUidContactFieldTemplateLabel));
        QCOMPARE(fields.at(5)->TextStorage()->Text().CompareF(customLabelField), 0);
    }

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
    QCOMPARE(nameInfo2->firstName(), firstnameDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldAdditionalName);
    newField->TextStorage()->SetTextL(middlenameField);
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo3(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo3->middleName(), middlenameDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    newField->TextStorage()->SetTextL(lastnameField);
    contactDetail = transformName->transformItemField(*newField, contact);
    const QContactName* nameInfo4(static_cast<const QContactName*>(contactDetail));
    QCOMPARE(nameInfo4->lastName(), lastnameDetail);
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

void TestCntTransformContactData::validateCntTransformNicknameL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformNickname = new CntTransformNickname();
    QVERIFY(transformNickname != 0);
    QVERIFY(transformNickname->supportsField(KUidContactFieldSecondName.iUid));
    QVERIFY(!(transformNickname->supportsField(0))); //Test for Wrong value
    QVERIFY(transformNickname->supportsDetail(QContactNickname::DefinitionName));
    QVERIFY(!(transformNickname->supportsDetail("WrongValue")));
    validateGetIdForField(*transformNickname, QContactNickname::FieldNickname,KUidContactFieldSecondName.iUid);
    validateGetIdForField(*transformNickname, "WrongValue", 0);
    QVERIFY( !(transformNickname->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldSecondName;
    validateSupportedSortingFieldTypes(*transformNickname,QContactNickname::FieldNickname,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformNickname,"WrongValue",uidsToVerify);


    validateContextsL(transformNickname);

    QContactNickname nickname;
    nickname.setNickname(detail);
    QList<CContactItemField *> fields = transformNickname->transformDetailL(nickname);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSecondName));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

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

void TestCntTransformContactData::validateCntTransformPhonenumberL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformPhoneNumber = new CntTransformPhoneNumber();
    QVERIFY(transformPhoneNumber != 0);
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldPhoneNumber.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldFax.iUid));
    QVERIFY(transformPhoneNumber->supportsField(KUidContactFieldDTMF.iUid));
    QVERIFY(!(transformPhoneNumber->supportsField(0))); //some wrong value
    QVERIFY(transformPhoneNumber->supportsDetail(QContactPhoneNumber::DefinitionName));
    QVERIFY(!(transformPhoneNumber->supportsDetail("WrongValue")));
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::FieldNumber,0);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeLandline,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeMobile,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeFax,KUidContactFieldFax.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypePager,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeVoice,0);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeModem,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeVideo,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeCar,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeBulletinBoardSystem,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeMessagingCapable,0);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeAssistant,KUidContactFieldPhoneNumber.iUid);
    validateGetIdForField(*transformPhoneNumber, QContactPhoneNumber::SubTypeDtmfMenu,KUidContactFieldDTMF.iUid);
    validateGetIdForField(*transformPhoneNumber, "WrongValue", 0);
    QVERIFY(transformPhoneNumber->supportsSubType(QContactPhoneNumber::FieldSubTypes));
    QVERIFY( !(transformPhoneNumber->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    //supportedSortingFieldTypes - FieldPrefix
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldPhoneNumber;
    uidsToVerify << KUidContactFieldFax;
    uidsToVerify << KUidContactFieldDTMF;
    validateSupportedSortingFieldTypes(*transformPhoneNumber,QContactPhoneNumber::FieldNumber,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformPhoneNumber,"WrongValue",uidsToVerify);

    validateContextsL(transformPhoneNumber);

    QContactPhoneNumber phoneNumber1;
    phoneNumber1.setNumber(detail);
    QList<CContactItemField *> fields = transformPhoneNumber->transformDetailL(phoneNumber1);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber2;
    phoneNumber2.setNumber(detail);
    phoneNumber2.setSubTypes(QContactPhoneNumber::SubTypeLandline);
    fields = transformPhoneNumber->transformDetailL(phoneNumber2);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapVOICE));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber3;
    phoneNumber3.setNumber(detail);
    phoneNumber3.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    fields = transformPhoneNumber->transformDetailL(phoneNumber3);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapCELL));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber4;
    phoneNumber4.setNumber(detail);
    phoneNumber4.setSubTypes(QContactPhoneNumber::SubTypeFax);
    fields = transformPhoneNumber->transformDetailL(phoneNumber4);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldFax));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapFAX));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber5;
    phoneNumber5.setNumber(detail);
    phoneNumber5.setSubTypes(QContactPhoneNumber::SubTypePager);
    fields = transformPhoneNumber->transformDetailL(phoneNumber5);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapPAGER));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber6;
    phoneNumber6.setNumber(detail);
    phoneNumber6.setSubTypes(QContactPhoneNumber::SubTypeBulletinBoardSystem);
    fields = transformPhoneNumber->transformDetailL(phoneNumber6);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapBBS));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber7;
    phoneNumber7.setNumber(detail);
    phoneNumber7.setSubTypes(QContactPhoneNumber::SubTypeCar);
    fields = transformPhoneNumber->transformDetailL(phoneNumber7);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapTEL);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapCAR));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber8;
    phoneNumber8.setNumber(detail);
    phoneNumber8.setSubTypes(QContactPhoneNumber::SubTypeDtmfMenu);
    fields = transformPhoneNumber->transformDetailL(phoneNumber8);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldDTMF));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactPhoneNumber phoneNumber9;
    phoneNumber9.setNumber(detail);
    phoneNumber9.setSubTypes(QContactPhoneNumber::SubTypeAssistant);
    fields = transformPhoneNumber->transformDetailL(phoneNumber9);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldPhoneNumber));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapAssistantTel);
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

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
    QVERIFY(phoneNumberInfo4->subTypes().contains(QContactPhoneNumber::SubTypeFax));
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

void TestCntTransformContactData::validateCntTransformAddressL(TPtrC16 countryField, QString countryDetail,
                              TPtrC16 postcodeField, QString postcodeDetail,
                              TPtrC16 streetField, QString streetDetail,
                              TPtrC16 localityField, QString localityDetail,
                              TPtrC16 regionField, QString regionDetail,
                              TPtrC16 postOfficeBoxField, QString postOfficeBoxDetail)
{
    CntTransformContactData* transformAddress = new CntTransformAddress();
    QVERIFY(transformAddress != 0);
    QVERIFY(transformAddress->supportsField(KUidContactFieldCountry.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldPostcode.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldAddress.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldLocality.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldRegion.iUid));
    QVERIFY(transformAddress->supportsField(KUidContactFieldPostOffice.iUid));
    QVERIFY(!(transformAddress->supportsField(0))); //Test for Wrong value
    QVERIFY(transformAddress->supportsDetail(QContactAddress::DefinitionName));
    QVERIFY(!(transformAddress->supportsDetail("WrongValue")));

    validateGetIdForField(*transformAddress, QContactAddress::FieldStreet,KUidContactFieldAddress.iUid);
    validateGetIdForField(*transformAddress, QContactAddress::FieldLocality,KUidContactFieldLocality.iUid);
    validateGetIdForField(*transformAddress, QContactAddress::FieldRegion,KUidContactFieldRegion.iUid);
    validateGetIdForField(*transformAddress, QContactAddress::FieldPostcode,KUidContactFieldPostcode.iUid);
    validateGetIdForField(*transformAddress, QContactAddress::FieldCountry,KUidContactFieldCountry.iUid);
    validateGetIdForField(*transformAddress, QContactAddress::FieldPostOfficeBox,KUidContactFieldPostOffice.iUid);
    validateGetIdForField(*transformAddress, QContactAddress::SubTypeParcel,0);
    validateGetIdForField(*transformAddress, QContactAddress::SubTypePostal,0);
    validateGetIdForField(*transformAddress, QContactAddress::SubTypeDomestic,0);
    validateGetIdForField(*transformAddress, QContactAddress::SubTypeInternational,0);
    validateGetIdForField(*transformAddress, "WrongValue", 0);
    QVERIFY(transformAddress->supportsSubType(QContactAddress::FieldSubTypes));
    QVERIFY( !(transformAddress->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    //supportedSortingFieldTypes - FieldStreet
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldAddress;
    validateSupportedSortingFieldTypes(*transformAddress,QContactAddress::FieldStreet,uidsToVerify);

    //supportedSortingFieldTypes - FieldLocality
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldLocality;
    validateSupportedSortingFieldTypes(*transformAddress,QContactAddress::FieldLocality,uidsToVerify);


    //supportedSortingFieldTypes - FieldRegion
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldRegion;
    validateSupportedSortingFieldTypes(*transformAddress,QContactAddress::FieldRegion,uidsToVerify);


    //supportedSortingFieldTypes - FieldPostcode
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldPostcode;
    validateSupportedSortingFieldTypes(*transformAddress,QContactAddress::FieldPostcode,uidsToVerify);


    //supportedSortingFieldTypes - FieldCountry
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldCountry;
    validateSupportedSortingFieldTypes(*transformAddress,QContactAddress::FieldCountry,uidsToVerify);


    //supportedSortingFieldTypes - FieldPostOfficeBox
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldPostOffice;
    validateSupportedSortingFieldTypes(*transformAddress,QContactAddress::FieldPostOfficeBox,uidsToVerify);

    //supportedSortingFieldTypes - Wrongvalue
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformAddress,"WrongValue",uidsToVerify);

    validateContextsL(transformAddress);

    QContactAddress address;
    address.setCountry(countryDetail);
    address.setPostcode(postcodeDetail);
    address.setStreet(streetDetail);
    address.setLocality(localityDetail);
    address.setRegion(regionDetail);
    address.setPostOfficeBox(postOfficeBoxDetail);
    QList<CContactItemField *> fields = transformAddress->transformDetailL(address);
    // Assume zero or 6 details
    if(countryDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
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
    }

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

void TestCntTransformContactData::validateCntTransformUrlL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformUrl = new CntTransformUrl();
    QVERIFY(transformUrl != 0);
    QVERIFY(transformUrl->supportsField(KUidContactFieldUrl.iUid));
    QVERIFY(transformUrl->supportsDetail(QContactUrl::DefinitionName));
    QVERIFY(!(transformUrl->supportsDetail("WrongValue")));
    validateGetIdForField(*transformUrl, QContactUrl::FieldUrl,0);
    validateGetIdForField(*transformUrl, QContactUrl::SubTypeHomePage,0);
    validateGetIdForField(*transformUrl, QContactUrl::SubTypeFavourite,0);
    validateGetIdForField(*transformUrl, "WrongValue", 0);
    QVERIFY(transformUrl->supportsSubType(QContactUrl::FieldSubType));
    QVERIFY( !(transformUrl->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
   //supportedSortingFieldTypes - FieldUrl
   QList<TUid> uidsToVerify;
   uidsToVerify << KUidContactFieldUrl;
   validateSupportedSortingFieldTypes(*transformUrl,QContactUrl::FieldUrl,uidsToVerify);
   uidsToVerify.clear();
   validateSupportedSortingFieldTypes(*transformUrl,"WrongValue",uidsToVerify);

    validateContextsL(transformUrl);

    QContactUrl url;
    url.setUrl(detail);
    url.setSubType(QContactUrl::SubTypeHomePage);
    QList<CContactItemField *> fields = transformUrl->transformDetailL(url);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldUrl));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

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

void TestCntTransformContactData::validateCntTransformBirthdayL(TTime field, QDate detail)
{
    CntTransformContactData* transformBirthday = new CntTransformBirthday();
    QVERIFY(transformBirthday != 0);
    QVERIFY(transformBirthday->supportsField(KUidContactFieldBirthday.iUid));
    QVERIFY(!(transformBirthday->supportsField(0))); //Test for Wrong value
    QVERIFY(transformBirthday->supportsDetail(QContactBirthday::DefinitionName));
    QVERIFY(!(transformBirthday->supportsDetail("WrongValue")));

    validateGetIdForField(*transformBirthday, QContactBirthday::FieldBirthday,KUidContactFieldBirthday.iUid);
    validateGetIdForField(*transformBirthday, "WrongValue", 0);
    QVERIFY( !(transformBirthday->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    //supportedSortingFieldTypes - FieldBirthday
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldBirthday;
    validateSupportedSortingFieldTypes(*transformBirthday,QContactBirthday::FieldBirthday,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformBirthday,"WrongValue",uidsToVerify);

    validateContextsL(transformBirthday);

    QContactBirthday birthday;
    birthday.setDate(detail);
    QList<CContactItemField *> fields = transformBirthday->transformDetailL(birthday);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeDateTime);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldBirthday));
    QCOMPARE(fields.at(0)->DateTimeStorage()->Time().DateTime().Year(), detail.year());
    QCOMPARE(fields.at(0)->DateTimeStorage()->Time().DateTime().Month() + 1, detail.month());
    QCOMPARE(fields.at(0)->DateTimeStorage()->Time().DateTime().Day() + 1, detail.day());

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeDateTime, KUidContactFieldBirthday);
    newField->DateTimeStorage()->SetTime(field);
    QContact contact;
    QContactDetail* contactDetail = transformBirthday->transformItemField(*newField, contact);
    const QContactBirthday* birthdayInfo(static_cast<const QContactBirthday*>(contactDetail));
    QCOMPARE(birthdayInfo->date().year(), field.DateTime().Year());
    QCOMPARE(birthdayInfo->date().month(), field.DateTime().Month() + 1);
    QCOMPARE(birthdayInfo->date().day(), field.DateTime().Day() + 1);

    delete contactDetail;
    delete newField;
    delete transformBirthday;
}

#ifdef SYMBIAN_BACKEND_USE_SQLITE

void TestCntTransformContactData::validateCntTransformOnlineAccountL(
    TPtrC16 sipField, QString sipDetail,
    TPtrC16 providerField, QString providerDetail,
    TPtrC16 /*presenceField*/, QString presenceDetail,
    TPtrC16 statusField, QString statusDetail)
{
	CntTransformContactData* transformOnlineAccount = new CntTransformOnlineAccount();
    QVERIFY(transformOnlineAccount != 0);
    QVERIFY(transformOnlineAccount->supportsField(KUidContactFieldSIPID.iUid));
    QVERIFY(transformOnlineAccount->supportsField(KUidContactFieldIMPP.iUid));
    QVERIFY(transformOnlineAccount->supportsField(KUidContactFieldServiceProvider.iUid));
    QVERIFY(!(transformOnlineAccount->supportsField(0))); //Test for Wrong value
    QVERIFY(transformOnlineAccount->supportsDetail(QContactOnlineAccount::DefinitionName));
    QVERIFY(!(transformOnlineAccount->supportsDetail("WrongValue")));

    validateGetIdForField(*transformOnlineAccount, QContactOnlineAccount::FieldAccountUri,0);
    validateGetIdForField(*transformOnlineAccount, QContactOnlineAccount::SubTypeSip,KUidContactFieldSIPID.iUid);
    validateGetIdForField(*transformOnlineAccount, QContactOnlineAccount::SubTypeImpp, KUidContactFieldIMPP.iUid);
    validateGetIdForField(*transformOnlineAccount, QContactOnlineAccount::SubTypeSipVoip,KUidContactFieldVCardMapVOIP.iUid);
    validateGetIdForField(*transformOnlineAccount, QContactOnlineAccount::SubTypeVideoShare,KUidContactFieldVCardMapSWIS.iUid);
    validateGetIdForField(*transformOnlineAccount, "WrongValue", 0);
    QVERIFY( !(transformOnlineAccount->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    //supportedSortingFieldTypes - FieldAccountUri
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldIMPP;
    uidsToVerify << KUidContactFieldSIPID;
    validateSupportedSortingFieldTypes(*transformOnlineAccount,QContactOnlineAccount::FieldAccountUri,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformOnlineAccount,"WrongValue",uidsToVerify);

    validateContextsL(transformOnlineAccount);

    QContactOnlineAccount onlineAccount1;
    onlineAccount1.setAccountUri(sipDetail);
    onlineAccount1.setServiceProvider(providerDetail);
    
    onlineAccount1.setSubTypes(QContactOnlineAccount::SubTypeSipVoip);
    QList<CContactItemField *> fields = transformOnlineAccount->transformDetailL(onlineAccount1);
    if(sipDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 2);
        
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSIPID));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapSIPID);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapVOIP));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);

        QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldServiceProvider));
        QCOMPARE(fields.at(1)->TextStorage()->Text(), providerField );
    }

    QContactOnlineAccount onlineAccount2;
    onlineAccount2.setAccountUri(sipDetail);
    onlineAccount2.setSubTypes(QContactOnlineAccount::SubTypeVideoShare);
    fields = transformOnlineAccount->transformDetailL(onlineAccount2);
    if(sipDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QCOMPARE(fields.count(), 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSIPID));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapSIPID);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapSWIS));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
    }

    QContactOnlineAccount onlineAccount3;
    onlineAccount3.setAccountUri(sipDetail);
    onlineAccount3.setSubTypes(QContactOnlineAccount::SubTypeSip);
    fields = transformOnlineAccount->transformDetailL(onlineAccount3);
    if(sipDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QCOMPARE(fields.count(), 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSIPID));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapSIPID);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVCardMapSIPID));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
    }

    QContactOnlineAccount onlineAccount4;
    onlineAccount4.setAccountUri(sipDetail);
    onlineAccount4.setSubTypes(QContactOnlineAccount::SubTypeImpp);
    fields = transformOnlineAccount->transformDetailL(onlineAccount4);
    if(sipDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QCOMPARE(fields.count(), 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldIMPP));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapUnknown);
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(sipField), 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSIPID);
    newField->TextStorage()->SetTextL(sipField);
    newField->AddFieldTypeL(KUidContactFieldVCardMapVOIP);
    QContact contact;
    QContactDetail* contactDetail = transformOnlineAccount->transformItemField(*newField, contact);
    const QContactOnlineAccount* onlineAccountDetail1(static_cast<const QContactOnlineAccount*>(contactDetail));
    QCOMPARE(onlineAccountDetail1->accountUri(), sipDetail);
    QVERIFY(onlineAccountDetail1->subTypes().contains(QContactOnlineAccount::SubTypeSipVoip));
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
    QVERIFY(onlineAccountDetail2->subTypes().contains(QContactOnlineAccount::SubTypeVideoShare));
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
    QVERIFY(onlineAccountDetail4->subTypes().contains(QContactOnlineAccount::SubTypeImpp));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldServiceProvider);
    newField->TextStorage()->SetTextL(providerField);
    contactDetail = transformOnlineAccount->transformItemField(*newField, contact);
    const QContactOnlineAccount* onlineAccountDetail5(static_cast<const QContactOnlineAccount*>(contactDetail));
    QCOMPARE(onlineAccountDetail5->serviceProvider(), providerDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    delete transformOnlineAccount;
}

#endif // SYMBIAN_BACKEND_USE_SQLITE

void TestCntTransformContactData::validateCntTransformOrganisationL(TPtrC16 companyField, QString companyDetail,
                                TPtrC16 departmentField, QStringList departmentDetail,
                                TPtrC16 jobtitleField, QString jobtitleDetail,
                                TPtrC16 assistantField, QString assistantDetail)
{
    CntTransformContactData* transformOrganisation = new CntTransformOrganisation();
    QVERIFY(transformOrganisation != 0);
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldCompanyName.iUid));
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldDepartmentName.iUid));
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldJobTitle.iUid));
    QVERIFY(transformOrganisation->supportsField(KUidContactFieldAssistant.iUid));
    QVERIFY(!(transformOrganisation->supportsField(0))); //Test for Wrong value
    QVERIFY(transformOrganisation->supportsDetail(QContactOrganization::DefinitionName));
    QVERIFY(!(transformOrganisation->supportsDetail("WrongValue")));

    validateGetIdForField(*transformOrganisation, QContactOrganization::FieldName,KUidContactFieldCompanyName.iUid);
    validateGetIdForField(*transformOrganisation, QContactOrganization::FieldLogoUrl,0);
    validateGetIdForField(*transformOrganisation, QContactOrganization::FieldDepartment,KUidContactFieldDepartmentName.iUid);
    validateGetIdForField(*transformOrganisation, QContactOrganization::FieldLocation,0);
    validateGetIdForField(*transformOrganisation, QContactOrganization::FieldTitle,KUidContactFieldJobTitle.iUid);
    validateGetIdForField(*transformOrganisation, QContactOrganization::FieldAssistantName,KUidContactFieldAssistant.iUid);
    validateGetIdForField(*transformOrganisation, "WrongValue", 0);
    QVERIFY( !(transformOrganisation->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    //supportedSortingFieldTypes - FieldPrefix
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldCompanyName;
    validateSupportedSortingFieldTypes(*transformOrganisation,QContactOrganization::FieldName,uidsToVerify);

    //supportedSortingFieldTypes - FieldDepartment
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldDepartmentName;
    validateSupportedSortingFieldTypes(*transformOrganisation,QContactOrganization::FieldDepartment,uidsToVerify);

    //supportedSortingFieldTypes - FieldTitle
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldJobTitle;
    validateSupportedSortingFieldTypes(*transformOrganisation,QContactOrganization::FieldTitle,uidsToVerify);

    //supportedSortingFieldTypes - FieldAssistantName
    uidsToVerify.clear();
    uidsToVerify << KUidContactFieldAssistant;
    validateSupportedSortingFieldTypes(*transformOrganisation,QContactOrganization::FieldAssistantName,uidsToVerify);

    //supportedSortingFieldTypes - Wrongvalue
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformOrganisation,"WrongValue",uidsToVerify);

    validateContextsL(transformOrganisation);

    QContactOrganization organisation;
    organisation.setName(companyDetail);
    organisation.setDepartment(departmentDetail);
    organisation.setTitle(jobtitleDetail);
    organisation.setAssistantName(assistantDetail);
    QList<CContactItemField *> fields = transformOrganisation->transformDetailL(organisation);
    // Assume zero or 4 details
    if(companyDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 4);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldCompanyName));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(companyField), 0);
        QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldDepartmentName));
        QCOMPARE(fields.at(1)->TextStorage()->Text().CompareF(departmentField), 0);
        QVERIFY(fields.at(2)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(2)->ContentType().ContainsFieldType(KUidContactFieldJobTitle));
        QCOMPARE(fields.at(2)->TextStorage()->Text().CompareF(jobtitleField), 0);
        QVERIFY(fields.at(3)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(3)->ContentType().ContainsFieldType(KUidContactFieldAssistant));
        QCOMPARE(fields.at(3)->TextStorage()->Text().CompareF(assistantField), 0);
    }

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

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldAssistant);
    newField->TextStorage()->SetTextL(assistantField);
    contactDetail = transformOrganisation->transformItemField(*newField, contact);
    const QContactOrganization* organisationInfo4(static_cast<const QContactOrganization*>(contactDetail));
    QCOMPARE(organisationInfo4->assistantName(), assistantDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    delete transformOrganisation;
}

void TestCntTransformContactData::validateCntTransformAvatarL(TPtrC16 /*field*/, QString /*detail*/)
{
    QFAIL("Refactor test to match new api!");
    /*
    CntTransformContactData* transformAvatar = new CntTransformAvatar();
    QVERIFY(transformAvatar != 0);
    QVERIFY(transformAvatar->supportsField(KUidContactFieldCodImage.iUid));
    QVERIFY(transformAvatar->supportsField(KUidContactFieldRingTone.iUid));
    QVERIFY(transformAvatar->supportsField(KUidContactFieldVideoRingTone.iUid));
    QVERIFY(!(transformAvatar->supportsField(0))); //Test for Wrong value
    QVERIFY(transformAvatar->supportsDetail(QContactAvatar::DefinitionName));
    QVERIFY(!(transformAvatar->supportsDetail("WrongValue")));

    validateGetIdForField(*transformAvatar, QContactAvatar::FieldAvatar,0);
    validateGetIdForField(*transformAvatar, QContactAvatar::SubTypeImage,0);
    validateGetIdForField(*transformAvatar, QContactAvatar::SubTypeVideo,0);
    validateGetIdForField(*transformAvatar, QContactAvatar::SubTypeTexturedMesh,0);
    validateGetIdForField(*transformAvatar, QContactAvatar::SubTypeAudioRingtone,0);
    validateGetIdForField(*transformAvatar, QContactAvatar::SubTypeVideoRingtone,0);
    validateGetIdForField(*transformAvatar, "WrongValue", 0);
    QVERIFY(transformAvatar->supportsSubType(QContactAvatar::FieldSubType));
    QVERIFY( !(transformAvatar->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    validateSupportedSortingFieldTypes(*transformAvatar,"WrongValue",uidsToVerify);

    validateContextsL(transformAvatar);

    QContactAvatar avatar1;
    avatar1.setAvatar(detail);
    avatar1.setSubType(QContactAvatar::SubTypeImage);
    QList<CContactItemField *> fields = transformAvatar->transformDetailL(avatar1);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldCodImage));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactAvatar avatar2;
    avatar2.setAvatar(detail);
    avatar2.setSubType(QContactAvatar::SubTypeAudioRingtone);
    fields = transformAvatar->transformDetailL(avatar2);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldRingTone));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    QContactAvatar avatar3;
    avatar2.setAvatar(detail);
    avatar2.setSubType(QContactAvatar::SubTypeVideoRingtone);
    fields = transformAvatar->transformDetailL(avatar2);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldVideoRingTone));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldCodImage);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformAvatar->transformItemField(*newField, contact);
    const QContactAvatar* avatarInfo1(static_cast<const QContactAvatar*>(contactDetail));
    QCOMPARE(avatarInfo1->avatar(), detail);
    QVERIFY(avatarInfo1->subType().contains(QContactAvatar::SubTypeImage));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldRingTone);
    newField->TextStorage()->SetTextL(field);
    contactDetail = transformAvatar->transformItemField(*newField, contact);
    const QContactAvatar* avatarInfo2(static_cast<const QContactAvatar*>(contactDetail));
    QCOMPARE(avatarInfo2->avatar(), detail);
    QVERIFY(avatarInfo2->subType().contains(QContactAvatar::SubTypeAudioRingtone));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldVideoRingTone);
    newField->TextStorage()->SetTextL(field);
    contactDetail = transformAvatar->transformItemField(*newField, contact);
    const QContactAvatar* avatarInfo3(static_cast<const QContactAvatar*>(contactDetail));
    QCOMPARE(avatarInfo3->avatar(), detail);
    QVERIFY(avatarInfo3->subType().contains(QContactAvatar::SubTypeVideoRingtone));
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    delete transformAvatar;
    */
}

void TestCntTransformContactData::validateCntTransformSyncTargetL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformSyncTarget = new CntTransformSyncTarget();
    QVERIFY(transformSyncTarget != 0);
    QVERIFY(transformSyncTarget->supportsField(KUidContactFieldClass.iUid));
    QVERIFY(transformSyncTarget->supportsDetail(QContactSyncTarget::DefinitionName));
    QVERIFY(!(transformSyncTarget->supportsDetail("WrongValue")));

    validateGetIdForField(*transformSyncTarget,QContactSyncTarget::FieldSyncTarget,KUidContactFieldClass.iUid);
    validateGetIdForField(*transformSyncTarget, "WrongValue", 0);
    QVERIFY( !(transformSyncTarget->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldClass;
    validateSupportedSortingFieldTypes(*transformSyncTarget,QContactSyncTarget::FieldSyncTarget,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformSyncTarget,"WrongValue",uidsToVerify);

    validateContextsL(transformSyncTarget);

    QContactSyncTarget syncTarget;
    syncTarget.setSyncTarget(detail);
    QList<CContactItemField *> fields = transformSyncTarget->transformDetailL(syncTarget);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldClass));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldClass);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformSyncTarget->transformItemField(*newField, contact);
    const QContactSyncTarget* syncTargetInfo(static_cast<const QContactSyncTarget*>(contactDetail));
    QCOMPARE(syncTargetInfo->syncTarget(), detail);

    delete contactDetail;
    delete newField;
    delete transformSyncTarget;
}

void TestCntTransformContactData::validateCntTransformGenderL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformGender = new CntTransformGender();
    QVERIFY(transformGender != 0);
    QVERIFY(transformGender->supportsField(KUidContactFieldGender.iUid));
    QVERIFY(!(transformGender->supportsField(0))); //Test for Wrong value
    QVERIFY(transformGender->supportsDetail(QContactGender::DefinitionName));
    QVERIFY(!(transformGender->supportsDetail("WrongValue")));

    validateGetIdForField(*transformGender, QContactGender::FieldGender,KUidContactFieldGender.iUid);
    validateGetIdForField(*transformGender, QContactGender::GenderMale,0);
    validateGetIdForField(*transformGender, QContactGender::GenderFemale,0);
    validateGetIdForField(*transformGender, QContactGender::GenderUnspecified,0);
    validateGetIdForField(*transformGender, "WrongValue", 0);
    QVERIFY( !(transformGender->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldGender;
    validateSupportedSortingFieldTypes(*transformGender,QContactGender::FieldGender,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformGender,"WrongValue",uidsToVerify);

    validateContextsL(transformGender);

    QContactGender gender;
    gender.setGender(detail);
    QList<CContactItemField *> fields = transformGender->transformDetailL(gender);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldGender));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGender);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformGender->transformItemField(*newField, contact);
    const QContactGender* genderInfo(static_cast<const QContactGender*>(contactDetail));
    QCOMPARE(genderInfo->gender(), detail);

    delete contactDetail;
    delete newField;
    delete transformGender;
}

void TestCntTransformContactData::validateCntTransformAnniversaryL(TPtrC16 field, QDate dateDetail, QString eventDetail)
{
    CntTransformContactData* transformAnniversary = new CntTransformAnniversary();
    QVERIFY(transformAnniversary != 0);
    QVERIFY(transformAnniversary->supportsField(KUidContactFieldAnniversary.iUid));
    QVERIFY(!(transformAnniversary->supportsField(0))); //Test for Wrong value
    QVERIFY(transformAnniversary->supportsDetail(QContactAnniversary::DefinitionName));
    QVERIFY(!(transformAnniversary->supportsDetail("WrongValue")));

    validateGetIdForField(*transformAnniversary, QContactAnniversary::FieldCalendarId,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::FieldOriginalDate,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::FieldEvent,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::SubTypeWedding,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::SubTypeEngagement,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::SubTypeHouse,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::SubTypeEmployment,0);
    validateGetIdForField(*transformAnniversary, QContactAnniversary::SubTypeMemorial,0);
    validateGetIdForField(*transformAnniversary, "WrongValue", 0);
    QVERIFY(transformAnniversary->supportsSubType(QContactAnniversary::FieldSubType));
    QVERIFY( !(transformAnniversary->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    validateSupportedSortingFieldTypes(*transformAnniversary,"WrongValue",uidsToVerify);

    validateContextsL(transformAnniversary);

    QContactAnniversary anniversary;
    anniversary.setOriginalDate(dateDetail);
    anniversary.setEvent(eventDetail);
    QList<CContactItemField *> fields = transformAnniversary->transformDetailL(anniversary);
    QVERIFY(fields.count() == 1);
    QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
    QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldAnniversary));
    QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldAnniversary);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformAnniversary->transformItemField(*newField, contact);
    const QContactAnniversary* anniversaryInfo(static_cast<const QContactAnniversary*>(contactDetail));
    QCOMPARE(anniversaryInfo->event(), eventDetail);
    if (dateDetail.isValid()) {
        QCOMPARE(anniversaryInfo->originalDate(), dateDetail);
    }

    delete contactDetail;
    delete newField;
    delete transformAnniversary;
}

void TestCntTransformContactData::validateCntTransformGeolocationL(TPtrC16 field, double latitudeDetail, double longitudeDetail)
{
    CntTransformContactData* transformGeolocation = new CntTransformGeolocation();
    QVERIFY(transformGeolocation != 0);
    QVERIFY(transformGeolocation->supportsField(KUidContactFieldGEO.iUid));
    QVERIFY(!(transformGeolocation->supportsField(0))); //Test for Wrong value
    QVERIFY(transformGeolocation->supportsDetail(QContactGeoLocation::DefinitionName));
    QVERIFY(!(transformGeolocation->supportsDetail("WrongValue")));

    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldLabel,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldLatitude,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldLongitude,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldAccuracy,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldAltitude,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldAltitudeAccuracy,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldHeading,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldSpeed,0);
    validateGetIdForField(*transformGeolocation, QContactGeoLocation::FieldTimestamp,0);
    validateGetIdForField(*transformGeolocation, "WrongValue", 0);
    QVERIFY( !(transformGeolocation->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    validateSupportedSortingFieldTypes(*transformGeolocation,"WrongValue",uidsToVerify);


    validateContextsL(transformGeolocation);

    QContactGeoLocation geolocation;
    geolocation.setLatitude(latitudeDetail);
    geolocation.setLongitude(longitudeDetail);
    QList<CContactItemField *> fields = transformGeolocation->transformDetailL(geolocation);
    if (latitudeDetail >= 0.0 || longitudeDetail >= 0.0) {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldGEO));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }
    else {
        QVERIFY(fields.count() == 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGEO);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformGeolocation->transformItemField(*newField, contact);
    const QContactGeoLocation* geolocationInfo(static_cast<const QContactGeoLocation*>(contactDetail));
    if (latitudeDetail >= 0.0 ) {
        QCOMPARE(geolocationInfo->latitude(), latitudeDetail);
    }
    else {
        QCOMPARE(geolocationInfo->latitude(), -1.0);
    }

    if (longitudeDetail > 0.0) {
        QCOMPARE(geolocationInfo->longitude(), longitudeDetail);
    }
    else {
        QCOMPARE(geolocationInfo->longitude(), -1.0);
    }

    delete contactDetail;
    delete newField;
    delete transformGeolocation;
}

void TestCntTransformContactData::validateCntTransformNoteL(TPtrC16 field, QString detail)
{
    CntTransformContactData* transformNote = new CntTransformNote();
    QVERIFY(transformNote != 0);
    QVERIFY(transformNote->supportsField(KUidContactFieldNote.iUid));
    QVERIFY(!(transformNote->supportsField(0))); //Test for Wrong value
    QVERIFY(transformNote->supportsDetail(QContactNote::DefinitionName));
    QVERIFY(!(transformNote->supportsDetail("WrongValue")));

    validateGetIdForField(*transformNote, QContactNote::FieldNote,KUidContactFieldNote.iUid);
    validateGetIdForField(*transformNote, "WrongValue", 0);
    QVERIFY( !(transformNote->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    uidsToVerify << KUidContactFieldNote;
    validateSupportedSortingFieldTypes(*transformNote,QContactNote::FieldNote,uidsToVerify);
    uidsToVerify.clear();
    validateSupportedSortingFieldTypes(*transformNote,"WrongValue",uidsToVerify);


    validateContextsL(transformNote);

    QContactNote note;
    note.setNote(detail);
    QList<CContactItemField *> fields = transformNote->transformDetailL(note);
    if(detail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 1);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldNote));
        QVERIFY(fields.at(0)->ContentType().Mapping() == KUidContactFieldVCardMapNOTE);
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(field), 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldNote);
    newField->TextStorage()->SetTextL(field);
    QContact contact;
    QContactDetail* contactDetail = transformNote->transformItemField(*newField, contact);
    const QContactNote* noteInfo(static_cast<const QContactNote*>(contactDetail));
    QCOMPARE(noteInfo->note(), detail);

    delete contactDetail;
    delete newField;
    delete transformNote;
}

void TestCntTransformContactData::validateCntTransformFamilyL(TPtrC16 spouseField, QString spouseDetail,
                             TPtrC16 childField, QString childDetail)
{
    CntTransformContactData* transformFamily = new CntTransformFamily();
    QVERIFY(transformFamily != 0);
    QVERIFY(transformFamily->supportsField(KUidContactFieldSpouse.iUid));
    QVERIFY(transformFamily->supportsField(KUidContactFieldChildren.iUid));
    QVERIFY(!(transformFamily->supportsField(0))); //Test for Wrong value
    QVERIFY(transformFamily->supportsDetail(QContactFamily::DefinitionName));
    QVERIFY(!(transformFamily->supportsDetail("WrongValue")));

    validateGetIdForField(*transformFamily, QContactFamily::FieldSpouse,KUidContactFieldSpouse.iUid);
    validateGetIdForField(*transformFamily, QContactFamily::FieldChildren,KUidContactFieldChildren.iUid);
    validateGetIdForField(*transformFamily, "WrongValue", 0);
    QVERIFY( !(transformFamily->supportsSubType("WrongValue")));

    //Test supportedSortingFieldTypes
    QList<TUid> uidsToVerify;
    validateSupportedSortingFieldTypes(*transformFamily,"WrongValue",uidsToVerify);

    validateContextsL(transformFamily);

    QContactFamily family;
    family.setSpouse(spouseDetail);
    family.setChildren(QStringList(childDetail));
    QList<CContactItemField *> fields = transformFamily->transformDetailL(family);
    if(spouseDetail.isEmpty()) {
        QVERIFY(fields.count() == 0);
    } else {
        QVERIFY(fields.count() == 2);
        QVERIFY(fields.at(0)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(0)->ContentType().ContainsFieldType(KUidContactFieldSpouse));
        QCOMPARE(fields.at(0)->TextStorage()->Text().CompareF(spouseField), 0);
        QVERIFY(fields.at(1)->StorageType() == KStorageTypeText);
        QVERIFY(fields.at(1)->ContentType().ContainsFieldType(KUidContactFieldChildren));
        QCOMPARE(fields.at(1)->TextStorage()->Text().CompareF(childField), 0);
    }

    CContactItemField* newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldSpouse);
    newField->TextStorage()->SetTextL(spouseField);
    QContact contact;
    QContactDetail* contactDetail = transformFamily->transformItemField(*newField, contact);
    const QContactFamily* familyInfo1(static_cast<const QContactFamily*>(contactDetail));
    QCOMPARE(familyInfo1->spouse(), spouseDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldChildren);
    newField->TextStorage()->SetTextL(childField);
    contactDetail = transformFamily->transformItemField(*newField, contact);
    contact.saveDetail(contactDetail);
    const QContactFamily* familyInfo2(static_cast<const QContactFamily*>(contactDetail));
    QCOMPARE(familyInfo2->children().count(), 1);
    QCOMPARE(familyInfo2->children().at(0), childDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    newField = CContactItemField::NewL(KStorageTypeText, KUidContactFieldChildren);
    newField->TextStorage()->SetTextL(childField);
    contactDetail = transformFamily->transformItemField(*newField, contact);
    const QContactFamily* familyInfo3(static_cast<const QContactFamily*>(contactDetail));
    QCOMPARE(familyInfo3->children().count(), 2);
    QCOMPARE(familyInfo3->children().at(1), childDetail);
    delete contactDetail;
    contactDetail = 0;
    delete newField;
    newField = 0;

    delete transformFamily;
}

void TestCntTransformContactData::validateContextsL(CntTransformContactData* transformContactData) const
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

/*!
 * Validates the getIdForField function
 *
 * \a transformContactData the tranformcontact instance
 * \a filedname - The filedname which has to be checked
 * \a idValue - the correct id value against which the comparison is done
 * \a isSame -  If this value is true then '==' comparison is done else '!=' comparison is done
 */
void TestCntTransformContactData::validateGetIdForField(
                    const CntTransformContactData& transformContactData,
                    const QString& filedname,
                    const quint32  idValue )const
{
    //Ravikiran
    quint32 idForField =  transformContactData.getIdForField(filedname);
    QVERIFY(idValue == idForField );
}
void TestCntTransformContactData::validateSupportedSortingFieldTypes(
        const CntTransformContactData& transformContactData,
        const QString& filedname,
        const QList<TUid>&  checkSortFieldList )const
{
    QList<TUid> sortFieldList = transformContactData.supportedSortingFieldTypes(filedname);

    QVERIFY(sortFieldList.count() == checkSortFieldList.count() );
    if(sortFieldList.count() == checkSortFieldList.count()){

    for(int i = 0; i<sortFieldList.count() ; i++ )
        {
        QVERIFY(sortFieldList.at(i) == checkSortFieldList.at(i));
        }

    }


}

