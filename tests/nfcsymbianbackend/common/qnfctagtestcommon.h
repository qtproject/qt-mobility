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
#ifndef QNFCTAGTESTCOMMON_H
#define QNFCTAGTESTCOMMON_H

#include <qnearfieldmanager.h>
#include <qnearfieldtarget.h>
#include <QtTest/QtTest>
#include "qnfctestcommon.h"
#include "qnfctestutil.h"
#include <qndefmessage.h>
#include <qndefnfctextrecord.h>
#include <qndefnfcurirecord.h>
#include <qnearfieldtagtype1.h>
#include <qnearfieldtagtype2.h>
#include <qnearfieldtagtype3.h>
#include <qnearfieldtagtype4.h>
#include "qdummyslot.h"

QTM_USE_NAMESPACE

template<typename TAG>
struct TagTrait
{
    static QNearFieldTarget::Type type() { return QNearFieldTarget::ProprietaryTag; }
    static QString info(){ return QString("unknow tag"); }
};

template<>
    struct TagTrait<QtMobility::QNearFieldTagType1>
{
    static QNearFieldTarget::Type type() { return QNearFieldTarget::NfcTagType1; }
    static QString info(){ return QString("tag type 1"); }
};

template<>
struct TagTrait<QtMobility::QNearFieldTagType2>
{
    static QNearFieldTarget::Type type() { return QNearFieldTarget::NfcTagType2; }
    static QString info(){ return QString("tag type 2"); }
};

template<>
struct TagTrait<QtMobility::QNearFieldTagType3>
{
    static QNearFieldTarget::Type type() { return QNearFieldTarget::NfcTagType3; }
    static QString info(){ return QString("tag type 3"); }
};

template<>
struct TagTrait<QtMobility::QNearFieldTagType4>
{
    static QNearFieldTarget::Type type() { return QNearFieldTarget::NfcTagType4; }
    static QString info(){ return QString("tag type 4"); }
};

template <typename TAG>
class QNfcTagTestCommon
{
public:
    QNfcTagTestCommon();
    ~QNfcTagTestCommon();
    void touchTarget();
    void removeTarget(); 
    
    // ndef access 
    void testNdefAccess();

    // raw command/commands
    void testRawCommand(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);


    void testWaitRawCommand(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);


    // mix ndef access and raw command 
    void testMixRawCommandAndNdefAccess(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);
    

    void testWaitMixRawCommandAndNdefAccess(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);

    // multiple wait
    void testWaitInSlot(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);

    // delete and remove tag before async request completed
    void testDeleteTargetBeforeAsyncRequestComplete(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);
    void testRemoveTargetBeforeAsyncRequestComplete(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet);
private:
    void GetSignalCount(const QVariantList& responseSet, int& errCount, int& okCount)
    {
        for(int i = 0; i < responseSet.count(); ++i)
        {
            if (responseSet.at(i).type() == QVariant::List)
            {
                QVariantList temp = responseSet.at(i).toList();
                bool isError = true;
                for(int j = 0; j < temp.count(); ++j)
                {
                    if (!temp.at(j).isValid())
                    {
                        isError = false;
                        break;
                    }
                }
                (isError) ? ++okCount : ++errCount;
            }
            else
            {
                (responseSet.at(i).isValid()) ? ++okCount : ++errCount;
            }
        } 
    }
public:
    QNearFieldManager manager;
    TAG* target;
};


Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QNearFieldTarget::Type)
Q_DECLARE_METATYPE(QNearFieldTarget::AccessMethod)

template<typename TAG>
QNfcTagTestCommon<TAG>::QNfcTagTestCommon()
{
    target = 0;
    qRegisterMetaType<QNdefMessage>("QNdefMessage");
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*"); 
    qRegisterMetaType<QNearFieldTarget::Error>("QNearFieldTarget::Error"); 
}


template<typename TAG>
QNfcTagTestCommon<TAG>::~QNfcTagTestCommon()
{
    if (target)
    {
        delete target;
    }
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::touchTarget()
{
    if (target)
    {
        delete target;
        target = 0;
    }
    QSignalSpy targetDetectedSpy(&manager, SIGNAL(targetDetected(QNearFieldTarget*)));
    QSignalSpy targetLostSpy(&manager, SIGNAL(targetLost(QNearFieldTarget*)));

    qDebug()<<"start detect tag type "<<TagTrait<TAG>::type()<<endl;
    manager.startTargetDetection(TagTrait<TAG>::type());

    QString hint("please touch ");
    hint += TagTrait<TAG>::info();
    hint += " with NDef Message inside";
    QNfcTestUtil::ShowMessage(hint);

    QTRY_VERIFY(!targetDetectedSpy.isEmpty()); 
    target = qobject_cast<TAG *>(targetDetectedSpy.at(targetDetectedSpy.count()-1).at(0).value<QNearFieldTarget *>());
    // make sure target can be detected
    QVERIFY(target);

    // make sure target uid is not empty
    QVERIFY(!target->uid().isEmpty());

    QCOMPARE(target->type(), TagTrait<TAG>::type());
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::removeTarget()
{
    QVERIFY(target);

    QSignalSpy targetLostSpy(&manager, SIGNAL(targetLost(QNearFieldTarget*)));
    QSignalSpy disconnectedSpy(target, SIGNAL(disconnected()));

    QNfcTestUtil::ShowMessage("please remove the tag");

    QTRY_VERIFY(!targetLostSpy.isEmpty());
    
    TAG *lostTarget = qobject_cast<TAG*>(targetLostSpy.first().at(0).value<QNearFieldTarget *>());

    QCOMPARE(target, lostTarget);

    QVERIFY(!disconnectedSpy.isEmpty());

    manager.stopTargetDetection(); 
}    

template<typename TAG>
void QNfcTagTestCommon<TAG>::testNdefAccess()
{
    touchTarget(); 

    QVERIFY(target->hasNdefMessage());

    target->readNdefMessages();
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage)));
    QSignalSpy ndefMessageWriteSpy(target, SIGNAL(ndefMessagesWritten()));

    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
    const QNdefMessage& ndefMessage(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());
    QVERIFY(ndefMessage.count()>0);

    ndefMessageReadSpy.clear();
    
    QNdefMessage message;
    
    QNdefNfcTextRecord textRecord;
    textRecord.setText(QLatin1String("nfc tag test"));
    
    message.append(textRecord);

    QNdefNfcUriRecord uriRecord;
    uriRecord.setUri(QUrl("http://qt.nokia.com"));
    message.append(uriRecord);

    /*QNdefRecord record;
    record.setTypeNameFormat(QNdefRecord::ExternalRtd);
    record.setType("com.nokia.qt:test");
    record.setPayload(QByteArray(2, quint8(0x55)));
    message.append(record);*/

    
    QList<QNdefMessage> messages;
    messages.append(message);
    
    target->writeNdefMessages(messages);
    QTRY_VERIFY(!ndefMessageWriteSpy.isEmpty());
    
    target->readNdefMessages();
    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
    
    const QNdefMessage& ndefMessage_new(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());
    QVERIFY(messages.at(0) == ndefMessage_new);

    removeTarget();
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::testRawCommand(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testRawCommand", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testRawCommand", "count mismatch");
    Q_ASSERT_X(discription.count() > 0, "testRawCommand", "list is empty");

    touchTarget();
    int okCount = 0;
    int errCount = 0;
    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    
    QList<QNearFieldTarget::RequestId> requests;

    GetSignalCount(responseSet, errCount, okCount);

    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
    }

    QTest::qWait(5000);
    qDebug()<<"signal count check"<<endl;
    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);

    qDebug()<<"response check"<<endl;
    for(int i = 0; i < requests.count(); ++i)
    {
        qDebug()<<"check "<<discription.at(i)<<" response"<<endl;
        QVERIFY(target->requestResponse(requests.at(i)) == responseSet.at(i));
    }

    removeTarget();
}


template<typename TAG>
void QNfcTagTestCommon<TAG>::testWaitRawCommand(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testWaitRawCommand", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testWaitRawCommand", "count mismatch");
    Q_ASSERT_X(discription.count() > 0, "testWaitRawCommand", "list is empty");
    
    touchTarget();
    int okCount = 0;
    int errCount = 0;
    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    
    QList<QNearFieldTarget::RequestId> requests;
    
    GetSignalCount(responseSet, errCount, okCount);
    
    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
    }
    
    // wait first request id.
    if (responseSet.first().isValid())
    {
        QVERIFY(target->waitForRequestCompleted(requests.first()));
    }
    else
    {
        QVERIFY(!(target->waitForRequestCompleted(requests.first())));
    }
    
    QVERIFY(!(target->waitForRequestCompleted(requests.first())));
    qDebug()<<"signal count check"<<endl;
    
    QTRY_COMPARE(errSpy.count(), errCount);
    qDebug()<<"get "<<errSpy.count()<<" err signal"<<endl;
    QTRY_COMPARE(okSpy.count(), okCount);
    qDebug()<<"get "<<okSpy.count()<<" ok signal"<<endl;
    
    qDebug()<<"response check"<<endl;
    for(int i = 0; i < requests.count(); ++i)
    {
        qDebug()<<"check "<<discription.at(i)<<" response"<<endl;
        QVERIFY(target->requestResponse(requests.at(i)) == responseSet.at(i));
    }
    
    removeTarget();    
}


template<typename TAG>
void QNfcTagTestCommon<TAG>::testMixRawCommandAndNdefAccess(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testMixRawCommandAndNdefAccess", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testMixRawCommandAndNdefAccess", "count mismatch");
    Q_ASSERT_X(discription.count() > 2, "testMixRawCommandAndNdefAccess", "list should at least have 2 elements");
    
    touchTarget();
    int okCount = 0;
    int errCount = 0;
    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage))); 
    QList<QNearFieldTarget::RequestId> requests;
    
    GetSignalCount(responseSet, errCount, okCount); 
    
    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        
        if (0 == i)
        {
            target->readNdefMessages();
        }
    }
    
    QTest::qWait(5000);
    qDebug()<<"signal count check"<<endl;
    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
    const QNdefMessage& ndefMessage(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());
    QVERIFY(ndefMessage.count()>0);
    
    qDebug()<<"response check"<<endl;
    for(int i = 0; i < requests.count(); ++i)
    {
        qDebug()<<"check "<<discription.at(i)<<" response"<<endl;
        QVERIFY(target->requestResponse(requests.at(i)) == responseSet.at(i));
    }
    
    removeTarget();
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::testWaitMixRawCommandAndNdefAccess(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testWaitMixRawCommandAndNdefAccess", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testWaitMixRawCommandAndNdefAccess", "count mismatch");
    Q_ASSERT_X(discription.count() > 2, "testWaitMixRawCommandAndNdefAccess", "list should at least have 2 elements");
    
    touchTarget();
    int okCount = 0;
    int errCount = 0;
    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage))); 
    QList<QNearFieldTarget::RequestId> requests;
    
    GetSignalCount(responseSet, errCount, okCount); 
    
    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        
        if (0 == i)
        {
            target->readNdefMessages();
        }
    }
    
    // wait first request id.
    if (responseSet.first().isValid())
    {
        QVERIFY(target->waitForRequestCompleted(requests.first()));
    }
    else
    {
        QVERIFY(!(target->waitForRequestCompleted(requests.first())));
    }
    
    qDebug()<<"signal count check"<<endl;
    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
    const QNdefMessage& ndefMessage(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());
    QVERIFY(ndefMessage.count()>0);
    
    qDebug()<<"response check"<<endl;
    for(int i = 0; i < requests.count(); ++i)
    {
        qDebug()<<"check "<<discription.at(i)<<" response"<<endl;
        QVERIFY(target->requestResponse(requests.at(i)) == responseSet.at(i));
    }
    
    removeTarget();
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::testWaitInSlot(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testWaitInSlot", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testWaitInSlot", "count mismatch");
    Q_ASSERT_X(discription.count() > 3, "testWaitInSlot", "list should at least have 3 elements");
    
    touchTarget();
    int okCount = 0;
    int errCount = 0;

    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage))); 

    QDummySlot waitSlot;
    QObject::connect(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)), 
                     &waitSlot, SLOT(requestCompletedHandling(const QNearFieldTarget::RequestId&)));
    QObject::connect(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)), 
                     &waitSlot, SLOT(errorHandling(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));

    QList<QNearFieldTarget::RequestId> requests;
    
    GetSignalCount(responseSet, errCount, okCount);
    
    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        
        if (0 == i)
        {
            target->readNdefMessages();
        }
    }

    waitSlot.tag = target; 
    waitSlot.iReqId = requests.at(0);

    // wait second request id.
    if (responseSet.at(1).isValid())
    {
        QVERIFY(target->waitForRequestCompleted(requests.at(1)));
    }
    else
    {
        QVERIFY(!(target->waitForRequestCompleted(requests.at(1))));
    }   

    QObject::disconnect(target, 0, &waitSlot, 0);

    QTest::qWait(5000);
    qDebug()<<"signal count check"<<endl;
    QTRY_COMPARE(okSpy.count(), okCount);

    // errCount should add 1 err signal for dummy command sent in slot
    QTRY_COMPARE(errSpy.count(), errCount+1);
    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
    const QNdefMessage& ndefMessage(ndefMessageReadSpy.first().at(0).value<QNdefMessage>());
    QVERIFY(ndefMessage.count()>0);
    
    qDebug()<<"response check"<<endl;
    for(int i = 0; i < requests.count(); ++i)
    {
        qDebug()<<"check "<<discription.at(i)<<" response"<<endl;
        QVERIFY(target->requestResponse(requests.at(i)) == responseSet.at(i));
    }
    
    removeTarget();
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::testDeleteTargetBeforeAsyncRequestComplete(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testDeleteTargetBeforeAsyncRequestComplete", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testDeleteTargetBeforeAsyncRequestComplete", "count mismatch");
    Q_ASSERT_X(discription.count() > 2, "testDeleteTargetBeforeAsyncRequestComplete", "list should at least have 2 elements");
    
    touchTarget();
    int okCount = 0;
    int errCount = 0;
    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage))); 
    QList<QNearFieldTarget::RequestId> requests;
    
    GetSignalCount(responseSet, errCount, okCount); 
    
    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        
        if (0 == i)
        {
            target->readNdefMessages();
        }
    }
    
    // wait first request id.
    if (responseSet.first().isValid())
    {
        QVERIFY(target->waitForRequestCompleted(requests.first()));
        errCount = 0;
        okCount = 1;
    }
    else
    {
        QVERIFY(!(target->waitForRequestCompleted(requests.first())));
        errCount = 1;
        okCount = 0;
    }
    
    delete target;
    target = 0;

    QNfcTestUtil::ShowMessage("please remove tag");

    qDebug()<<"signal count check"<<endl;
    QTRY_COMPARE(okSpy.count(), okCount);
    QTRY_COMPARE(errSpy.count(), errCount);
    QTRY_VERIFY(ndefMessageReadSpy.isEmpty());
}

template<typename TAG>
void QNfcTagTestCommon<TAG>::testRemoveTargetBeforeAsyncRequestComplete(const QStringList& discription, const QVariantList& commandSet, const QVariantList& responseSet)
{
    Q_ASSERT_X(discription.count() == commandSet.count(), "testRemoveTargetBeforeAsyncRequestComplete", "count mismatch");
    Q_ASSERT_X(discription.count() == responseSet.count(), "testRemoveTargetBeforeAsyncRequestComplete", "count mismatch");
    Q_ASSERT_X(discription.count() > 2, "testRemoveTargetBeforeAsyncRequestComplete", "list should at least have 2 elements");
    
    touchTarget();
    QNfcTestUtil::ShowMessage("please remove the tag in 5 seconds");
    int okCount = 0;
    int errCount = 0;
    QSignalSpy okSpy(target, SIGNAL(requestCompleted(const QNearFieldTarget::RequestId&)));
    QSignalSpy errSpy(target, SIGNAL(error(QNearFieldTarget::Error, const QNearFieldTarget::RequestId&)));
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(QNdefMessage))); 
    QList<QNearFieldTarget::RequestId> requests;
    
    GetSignalCount(responseSet, errCount, okCount); 
    
    for (int i = 0; i < discription.count(); ++i)
    {
        qDebug()<<"test "<<discription.at(i)<<endl;
        // sendCommand
        if (commandSet.at(i).type() == QVariant::ByteArray)
        {
            QByteArray command = commandSet.at(i).toByteArray();
            QNearFieldTarget::RequestId id = target->sendCommand(command);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        else // sendCommands
        {
            QVariantList lists = commandSet.at(i).toList();
            QList<QByteArray> commands;
            for (int j = 0; j < lists.count(); ++j)
            {
                commands.append(lists.at(j).toByteArray());
            }
            QNearFieldTarget::RequestId id = target->sendCommands(commands);
            QVERIFY(id.isValid());
            requests.append(id);
        }
        
        if (0 == i)
        {
            target->readNdefMessages();
        }
    }
    
    QTest::qSleep(5000);

    qDebug()<<"signal count check"<<endl;

    QTest::qWait(5000);
    QTRY_VERIFY((errSpy.count() == responseSet.count() + 1) || (errSpy.count() == responseSet.count()));
    qDebug()<<errSpy.count()<<endl;
    QTRY_VERIFY(ndefMessageReadSpy.isEmpty());

    qDebug()<<"delete the target"<<endl;
    delete target;
    target = 0;
}

#endif // QNFCTAGTESTCOMMON_H
