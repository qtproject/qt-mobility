#ifndef DUMMYSLOT_H
#define DUMMYSLOT_H

#include <QObject>
#include <QtTest/QtTest>
#include "qnearfieldtagtype1.h"
#include "qnfctestcommon.h"
#include "qnfctestutil.h"

QTM_USE_NAMESPACE
    
class QDummySlot : public QObject
{
    Q_OBJECT
public:
    explicit QDummySlot(QObject *parent = 0);

public slots:
    void errorHandling(QNearFieldTarget::Error error);
public:
    // NOT Own
    QNearFieldTagType1 * tag;
    QNearFieldTarget::RequestId id;
};

#endif // DUMMYSLOT_H
