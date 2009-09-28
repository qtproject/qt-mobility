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
#include <vector>

#include <QtTest/QtTest>
#include <QTime>

void TestPluginPerfomance::initTestCase()
{
    QString absolutePath("C:/logs/qtphonebook/logs.htm");
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

void TestPluginPerfomance::createContactsTestCase()
{
    int noOfContacts = 1000;
    QTime t;
    t.start();
    for(int i=0; i<noOfContacts; i++) {
        QChar c(i);
        QString first("Alice");
        QString last("Jones");
        
        QContact alice;

        /* Set the contact's name */
        QContactName aliceName;
        aliceName.setFirst(first.append(c));
        aliceName.setLast(last.append(c));
        alice.saveDetail(&aliceName);
        
        QString label("Ally Jones");
        QContactDisplayLabel aliceDisplay;
        aliceDisplay.setLabel(label.append(c));
        alice.saveDetail(&aliceDisplay);

        /* Add a phone number */
        QContactPhoneNumber number;
        number.setContexts("Home");
        number.setSubTypes("Mobile");
        number.setNumber("12345678");
        alice.saveDetail(&number);
        alice.setPreferredDetail("DialAction", number);

        /* Add a second phone number */
        QContactPhoneNumber number2;
        number2.setContexts("Work");
        number2.setSubTypes("Landline");
        number2.setNumber("555-4444");
        alice.saveDetail(&number2);

        /* Save the contact */
        if (mCntMng->saveContact(&alice))
            mContactList.append(alice.id());
        
    }   
    log() << "Created " << noOfContacts << " contacts in " 
        << t.elapsed() / 1000 << " s " << t.elapsed() % 1000 << " ms <br>";
}

void TestPluginPerfomance::removeContactsTestCase()
{
    QTime t;
    t.start();
    
    for(int i=0; i<mContactList.count(); i++)    
        mCntMng->removeContact(mContactList[i]);
    
    log() << "Removed " << mContactList.count() << " contacts in " 
        << t.elapsed() / 1000 << " s " << t.elapsed() % 1000 << " ms <br>";
}

void TestPluginPerfomance::cleanupTestCase()
{
    log() << "------------- End symbian backend performance test --------------- <br>";
    delete mCntMng;
    mFile->close();
    delete mFile;
    delete mDebug;
}

QDebug TestPluginPerfomance::log()
{
    if (mDebug != NULL) {
       return *mDebug;
    } else {
       return qDebug();
    }
}

QTEST_MAIN(TestPluginPerfomance);
