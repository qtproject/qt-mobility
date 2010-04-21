/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ut_cntsymbiandatabase.h"
#include "cntsymbiandatabase.h"
#include "qcontactmanagerengine.h"
#include "mock_cntdb.h"

#include <QtTest/QtTest>

void TestCntSymbianDatabase::initTestCase()
{
}

void TestCntSymbianDatabase::cleanupTestCase()
{
}

void TestCntSymbianDatabase::ctor()
{
    QContactManagerEngine *engine = 0;
    QContactManager::Error error;
    CntSymbianDatabase *db = 0;
    db = new CntSymbianDatabase(engine, &error);
    QVERIFY(db != 0);
    QVERIFY(db->m_engine == engine);
#ifndef SYMBIAN_BACKEND_USE_SQLITE
    QVERIFY(db->m_contactChangeNotifier != 0);
#endif
    QVERIFY(db->m_contactDatabase);
    delete db;
}

void TestCntSymbianDatabase::contactDatabase()
{
    QContactManagerEngine *engine = 0;
    QContactManager::Error error;
    CntSymbianDatabase *db;
    db = new CntSymbianDatabase(engine, &error);
    QVERIFY(db != 0);
    QVERIFY(error == QContactManager::NoError);

    CContactDatabase *cdb = 0;
    cdb = db->contactDatabase();
    QVERIFY(cdb != 0);
    CContactDatabase *cdb1 = db->m_contactDatabase;
    QVERIFY(cdb1 == db->contactDatabase());
    delete db;
}

void TestCntSymbianDatabase::appendFunctions()
{
    QContactManagerEngine *engine = 0;
    QContactManager::Error error;
    CntSymbianDatabase *db;
    db = new CntSymbianDatabase(engine, &error);
    QVERIFY(db != 0);
    QVERIFY(error == QContactManager::NoError);

    QContactLocalId id(123);
    db->appendContactEmitted(id);
    QVERIFY(db->m_contactsEmitted.contains(id));
    db->m_contactsEmitted.clear();

    QList<QContactLocalId> contactList;
    QContactLocalId id1(345);
    QContactLocalId id2(678);
    QContactLocalId id3(910);

    contactList.append(id1);
    contactList.append(id2);
    contactList.append(id3);
    db->appendContactsEmitted(contactList);
    QVERIFY(db->m_contactsEmitted.contains(id1));
    QVERIFY(db->m_contactsEmitted.contains(id2));
    QVERIFY(db->m_contactsEmitted.contains(id3));
    delete db;
}

void TestCntSymbianDatabase::databaseEvents()
{
    QContactManagerEngine *engine = 0;
    QContactManager::Error error;
    CntSymbianDatabase *db = 0;
    db = new CntSymbianDatabase(engine, &error);
    QVERIFY(db != 0);
    QVERIFY(db->m_contactDatabase != 0);
    // This only defined when the mock database is used
#if 0
    TRAPD(err, db->m_contactDatabase->sendEventsL());
    QVERIFY(err == KErrNone);
    delete db;
#else
    delete db;
    QFAIL("Mock database not currently compiled in, test meaningless?");
#endif
}

