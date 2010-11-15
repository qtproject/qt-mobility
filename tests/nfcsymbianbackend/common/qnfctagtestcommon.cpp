#include "qnfctagtestcommon.h"
#include "qnfctestcommon.h"
#include "qnfctestutil.h"

Q_DECLARE_METATYPE(QNearFieldTarget*)
Q_DECLARE_METATYPE(QNearFieldTarget::Type)

QNfcTagTestCommon::QNfcTagTestCommon()
{
    target = 0;
}

QNfcTagTestCommon::~QNfcTagTestCommon()
{
    delete target;
}

QNearFieldTarget * QNfcTagTestCommon::touchTarget(QNearFieldTarget::Type targetType)
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
    QNfcTestUtil::ShowMessage(hint);
    QTRY_VERIFY(!targetDetectedSpy.isEmpty());

    target = targetDetectedSpy.first().at(0).value<QNearFieldTarget *>();
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

