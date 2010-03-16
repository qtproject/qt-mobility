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

#include <QtTest/QtTest>
#include <QObject>

#include <qtcontacts.h>

#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <etelmm_etel_test_server.h>
#else
#include <etelmm.h>
#endif
#include <mmtsy_names.h>

QTM_USE_NAMESPACE

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

//TESTED_CLASS=
//TESTED_FILES=

/*!
*/
class tst_SimCMAsync : public QObject
{
Q_OBJECT

public:
    tst_SimCMAsync();
    virtual ~tst_SimCMAsync();

public slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

private slots:
    void fetchContactReq();
    void localIdFetchReq();
    void saveContactReq();
    void removeContactReq();
    void detailDefinitionFetchReq();
    void notSupportedRequests();
    void multipleRequests();

private:
    void initManager(QString simStore);
    QContact createContact(QString name, QString number);
    QContact saveContact(QString name, QString number);

private:
    QContactManager* m_cm;
};

tst_SimCMAsync::tst_SimCMAsync() :
    m_cm(0)
{
	qRegisterMetaType<QContactAbstractRequest::State>("QContactAbstractRequest::State");
}

tst_SimCMAsync::~tst_SimCMAsync()
{
}

void tst_SimCMAsync::init()
{
    // remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(ids, 0);   
}

void tst_SimCMAsync::cleanup()
{
    // remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(ids, 0);   
}

void tst_SimCMAsync::initTestCase()
{
    initManager(QString());
}

void tst_SimCMAsync::cleanupTestCase()
{
    delete m_cm;
    m_cm = 0;
}

void tst_SimCMAsync::initManager(QString simStore)
{
    delete m_cm;
    m_cm = 0;

    QString uri("qtcontacts:symbiansim");

    // Set the sim store if available (simbackend defaults to ADN if not available)
    if(!simStore.isEmpty()) {
        uri.append(":store=");
        uri.append(simStore);
    }

    m_cm = QContactManager::fromUri(uri);
}

QContact tst_SimCMAsync::createContact(QString name, QString number)
{
    QContact c;
    
    QContactName n;
    n.setCustomLabel(name);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(number);
    c.saveDetail(&nb);

    return c;
}

QContact tst_SimCMAsync::saveContact(QString name, QString number)
{
    QContact c;
    
    QContactName n;
    n.setCustomLabel(name);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(number);
    c.saveDetail(&nb);
    
    if (!m_cm->saveContact(&c)) {
        qWarning("*FATAL* could not save contact!");
    }
    
    return c;
}

void tst_SimCMAsync::fetchContactReq()
{
    QContactFetchRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");
    QContact c3 = saveContact("c", "1111111");
    
    // Fetch the contacts
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);    
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.contacts().count() == 3);

    // Test cancelling
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);    
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
   
    // Remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(ids, 0);    
    
    // Test fetching nothing
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(req.contacts().count() == 0);
}

void tst_SimCMAsync::localIdFetchReq()
{
    QContactLocalIdFetchRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");
    QContact c3 = saveContact("c", "1111111");
    
    // Fetch the contacts
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.ids().count() == 3);
    QVERIFY(req.ids().contains(c1.localId()));
    QVERIFY(req.ids().contains(c2.localId()));
    QVERIFY(req.ids().contains(c3.localId()));
    
    // Test cancelling
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);    
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);    
    
    // Remove all contacts
    QList<QContactLocalId> ids = m_cm->contactIds();
    m_cm->removeContacts(ids, 0);    
    
    // Start again
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(req.ids().count() == 0);    
}

void tst_SimCMAsync::saveContactReq()
{
    QContactSaveRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Create some contacts
    QContact c1 = createContact("Keeppu", "47474747");
    QContact c2 = createContact("Rilli", "74747474");
    
    // Test cancel
    QList<QContact> contacts;
    contacts << c1 << c2;
    req.setContacts(contacts);    
    QVERIFY(!req.cancel());
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    QVERIFY(m_cm->contactIds().count() == 0);
    
    // Test save
    stateSpy.clear();
    req.setContacts(contacts); 
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(req.errorMap().count() == 0);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.contacts().count() == 2);
    QContact c = req.contacts().at(0);
    QVERIFY(c.id().managerUri() == m_cm->managerUri());
    QVERIFY(c.localId() != QContactLocalId(0));
    QVERIFY(c.detail<QContactName>().firstName() == c1.detail<QContactName>().firstName());
    QVERIFY(c.detail<QContactPhoneNumber>().number() == c1.detail<QContactPhoneNumber>().number());
    QVERIFY(m_cm->contactIds().count() == 2);
    
    // Test saving again
    c1 = req.contacts().at(0);
    c2 = req.contacts().at(1);

    QContactName name = c1.detail<QContactName>();
    name.setCustomLabel("Keeputin");
    c1.saveDetail(&name);
    QContactPhoneNumber number = c1.detail<QContactPhoneNumber>();
    c1.removeDetail(&number);
    
    contacts.clear();
    contacts << c1 << c2;
    req.setContacts(contacts);
    
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(req.errorMap().count() == 0);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.contacts().count() == 2);
    QVERIFY(req.contacts().at(0).localId() == c1.localId());
    QVERIFY(req.contacts().at(1).localId() == c2.localId());
    c = req.contacts().at(0);
    QVERIFY(c.details(QContactPhoneNumber::DefinitionName).count() == 0);
    QVERIFY(c.detail<QContactName>().customLabel() == c1.detail<QContactName>().customLabel());
    QVERIFY(m_cm->contactIds().count() == 2);
    c = m_cm->contact(c1.localId(), QStringList());
    QVERIFY(c.details(QContactPhoneNumber::DefinitionName).count() == 0);
    QVERIFY(c.detail<QContactName>().customLabel() == c1.detail<QContactName>().customLabel());
}

void tst_SimCMAsync::removeContactReq()
{
    QContactRemoveRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));
    
    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");

    // Remove the contacts
    QList<QContactLocalId> ids;
    ids << c1.localId() << c2.localId();
    req.setContactIds(ids);
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(req.errorMap().count() == 0);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(m_cm->contactIds().count() == 0);
    
    // Test cancel
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);
    
    // Remove same ones again
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    // NOTE: In emulator removing a nonexisting contact will return DoesNotExistError
    // and in hardware there will be no error.
    QVERIFY(req.error() == QContactManager::DoesNotExistError || req.error() == QContactManager::NoError);
    if (req.errorMap().count()) {
        QVERIFY(req.errorMap().count() == 2);
        QVERIFY(req.errorMap().value(0) == QContactManager::DoesNotExistError);
        QVERIFY(req.errorMap().value(1) == QContactManager::DoesNotExistError);
    }
}

void tst_SimCMAsync::detailDefinitionFetchReq()
{
    QContactDetailDefinitionFetchRequest req;
    req.setManager(m_cm);
    
    QSignalSpy stateSpy(&req, SIGNAL(stateChanged(QContactAbstractRequest::State)));
    QSignalSpy resultSpy(&req, SIGNAL(resultsAvailable()));

    // Fetch all
    req.setContactType(QContactType::TypeContact);
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NoError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.definitions().count());
    QVERIFY(req.definitions() == m_cm->detailDefinitions(req.contactType()));
    
    // Test cancel
    stateSpy.clear();
    resultSpy.clear();
    QVERIFY(!req.cancel());
    QVERIFY(req.start());
    QVERIFY(!req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.cancel());
    QVERIFY(!req.cancel());
    QVERIFY(req.state() == QContactAbstractRequest::CanceledState);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 0);

    // Fetch some defs
    stateSpy.clear();
    resultSpy.clear();
    req.setDefinitionNames(QStringList() << QContactPhoneNumber::DefinitionName << QContactNote::DefinitionName);
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QTRY_COMPARE(resultSpy.count(), 1);
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);
    QVERIFY(req.errorMap().value(1) == QContactManager::DoesNotExistError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
    QVERIFY(req.definitions().count() == 1);
    QVERIFY(req.definitions().contains(QContactPhoneNumber::DefinitionName));
    
    // Fetch non-existing type
    stateSpy.clear();
    resultSpy.clear();
    req.setContactType(QContactType::TypeGroup);
    req.setDefinitionNames(QStringList());
    QVERIFY(req.start());
    QVERIFY(req.state() == QContactAbstractRequest::ActiveState);
    QVERIFY(stateSpy.count() == 1);
    QVERIFY(req.waitForFinished(0));
    QVERIFY(req.state() == QContactAbstractRequest::FinishedState);
    QVERIFY(req.error() == QContactManager::NotSupportedError);
    QVERIFY(stateSpy.count() == 2);
    QVERIFY(resultSpy.count() == 1);
}

void tst_SimCMAsync::notSupportedRequests()
{
    QVERIFY(!m_cm->hasFeature(QContactManager::Relationships));
    QContactRelationshipFetchRequest rfreq;
    rfreq.setManager(m_cm);
    QVERIFY(!rfreq.start());
    QContactRelationshipRemoveRequest rrreq;
    rrreq.setManager(m_cm);
    QVERIFY(!rrreq.start());
    
    QVERIFY(!m_cm->hasFeature(QContactManager::MutableDefinitions));
    QContactDetailDefinitionRemoveRequest ddrreq;
    ddrreq.setManager(m_cm);
    QVERIFY(!ddrreq.start());
    QContactDetailDefinitionSaveRequest ddsreq;
    ddsreq.setManager(m_cm);
    QVERIFY(!ddsreq.start());
}

void tst_SimCMAsync::multipleRequests()
{
    // Save some contacts
    QContact c1 = saveContact("a", "1234567");
    QContact c2 = saveContact("b", "7654321");
    
    QContactLocalIdFetchRequest req1;
    req1.setManager(m_cm);

    QContactFetchRequest req2;
    req2.setManager(m_cm);
    
    QVERIFY(req1.start());
    QVERIFY(!req2.start());
    QVERIFY(req1.isActive());
    QVERIFY(!req2.isActive());
    QVERIFY(!req2.waitForFinished(0));
    QVERIFY(req1.waitForFinished(0));
    QVERIFY(req1.error() == QContactManager::NoError);
        
    QVERIFY(req2.start());
    QVERIFY(!req1.start());
    QVERIFY(req2.isActive());
    QVERIFY(!req1.isActive());
    QVERIFY(req1.waitForFinished(0)); // already at finished state so will return true
    QVERIFY(req2.waitForFinished(0));
    QVERIFY(req2.error() == QContactManager::NoError);
}


QTEST_MAIN(tst_SimCMAsync)
#include "tst_simcmasync.moc"
