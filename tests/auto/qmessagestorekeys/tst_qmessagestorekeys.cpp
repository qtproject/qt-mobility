/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QObject>
#include <QTest>
#include <QDebug>

#include "qtmessaging.h"
#include "../support/support.h"

//TESTED_CLASS=
//TESTED_FILES=

/*
    Unit test for classes implementing query keys for the QMessageStore class.
*/
class tst_QMessageStoreKeys : public QObject
{
    Q_OBJECT

public:
    tst_QMessageStoreKeys();
    virtual ~tst_QMessageStoreKeys();

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testAccountFilterKey_data();
    void testAccountFilterKey();

private:
    QMessageAccountIdList accountIds;
    QMessageFolderIdList folderIds;
    QMessageIdList messageIds;
};

Q_DECLARE_METATYPE(QMessageAccountIdList)
Q_DECLARE_METATYPE(QMessageAccountFilterKey)

QDebug &operator<<(QDebug &dbg, const QMessageAccountId &id)
{
    return dbg << id.toString();
}

QTEST_MAIN(tst_QMessageStoreKeys)

#include "tst_qmessagestorekeys.moc"

tst_QMessageStoreKeys::tst_QMessageStoreKeys()
{
}

tst_QMessageStoreKeys::~tst_QMessageStoreKeys()
{
}

class Params
{
    QMap<const char*, const char*> _map;

public:
    Params() {}

    Params& operator()(const char* name, const char* value)
    {
        _map.insert(name, value); return *this; 
    }

    operator Support::Parameters() const
    {
        Support::Parameters result;
        QMap<const char*, const char*>::const_iterator it = _map.begin(), end = _map.end();
        for ( ; it != end; ++it) {
            result.insert(it.key(), it.value());
        }
        return result;
    }
};

void tst_QMessageStoreKeys::initTestCase()
{
    Support::clearMessageStore();

    QList<Support::Parameters> accountParams;
    QList<Support::Parameters> folderParams;
    QList<Support::Parameters> messageParams;

    accountParams << Params()("name", "Work")
                             ("fromAddress", "Important.Person@example.com")
                  << Params()("name", "Personal")
                             ("fromAddress", "superstar2000@example.net");

    foreach (const Support::Parameters &params, accountParams) {
        accountIds.append(Support::addAccount(params));
        QVERIFY(accountIds.last().isValid());
    }
    foreach (const Support::Parameters &params, folderParams) {
        folderIds.append(Support::addFolder(params));
        QVERIFY(folderIds.last().isValid());
    }
    foreach (const Support::Parameters &params, messageParams) {
        messageIds.append(Support::addMessage(params));
        QVERIFY(messageIds.last().isValid());
    }
}

void tst_QMessageStoreKeys::cleanup()
{
}

void tst_QMessageStoreKeys::cleanupTestCase()
{
}

void tst_QMessageStoreKeys::testAccountFilterKey_data()
{
    QTest::addColumn<QMessageAccountFilterKey>("key");
    QTest::addColumn<QMessageAccountIdList>("ids");
    QTest::addColumn<QMessageAccountIdList>("negatedIds");

    QTest::newRow("empty key")
        << QMessageAccountFilterKey() 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id equality 1")
        << QMessageAccountFilterKey::id(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id equality 2")
        << QMessageAccountFilterKey::id(accountIds[1], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id equality invalid")
        << QMessageAccountFilterKey::id(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id inequality 1")
        << QMessageAccountFilterKey::id(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageAccountFilterKey::id(accountIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageAccountFilterKey::id(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id list inclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id list inclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id list inclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountIdList(), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id list exclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id list exclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountIdList(), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Work"), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id filter inclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Personal"), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id filter inclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Work"), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Personal"), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id filter exclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name equality 1")
        << QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name equality 2")
        << QMessageAccountFilterKey::name("Personal", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name equality non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inequality 1")
        << QMessageAccountFilterKey::name("Work", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name inequality 2")
        << QMessageAccountFilterKey::name("Personal", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name inequality non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion 1")
        << QMessageAccountFilterKey::name("ork", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name inclusion 2")
        << QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inclusion empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion 1")
        << QMessageAccountFilterKey::name("ork", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name exclusion 2")
        << QMessageAccountFilterKey::name("o", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality 1")
        << QMessageAccountFilterKey::fromAddress("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("fromAddress equality 2")
        << QMessageAccountFilterKey::fromAddress("superstar2000@example.net", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress equality non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inequality 1")
        << QMessageAccountFilterKey::fromAddress("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress inequality 2")
        << QMessageAccountFilterKey::fromAddress("superstar2000@example.net", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("fromAddress inequality non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion 1")
        << QMessageAccountFilterKey::fromAddress("Import", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("fromAddress inclusion 2")
        << QMessageAccountFilterKey::fromAddress("@example.", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inclusion empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion 1")
        << QMessageAccountFilterKey::fromAddress("Import", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress exclusion 2")
        << QMessageAccountFilterKey::fromAddress("@example.", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress exclusion non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress exclusion zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;
}

void tst_QMessageStoreKeys::testAccountFilterKey()
{
    QFETCH(QMessageAccountFilterKey, key);
    QFETCH(QMessageAccountIdList, ids);
    QFETCH(QMessageAccountIdList, negatedIds);

    // Order is irrelevant for filtering
    QCOMPARE(QMessageStore::instance()->queryAccounts(key).toSet(), ids.toSet());
    QCOMPARE(QMessageStore::instance()->queryAccounts(~key).toSet(), negatedIds.toSet());

    /*
    {
        QSet<QMessageAccountId> x(QMessageStore::instance()->queryAccounts(key).toSet());
        QSet<QMessageAccountId> y(ids.toSet());
        if (x != y) {
            qDebug() << "x:" << x;
            qDebug() << "y:" << y;
        }
        QCOMPARE(x, y);
    }

    {
        QSet<QMessageAccountId> x(QMessageStore::instance()->queryAccounts(~key).toSet());
        QSet<QMessageAccountId> y(negatedIds.toSet());
        if (x != y) {
            qDebug() << "x:" << x;
            qDebug() << "y:" << y;
        }
        QCOMPARE(x, y);
    }
    */
}

