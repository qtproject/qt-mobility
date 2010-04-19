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

//Q_DECLARE_METATYPE(QSystemInfo::Version);
//Q_DECLARE_METATYPE(QSystemInfo::Feature);

class tst_Contact : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void tst_createTime();

    void tst_fetchAllContactIds();
    void tst_fetchOneContact();
    void tst_fetchTenContact();
    //void tst_fetchAllContact();

    void tst_createContact();    
    void tst_saveContact();

    void tst_nameFilter();

    void tst_removeOneContact();
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
    void setBackend(QString);

private:
    void createContact();
    void clearContacts();
    int countContacts();

    enum {
      BackendQContacts,
      BackendContactsModel
    } m_backend;
    QString manager;
    QEventLoop *loop;
    QContactManager *m_qm;
    int m_num_contacts;
    QList<QContactLocalId> id_list;

    int m_run;
    
    int m_num_start;

    QTimer *m_timer;
    QStringList firstNames;
    QStringList lastNames;    

};

void tst_Contact::setBackend(QString backend)
{
    manager = backend;
    if(manager == "SymbianContactsModel") // Only one at the moment
      m_backend = BackendContactsModel;
    else
      m_backend = BackendQContacts;
    qWarning() << "Backend set to: " << manager;
}

void tst_Contact::initTestCase()
{
    qDebug() << "Managers: " << QContactManager::availableManagers();    
    m_run = 0;

#if defined(Q_WS_MAEMO_6)
    QStringList list = QContactManager::availableManagers();
    int found = 0;
    while(!list.empty()){
        if(list.takeFirst() == "tracker"){
            found = 1;
            break;
        }
    }
    if(!found)
        QFAIL("Unable to find Maemo 6 tracker plugin. Please check install");

    if(manager.isEmpty())
        manager = "memory";
    m_qm = new QContactManager(manager);
#elif defined(Q_WS_MAEMO_5)
    if(manager.isEmpty()){
        //Looking for a manager
        QStringList list = QContactManager::availableManagers();
        if (list.contains("maemo5")){
            manager = "maemo5";
        } else {
            QFAIL("Unable to find Maemo 5 plugin. Please check install");
        }
    }
    m_qm = new QContactManager(manager);
    
#elif defined(Q_OS_SYMBIAN)
    if(m_backend != BackendContactsModel) {
      QStringList list = QContactManager::availableManagers();
      int found = 0;
      while(!list.empty()){
        if(list.takeFirst() == "symbian"){
          found = 1;
          break;
        }
      }
      if(!found) {
        QFAIL("Unable to find Symbian plugin. Please check install");
      }

      if(manager.isEmpty()) {
        manager = "symbian";
      }    
      m_qm = new QContactManager(manager);
    }
    else {
      m_qm = 0x0;
    }
#else
    if(manager != "memory"){
      QFAIL("Platform no supported");      
    }
    else {
      m_qm = new QContactManager(manager);      
    }
#endif


    // setup an event loop for waiting
    loop = new QEventLoop;

    firstNames << "Anahera" << "Anaru" << "Hemi" << "Hine" << "Kiri" << "Maata" << "Mere" << "Moana" << "Paora" << "Petera" << "Piripi" << "Ruiha" << "Tane" << "Whetu";
    lastNames << "Ati Awa" << "Kai Taho" << "Moriori" << "Muaupoko" << "Nga Rauru" << "Taranaki" << "Opotoki" << "Aotea" << "Taninui" << "Tuhourangi" << "Tainui" << "Waitaha";
    
    m_num_start = countContacts();
    qDebug() << "Number of Contact: " << m_num_start;


    for(int i = 0; i < 20; i++){
        createContact();
    }
    
    int after = countContacts();
    if(after - m_num_start != 20){
        qWarning() << "Failed to create 20 contacts";
    }

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

}

int tst_Contact::countContacts()
{  
    if(m_backend == BackendQContacts) {
        QList<QContactLocalId> qcl = m_qm->contactIds();
        return qcl.count();
    } else if(m_backend == BackendContactsModel){
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
    clearContacts();
    int num_end = countContacts();          
    if(m_num_start != num_end){
      QFAIL(QString("Number of contacts ending: %2 is different that starting number %1.  Poor cleanup").arg(m_num_start).arg(num_end).toAscii());
    }
}

void tst_Contact::clearContacts()
{
  if(m_backend == BackendQContacts) {
    QMap<int, QContactManager::Error> errorMap;
    m_qm->removeContacts(id_list, &errorMap);
    id_list.clear();
  }
  else if(m_backend == BackendContactsModel){
#ifdef Q_OS_SYMBIAN
    CContactDatabase* db = CContactDatabase::OpenL();
    CleanupStack::PushL(db);

    CContactIdArray* idArray = CContactIdArray::NewLC();
    while(!id_list.isEmpty())
      idArray->AddL(id_list.takeFirst());
    db->DeleteContactsL(*idArray);    
    
    CleanupStack::PopAndDestroy(2); //idArray, contactsDb
#endif
  }

}

void tst_Contact::tst_createTime()
{
  
  if(m_backend == BackendQContacts){
    QContactManager *qm = 0x0;

    QBENCHMARK {
        qm = new QContactManager(manager);
    }

    delete qm;
  }
  else if(m_backend == BackendContactsModel){
#ifdef Q_OS_SYMBIAN
    CContactDatabase* db = 0x0;
    QBENCHMARK {
      db = CContactDatabase::OpenL();     
    }
    CleanupStack::PushL(db);
    CleanupStack::PopAndDestroy(1); //db
#endif
  }
}

void tst_Contact::tst_fetchAllContactIds()
{    
  if(m_backend == BackendQContacts) {
    QList<QContactLocalId> ql;
    QBENCHMARK {
        ql = m_qm->contactIds();
    }        
  }
  else if(m_backend == BackendContactsModel){
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
    }
    
    CleanupStack::PopAndDestroy(3); //iContacts, filter, contactsDb    
#endif 
  }
}

void tst_Contact::tst_fetchOneContact()
{
  if(m_backend == BackendQContacts){
    QContact c;    

    m_run++;

#if defined(Q_WS_MAEMO_6)
    int ret;   
    QContactFetchRequest* req = new QContactFetchRequest;

    QList<QContactLocalId> qcl = m_qm->contactIds();
    if(qcl.count() < 1)
        QFAIL("No contacts to pull from tracker");
    QList<QContactLocalId> one;
    one += qcl.takeFirst();
    QContactLocalIdFilter idFil;
    idFil.setIds(one);
    req->setFilter(idFil);

    req->setManager(m_qm);    
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

//#elif defined(Q_OS_SYMBIAN)
#else
    QList<QContactLocalId> qcl = m_qm->contactIds();    
    if(qcl.count() < 1)
        QFAIL("No contacts to pull from tracker");
    
    QBENCHMARK {
       c = m_qm->contact(qcl.first());
    }
#endif
  }
  else if(m_backend == BackendContactsModel){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);
    
    int id = id_list.takeFirst();
    id_list.append(id);
    
    CContactItem *item = 0;
    TInt r;
        
    QBENCHMARK {
      TRAP(r, item = contactDb->ReadContactL(id));
    }
    CleanupStack::PushL(item);
    
    if(r != KErrNone){ qWarning() << "Error by OpenContactL: " << r; }
        
//    TRAP(r, contactDb->CloseContactL(id));
//    if(r != KErrNone){qWarning() << "Error by CloseContactL: " << r; }
    
    //qDebug() << "Call FetchContactDone: " << id;        
    
    CleanupStack::PopAndDestroy(2); //contact, lock, contactsDb    

#endif
  }                
}



void tst_Contact::tst_fetchTenContact()
{
  if(m_backend == BackendQContacts){
    QContact c;
    m_run++;

#if defined(Q_WS_MAEMO_6)
    int ret;

    QContactFetchRequest* req = new QContactFetchRequest;

    QList<QContactLocalId> qcl = m_qm->contactIds();
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

    req->setManager(m_qm);

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

//#elif defined(Q_OS_SYMBIAN)
#else
    QList<QContactLocalId> qcl = m_qm->contactIds();
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
      qlc = m_qm->contacts(idFil, QList<QContactSortOrder>(), QContactFetchHint());
    }
    
    if(qlc.count() != 10){
      QFAIL("Did not get 10 contacts back");    
    }
    
#endif    
  }
  else if(m_backend == BackendContactsModel){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);
    
    int id = id_list.takeFirst();
    id_list.append(id);
    
    TInt r;
    
    CContactItem *item1 = 0;
    CContactItem *item2 = 0;
    CContactItem *item3 = 0;
    CContactItem *item4 = 0;
    CContactItem *item5 = 0;
    CContactItem *item6 = 0;
    CContactItem *item7 = 0;
    CContactItem *item8 = 0;
    CContactItem *item9 = 0;
    CContactItem *item10 = 0;
        
    QBENCHMARK {
      TRAP(r, item1 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item2 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item3 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item4 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item5 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item6 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item7 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item8 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item9 = contactDb->ReadContactL(id));
      id = id_list.takeFirst();
      id_list.append(id);
      TRAP(r, item10 = contactDb->ReadContactL(id));
    }
    CleanupStack::PushL(item1);
    CleanupStack::PushL(item2);
    CleanupStack::PushL(item3);
    CleanupStack::PushL(item4);
    CleanupStack::PushL(item5);
    CleanupStack::PushL(item6);
    CleanupStack::PushL(item7);
    CleanupStack::PushL(item8);
    CleanupStack::PushL(item9);
    CleanupStack::PushL(item10);
     
    if(r != KErrNone){ qWarning() << "Error by OpenContactL: " << r; }
            
    CleanupStack::PopAndDestroy(11); //10*item + contactsDb    
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

void tst_Contact::tst_createContact()
{
    QBENCHMARK {
        createContact();
    }
}

void tst_Contact::tst_saveContact()
{
  if(m_backend == BackendQContacts) {    
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
      ret = m_qm->saveContact(c);
    }
    if(!ret){
      qDebug() << "Failed to create contact durring setup";
      return;
    }
    id_list.append(c->localId());
    delete c;
  }
  else if(m_backend == BackendContactsModel){
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
#ifndef Q_WS_MAEMO_5
    CContactItemField* prefix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPrefixName);
    _LIT(KPrefix, "Mr");
    prefix->TextStorage()->SetTextL(KPrefix);
    newCard->AddFieldL(*prefix);
    CleanupStack::Pop(prefix);
#endif
    QBENCHMARK {
      // Add newCard to the database
      const TContactItemId contactId = db->AddNewContactL(*newCard);
      db->CloseContactL(contactId);
      id_list.append(contactId);      
    }
        
    CleanupStack::PopAndDestroy(2); //newCard, contactsDb
#else
    qWarning("ContactModel set but Q_OS_SYMBIAN not set, this doesn't make sense");
#endif 
  }
}


void tst_Contact::createContact()
{
  if(m_backend == BackendQContacts) {    
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

    if(!m_qm->saveContact(c)){
      qDebug() << "Failed to create contact during setup";
      return;
    }
    id_list.append(c->localId());
    delete c;
  }
  else if(m_backend == BackendContactsModel){
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
#ifndef Q_WS_MAEMO_5
    CContactItemField* prefix = CContactItemField::NewLC(KStorageTypeText, KUidContactFieldPrefixName);
    _LIT(KPrefix, "Mr");
    prefix->TextStorage()->SetTextL(KPrefix);
    newCard->AddFieldL(*prefix);
    CleanupStack::Pop(prefix);
#endif
    // Add newCard to the database
    const TContactItemId contactId = db->AddNewContactL(*newCard);
    db->CloseContactL(contactId);    
    
    id_list.append(contactId);
    CleanupStack::PopAndDestroy(2); //newCard, contactsDb
#else
    qWarning("ContactModel set but Q_OS_SYMBIAN not set, this doesn't make sense");
#endif 
  }
}

void tst_Contact::tst_nameFilter()
{    
  if(m_backend == BackendQContacts){
    QContactFilter fil = QContactName::match(firstNames.first(),""); // pick one first name to find
    //QContactFilter fil = QContactName::match("sdfsdfsdfjhsjkdfshdkf", ""); // pick one first name to find
    QContact c;    

    m_run++;



#if defined(Q_WS_MAEMO_6)
    int ret;
    QContactFetchRequest* req = new QContactFetchRequest;
    req->setFilter(fil);
    req->setManager(m_qm);

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
      qlc = m_qm->contacts(fil, QList<QContactSortOrder>(), QContactFetchHint());
    }
    
//    while(!qlc.isEmpty()){
//        QContact c = qlc.takeFirst();
//        qDebug() << "Contact: " << c.displayLabel();
//    }
#endif
  }
  else if(m_backend == BackendContactsModel){
#ifdef Q_OS_SYMBIAN
    //open database
    // Open the default contact database
    CContactDatabase* contactDb = CContactDatabase::OpenL();
    CleanupStack::PushL(contactDb);
       
    CContactItem *item = 0x0;
            
    const TPtrC Firstname(reinterpret_cast<const TUint16*>(firstNames.first().utf16()));
    CContactIdArray* idArray;
    
    CContactItemFieldDef* fieldDef = new (ELeave) CContactItemFieldDef();
    CleanupStack::PushL(fieldDef);
    
    fieldDef->AppendL( KUidContactFieldGivenName);
       
    QBENCHMARK {      
      idArray = contactDb->FindLC(Firstname, fieldDef);      
      if(idArray->Count() > 0)
        item = contactDb->ReadContactL((*idArray)[0]);
      else
        QFAIL("No contacts returned from CContactDatabase::FindLC");
    }
    CleanupStack::PushL(item);    
    
    CleanupStack::PopAndDestroy(4); //item, idArray, fielddef, lock, contactsDb
#endif
  }
}

void tst_Contact::tst_removeOneContact()
{
  if(m_backend == BackendQContacts){
    QList<QContactLocalId> one;
    QMap<int, QContactManager::Error> errorMap;

    if(id_list.count() < 1){ // incase we run out of contacts
      createContact();
    }
    
    one += id_list.takeFirst();
    QBENCHMARK {
        m_qm->removeContacts(one, &errorMap);
    }

  }
  else if(m_backend == BackendContactsModel){    
#ifdef Q_OS_SYMBIAN
    CContactDatabase* db = CContactDatabase::OpenL();
    CleanupStack::PushL(db);    
        
    if(id_list.isEmpty())
      QFAIL("no contacts available to be removed for tst_removeOnContact()");
    
    
    TInt32 id = id_list.takeFirst();
    
    TInt err = 0;
    
    QBENCHMARK_ONCE {
      TRAP(err, db->DeleteContactL(id));    
    }
    
    if(err){      
      QFAIL("Failed to delete a contact");
    }
    
    CleanupStack::PopAndDestroy(1); //idArray, contactsDb
#endif
  }
}

void tst_Contact::tst_removeAllContacts()
{    
    int before = countContacts();

    if(before < 20) {
        for(int i = before; i < 20; i++){
            createContact();
        }
    }
    
    QBENCHMARK {
        clearContacts();
    }
    
}

int main(int argc, char **argv){

    QApplication app(argc, argv);

    tst_Contact test1;
    test1.setBackend("memory");
    QTest::qExec(&test1, argc, argv);

//    tst_Contact test2;
//    test2.setBackend("tracker");
//    QTest::qExec(&test2, argc, argv);
#if defined(Q_WS_MAEMO_5)
    tst_Contact test2;
    test2.setBackend("maemo5");
    QTest::qExec(&test2, argc, argv);
#endif
#if defined(Q_OS_SYMBIAN)   
    tst_Contact test2;
    test2.setBackend("symbian");
    QTest::qExec(&test2, argc, argv);
    
    tst_Contact test3;
    test3.setBackend("SymbianContactsModel");
    QTest::qExec(&test3, argc, argv);
#endif

}

#include "tst_bm_contacts.moc"

