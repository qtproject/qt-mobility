#ifndef TESTSMS_H
#define TESTSMS_H

#include <QObject>
#include "qmessageservice.h"
#include "qmessagemanager.h"

QTM_USE_NAMESPACE

class TestSms : public QObject
{
Q_OBJECT
public:
    TestSms(QObject *parent = 0);
    static void debugMessage(QMessage &message);
    void debugMeesageIdList ( const QMessageIdList & ids );
    void debugMeesageFolderIdList ( const QMessageFolderIdList & ids );
    bool startSmsSending();
    bool testMessageService_countMessages();
    bool testMessageManager_queryCount();
    bool testMessageManager_queryMessage(const QMessageFilter &filter);
    bool storeMessage(QMessage &aMessage);
    bool testQueryFolders();
    bool testMessage();
    bool getMessage(const QString &strId);
    QMessage createSms(const QString &text);

signals:

public slots:
    void onMessagesCounted(int count);
    void onMessagesFound(const QMessageIdList &ids);
   // void progressChanged ( uint value, uint total );
    //void stateChanged ( QMessageService::State newState );

private:
    QMessageService *m_pMS;
    QMessageManager *m_pMM;
    int m_numberOfCompletedCalls;
};

#endif // TESTSMS_H
