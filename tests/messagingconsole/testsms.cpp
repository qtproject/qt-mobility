#include "testsms.h"

#include "qmessage.h"
#include "qmessageaccount.h"
#include <QDebug>

TestSms::TestSms(QObject *parent) :
        QObject(parent),m_pMS(new QMessageService()), m_pMM(new QMessageManager())
{
    if (m_pMM)
    {
        m_pMM->registerNotificationFilter(QMessageFilter::byType(QMessage::Sms));

        connect(m_pMM, SIGNAL(messageAdded(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)),
            SLOT(onMessageAdded(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)));

        connect(m_pMM, SIGNAL(messageRemoved(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)),
            SLOT(onMessageRemoved(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)));

        connect(m_pMM, SIGNAL(messageUpdated(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)),
            SLOT(onMessageUpdated(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)));
    }
}

void TestSms::debugMessage(QMessage &message)
{
    //QDEBUG_FUNCTION_BEGIN
    qDebug() << "id:" << message.id().toString() << "type:" << message.type() << "size:" << message.size() << "status:" << message.status() << "priority:" << message.priority();
    qDebug() << "AccountId:" << message.parentAccountId().toString() << "StantardFolder" << message.standardFolder() << "parenFolderId:" << message.parentFolderId().toString();
    qDebug() << "Date:" << message.date() << "receivedDate:" << message.receivedDate() << "Subject:" << message.subject();
    qDebug() << "From:" << message.from().addressee();
    qDebug() << "To:" << (message.to().isEmpty() ? "**none**" : message.to().first().addressee());
    qDebug() << "Body:" << message.textContent();
    //QDEBUG_FUNCTION_END
}

QMessage TestSms::createEmail(const QString &text)
{
    qDebug() << __PRETTY_FUNCTION__ << "begin";
    //bool ret = false;

    QMessage message;
    QList<QMessageAccountId> m_accountList;


    /*
    foreach (const QMessageAccountId& id, m_accountList) {
        QMessageAccount acc(id);
        if(acc.messageTypes() & QMessage::Email)
            message.setParentAccountId(id);
    }*/

    message.setType(QMessage::Email);
    message.setTo(QMessageAddress(QMessageAddress::Email, "poniz@yandex.ru"));
    message.setFrom(QMessageAddress(QMessageAddress::Email, "alexander.ponizovkin@gmail.com"));
    message.setBody(text);
    message.setSubject("This is subject");
    //message.setDate(QDateTime::fromTime_t(11));
    //message.setReceivedDate(QDateTime::currentDateTime());

    return message;
}

QMessage TestSms::createSms(const QString &text)
{
    qDebug() << __PRETTY_FUNCTION__ << "begin";
    //bool ret = false;

    QMessage message;
    QList<QMessageAccountId> m_accountList;


    foreach (const QMessageAccountId& id, m_accountList) {
        QMessageAccount acc(id);
        if(acc.messageTypes() & QMessage::Sms)
            message.setParentAccountId(id);
    }

    message.setType(QMessage::Sms);
    message.setTo(QMessageAddress(QMessageAddress::Phone, "+358403268057"));
    message.setFrom(QMessageAddress(QMessageAddress::Phone, "+1111111111111"));
    message.setBody(text);
    message.setDate(QDateTime::fromTime_t(11));
    message.setReceivedDate(QDateTime::currentDateTime());

    return message;
}

bool TestSms::testMessage()
{
    QMessage m1 = createSms("Just a test message");
    QMessageManager l_mm;

    l_mm.addMessage(&m1);

    QMessageId newId = m1.id();

    QMessage m2 = l_mm.message(newId);

    if (m2.to() != m1.to()) {
        qWarning() << "m2.to() != m1.to()";
        return false;
    }

    if (m2.from() != m1.from()) {
        qWarning() << "m2.from() != m1.from()";
        return false;
    }

    if (m2.textContent() != m1.textContent()) {
        qWarning() << "m2.textContent() != m1.textContent()";
        return false;
    }
    else
        qDebug() << "textContent():" << m2.textContent();

    if (m2.parentAccountId() != m1.parentAccountId()) {
        qWarning() << "m2.parentAccountId() != m1.parentAccountId()";
        return false;
    }
    else
        qDebug() << "parentAccountId():" << m2.parentAccountId().toString();

    qWarning() << "TestSms::testMessage(): PASS";
    debugMessage(m1);
    debugMessage(m2);
    return true;
}

bool TestSms::storeMessage(QMessage &aMessage)
{
    QMessageManager l_mm;
    //aMessage.setFolder(0;)

    l_mm.addMessage(&aMessage);
    return true;
}

bool TestSms::startSmsSending()
{
    bool ret = false;
    QMessage m1 = createSms("111");
    QMessage m2 = createSms("222");
    QMessageService ms;

    ret = ms.send(m1) && ms.send(m2);

    if (ret)
    {
        qDebug() << "sendSms(): successfull SMS sending  " << endl;

    }
    else
    {
        qDebug() << "sendSms(): SMS sending failed " << endl;

    }





    //int accounts_counter = m_mm.countAccounts();
    //qDebug() << "accounts_counter = "  << accounts_counter;

    return ret;
}

bool TestSms::testMessageManager_queryCount()
{
    /*


    QMessageFilter smsfilter1 = QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::InboxFolder)  ;
    QMessageFilter smsfilter2 = QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::SentFolder);
    QMessageFilter smsfilter3 = QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::DraftsFolder);

    int c = mm.countMessages();
    */
    qDebug() << __PRETTY_FUNCTION__ << "Total" << m_pMM->countMessages();

    qDebug() << __PRETTY_FUNCTION__ << "Total emails" << m_pMM->countMessages(QMessageFilter::byType(QMessage::Email));

    qDebug() << __PRETTY_FUNCTION__ << "Total sms" << m_pMM->countMessages(QMessageFilter::byType(QMessage::Sms));

    qDebug() << __PRETTY_FUNCTION__ << "Sms from Inbox" << m_pMM->countMessages(QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::InboxFolder));

    qDebug() << __PRETTY_FUNCTION__ << "Sms from Sent" << m_pMM->countMessages(QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::SentFolder));

    qDebug() << __PRETTY_FUNCTION__ << "Sms from Drafts" << m_pMM->countMessages(QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::DraftsFolder));




    return true;
}

bool TestSms::testMessageManager_queryMessage(const QMessageFilter &filter)
{
    QMessageIdList ids = m_pMM->queryMessages(filter);
     qDebug() << __PRETTY_FUNCTION__ << "Completed m_pMM->queryMessages" ;
    debugMeesageIdList(ids);

    //m_pMM->removeMessages(QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::DraftsFolder));

    return true;
}

bool TestSms::testMessageService_countMessages()
{
    qWarning( ) << __PRETTY_FUNCTION__ << "begin";

    m_numberOfCompletedCalls = 0;

    //connect(&m_ms, SIGNAL(messagesFound(const QMessageIdList &)), SLOT(onMessagesFound(const QMessageIdList &)));
    connect(m_pMS, SIGNAL(messagesCounted(int)), SLOT(onMessagesCounted(int)));

    /*
    QMessageFilter emailfilter1 = QMessageFilter::byType(QMessage::Email) & QMessageFilter::byStandardFolder(QMessage::InboxFolder)  ;

    QMessageFilter smsfilter1 = QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::InboxFolder)  ;
    QMessageFilter smsfilter2 = QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::SentFolder);
    QMessageFilter smsfilter3 = QMessageFilter::byType(QMessage::Sms) & QMessageFilter::byStandardFolder(QMessage::DraftsFolder);
*/
    if (m_pMS && (m_pMS->countMessages(QMessageFilter::byType(QMessage::Sms))))
        qWarning() << "countMessages() started successfully";
    else
        qWarning() << "countMessages() start failed";





    //QMessageIdList ids = m_mm.queryMessages(filter1);
    //test search
    //QMessageIdList ids = m_mm.queryMessages(filter1, "Agat");
    //debugMeesageIdList(ids);

    //qDebug() << "TOTAL COUNTER = " << m_mm.countMessages();
    qWarning() << __PRETTY_FUNCTION__ << "end";
    return true;
}


void TestSms::onMessagesCounted ( int count )
{
    qDebug() << __PRETTY_FUNCTION__ << count << "items";
    m_numberOfCompletedCalls++;

    if (m_numberOfCompletedCalls >= 2)
    {
        m_numberOfCompletedCalls = 0;
        return;
    }

    if (!m_pMS)
        qWarning() << "m_pMS == NULL";

    if (m_pMS && (m_pMS->countMessages(QMessageFilter::byType(QMessage::Sms))))
        qWarning() << "countMessages() started successfully";
    else
        qWarning() << "countMessages() start failed";
}

void TestSms::onMessagesFound ( const QMessageIdList & ids )
{
    debugMeesageIdList(ids);
}

void TestSms::debugMeesageIdList ( const QMessageIdList & ids )
{
    qDebug() << __PRETTY_FUNCTION__ << ids.size() << "items";

    for (int i = 0; i < ids.size(); i++)
    {
        qDebug() << "=====================================================================";
        QMessageId id = ids.at(i);
        qDebug() << "ID = " << id.toString();
        QMessage message = m_pMM->message(id);
        debugMessage(message);

    }
    qDebug() << "=====================================================================";
    qDebug() << __PRETTY_FUNCTION__ << ids.size() << "items";

}


void TestSms::debugMeesageFolderIdList ( const QMessageFolderIdList & ids )
{
    qDebug() << __PRETTY_FUNCTION__ << ids.size() << "folders";

    for (int i = 0; i < ids.size(); i++)
    {
        qDebug() << "=====================================================================";
        const QMessageFolderId id = ids.at(i);
        qDebug() << "ID = " << id.toString();
        QMessageFolder folder = m_pMM->folder(id);
        qDebug() << "Name = " << folder.name();

    }
    qDebug() << "=====================================================================";
    qDebug() << __PRETTY_FUNCTION__ << ids.size() << "items";

}

bool TestSms::testQueryFolders()
{

     QMessageFolderIdList ids = m_pMM->queryFolders();
     debugMeesageFolderIdList(ids);

     return true;
}

bool TestSms::getMessage(const QString &strId)
{
     QMessageId id(strId);

     QMessage mes = m_pMM->message(id);

     debugMessage(mes);


     return true;
}

bool TestSms::compose()
{
    QMessage m = createSms("This is testing of compose()");
    return m_pMS && m_pMS->compose(m);
}

bool TestSms::show(const QString &strId)
{
    //QMessage m = createSms("This is testing of show()");
    qDebug() << __PRETTY_FUNCTION__ << "begin";
    QMessageId id(strId);

    if (id.isValid())
        qDebug() <<  __PRETTY_FUNCTION__ << "ID is valid";
    else
        qDebug() <<  __PRETTY_FUNCTION__ << "ID is NOT valid";

    bool ret = /*m_pMS &&*/  m_pMS->show(id);
    qDebug() << __PRETTY_FUNCTION__ << "end";
    return ret;
}

bool TestSms::qmf_show(const QString &strId)
{
    //QMessage m = createSms("This is testing of show()");
    QMessageId id("QMF_" + strId);

    return m_pMS && m_pMS->show(id);
}

bool TestSms::qmf_compose()
{
    QMessage m = createEmail("This is testing of compose()");
    //QMessageId id("QMF_" + strId);

    return m_pMS && m_pMS->compose(m);
}

void TestSms::onMessageAdded(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)
{
    qDebug() << __PRETTY_FUNCTION__ << "Message ID = " << id.toString();
}

void TestSms::onMessageRemoved(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)
{
    qDebug() << __PRETTY_FUNCTION__ << "Message ID = " << id.toString();
}

void TestSms::onMessageUpdated(const QMessageId & id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds)
{
    qDebug() << __PRETTY_FUNCTION__ << "Message ID = " << id.toString();
}

