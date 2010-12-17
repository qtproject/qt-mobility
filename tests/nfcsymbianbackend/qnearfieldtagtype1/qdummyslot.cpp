#include "qdummyslot.h"

QDummySlot::QDummySlot(QObject *parent) :
    QObject(parent)
{
    tag = 0;
}

void QDummySlot::errorHandling(QNearFieldTarget::Error error)
{
    if (tag)
    {
        QVERIFY(error == QNearFieldTarget::UnsupportedError);
        qDebug()<<"get the error signal, wait for request"<<endl;
        QVERIFY(tag->waitForRequestCompleted(id));
        qDebug()<<"waited request completed"<<endl;
        tag = 0;
    }
}
