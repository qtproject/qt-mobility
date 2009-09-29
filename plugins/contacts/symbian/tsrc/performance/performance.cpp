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

#include "performance.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QFile>
#include <QTime>

void tst_SymbianPluginPerfomance::initTestCase()
{
    QString absolutePath("C:/logs/qtphonebook/performancelog.htm");
	mFile = new QFile(absolutePath);
    
    if (mFile->open(QIODevice::Text | QIODevice::WriteOnly))
        mDebug = new QDebug(mFile);
    
    log() << "------------- Start symbian backend performance test ---------------<br>";
    mCntMng = new QContactManager("symbian");
    
    if (mCntMng == NULL) {
        log() << "No results. QContactManager was not created<br>";
        QSKIP("Contact manager was not created", SkipAll);
    }
}

void tst_SymbianPluginPerfomance::cleanupTestCase()
{
    log() << "------------- End symbian backend performance test ---------------<br>";
    delete mCntMng;
    delete mDebug;
    mFile->close();
    delete mFile;
}

void tst_SymbianPluginPerfomance::simpleContactsTestCase()
{
    QTime t;
    t.start();
    
    // Create N contacts
    for(int i=0; i<NO_OF_CONTACTS; i++) {
        QChar c(i);
        QString first("Alice");        
        QContact alice;
    
        // Contact details
        QContactName aliceName;
        aliceName.setFirst(first.append(c));
        alice.saveDetail(&aliceName);
            
        // Save the contact
        if (mCntMng->saveContact(&alice))
            mContactList.append(alice.id());
    }    
    log() << "Created " << mContactList.count() << " simple contacts in " 
        << t.elapsed() / 1000 << "s" << t.elapsed() % 1000 << "ms<br>";
    
    // Remove N contacts
    t.restart();
    for(int i=0; i<mContactList.count(); i++)    
        mCntMng->removeContact(mContactList[i]);
    
    log() << "Removed " << mContactList.count() << " simple contacts in " 
        << t.elapsed() / 1000 << "s" << t.elapsed() % 1000 << "ms<br>";
    
    mContactList.clear();
}

void tst_SymbianPluginPerfomance::complexContactsTestCase()
{
    QTime t;
    t.start();
    
    // Create N contacts
    for(int i=0; i<NO_OF_CONTACTS; i++) {
        QContact alice;
        
        // Contact details
        QChar c(i);
        QString first("Alice");
        QString last("Jones");
        
        QContactName aliceName;
        aliceName.setFirst(first.append(c));
        aliceName.setLast(last.append(c));
        alice.saveDetail(&aliceName);
        
        QString label("Ally Jones");
        QContactDisplayLabel aliceDisplay;
        aliceDisplay.setLabel(label.append(c));
        alice.saveDetail(&aliceDisplay);

        QContactPhoneNumber number;
        number.setContexts("Home");
        number.setSubTypes("Mobile");
        number.setNumber("12345678");
        alice.saveDetail(&number);
        alice.setPreferredDetail("DialAction", number);

        QContactPhoneNumber number2;
        number2.setContexts("Work");
        number2.setSubTypes("Landline");
        number2.setNumber("555-4444");
        alice.saveDetail(&number2);

        QContactAddress add;
        add.setStreet("Leeds West Yorkshire");
        add.setPostcode("10087");
        add.setRegion("New York");
        add.setCountry("United States");
        alice.saveDetail(&add);
        
        QContactGender gender;
        gender.setGender("Female");
        alice.saveDetail(&gender);
        
        QContactBirthday bday;
        bday.setDate(QDate(25,10,1978));
        alice.saveDetail(&bday);
        
        QContactOnlineAccount acc;
        acc.setAccountUri("sips:alice.jones@nokia.com");
        alice.saveDetail(&acc);
        
        QContactEmailAddress email;
        email.setEmailAddress("mailto:alice.jones@nokia.com");
        alice.saveDetail(&email);
        
        QContactOrganisation org;
        org.setDepartment("Services");
        org.setTitle("Assistant Manager");
        org.setLocation("Nokia Cyber Park");
        alice.saveDetail(&email);
        
        // Save the contact
        if (mCntMng->saveContact(&alice))
            mContactList.append(alice.id());
        
    }   
    log() << "Created " << mContactList.count() << " complex contacts in " 
        << t.elapsed() / 1000 << "s" << t.elapsed() % 1000 << "ms<br>";
    
    // Remove N contacts
    t.restart();
    for(int i=0; i<mContactList.count(); i++)    
        mCntMng->removeContact(mContactList[i]);
    
    log() << "Removed " << mContactList.count() << " complex contacts in " 
        << t.elapsed() / 1000 << "s" << t.elapsed() % 1000 << "ms<br>";
    
    mContactList.clear();
}

QDebug tst_SymbianPluginPerfomance::log()
{
    if (mDebug != NULL) {
        return *mDebug;
    } else {
       return qDebug();
    }
}

QTEST_MAIN(tst_SymbianPluginPerfomance);
