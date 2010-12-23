#ifndef DUMMYSLOT_H
#define DUMMYSLOT_H

#include <QObject>
#include <QtTest/QtTest>
#include "qnearfieldtarget.h"
#include "qnfctestcommon.h"
#include "qnfctestutil.h"

QTM_USE_NAMESPACE
    
class QDummySlot : public QObject
{
    Q_OBJECT
public:
    explicit QDummySlot(QObject *parent = 0);

public slots:
    void errorHandling(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId& id);
    void requestCompletedHandling(const QNearFieldTarget::RequestId& id);
public:
    // NOT Own
    QNearFieldTarget * tag;
    QNearFieldTarget::RequestId iReqId;
    QNearFieldTarget::RequestId iWaitId;
};

#endif // DUMMYSLOT_H
