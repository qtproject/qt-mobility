#include "qnfctagtestcommon.h"
#include "qnfctestcommon.h"
#include "qnfctestutil.h"
#include <qndefmessage.h>
#include <qndefnfctextrecord.h>
#include <qndefnfcurirecord.h>

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QNearFieldTarget::Type)
Q_DECLARE_METATYPE(QNearFieldTarget::AccessMethod)

QNfcTagTestCommon::QNfcTagTestCommon()
{
    target = 0;
    qRegisterMetaType<QNdefMessage>("QNdefMessage");
    qRegisterMetaType<QNearFieldTarget *>("QNearFieldTarget*"); 
    qRegisterMetaType<QNearFieldTarget::Error>("QNearFieldTarget::Error"); 
}

QNfcTagTestCommon::~QNfcTagTestCommon()
{
    delete target;
}

void QNfcTagTestCommon::touchTarget(QNearFieldTarget::Type targetType)
{
    if (target)
    {
        delete target;
        target = 0;
    }
    QSignalSpy targetDetectedSpy(&manager, SIGNAL(targetDetected(QNearFieldTarget*)));
    QSignalSpy targetLostSpy(&manager, SIGNAL(targetLost(QNearFieldTarget*)));

    manager.startTargetDetection(targetType);

    QString hint = "please touch tag type ";

    switch(targetType)
    {
        case QNearFieldTarget::NfcTagType1:
        {
            hint += "1";
            break;
        } 
        case QNearFieldTarget::NfcTagType2:
        {
            hint += "2";
            break;
        } 
        case QNearFieldTarget::NfcTagType3:
        {
            hint += "3";
            break;
        } 
        case QNearFieldTarget::NfcTagType4:
        {
            hint += "4";
            break;
        } 
    }
    hint += " with NDef Message inside";
    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty()); 
    target = targetDetectedSpy.at(targetDetectedSpy.count()-1).at(0).value<QNearFieldTarget *>();
    QVERIFY(target);

    QVERIFY(!target->uid().isEmpty());

    if (targetType != QNearFieldTarget::AnyTarget)
    {
        QCOMPARE(target->type(), targetType);
    }
}

void QNfcTagTestCommon::removeTarget()
{
    QVERIFY(target);

    QSignalSpy targetLostSpy(&manager, SIGNAL(targetLost(QNearFieldTarget*)));
    QSignalSpy disconnectedSpy(target, SIGNAL(disconnected()));

    QNfcTestUtil::ShowMessage("please remove the tag");

    QTRY_VERIFY(!targetLostSpy.isEmpty());
    
    QNearFieldTarget *lostTarget = targetLostSpy.first().at(0).value<QNearFieldTarget *>();

    QCOMPARE(target, lostTarget);

    QVERIFY(!disconnectedSpy.isEmpty());

    manager.stopTargetDetection(); 
}    

void QNfcTagTestCommon::NdefCheck()
{
    // tag is already in near field
    //QCOMPARE(target->accessMethods(), QNearFieldTarget::NdefAccess|QNearFieldTarget::TagTypeSpecificAccess);

    QVERIFY(target->hasNdefMessage());

    target->readNdefMessages();
    QSignalSpy ndefMessageReadSpy(target, SIGNAL(ndefMessageRead(const QNdefMessage&)));

    QTRY_VERIFY(!ndefMessageReadSpy.isEmpty());
#if 0
    QNdefNfcTextRecord textRecord;
    textRecord.setText(QLatin1String("nfc tag test"));
    QNdefMessage message;
    message.append(textRecord);

    QNdefNfcUriRecord uriRecord;
    uriRecord.setUri(QUrl("http://qt.nokia.com"));
    message.append(uriRecord);

    QNdefRecord record;
    record.setTypeNameFormat(QNdefRecord::ExternalRtd);
    record.setType("com.nokia.qt:test");
    record.setPayload(QByteArray(2, quint8(0x55)));
    message.append(record);

    messages.append(message);

    target->setNdefMessages(messages);

    QVERIFY(target->hasNdefMessage());

    QList<QNdefMessage> storedMessages = target->ndefMessages();

    QVERIFY(messages == storedMessages);
#endif
}
