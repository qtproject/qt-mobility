/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmessageservice.h"
#include "winhelpers_p.h"
#include "qmessagemanager.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessage_p.h"
#include "qmessagestore_p.h"
#include "qmessagecontentcontainer_p.h"
#include "qmessagecontentcontainerid_p.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <mapix.h>
#include <objbase.h>
#include <mapiutil.h>
#include <qmobilityglobal.h>
#ifdef _WIN32_WCE
#include <cemapi.h>
#endif

using namespace QtMobility::WinHelpers;

QTM_BEGIN_NAMESPACE

static const unsigned long SmsCharLimit = 160;

class QMessageServicePrivate : public QObject
{
    Q_OBJECT

    Q_DECLARE_PUBLIC(QMessageService)

public:
    QMessageServicePrivate(QMessageService* parent);
    ~QMessageServicePrivate();

    bool send(const QMessage& message, bool showComposer = false);
    bool show(const QMessageId& id);
#ifdef _WIN32_WCE
    bool isPartiallyDownloaded(const QMessageId& id, bool considerAttachments = false);
    bool markForDownload(const QMessageId& id, bool includeAttachments = false);
    bool synchronize(const QMessageAccountId& id);
    bool registerUpdates(const QMessageId& id);
    void unregisterUpdates();
    bool retrieveBody(const QMessage& partialMessage);
#endif

    void setFinished(bool successful);

public slots:
    void completed();
    void reportMatchingIds();
    void reportMessagesCounted();

#ifdef _WIN32_WCE
    void messageUpdated(const QMessageId& id);
#endif

signals:
    void stateChanged(QMessageService::State);
    void messagesFound(const QMessageIdList&);
    void messagesCounted(int);
    void progressChanged(uint, uint);

public:
    QMessageService* q_ptr;
    QMessageManager _manager;
    bool _active;
    QMessageManager::Error _error;
    QMessageIdList _candidateIds;
    int _count;
    QMessageService::State _state;
    QMessageId m_bodyDownloadTarget;
    QMessageManager::NotificationFilterId m_bodyDownloadFilterId;
    bool m_registeredUpdates;
    QThread *m_queryThread;
    QList<QThread*> m_obsoleteThreads;
};

namespace {

class QueryThread : public QThread
{
    Q_OBJECT

    QMessageServicePrivate *_parent;
    QMessageFilter _filter;
    QString _body;
    QMessageDataComparator::MatchFlags _matchFlags;
    QMessageSortOrder _ordering;
    uint _limit;
    uint _offset;

    // Ensure that the main thread has instantiated the store before we access it from another thread:
    QMessageManager _manager;

public:
    QueryThread(QMessageServicePrivate *parent, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset);
    void run();

signals:
    void completed();
};

QueryThread::QueryThread(QMessageServicePrivate *parent, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
: QThread(),
    _parent(parent),
    _filter(filter),
    _body(body),
    _matchFlags(matchFlags),
    _ordering(sortOrder),
    _limit(limit),
    _offset(offset)
{
}

void QueryThread::run()
{
    // Ensure that this thread has initialized MAPI
    WinHelpers::MapiInitializationToken token(WinHelpers::initializeMapi());

    _parent->_candidateIds = _manager.queryMessages(_filter, _body, _matchFlags, _ordering, _limit, _offset);
    _parent->_error = _manager.error();
    emit completed();
}

}

void QMessageServicePrivate::completed()
{
    _active = false;
    _state = QMessageService::FinishedState;
    emit stateChanged(_state);
}

void QMessageServicePrivate::reportMatchingIds()
{
    if (_error == QMessageManager::NoError) {
        emit messagesFound(_candidateIds);
    }

    completed();
}

void QMessageServicePrivate::reportMessagesCounted()
{
    if (_error == QMessageManager::NoError) {
        emit messagesCounted(_candidateIds.count());
    }
    completed();
}

#ifdef _WIN32_WCE
void QMessageServicePrivate::messageUpdated(const QMessageId& id)
{
    if (id == m_bodyDownloadTarget) {
        bool isBodyDownloaded = !isPartiallyDownloaded(id);
        if (isBodyDownloaded) {
            unregisterUpdates();

            completed();
        }
    }
}

#endif

QMessageServicePrivate::QMessageServicePrivate(QMessageService* parent)
    :q_ptr(parent),
     _active(false),
     _state(QMessageService::InactiveState),
     m_registeredUpdates(false),
     m_queryThread(0)
{
}

QMessageServicePrivate::~QMessageServicePrivate()
{
    qDeleteAll(m_obsoleteThreads);
    delete m_queryThread;
    _manager.unregisterNotificationFilter(m_bodyDownloadFilterId);
}

static Lptstr createMCFRecipients(QMessageAddress::Type filterAddressType, const QMessageAddressList& addressList)
{
    QStringList temp;

    foreach(const QMessageAddress& a, addressList)
    {
        if(a.type() == filterAddressType)
            temp.append(a.addressee());
    }

    return temp.isEmpty() ? Lptstr(0) : LptstrFromQString(temp.join(";"));
}

bool QMessageServicePrivate::send(const QMessage& message, bool showComposer)
{
    //check message type

    if(message.type() == QMessage::AnyType || message.type() == QMessage::NoType)
    {
        qWarning() << "Unsupported message type for sending/composition";
        _error = QMessageManager::ConstraintFailure;
        return false;
    }

    //check account

    QMessageAccountId accountId = message.parentAccountId();

    if(!accountId.isValid())
    {
        accountId = QMessageAccount::defaultAccount(message.type());
        if(!accountId.isValid())
        {
            qWarning() << "Invalid account for sending/composition";
            _error = QMessageManager::InvalidId;
            return false;
        }
    }

    //check account/message type compatibility

    QMessageAccount account(accountId);
    if(!(account.messageTypes() & message.type()))
    {
        qWarning() << "Message type unsupported by account";
        _error = QMessageManager::ConstraintFailure;
        return false;
    }

#ifdef _WIN32_WCE
    if(showComposer)
    {
        MAILCOMPOSEFIELDS mcf;
        memset(&mcf,0,sizeof(mcf));

        Lptstr accountName = LptstrFromQString(QMessageAccount(accountId).name());
        Lptstr to;
        Lptstr cc;
        Lptstr bcc;
        Lptstr subject;
        Lptstr attachments;
        Lptstr bodyText;

        //account

        mcf.pszAccount = accountName;
        mcf.dwFlags = MCF_ACCOUNT_IS_NAME;

        if(message.type() == QMessage::Email)
        {
            //recipients

            to = createMCFRecipients(QMessageAddress::Email, message.to());
            cc = createMCFRecipients(QMessageAddress::Email, message.cc());
            bcc = createMCFRecipients(QMessageAddress::Email, message.bcc());
            mcf.pszTo = to;
            mcf.pszCc = cc;
            mcf.pszBcc = bcc;

            //subject

            subject = LptstrFromQString(message.subject());
            mcf.pszSubject = subject;

            //body

            QMessageContentContainerId bodyId = message.bodyId();
            if(bodyId.isValid())
            {
                const QMessageContentContainer& bodyContainer = message.find(bodyId);
                bodyText = LptstrFromQString(bodyContainer.textContent());
                mcf.pszBody = bodyText;
            }

            //attachments

            if(message.status() & QMessage::HasAttachments)
            {
                QStringList attachmentList;

                foreach(const QMessageContentContainerId& id, message.attachmentIds())
                {
                    const QMessageContentContainer& attachmentContainer = message.find(id);
                    attachmentList.append(QMessageContentContainerPrivate::attachmentFilename(attachmentContainer));
                }

                mcf.cAttachments = attachmentList.count();
                QChar nullChar(0);
                attachments = LptstrFromQString(attachmentList.join(nullChar));
                mcf.pszAttachments = attachments;
            }
        }
        else if(message.type() == QMessage::Sms)
        {
            //recipients

            to = createMCFRecipients(QMessageAddress::Phone, message.to());
            mcf.pszTo = to;

            //body

            QMessageContentContainerId bodyId = message.bodyId();
            if(bodyId.isValid())
            {
                const QMessageContentContainer& bodyContainer = message.find(bodyId);
                QString textContent = bodyContainer.textContent();
                if(textContent.length() > SmsCharLimit)
                {
                    textContent.truncate(SmsCharLimit);
                    qWarning() << "SMS messages may not exceed " << SmsCharLimit << " characters. Body trucated.";
                }
                bodyText = LptstrFromQString(textContent);
                mcf.pszBody = bodyText;
            }
        }

       mcf.cbSize = sizeof(mcf);

       if(FAILED(MailComposeMessage(&mcf)))
       {
           _error = QMessageManager::FrameworkFault;
           qWarning() << "MailComposeMessage failed";
           return false;
       }
    }
    else
    {
#endif

    //check recipients
    QMessageAddressList recipients = message.to() + message.bcc() + message.cc();
    if(recipients.isEmpty() && !showComposer)
    {
        qWarning() << "Message must have recipients for sending";
        _error = QMessageManager::ConstraintFailure;
        return false;
    }

    MapiSessionPtr mapiSession(MapiSession::createSession(&_error));
    if (_error != QMessageManager::NoError)
    {
        qWarning() << "Could not create MAPI session for sending";
        return false;
    }

    QMessage outgoing(message);

    //ensure the message is marked read otherwise MapiForm displays the message as incomming
    outgoing.setStatus(QMessage::Read,true);

    //set default account if unset
    if(!outgoing.parentAccountId().isValid())
        outgoing.setParentAccountId(accountId);

    MapiStorePtr mapiStore = mapiSession->findStore(&_error, outgoing.parentAccountId(),false);

    if(mapiStore.isNull() || _error != QMessageManager::NoError)
    {
        qWarning() << "Unable to retrieve MAPI store for account";
        return false;
    }

    //try first to create message in outbox for store, failing that attempt draft

    MapiFolderPtr mapiFolder = mapiStore->findFolder(&_error,QMessage::OutboxFolder);

    if( mapiFolder.isNull() || _error != QMessageManager::NoError ) {
        qWarning() << "Unable to retrieve outbox MAPI folder for sending, attempting drafts...";
        mapiFolder = mapiStore->findFolder(&_error,QMessage::DraftsFolder);
        if(mapiFolder.isNull() || _error != QMessageManager::NoError) {
            qWarning() << "Unable to retrieve drafts MAPI folder for sending";
            return false;
        }
    }

    IMessage* mapiMessage = mapiFolder->createMessage(&_error, outgoing, mapiSession, DontSavePropertyChanges);

    if(!mapiMessage || _error != QMessageManager::NoError)
    {
        qWarning() << "Unable to create MAPI message from source";
        mapiRelease(mapiMessage);
        return false;
    }

#ifndef _WIN32_WCE
    if(showComposer)
    {
        MapiForm* mapiForm = new MapiForm(mapiStore->store(),mapiSession->session(),mapiFolder->folder(),mapiMessage);
        bool result = mapiForm->show();
        mapiRelease(mapiForm);

        if(!result)
        {
            qWarning() << "MapiForm::Show failed";
            _error = QMessageManager::FrameworkFault;
            return false;
        }
    }
    else
#endif
    {
        if(FAILED(mapiMessage->SubmitMessage(0)))
        {
            qWarning() << "MAPI SubmitMessage failed.";
            _error = QMessageManager::FrameworkFault;
            mapiRelease(mapiMessage);
            return false;
        }
    }

#ifdef _WIN32_WCE
    }
#endif

    return true;
}

bool QMessageServicePrivate::show(const QMessageId& messageId)
{
    if(!messageId.isValid())
    {
        _error = QMessageManager::InvalidId;
        qWarning() << "Invalid QMessageId";
        return false;
    }

#ifdef _WIN32_WCE
    MapiEntryId entryId = QMessageIdPrivate::entryId(messageId);
    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    if(FAILED(MailDisplayMessage(entryIdPtr,entryId.length())))
    {
        qWarning() << "MailDisplayMessage failed";
        _error = QMessageManager::FrameworkFault;
        return false;
    }
    return true;

#else

    MapiSessionPtr mapiSession(MapiSession::createSession(&_error));
    if (_error != QMessageManager::NoError)
    {
        qWarning() << "Could not create MAPI seesion";
        return false;
    }

    MapiEntryId entryId = QMessageIdPrivate::entryId(messageId);
    MapiRecordKey folderRecordKey = QMessageIdPrivate::folderRecordKey(messageId);
    MapiRecordKey storeRecordKey = QMessageIdPrivate::storeRecordKey(messageId);

    MapiStorePtr mapiStore = mapiSession->findStore(&_error,QMessageAccountIdPrivate::from(storeRecordKey));

    if(mapiStore.isNull() || _error != QMessageManager::NoError)
    {
        qWarning() << "Unable to retrieve MAPI store for account";
        return false;
    }

    MapiFolderPtr mapiFolder = mapiStore->openFolderWithKey(&_error,folderRecordKey);

    if( mapiFolder.isNull() || _error != QMessageManager::NoError ) {
        qWarning() << "Unable to retrieve MAPI folder for message";
        return false;
    }

    IMessage* mapiMessage = mapiFolder->openMessage(&_error,entryId);

    if(!mapiMessage || _error != QMessageManager::NoError)
    {
        qWarning() << "Unable to retrieve MAPI message";
        mapiRelease(mapiMessage);
        return false;
    }

    MapiForm* mapiForm = new MapiForm(mapiStore->store(),mapiSession->session(),mapiFolder->folder(),mapiMessage);
    bool result = mapiForm->show();
    mapiRelease(mapiForm);

    if(!result)
    {
        qWarning() << "MapiForm::show failed.";
        _error = QMessageManager::FrameworkFault;
        return false;
    }

    mapiRelease(mapiMessage);

    return result;

#endif
}

#ifdef _WIN32_WCE

bool QMessageServicePrivate::isPartiallyDownloaded(const QMessageId& id, bool considerAttachments)
{
    if(!id.isValid())
    {
        _error = QMessageManager::InvalidId;
        qWarning() << "Invalid QMessageId";
        return false;
    }

    MapiSessionPtr mapiSession(MapiSession::createSession(&_error));

    if (_error != QMessageManager::NoError)
    {
        qWarning() << "Could not create MAPI session";
        return false;
    }

    MapiStorePtr mapiStore = mapiSession->openStore(&_error,QMessageIdPrivate::storeRecordKey(id));

    if(mapiStore.isNull() || _error != QMessageManager::NoError) {
        qWarning() << "Unable to retrieve MAPI store for account";
        return false;
    }

    IMessage* message = mapiStore->openMessage(&_error,QMessageIdPrivate::entryId(id));

    ULONG status = 0;
    if(!getMapiProperty(message,PR_MSG_STATUS,&status)) {
        qWarning() << "Unable to get MAPI message status flags";
        _error = QMessageManager::FrameworkFault;
        return false;
    }
    else
    {
        mapiRelease(message);
        bool bodyNotDownloaded = (status & MSGSTATUS_HEADERONLY) || (status & MSGSTATUS_PARTIAL);
        bool attachmentsNotDownloaded = (status & MSGSTATUS_PENDING_ATTACHMENTS);
        return considerAttachments ? bodyNotDownloaded && attachmentsNotDownloaded : bodyNotDownloaded;
    }
}

bool QMessageServicePrivate::markForDownload(const QMessageId& id, bool includeAttachments)
{
    if(!id.isValid())
    {
        _error = QMessageManager::InvalidId;
        qWarning() << "Invalid QMessageId";
        return false;
    }

    MapiSessionPtr mapiSession(MapiSession::createSession(&_error));

    if (_error != QMessageManager::NoError)
    {
        qWarning() << "Could not create MAPI session";
        return false;
    }

    MapiStorePtr mapiStore = mapiSession->openStore(&_error,QMessageIdPrivate::storeRecordKey(id));

    if(mapiStore.isNull() || _error != QMessageManager::NoError)
    {
        qWarning() << "Unable to retrieve MAPI store for message account";
        return false;
    }

    IMessage* message = mapiStore->openMessage(&_error,QMessageIdPrivate::entryId(id));

    ULONG status = 0;

    if(!getMapiProperty(message,PR_MSG_STATUS,&status))
    {
        qWarning() << "Unable to get MAPI message status flags";
        _error = QMessageManager::FrameworkFault;
        return false;
    }
    else
    {
        //mark the message to download on the next sync

        status |= MSGSTATUS_REMOTE_DOWNLOAD;
        if(includeAttachments)
            status |= MSGSTATUS_REMOTE_DOWNLOAD_ATTACH;

        if(!setMapiProperty(message, PR_MSG_STATUS, status))
        {
            qWarning() << "Could not mark the MAPI message for download!";
            _error = QMessageManager::FrameworkFault;
            return false;
        }

        mapiRelease(message);

        //TODO investigate possiblity of interacting with mapi transport directly
        /*
        QString transportName = mapiStore->transportName();
        if(transportName.isEmpty())
        {
            qWarning() << "Could not get transport name for mapi store";
            return false;
        }
        */
    }
    return true;
}

bool QMessageServicePrivate::synchronize(const QMessageAccountId& id)
{
    if(!id.isValid())
    {
        qWarning() << "Cannot synchronize invalid QMessageAccountId";
        _error = QMessageManager::InvalidId;
        return false;
    }

    QMessageAccount account(id);
    if(FAILED(MailSyncMessages(LptstrFromQString(account.name()),MCF_ACCOUNT_IS_NAME | MCF_RUN_IN_BACKGROUND)))
    {
        qWarning() << "MailSyncMessages failed for account: " << account.name();
        _error = QMessageManager::FrameworkFault;
        return false;
    }
    return true;
}

bool QMessageServicePrivate::registerUpdates(const QMessageId& id)
{
    if(!id.isValid())
    {
        qWarning() << "Cannot register for update notifications on invalid QMessageId";
        _error = QMessageManager::InvalidId;
        return false;
    }

    if(!m_registeredUpdates)
    {
        connect(&_manager, SIGNAL(messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),this,SLOT(messageUpdated(const QMessageId&)));
        m_bodyDownloadFilterId = _manager.registerNotificationFilter(QMessageFilter::byId(id));
        m_bodyDownloadTarget = id;
        m_registeredUpdates = true;
    }
    return m_registeredUpdates;
}

void QMessageServicePrivate::unregisterUpdates()
{
    disconnect(&_manager, SIGNAL(messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),this,SLOT(messageUpdated(const QMessageId&)));
    _manager.unregisterNotificationFilter(m_bodyDownloadFilterId);
    m_bodyDownloadFilterId = 0;
    m_registeredUpdates = false;
}

bool QMessageServicePrivate::retrieveBody(const QMessage& partialMessage)
{
    if(partialMessage.type() != QMessage::Email)
    {
        qWarning() << "Cannot retrieve body for non-email message type";
        _error = QMessageManager::ConstraintFailure;
        return false;
    }

    if(isPartiallyDownloaded(partialMessage.id()))
    {
        //only valid for Email
        if(markForDownload(partialMessage.id(),true))
            if(registerUpdates(partialMessage.id()))
                if(!synchronize(partialMessage.parentAccountId()))
                    unregisterUpdates();
    }
    else QTimer::singleShot(0,this,SLOT(completed()));

    return (_error == QMessageManager::NoError);
}

#endif

void QMessageServicePrivate::setFinished(bool successful)
{
    if (!successful && (_error == QMessageManager::NoError)) {
        _error = QMessageManager::RequestIncomplete;
    }

    completed();
}

QMessageService::QMessageService(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServicePrivate(this))
{
    connect(d_ptr, SIGNAL(stateChanged(QMessageService::State)),
        this, SIGNAL(stateChanged(QMessageService::State)));
    connect(d_ptr, SIGNAL(messagesFound(const QMessageIdList&)),
        this, SIGNAL(messagesFound(const QMessageIdList&)));
    connect(d_ptr, SIGNAL(messagesCounted(int)),
        this, SIGNAL(messagesCounted(int)));
    connect(d_ptr, SIGNAL(progressChanged(uint, uint)),
        this, SIGNAL(progressChanged(uint, uint)));
}

QMessageService::~QMessageService()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    return queryMessages(filter, QString(), QMessageDataComparator::MatchFlags(), sortOrder, limit, offset);
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }


    d_ptr->_active = true;
    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_state = QMessageService::ActiveState;
    emit stateChanged(d_ptr->_state);

#if 0
    d_ptr->_candidateIds = d_ptr->_manager.queryMessages(filter, body, matchFlags, sortOrder, limit, offset);
    d_ptr->_error = d_ptr->_manager.error();
    QTimer::singleShot(0,d_ptr,SLOT(reportMatchingIds()));
#else
    // Perform the query in another thread to keep the UI thread free
    QueryThread *query = new QueryThread(d_ptr, filter, body, matchFlags, sortOrder, limit, offset);
    connect(query, SIGNAL(completed()), d_ptr, SLOT(reportMatchingIds()), Qt::QueuedConnection);
    query->start();

    if (d_ptr->m_queryThread) {
        // Don't delete the previous thread object immediately
        if (!d_ptr->m_obsoleteThreads.isEmpty()) {
            qDeleteAll(d_ptr->m_obsoleteThreads);
            d_ptr->m_obsoleteThreads.clear();
        }
        d_ptr->m_obsoleteThreads.append(d_ptr->m_queryThread);
    }
    d_ptr->m_queryThread = query;
#endif
    return true;
}

bool QMessageService::countMessages(const QMessageFilter &filter)
{
    if (d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }
    d_ptr->_active = true;
    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_state = QMessageService::ActiveState;
    emit stateChanged(d_ptr->_state);

    // Perform the query in another thread to keep the UI thread free
    QueryThread *query = new QueryThread(d_ptr, filter, QString(), QMessageDataComparator::MatchFlags(), QMessageSortOrder(), 0, 0);
    connect(query, SIGNAL(completed()), d_ptr, SLOT(reportMessagesCounted()), Qt::QueuedConnection);
    query->start();

    if (d_ptr->m_queryThread) {
        // Don't delete the previous thread object immediately
        if (!d_ptr->m_obsoleteThreads.isEmpty()) {
            qDeleteAll(d_ptr->m_obsoleteThreads);
        }
        d_ptr->m_obsoleteThreads.append(d_ptr->m_queryThread);
    }
    d_ptr->m_queryThread = query;

    return true;
}

bool QMessageService::send(QMessage &message)
{
    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

    d_ptr->_active = true;
    d_ptr->_state = QMessageService::ActiveState;
    d_ptr->_error = QMessageManager::NoError;
    emit stateChanged(d_ptr->_state);

    bool result = d_ptr->send(message);
    d_ptr->setFinished(result);

    return result;
}

bool QMessageService::compose(const QMessage &message)
{
    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

    d_ptr->_active = true;
    d_ptr->_state = QMessageService::ActiveState;
    d_ptr->_error = QMessageManager::NoError;
    emit stateChanged(d_ptr->_state);

    bool result = d_ptr->send(message,true);
    d_ptr->setFinished(result);

    return result;
}

bool QMessageService::retrieveHeader(const QMessageId& id)
{
    Q_UNUSED(id);

    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

    d_ptr->_error = QMessageManager::NoError;
    d_ptr->setFinished(true);

    return true;
}

bool QMessageService::retrieveBody(const QMessageId& id)
{

    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

#ifdef _WIN32_WCE

    d_ptr->_active = true;
    d_ptr->_state = ActiveState;
    d_ptr->_error = QMessageManager::NoError;
    emit stateChanged(d_ptr->_state);

    if(!id.isValid())
    {
        qWarning() << "Invalid QMessageId";
        d_ptr->_error = QMessageManager::InvalidId;
    }

    QMessage message;

    if(d_ptr->_error == QMessageManager::NoError)
    {
        message = QMessage(id);
        d_ptr->_error = QMessageManager().error();
    }


    if(d_ptr->_error == QMessageManager::NoError)
        d_ptr->retrieveBody(message);

    //emit failure immediately
    if (d_ptr->_error != QMessageManager::NoError) {
        d_ptr->setFinished(false);
        return false;
    }

    return true;

#else
    Q_UNUSED(id);

    d_ptr->_error = QMessageManager::NotYetImplemented;
    d_ptr->setFinished(false);
    return false;
#endif
}

bool QMessageService::retrieve(const QMessageId& messageId, const QMessageContentContainerId& id)
{

    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

#ifdef _WIN32_WCE

    d_ptr->_active = true;
    d_ptr->_state = ActiveState;
    d_ptr->_error = QMessageManager::NoError;
    emit stateChanged(d_ptr->_state);

    if(!messageId.isValid())
    {
        qWarning() << "Invalid QMessageId";
        d_ptr->_error = QMessageManager::InvalidId;
    }

    QMessage message;

    if(d_ptr->_error == QMessageManager::NoError)
    {
        message = QMessage(messageId);
        d_ptr->_error = d_ptr->_manager.error();
    }

    if(d_ptr->_error == QMessageManager::NoError)
    {
        bool isBodyContainer = message.bodyId() == id;
        if(isBodyContainer)
            d_ptr->retrieveBody(message);
        //TODO downloading attachment programatically possible?
    }

    //emit failure immediately
    if (d_ptr->_error != QMessageManager::NoError) {
        d_ptr->setFinished(false);
        return false;
    }

    return true;

#else
    Q_UNUSED(messageId)
    Q_UNUSED(id)

    d_ptr->_error = QMessageManager::NotYetImplemented;
    d_ptr->setFinished(false);
#endif

    return false;
}

bool QMessageService::show(const QMessageId& id)
{
    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

    d_ptr->_active = true;
    d_ptr->_state = ActiveState;
    d_ptr->_error = QMessageManager::NoError;
    emit stateChanged(d_ptr->_state);

    bool result = d_ptr->show(id);
    d_ptr->setFinished(result);

    return result;
}

bool QMessageService::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id);

    if(d_ptr->_active) {
        qWarning() << "Service is currently busy";
        return false;
    }

    d_ptr->_error = QMessageManager::NotYetImplemented;
    d_ptr->setFinished(false);

    return false;
}

QMessageService::State QMessageService::state() const
{
    return d_ptr->_state;
}

void QMessageService::cancel()
{
#ifdef _WIN32_WCE
    if(d_ptr->_active)
    {
        bool awaitingBodyRetrieval(d_ptr->m_bodyDownloadFilterId != 0);

        if(awaitingBodyRetrieval)
        {
            d_ptr->unregisterUpdates();
            d_ptr->_error = QMessageManager::NoError;
            d_ptr->_state = QMessageService::InactiveState;
            d_ptr->_active = false;
            emit stateChanged(d_ptr->_state);
        }
    }
#else
    //NOOP
#endif
}

QMessageManager::Error QMessageService::error() const
{
    return d_ptr->_error;
}

#include <qmessageservice_win.moc>

QTM_END_NAMESPACE
