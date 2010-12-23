#include "qdummyslot.h"

QDummySlot::QDummySlot(QObject *parent) :
    QObject(parent)
{
    tag = 0;
}

void QDummySlot::errorHandling(QNearFieldTarget::Error error, const QNearFieldTarget::RequestId& id)
{
    if (id == iReqId)
    {
        qDebug()<<"get the error signal, wait for dummy request"<<endl;
        QByteArray dummyCommand;
        dummyCommand.append((char)0);
        iWaitId = tag->sendCommand(dummyCommand);
        QVERIFY(!tag->waitForRequestCompleted(iWaitId));
        qDebug()<<"waited request completed"<<endl;
    }
}

void QDummySlot::requestCompletedHandling(const QNearFieldTarget::RequestId& id)
{
    if (id == iReqId)
    {
        qDebug()<<"get the requestCompleted signal, wait for dummy request"<<endl;
        QByteArray dummyCommand;
        dummyCommand.append((char)0);
        iWaitId = tag->sendCommand(dummyCommand);
        QVERIFY(!tag->waitForRequestCompleted(iWaitId));
        qDebug()<<"waited request completed"<<endl;
    }
}
        
