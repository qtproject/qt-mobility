#include "qdummyslot.h"

QDummySlot::QDummySlot(QObject *parent) :
    QObject(parent)
{
    tester = 0;
}

void QDummySlot::errorHandling(QNearFieldTarget::Error error)
{
    QVERIFY(error == QNearFieldTarget::UnsupportedError);
    if (tester)
    {
        tester->removeTarget();
    }
}
