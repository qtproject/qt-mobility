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

#include "ut_transformcontact.h"
#include "cnttransformcontact.h"

#include <QtTest/QtTest>


void TestCntTransformContact::initTestCase()
{
}

void TestCntTransformContact::cleanupTestCase()
{
}


void TestCntTransformContact::executeCntTransformSymbianContact()
{
	CntTransformContact transformContact;

#if 0 /*BELOW CODE DOESN'T TEST ANYTHING*/

	//empty contact
	CContactCard* symContact = CContactCard::NewLC();
	QContact qcontact = transformContact.transformContact(*symContact);
	CleanupStack::PopAndDestroy(symContact);
#endif

}

void TestCntTransformContact::executeCntTransformQtContact()
{
    
	CntTransformContact transformContact;

#if 0 /*BELOW CODE DOESN'T TEST ANYTHING*/
	//empty contact
	QContact contact;
	CContactItem *contactItem = transformContact.transformContact(contact);
	//validate data
	delete contactItem;
	
	QContactPhoneNumber phoneNumber;
	phoneNumber.setNumber("123456");
	phoneNumber.setAttribute(QContactPhoneNumber::AttributeSubType, QContactPhoneNumber::AttributeSubTypeLandline);
	contact.saveDetail(&phoneNumber);
	
	contactItem = transformContact.transformContact(contact);
	//validate data
	delete contactItem;
#endif
}


//======================================================================================================//
//Validate QContact
void TestCntTransformContact::validateQContactEmpty(const QContact& contact) const
{
}

void TestCntTransformContact::validateQContactPhoneNumber(const QContact& contact) const
{
}


//======================================================================================================//
void TestCntTransformContact::validateSymbianContactEmpty(const CContactItem& contactItem) const
{
}

void TestCntTransformContact::validateSymbianContactPhoneNumber(const CContactItem& contactItem) const
{
}




	

QTEST_MAIN(TestCntTransformContact);
