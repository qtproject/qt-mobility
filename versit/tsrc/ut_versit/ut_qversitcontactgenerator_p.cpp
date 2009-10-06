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

#include "qversitcontactgenerator_p.h"
#include "ut_qversitcontactgenerator_p.h"


#include <qversitproperty.h>
#include <qversitdocument.h>
#include <QtTest/QtTest>

void UT_QVersitContactGeneratorPrivate::initTestCase()
{    
   d_ptr = new QVersitContactGeneratorPrivate();
   QVERIFY(d_ptr);
}

void UT_QVersitContactGeneratorPrivate::cleanupTestCase()
{
   delete d_ptr;
}

void UT_QVersitContactGeneratorPrivate::generateContacts()
{
   /*
   QByteArray firstName = "Homer";
   QByteArray lastName = "Simpson";
   QByteArray mobileNo = "+358704860613";
   QByteArray email = "homer@holycow.com";
   QVersitDocument* doc = new QVersitDocument();
   QVersitProperty* property = new QVersitProperty();
   // test for basic firstname,lastname
   property->setName("FN");
   property->setValue(firstName);
   doc->addProperty(*property);
   delete property;
   property = new QVersitProperty();
   property->setName("LN");
   property->setValue(lastName);
   doc->addProperty(*property);
   delete property;

   QList<QContact> contacts = d_ptr->generateContacts(*doc);

   foreach( QContact contact,contacts){
       // Verify for contact details matched with.
   }
   delete doc;
   */
}
