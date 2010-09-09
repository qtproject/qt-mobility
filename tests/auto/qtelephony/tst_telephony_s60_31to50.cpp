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

//TESTED_COMPONENT=src/telephony

#include <qmobilityglobal.h>
#include <QtTest/QtTest>
#include <QtCore>
#include <QDebug>

#include <qtelephony.h>
#include <tst_telephony_moc_s60_31to50.h>

#include "../qsfwtestutil.h"
//#define private public
#include "qtelephonycalllist.h"
#include "../../../src/telephony/qtelephonycalllist_symbian.cpp"

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

_LIT(KVoiceCallName, "test voice call");
_LIT(KDataCallName, "test data call");
_LIT(KDialledParty, "12345");

//GLOBAL
TPtrC16 mock_callname(KVoiceCallName);
TPtrC16 mock_dialledparty(KDialledParty);
TUint mock_istatus = KErrNone;
TUint mock_linestatus; 
TUint mock_calltype;

QTM_BEGIN_NAMESPACE

class tst_TelefonyCallList : public QTelephonyCallList
{
    Q_OBJECT
public:
    tst_TelefonyCallList() : QTelephonyCallList(), pcallInfoPrivate(0)
    {
        this->connect(this, SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo&)), this, SLOT(activeCallStatusChangedSlot(const QTelephonyCallInfo&)));
        this->connect(this, SIGNAL(activeCallRemoved(const QTelephonyCallInfo&)), this, SLOT(activeCallRemovedSlot(const QTelephonyCallInfo&)));
        this->connect(this, SIGNAL(activeCallAdded(const QTelephonyCallInfo&)), this, SLOT(activeCallAddedSlot(const QTelephonyCallInfo&)));
    }
    QTelephonyCallListPrivate* getPrivateTelefonyList() { return d; }
    void connectNotify ( const char* /*signal*/ ){
    }
private slots:
    void activeCallStatusChangedSlot(const QTelephonyCallInfo& call)
    {
        Q_UNUSED(call);
        if(values.contains("activeCallStatusChangedSlot_RefCount"))
            values.remove("activeCallStatusChangedSlot_RefCount");
        if(pcallInfoPrivate)
            values.insert("activeCallStatusChangedSlot_RefCount", QVariant(pcallInfoPrivate->ref));
    }
    void activeCallRemovedSlot(const QTelephonyCallInfo& call)
    {
        Q_UNUSED(call);
        if(values.contains("activeCallRemovedSlot_RefCount"))
            values.remove("activeCallRemovedSlot_RefCount");
        if(pcallInfoPrivate)
            values.insert("activeCallRemovedSlot_RefCount", QVariant(pcallInfoPrivate->ref));
    }
    void activeCallAddedSlot(const QTelephonyCallInfo& call)
    {
        Q_UNUSED(call);
        if(values.contains("activeCallAddedSlot_RefCount"))
            values.remove("activeCallAddedSlot_RefCount");
        if(pcallInfoPrivate)
            values.insert("activeCallAddedSlot_RefCount", QVariant(pcallInfoPrivate->ref));
    }
public:
    QHash<QString, QVariant> values;
    QTelephonyCallInfoPrivate* pcallInfoPrivate;
};

class tst_Telephony: public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    //Test cases
    void test_sharedPointer();
    void test_activeCalls();
    void test_statusChanges();
        
private:
    tst_TelefonyCallList* pCallList;
};

void tst_Telephony::init()
{
    qDebug("called before everything else");
    mock_linestatus = QTelephony::Idle;
    pCallList = new tst_TelefonyCallList();
}

//Test cases
void tst_Telephony::test_sharedPointer()
{
    QTelephonyCallListPrivate* p = pCallList->getPrivateTelefonyList();
    QVERIFY(p != 0);
    pCallList->pcallInfoPrivate = new QTelephonyCallInfoPrivate();

    //Adding to list should increment the ref counter to 1
    p->callInfoList.append(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(pCallList->pcallInfoPrivate));
    QVERIFY(pCallList->pcallInfoPrivate->ref == 1);

    int refcount = -1;

    //Now emit the activeCallAdded signal this should increment the ref counter to 2
    qDebug("emit ActiveCallAdded");
    p->emitActiveCallAdded(*pCallList->pcallInfoPrivate);
    if(pCallList->values.contains("activeCallAddedSlot_RefCount")){
        QVariant var = pCallList->values.value("activeCallAddedSlot_RefCount");
        refcount = var.toInt();
    }
    QVERIFY(refcount == 2);
    qDebug("- ref counting OK");

    //Now emit the activeCallStatusChanged signal this should increment the ref counter to 2
    qDebug("emit ActiveCallStatusChanged");
    p->emitActiveCallStatusChanged(*pCallList->pcallInfoPrivate);
    refcount = -1;
    if(pCallList->values.contains("activeCallStatusChangedSlot_RefCount")){
        QVariant var = pCallList->values.value("activeCallStatusChangedSlot_RefCount");
        refcount = var.toInt();
    }
    QVERIFY(refcount == 2);
    qDebug("- ref counting OK");

    //Now emit the activeCallRemoved signal this should increment the ref counter to 2
    qDebug("emit ActiveCallRemoved");
    p->emitActiveCallRemoved(*pCallList->pcallInfoPrivate);
    refcount = -1;
    if(pCallList->values.contains("activeCallRemovedSlot_RefCount")){
        QVariant var = pCallList->values.value("activeCallRemovedSlot_RefCount");
        refcount = var.toInt();
    }
    QVERIFY(refcount == 2);
    qDebug("- ref counting OK");

    //After emitint we should have ref counter == 1
    QVERIFY(pCallList->pcallInfoPrivate->ref == 1);
}

void tst_Telephony::test_activeCalls()
{
    QCOMPARE(pCallList->activeCalls().count(), 0);
    mock_linestatus = RMobileCall::EStatusConnected;
    mock_calltype = RMobilePhone::EVoiceService;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(pCallList->activeCalls().count(), 1);
    QCOMPARE(pCallList->activeCalls().at(0).remotePartyIdentifier(), 
             pCallList->getPrivateTelefonyList()->m_observers[0]->toQString(mock_dialledparty));
    mock_linestatus = RMobileCall::EStatusIdle;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(pCallList->activeCalls().count(), 0);
    mock_linestatus = RMobileCall::EStatusConnected;
    mock_callname.Set(KDataCallName);
    mock_calltype = RMobilePhone::EPacketDataService;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(pCallList->activeCalls().count(), 1);
    QCOMPARE(pCallList->activeCalls().at(0).type(), TUint(QTelephony::Video));
    mock_callname.Set(KVoiceCallName);
    mock_calltype = RMobilePhone::EVoiceService;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(pCallList->activeCalls().count(), 2);
    mock_linestatus = RMobileCall::EStatusIdle;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(pCallList->activeCalls().count(), 0);
    pCallList->getPrivateTelefonyList()->m_observers.clear();  
}

void tst_Telephony::test_statusChanges()
{
    qRegisterMetaType<QTelephonyCallInfo> ("QTelephonyCallInfo");
    QSignalSpy spy_add(pCallList, SIGNAL(activeCallAdded(QTelephonyCallInfo)));
    QSignalSpy spy_change(pCallList, SIGNAL(activeCallStatusChanged(QTelephonyCallInfo)));
    QSignalSpy spy_remove(pCallList, SIGNAL(activeCallRemoved(QTelephonyCallInfo)));
    QCOMPARE(pCallList->activeCalls().count(), 0);
    mock_linestatus = RMobileCall::EStatusDialling;
    mock_calltype = RMobilePhone::EVoiceService;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(spy_add.count(), 1);
    mock_linestatus = RMobileCall::EStatusAnswering;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();
    QCOMPARE(spy_change.count(), 1);
    mock_linestatus = RMobileCall::EStatusConnected;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();    
    QCOMPARE(spy_change.count(), 2);
    mock_linestatus = RMobileCall::EStatusDisconnecting;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();    
    QCOMPARE(spy_change.count(), 3);
    mock_linestatus = RMobileCall::EStatusIdle;
    pCallList->getPrivateTelefonyList()->m_observers[0]->m_callStatus = RMobileCall::TMobileCallStatus(mock_linestatus);
    pCallList->getPrivateTelefonyList()->m_observers[0]->updateCallList();    
    QCOMPARE(spy_change.count(), 4);
    QCOMPARE(spy_remove.count(), 1);
    QCOMPARE(pCallList->activeCalls().count(), 0);
    pCallList->getPrivateTelefonyList()->m_observers.clear();  
}

void tst_Telephony::cleanup()
{
    delete pCallList;
}

#include "tst_telephony_s60_31to50.moc"

QTM_END_NAMESPACE

QTM_USE_NAMESPACE
QTEST_MAIN(tst_Telephony)
