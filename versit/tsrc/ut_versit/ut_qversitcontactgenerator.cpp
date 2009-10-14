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

#include "ut_qversitcontactgenerator.h"
#include <qversitcontactgenerator.h>
#include <qversitproperty.h>
#include <qversitdocument.h>
#include <QtTest/QtTest>
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>

#include "qversitdefs.h"

void UT_QVersitContactGenerator::init()
{    
   mGenerator = new QVersitContactGenerator();
   QVERIFY(mGenerator);
}

void UT_QVersitContactGenerator::cleanup()
{
   delete mGenerator;
}

void UT_QVersitContactGenerator::testAddName()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    QStringList val;
    val.append("Simpson");//FirstName
    val.append("Homer");//LastName
    val.append("BellyBoy");//GivenName
    val.append("Dr");//PreFix
    val.append("MSc");//Suffix
    nameProperty.setName(QString::fromAscii(versitNameId));
    nameProperty.setValue(val.join(versitValueSeparator).toAscii());
    document.addProperty(nameProperty);        
    QContact contact = mGenerator->generateContact(document);    
    QCOMPARE(contact.details().count(),2);
    const QContactName& name = (QContactName)contact.detail(QContactName::DefinitionName);    
    QCOMPARE(name.last(),val[0]);
    QCOMPARE(name.first(),val[1]);
    QCOMPARE(name.middle(),val[2]);
    QCOMPARE(name.prefix(),val[3]);
    QCOMPARE(name.suffix(),val[4]);
}

void UT_QVersitContactGenerator::testAddTel()
{
    QVersitDocument document;
    QVersitProperty nameProperty;
    nameProperty.setName(QString::fromAscii(versitPhoneId));
    QByteArray val("+35850486321");
    nameProperty.setValue(val);
    QStringList param;
    param.append(versitVoiceId);
    param.append(versitCellId);
    param.append(versitModemId);
    param.append(versitCarId);
    param.append(versitVideoId);
    param.append(versitContextHomeId);
    param.append(versiContextWorkId);
    param.append("");//other type
    nameProperty.addParameter(versitType,param.join(versitValueSeparator));
    document.addProperty(nameProperty);
    QContact contact = mGenerator->generateContact(document);
    const QContactPhoneNumber& phone = (QContactPhoneNumber)contact.detail(
                                             QContactPhoneNumber::DefinitionName);
    QCOMPARE(phone.number(),QString(val));

    const QStringList subTypes = phone.subTypes();
    QCOMPARE(subTypes.count(),5);
    QCOMPARE(subTypes[0],QContactPhoneNumber::SubTypeVoice.operator QString());
    QCOMPARE(subTypes[1],QContactPhoneNumber::SubTypeMobile.operator QString());
    QCOMPARE(subTypes[2],QContactPhoneNumber::SubTypeModem.operator QString());
    QCOMPARE(subTypes[3],QContactPhoneNumber::SubTypeCar.operator QString());
    QCOMPARE(subTypes[4],QContactPhoneNumber::SubTypeVideo.operator QString());

    const QStringList contexts = phone.contexts();
    QCOMPARE(contexts.count(),3);
    QCOMPARE(contexts[0],QContactDetail::ContextHome.operator QString());
    QCOMPARE(contexts[1],QContactDetail::ContextWork.operator QString());
    QCOMPARE(contexts[2],QContactDetail::ContextOther.operator QString());
}
