/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QApplication>
#include <QtTest/QtTest>
//#include "contact.h"
//#include <QContactManager>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#include <qcontactabstractrequest.h>
#include <requests/qcontactfetchrequest.h>
#include <filters/qcontactlocalidfilter.h>
#include <details/qcontactdetails.h>

#ifdef Q_OS_SYMBIAN
#include <cntfilt.h>
#include <cntitem.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>
#include <cntdef.h>
#include <cntdef.hrh> 
#endif

QTM_USE_NAMESPACE

class tst_Contact : public QObject
{
    Q_OBJECT

public:
    enum platform {
        QContacts,
        Native
    };

    
private slots:
    void initTestCase();
    void cleanupTestCase();

    void tst_createTime_data();
    void tst_createTime();

    void tst_fetchAllContactIds_data();
    void tst_fetchAllContactIds();
    void tst_fetchOneContact_data();
    void tst_fetchOneContact();
    void tst_fetchTenContact_data();
    void tst_fetchTenContact();
    //void tst_fetchAllContact();

    void tst_createContact_data();  
    void tst_createContact();
    
    void tst_saveContact_data();
    void tst_saveContact();    

    void tst_nameFilter_data();
    void tst_nameFilter();

    void tst_removeOneContact_data();
    void tst_removeOneContact();
    
    void tst_removeAllContacts_data();
    void tst_removeAllContacts();


//    void tst_currentLanguage();
//    void tst_availableLanguages();
//
//    void tst_versions_data();
//    void tst_versions();
//
//    void tst_hasFeatures_data();
//    void tst_hasFeatures();

public slots:

    void gotContact(QContactFetchRequest*,bool);
    void stateChanged(QContactAbstractRequest::State newState);
    void timeout();
    void resultsAvailable();
    void resultsAvailableFilter();
    //void setManager(QContactManager *mgr);

private:
    void createContact(QContactManager *mgr, enum platform p = QContacts);
    void clearContacts(QContactManager *mgr, enum platform p = QContacts);
    int countContacts(QContactManager *mgr, enum platform p = QContacts);
    
    QString manager;
    QEventLoop *loop;
//    QContactManager *m_qm;
    QContactManager *m_systemManager;
    QList<QContactManager *> m_managers;
//    enum platform m_platform;
    int m_num_contacts;
    QList<QContactLocalId> id_list;

    int m_run;
    
    int m_num_start;

    QTimer *m_timer;
    QStringList firstNames;
    QStringList lastNames;    

};

Q_DECLARE_METATYPE(QContactManager *);
Q_DECLARE_METATYPE(tst_Contact::platform);

void tst_Contact::initTestCase()
{    
    qDebug() << "Managers: " << QContactManager::availableManagers();    
    m_run = 0;
    m_systemManager = 0x0;

    QStringList list = QContactManager::availableManagers();       
    while(!list.empty()){
      QString mgr = list.takeFirst();
      if((mgr != "invalid") && (mgr != "testdummy") && (mgr != "maliciousplugin")){
        m_managers.append(new QContactManager(mgr));
        if((mgr == "symbian") || (mgr == "tracker") || (mgr == "maemo5")) {
          qDebug() << "Found system manager: " << mgr;
          m_systemManager = m_managers.last();
        }
        if(!m_systemManager && mgr == "memory"){
            qDebug() << "Found system manager of last resort: " << mgr;
            m_systemManager = m_managers.last();
        }
      }      
    }
    if(m_managers.isEmpty()) {
      QFAIL("Unable to find valid managers. Please check install");
    }
    if(!m_systemManager){
      QFAIL("Unable to find a default plugin to use, please install plugins or fix test");
    }

    // setup an event loop for waiting
    loop = new QEventLoop;

    firstNames << "Anahera" << "Anaru" << "Hemi" << "Hine" << "Kiri" << "Maata" << "Mere" << "Moana" << "Paora" << "Petera" << "Piripi" << "Ruiha" << "Tane" << "Whetu";
    lastNames << "Ati Awa" << "Kai Taho" << "Moriori" << "Muaupoko" << "Nga Rauru" << "Taranaki" << "Opotoki" << "Aotea" << "Taninui" << "Tuhourangi" << "Tainui" << "Waitaha";
    
    m_num_start = countContacts(m_systemManager);
    qDebug() << "Number of Contact: " << m_num_start;


    for(int i = 0; i < 20; i++){
      foreach(QContactManager *mgr, m_managers){
        createContact(mgr);
      }
    }
    
    int after = countContacts(m_systemManager);
    if(after - m_num_start != 20){
        qWarning() << "Failed to create 20 contacts";
    }

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

}

int tst_Contact::countContacts(QContactManager *manager, enum platform platform)
{  
  if(platform == tst_Contact::QContacts) {
        QList<QContactLocalId> qcl = manager->contactIds();
        return qcl.count();
  } 
  else if(platform == tst_Contact::Native){ 
#ifdef Q_OS_SYMBIAN
    CContactDatabase* contactsDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactsDb);
    
    int num = contactsDb->CountL();
    
    CleanupStack::PopAndDestroy(contactsDb);
    
    return num;    
#endif
    }

    qWarning("No backend support in countContacts()");
    return 0;

}

void tst_Contact::cleanupTestCase()
{
  foreach(QContactManager *manager, m_managers) {
    clearContacts(manager);          
  }
  if(countContacts(0x0, Native))
    clearContacts(0x0, tst_Contact::Native);
  int num_end = countContacts(0x0, tst_Contact::Native);  
  if(num_end){
    qWarning() << "Ended with: " << num_end << " contacts, should be 0";
  }
}

void tst_Contact::createContact(QContactManager *manager, enum platform platform)
{
  if(platform == tst_Contact::QContacts) {    
    QContact *c = new QContact;
    c->setType(QContactType::TypeContact);
    QContactName cname;
    QString name;
    name = firstNames.takeFirst();
    firstNames.push_back(name);
    cname.setFirstName(name);
    name = lastNames.takeFirst();
    lastNames.push_back(name);
    cname.setLastName(name);
#ifndef Q_WS_MAEMO_5
    cname.setPrefix("Mr");
#endif
    c->saveDetail(&cname);

    if(!manager->saveContact(c)){
      qDebug() << "Failed to create contact during setup";
      return;
    }
    id_list.append(c->localId());
    delete c;
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    // Create a contact card and add a work phone number. Numeric values are 
    // stored in a text field (storage type = KStorageTypeText).
    
    CContactDatabase* db = CContactDatabase::OpenL();
    CleanupStack::PushL(db);

    CContactCard* newCard = CContactCard::NewLC();    
    
    QString name;
    
    // Create the firstName field and add the data to it
    name = firstNames.takeFirst();
    firstNames.push_back(name);        
    CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    TPtrC Firstname(reinterpret_cast<const TUint16*>(name.utf16()));
    firstName->TextStorage()->SetTextL(Firstname);      
    newCard->AddFieldL(*firstName);
    CleanupStack::Pop(firstName);

    // Create the lastName field and add the data to it
    name = lastNames.takeFirst();
    lastNames.push_back(name);        
    CContactItemField* lastName= CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
    TPtrC Lastname(reinterpret_cast<const TUint16*>(name.utf16()));
    lastName->TextStorage()->SetTextL(Lastname);
    newCard->AddFieldL(*lastName);
    CleanupStack::Pop(lastName);

    CContactItemField* prefix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPrefixName);
    _LIT(KPrefix, "Mr");
    prefix->TextStorage()->SetTextL(KPrefix);
    newCard->AddFieldL(*prefix);
    CleanupStack::Pop(prefix);

    // Add newCard to the database
    const TContactItemId contactId = db->AddNewContactL(*newCard);
    db->CloseContactL(contactId);    
    
    id_list.append(contactId);
    CleanupStack::PopAndDestroy(2, db); //newCard, contactsDb
#else
    qWarning("No native test for createContact");
#endif 
  }
}

void tst_Contact::clearContacts(QContactManager *manager, enum platform platform)
{
  if(platform == tst_Contact::QContacts) {
    QMap<int, QContactManager::Error> errorMap;
    manager->removeContacts(id_list, &errorMap);
    id_list.clear();
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);
    
    CCntFilter *filter = CCntFilter::NewL();
    CleanupStack::PushL(filter);
    //get all contact items (no groups, templates...)
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    TRAPD(err, contactDb->FilterDatabaseL(*filter));
    if(err != KErrNone){
      qWarning() << "clearContacts failed to get any contacts error: " << err;
      CleanupStack::PopAndDestroy(2, contactDb);
      return;
    }
    
    CContactIdArray *iContacts = 0;
    TRAP(err, iContacts = CContactIdArray::NewL(filter->iIds));
    if(err != KErrNone){      
      CleanupStack::PopAndDestroy(2, contactDb); //idArray, contactsDb
      return;
    }
    CleanupStack::PushL(iContacts);
    
    TRAP(err, contactDb->DeleteContactsL(*iContacts));
    if(err != KErrNone)
      qWarning() << "DeleteContacts failed with: " << err;
    
    CleanupStack::PopAndDestroy(3, contactDb); //idArray, contactsDb
#endif
  }

}

void tst_Contact::tst_createTime_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}

void tst_Contact::tst_createTime()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);
  
  if(platform == tst_Contact::QContacts){
    
    QContactManager *qm = 0x0;

    QBENCHMARK {
        qm = new QContactManager(manager->managerName());
    }
    delete qm;
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    CContactDatabase* db = 0x0;
    QBENCHMARK {
      db = CContactDatabase::OpenL();     
    }
    CleanupStack::PushL(db);
    CleanupStack::PopAndDestroy(db); //db
#endif
  }
}

void tst_Contact::tst_fetchAllContactIds_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}


void tst_Contact::tst_fetchAllContactIds()
{   
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);

  if(platform == tst_Contact::QContacts) {
    QList<QContactLocalId> ql;    
    
    QBENCHMARK {
        ql = manager->contactIds();
    }        
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactsDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactsDb);    

    CCntFilter *filter = CCntFilter::NewLC();
    
    //get all contact items (no groups, templates...)
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    QBENCHMARK {
      contactsDb->FilterDatabaseL(*filter);
      CContactIdArray *iContacts = CContactIdArray::NewLC(filter->iIds);
      CleanupStack::PopAndDestroy(iContacts);
    }
    CleanupStack::PopAndDestroy(2, contactsDb); //iContacts, filter, contactsDb    
#endif 
  }
}

void tst_Contact::tst_fetchOneContact_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}

void tst_Contact::tst_fetchOneContact()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);

  if(platform == tst_Contact::QContacts){
    QContact c;    

    m_run++;

#if defined(Q_WS_MAEMO_6)
    int ret;   
    QContactFetchRequest* req = new QContactFetchRequest;

    QList<QContactLocalId> qcl = manager->contactIds();
    if(qcl.count() < 1)
        QFAIL("No contacts to pull from tracker");
    QList<QContactLocalId> one;
    one += qcl.takeFirst();
    QContactLocalIdFilter idFil;
    idFil.setIds(one);
    req->setFilter(idFil);

    req->setManager(manager);    
    //connect(req, SIGNAL(progress(QContactFetchRequest*, bool)), this, SLOT(gotContact(QContactFetchRequest*,bool)));
    //connect(req, SIGNAL(stateChanged(QContactAbstractRequest::State)), this, SLOT(stateChanged(QContactAbstractRequest::State)));
    connect(req, SIGNAL(resultsAvailable()), this, SLOT(resultsAvailable()));

    m_num_contacts = 1;
    m_timer->start(1000);

    QBENCHMARK {
        req->start();
        ret = loop->exec();
    }
    m_timer->stop();

    //qDebug() << "Got Contact: " << qm->synthesizeDisplayLabel(c);
    if(ret){
        QFAIL("Failed to load one contact");
    }
    delete req;

#elif defined(Q_OS_SYMBIAN)    
    QList<QContactLocalId> qcl = manager->contactIds();    
    if(qcl.count() < 1)
        QFAIL("No contacts to pull from tracker");
    
    QBENCHMARK {
       c = manager->contact(qcl.first());
    }
#endif
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);       
    CContactItem *item = 0;
    TInt r;
    
    CCntFilter *filter = CCntFilter::NewL();
    CleanupStack::PushL(filter);

    //get all contact items (no groups, templates...)
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    contactDb->FilterDatabaseL(*filter);
    CContactIdArray *iContacts = CContactIdArray::NewL(filter->iIds);
    CleanupStack::PushL(iContacts);
    int cnt = iContacts->Count();
    int i = 0;
               
    QBENCHMARK {      
      TRAP(r, item = contactDb->ReadContactL((*iContacts)[i++]));
      if(r == KErrNone)
        delete item;
      if(i >= cnt)
        i = 0;
    }      
    
    if(r != KErrNone){ qWarning() << "Error by OpenContactL: " << r; }
            
    CleanupStack::PopAndDestroy(3, contactDb); //contact, filter, IdArray    

#endif
  }                
}


void tst_Contact::tst_fetchTenContact_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}

void tst_Contact::tst_fetchTenContact()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);

  if(platform == tst_Contact::QContacts){
    QContact c;
    m_run++;

#if defined(Q_WS_MAEMO_6)
    int ret;

    QContactFetchRequest* req = new QContactFetchRequest;

    QList<QContactLocalId> qcl = manager->contactIds();
    if(qcl.count() < 10){
        QFAIL("No enough contacts to get 10");
    }
    QList<QContactLocalId> one;
    for(int i = 0; i<10; i++)
        one += qcl.takeFirst();
    m_num_contacts = 10;

    QContactLocalIdFilter idFil;
    idFil.setIds(one);
    req->setFilter(idFil);

    req->setManager(manager);

    //    connect(req, SIGNAL(progress(QContactFetchRequest*, bool)), this, SLOT(gotContact(QContactFetchRequest*,bool)));
    connect(req, SIGNAL(resultsAvailable()), this, SLOT(resultsAvailable()));

    m_timer->start(1000);

    QBENCHMARK {
        req->start();
        ret = loop->exec();
    }
    m_timer->stop();

    //qDebug() << "Got Contact: " << qm->synthesizeDisplayLabel(c);
    if(ret){
        QFAIL("Failed to load one contact");
    }

    delete req;

#elif defined(Q_OS_SYMBIAN)
    QList<QContactLocalId> qcl = manager->contactIds();
    if(qcl.count() < 10){
        QFAIL("No enough contacts to get 10");
    }
    QList<QContactLocalId> one;
    for(int i = 0; i<10; i++)
        one += qcl.takeFirst();

    QContactLocalIdFilter idFil;
    idFil.setIds(one);
    
    QList<QContact> qlc;
    
    QBENCHMARK {
      qlc = manager->contacts(idFil, QList<QContactSortOrder>(), QContactFetchHint());
    }
    
    if(qlc.count() != 10){
      QFAIL("Did not get 10 contacts back");    
    }
    
#endif    
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);

    CCntFilter *filter = CCntFilter::NewL();
    CleanupStack::PushL(filter);

    //get all contact items (no groups, templates...)
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    contactDb->FilterDatabaseL(*filter);
    CContactIdArray *iContacts = CContactIdArray::NewL(filter->iIds);
    CleanupStack::PushL(iContacts);
    if(iContacts->Count() <= 10){
      QFAIL("There are less than 10 contacts to fetch");
    }
    
    TInt r;
    int i = 0;
    
    CContactItem *item = 0;

    // there is no multiple fetch in symbian
    
    
    QBENCHMARK {
      for(i = 0; i < 10; i++){
        TRAP(r, item = contactDb->ReadContactL((*iContacts)[i]));
        if(r == KErrNone)
          delete item;
      }
    }
    
    CleanupStack::PopAndDestroy(3, contactDb); //iContacts, filer, contactsDb    
#endif
  }
}

void tst_Contact::timeout()
{    
    qDebug() << "***** Timeout, haven't received the signal/contact within 1sec";
    loop->exit(1); // timeout, fail test

}

void tst_Contact::gotContact(QContactFetchRequest *request, bool appendOnly)
{
    Q_UNUSED(appendOnly);

    // first, check to make sure that the request is still valid.
    if (request->state() == QContactAbstractRequest::CanceledState) {
        delete request;
        QWARN("Contact request canceled");
        loop->exit(1);
        return; // ignore these results.
    }

    if(request->contacts().count() > 0) {        
        m_num_contacts -= request->contacts().count();
        if(m_num_contacts <= 0)
            loop->exit(0);
        return; // got one or more
    }

    // check to see if the request status is "finished" - clean up.
    if (request->state() == QContactAbstractRequest::FinishedState) {
        delete request;        
    }

}

void tst_Contact::resultsAvailable()
{

    QContactFetchRequest *req = qobject_cast<QContactFetchRequest *>(sender());
    if(req){
        //qDebug() << m_run << " Got resultsAvailable: " << req->contacts().count() << " need: " << m_num_contacts;
        if(!req->contacts().empty()) {
            m_num_contacts -= req->contacts().count();
            if(m_num_contacts <= 0)
                loop->exit(0);
            return; // got one or more
        }
    }

}

void tst_Contact::resultsAvailableFilter()
{

    QContactFetchRequest *req = qobject_cast<QContactFetchRequest *>(sender());
    if(req){
        if(!req->contacts().empty()) { // we got enough certainly...don't know how many are coming back with the filter
            loop->exit(0);
            return; // got one or more
        }
    }
}

void tst_Contact::stateChanged(QContactAbstractRequest::State /*newState*/)
{
    qDebug() << "Got state change";
}

void tst_Contact::tst_createContact_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}

void tst_Contact::tst_createContact()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);

  QBENCHMARK {
        createContact(manager, platform);
  }
}

void tst_Contact::tst_saveContact_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}

void tst_Contact::tst_saveContact()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);

  if(platform == tst_Contact::QContacts) {    
    QContact *c = new QContact;
    c->setType("Contact");
    QContactName cname;
    QString name;
    name = firstNames.takeFirst();
    firstNames.push_back(name);
    cname.setFirstName(name);
    name = lastNames.takeFirst();
    lastNames.push_back(name);
    cname.setLastName(name);
#ifndef Q_WS_MAEMO_5
    cname.setPrefix("Mr");
#endif
    c->saveDetail(&cname);

    int ret = 0; 
    
    QBENCHMARK {
      ret = manager->saveContact(c);
    }
    if(!ret){
      qDebug() << "Failed to create contact durring setup";
      return;
    }
    id_list.append(c->localId());
    delete c;
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    // Create a contact card and add a work phone number. Numeric values are 
    // stored in a text field (storage type = KStorageTypeText).
    
    CContactDatabase* db = CContactDatabase::OpenL();
    CleanupStack::PushL(db);

    CContactCard* newCard = CContactCard::NewLC();    
    
    QString name;
    
    // Create the firstName field and add the data to it
    name = firstNames.takeFirst();
    firstNames.push_back(name);        
    CContactItemField* firstName = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldGivenName);
    TPtrC Firstname(reinterpret_cast<const TUint16*>(name.utf16()));
    firstName->TextStorage()->SetTextL(Firstname);      
    newCard->AddFieldL(*firstName);
    CleanupStack::Pop(firstName);

    // Create the lastName field and add the data to it
    name = lastNames.takeFirst();
    lastNames.push_back(name);        
    CContactItemField* lastName= CContactItemField::NewLC(KStorageTypeText, KUidContactFieldFamilyName);
    TPtrC Lastname(reinterpret_cast<const TUint16*>(name.utf16()));
    lastName->TextStorage()->SetTextL(Lastname);
    newCard->AddFieldL(*lastName);
    CleanupStack::Pop(lastName);

    CContactItemField* prefix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPrefixName);
    _LIT(KPrefix, "Mr");
    prefix->TextStorage()->SetTextL(KPrefix);
    newCard->AddFieldL(*prefix);
    CleanupStack::Pop(prefix);

    QBENCHMARK {
      // Add newCard to the database
      const TContactItemId contactId = db->AddNewContactL(*newCard);
      db->CloseContactL(contactId);
      id_list.append(contactId);      
    }
        
    CleanupStack::PopAndDestroy(2, db); //newCard, contactsDb
#else
    qWarning("No native test defined for this platform and tst_saveContact()");
#endif 
  }
}

void tst_Contact::tst_nameFilter_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}


void tst_Contact::tst_nameFilter()
{ 
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);
 
  if(platform == tst_Contact::QContacts){
    QContactFilter fil = QContactName::match(firstNames.first(),""); // pick one first name to find
    //QContactFilter fil = QContactName::match("sdfsdfsdfjhsjkdfshdkf", ""); // pick one first name to find
    QContact c;    

    m_run++;



#if defined(Q_WS_MAEMO_6)
    int ret;
    QContactFetchRequest* req = new QContactFetchRequest;
    req->setFilter(fil);
    req->setManager(manager);

    connect(req, SIGNAL(resultsAvailable()), this, SLOT(resultsAvailableFilter()));

    m_timer->start(1000);

    QBENCHMARK {
        req->start();
        ret = loop->exec();
    }
    m_timer->stop();

    //qDebug() << "Got Contact: " << qm->synthesizeDisplayLabel(c);
    if(ret){
        QFAIL("Failed to load one contact");
    }

//    QList<QContact> qcl = req->contacts();
//    while(!qcl.isEmpty()){
//        QContact c = qcl.takeFirst();
//        qDebug() << "Contact: " << c.displayLabel();
//    }
    delete req;
    
#elif defined(Q_OS_SYMBIAN)
    QList<QContact> qlc;
    
    QBENCHMARK {
      qlc = manager->contacts(fil, QList<QContactSortOrder>(), QContactFetchHint());
    }
    
//    while(!qlc.isEmpty()){
//        QContact c = qlc.takeFirst();
//        qDebug() << "Contact: " << c.displayLabel();
//    }
#endif
  }
  else if(platform == tst_Contact::Native){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);
       
    CContactItem *item = 0x0;

    CCntFilter *filter = CCntFilter::NewL();
    CleanupStack::PushL(filter);

    //get all contact items (no groups, templates...)
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    contactDb->FilterDatabaseL(*filter);
    CContactIdArray *iContacts;
    TRAPD(filter_err, iContacts = CContactIdArray::NewL(filter->iIds));
    if(filter_err != KErrNone){
      CleanupStack::PopAndDestroy(2, contactDb); // filter, contactDB
      QFAIL("Failed to fetch a list of contacts");
    }
    CleanupStack::PushL(iContacts);
    
    TBuf<100> Firstname(0);
    
    for(int i = 0; i< iContacts->Count(); i++){
      TRAPD(err, item = contactDb->ReadContactL((*iContacts)[0]));
      if(err != KErrNone) {
        CleanupStack::PopAndDestroy(3, contactDb); // iContacts, filers, db
        QFAIL("Unable to fetch a sample item from the contacts file");
      }
      CleanupStack::PushL(item);
      
      int fieldNum = item->CardFields().Find(KUidContactFieldGivenName);
      if(fieldNum != KErrNotFound){
        Firstname.Copy((item->CardFields())[fieldNum].TextStorage()->Text());
        CleanupStack::PopAndDestroy(item); // item
        break;
      }
      CleanupStack::PopAndDestroy(item); // item
    }
    if(Firstname.Length() == 0){
      QFAIL("Unable to find a contact to search for");
    }
      

    //const TPtrC Firstname(reinterpret_cast<const TUint16*>(firstNames.first().utf16()));
    CContactIdArray* idArray;
    
    CContactItemFieldDef* fieldDef = new (ELeave) CContactItemFieldDef();
    CleanupStack::PushL(fieldDef);
    
    fieldDef->AppendL( KUidContactFieldGivenName);
       
    QBENCHMARK {      
      idArray = contactDb->FindLC(Firstname, fieldDef);      
      if(idArray->Count() > 0) {
          item = contactDb->ReadContactL((*idArray)[0]);
          delete item;
          CleanupStack::PopAndDestroy(idArray);
      }
      else {
          qDebug() << "Number of contacts native: " << countContacts(0x0, Native);
          qDebug() << "Number of contacts qt/system: " << countContacts(m_systemManager);
          QFAIL("No contacts returned from CContactDatabase::FindLC");
      }
    }   
    
    CleanupStack::PopAndDestroy(4, contactDb); //item, idArray, fielddef, lock, contactsDb
#endif
  }
}

void tst_Contact::tst_removeOneContact_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}

void tst_Contact::tst_removeOneContact()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);
  
  if(platform == tst_Contact::QContacts){
    QList<QContactLocalId> one;
    QMap<int, QContactManager::Error> errorMap;

    if(id_list.count() < 1){ // incase we run out of contacts
      createContact(manager, platform);
    }
        
    one += id_list.takeFirst();
    
    QBENCHMARK_ONCE {
        manager->removeContacts(one, &errorMap);
    }    

  }
  else if(platform == tst_Contact::Native){    
#ifdef Q_OS_SYMBIAN
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);

    CCntFilter *filter = CCntFilter::NewL();
    CleanupStack::PushL(filter);

    //get all contact items (no groups, templates...)
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    contactDb->FilterDatabaseL(*filter);
    CContactIdArray *iContacts = CContactIdArray::NewL(filter->iIds);
    CleanupStack::PushL(iContacts);
    int i = 0;
     
    QBENCHMARK_ONCE { // Do it once only, to many removals becomes a problem
      contactDb->DeleteContactL((*iContacts)[i++]);
      if(i >= iContacts->Count()){
        break;
      }
    }
    
    CleanupStack::PopAndDestroy(3, contactDb); //idArray, contactsDb
#endif
  }
}

void tst_Contact::tst_removeAllContacts_data()
{
  QTest::addColumn<tst_Contact::platform>("platform");
  QTest::addColumn<QContactManager *>("manager");
  
  QTest::newRow("Native") << tst_Contact::Native << (QContactManager *)0x0;
  foreach(QContactManager *mgr, m_managers){
    QTest::newRow(QString("QContact-" + mgr->managerName()).toAscii()) << tst_Contact::QContacts << mgr; 
  }  
}


void tst_Contact::tst_removeAllContacts()
{
  QFETCH(tst_Contact::platform, platform);
  QFETCH(QContactManager *, manager);

  int before = countContacts(manager, platform);

  if(before < 20) {
    for(int i = before; i < 20; i++){
      createContact(manager, platform);
    }
  }  

  QBENCHMARK {
    clearContacts(manager, platform);
  }
}

int main(int argc, char **argv){

    QApplication app(argc, argv);

    tst_Contact test1;
    QTest::qExec(&test1, argc, argv);

}

#include "tst_bm_contacts.moc"

