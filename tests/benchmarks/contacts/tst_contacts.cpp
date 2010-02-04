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
#include <QContactManager>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

#include <qcontactabstractrequest.h>
#include <requests/qcontactfetchrequest.h>
#include <filters/qcontactlocalidfilter.h>
#include <details/qcontactdetails.h>


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

    QString manager;
    QEventLoop *loop;
    QContactManager *m_qm;
    int m_num_contacts;
    QList<QContactLocalId> id_list;

    int m_run;

    QTimer *m_timer;
    QStringList firstNames;
    QStringList lastNames;


};

void tst_Contact::setBackend(QString backend)
{
    manager = backend;
}

void tst_Contact::initTestCase()
{
    //qRegisterMetaType<QSystemInfo::Version>("QSystemInfo::Version");
    //qRegisterMetaType<QSystemInfo::Feature>("QSystemInfo::Feature");
    qDebug() << "Managers: " << QContactManager::availableManagers();
    manager = "";
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
    QFAIL("Maemo 5 does not support QContacts");
#else
    QFAIL("Platform not supported");
#endif


    // setup an event loop for waiting
    loop = new QEventLoop;

    firstNames << "Anahera" << "Anaru" << "Hemi" << "Hine" << "Kiri" << "Maata" << "Mere" << "Moana" << "Paora" << "Petera" << "Piripi" << "Ruiha" << "Tane" << "Whetu";
    lastNames << "Ati Awa" << "Kai Taho" << "Moriori" << "Muaupoko" << "Nga Rauru" << "Taranaki" << "Opotoki" << "Aotea" << "Taninui" << "Tuhourangi" << "Tainui" << "Waitaha";

    QList<QContactLocalId> qcl = m_qm->contactIds();
    int before = qcl.count();
    qDebug() << "Number of Contact: " << before;

    for(int i = 0; i < 20; i++){
        createContact();
    }
    qcl = m_qm->contactIds();
    int after = qcl.count();
    qDebug() << "Number of Contacts after: " << after;
    if(after - before != 20){
        qWarning() << "Failed to create 20 contacts";
    }

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

}

void tst_Contact::cleanupTestCase()
{
    clearContacts();
}

void tst_Contact::clearContacts()
{
    QMap<int, QContactManager::Error> errorMap;
    m_qm->removeContacts(&id_list, &errorMap);
    id_list.clear();
}

void tst_Contact::tst_createTime()
{    
    QContactManager *qm = 0x0;

    QBENCHMARK {
        qm = new QContactManager(manager);
    }

    delete qm;
}

void tst_Contact::tst_fetchAllContactIds()
{    
    QList<QContactLocalId> ql;
    QBENCHMARK {
        ql = m_qm->contactIds();
    }        
}

void tst_Contact::tst_fetchOneContact()
{
    QContact c;
    int ret;    

    m_run++;

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

#if defined(Q_WS_MAEMO_6)
    QBENCHMARK {
        req->start();
        ret = loop->exec();
    }
    m_timer->stop();

    //qDebug() << "Got Contact: " << qm->synthesizeDisplayLabel(c);
    if(ret){
        QFAIL("Failed to load one contact");
    }

#endif    
    delete req;

}



void tst_Contact::tst_fetchTenContact()
{
    QContact c;
    int ret;

    m_run++;

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

#if defined(Q_WS_MAEMO_6)
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

#endif    

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
    if (request->status() == QContactAbstractRequest::Cancelled) {
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
    if (request->status() == QContactAbstractRequest::Finished) {
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

void tst_Contact::stateChanged(QContactAbstractRequest::State newState)
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

    QContact *c = new QContact;
    c->setType("Contact");
    QContactName name;
    name.setFirstName("FirstName");
    name.setLastName("LastName");
    name.setPrefix("Mr");
    c->saveDetail(&name);

    QBENCHMARK {
        m_qm->saveContact(c);
    }
    id_list.append(c->localId());

    delete c;
}


void tst_Contact::createContact()
{

    static int id = 0;
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
    cname.setPrefix("Mr");
    c->saveDetail(&cname);

    if(!m_qm->saveContact(c)){
        qDebug() << "Failed to create contact durring setup";
        return;
    }
    id_list.append(c->localId());
    delete c;

}

void tst_Contact::tst_nameFilter()
{    
    QContactFilter fil = QContactName::match(firstNames.first(),""); // pick one first name to find
    //QContactFilter fil = QContactName::match("sdfsdfsdfjhsjkdfshdkf", ""); // pick one first name to find
    QContact c;
    int ret;

    m_run++;

    QContactFetchRequest* req = new QContactFetchRequest;
    req->setFilter(fil);
    req->setManager(m_qm);

    connect(req, SIGNAL(resultsAvailable()), this, SLOT(resultsAvailableFilter()));

    m_timer->start(1000);

#if defined(Q_WS_MAEMO_6)
    QBENCHMARK {
        req->start();
        ret = loop->exec();
    }
    m_timer->stop();

    //qDebug() << "Got Contact: " << qm->synthesizeDisplayLabel(c);
    if(ret){
        QFAIL("Failed to load one contact");
    }

    QList<QContact> qcl = req->contacts();
    while(!qcl.isEmpty()){
        QContact c = qcl.takeFirst();
        qDebug() << "Contact: " << c.displayLabel();
    }

#endif
    delete req;

}

void tst_Contact::tst_removeOneContact()
{

    QList<QContactLocalId> one;
    QMap<int, QContactManager::Error> errorMap;

    one += id_list.takeFirst();
    QBENCHMARK {
        m_qm->removeContacts(&id_list, &errorMap);
    }


}

void tst_Contact::tst_removeAllContacts()
{
    QList<QContactLocalId> qcl = m_qm->contactIds();
    int before = qcl.count();

    if(before < 20) {
        for(int i = before; i < 20; i++){
            createContact();
        }
    }
    QBENCHMARK {
        clearContacts();
    }
}

//void tst_Contact::tst_currentLanguage()
//{
//    QSystemInfo si;
//    QBENCHMARK {
//        QString s = si.currentLanguage();
//    }
//
//}
//
//void tst_Contact::tst_availableLanguages()
//{
//    QSystemInfo si;
//    QBENCHMARK {
//        QStringList available = si.availableLanguages();
//    }
//}
//
//void tst_Contact::tst_versions_data()
//{
//    QTest::addColumn<QSystemInfo::Version>("version");
//    QTest::addColumn<QString>("parameter");
//
//    QTest::newRow("Os") << QSystemInfo::Os << "";
//    QTest::newRow("QtCore") << QSystemInfo::QtCore << "";
//    QTest::newRow("Firmware") << QSystemInfo::Firmware << "";
//
//}
//
//void tst_Contact::tst_versions()
//{
//    QBENCHMARK {
//        QFETCH(QSystemInfo::Version, version);
//        QFETCH(QString, parameter);
//        QSystemInfo si;
//        QString vers = si.version(version, parameter);
//    }
//}
//
//void tst_Contact::tst_hasFeatures_data()
//{
//    QTest::addColumn<QSystemInfo::Feature>("feature");
//
//    QTest::newRow("Bluetooth") << QSystemInfo::BluetoothFeature;
//    QTest::newRow("CameraFeature") << QSystemInfo::CameraFeature;
//    QTest::newRow("FmradioFeature") << QSystemInfo::FmradioFeature;
//    QTest::newRow("IrFeature") << QSystemInfo::IrFeature;
//    QTest::newRow("LedFeature") << QSystemInfo::LedFeature;
//    QTest::newRow("MemcardFeature") << QSystemInfo::MemcardFeature;
//    QTest::newRow("UsbFeature") << QSystemInfo::UsbFeature;
//    QTest::newRow("VibFeature") << QSystemInfo::VibFeature;
//    QTest::newRow("WlanFeature") << QSystemInfo::WlanFeature;
//    QTest::newRow("SimFeature") << QSystemInfo::SimFeature;
//    QTest::newRow("LocationFeature") << QSystemInfo::LocationFeature;
//    QTest::newRow("VideoOutFeature") << QSystemInfo::VideoOutFeature;
//    QTest::newRow("HapticsFeature") << QSystemInfo::HapticsFeature;
//}
//
//void tst_Contact::tst_hasFeatures()
//{
//    QBENCHMARK {
//        QFETCH(QSystemInfo::Feature, feature);
//        QSystemInfo si;
//        si.hasFeatureSupported(feature);
//
//    }
//}


//QTEST_APPLESS_MAIN(tst_Contact)
//QTEST_MAIN(tst_Contact)

int main(int argc, char **argv){

    QApplication app(argc, argv);

    tst_Contact test1;
    test1.setBackend("memory");
    QTest::qExec(&test1, argc, argv);

//    tst_Contact test2;
//    test2.setBackend("tracker");
//    QTest::qExec(&test2, argc, argv);
}

#include "tst_contacts.moc"
