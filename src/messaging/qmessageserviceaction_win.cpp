/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmessageserviceaction.h"
#include "winhelpers_p.h"
#include "qmessagestore.h"
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
#ifdef _WIN32_WCE
#include <cemapi.h>
#endif

using namespace WinHelpers;

QTM_BEGIN_NAMESPACE

static const unsigned long SmsCharLimit = 160;

class QMessageServiceActionPrivate : public QObject
{
    Q_OBJECT

    Q_DECLARE_PUBLIC(QMessageServiceAction)

public:
    QMessageServiceActionPrivate(QMessageServiceAction* parent);
    ~QMessageServiceActionPrivate();

    bool send(const QMessage& message, bool showComposer = false);
    bool show(const QMessageId& id);
#ifdef _WIN32_WCE
    bool isPartiallyDownloaded(const QMessageId& id, bool considerAttachments = false);
    bool markForDownload(const QMessageId& id, bool includeAttachments = false);
    bool synchronize(const QMessageAccountId& id);
    bool registerUpdates(const QMessageId& targetMessage);
    void unregisterUpdates();
#endif

public slots:
    void completed();
    void reportMatchingIds();
    void reportMessagesCounted();
#ifdef _WIN32_WCE
    void messageUpdated(const QMessageId& id);
#endif

signals:
    void stateChanged(QMessageServiceAction::State);
    void messagesFound(const QMessageIdList&);
    void messagesCounted(int);
    void progressChanged(uint, uint);

public:
    QMessageServiceAction* q_ptr;
    bool _active;
    QMessageStore::ErrorCode _lastError;
    QMessageIdList _candidateIds;
    int _count;
    QMessageServiceAction::State _state;
    QMessageId m_bodyDownloadTarget;
    QMessageStore::NotificationFilterId m_bodyDownloadFilterId;
    bool m_registeredUpdates;
    QThread *m_queryThread;
    QList<QThread*> m_obsoleteThreads;
};


void QMessageServiceActionPrivate::completed()
{
    _active = false;
    emit stateChanged(_lastError == QMessageStore::NoError ? QMessageServiceAction::Successful : QMessageServiceAction::Failed);
}

void QMessageServiceActionPrivate::reportMatchingIds()
{
    if (_lastError == QMessageStore::NoError) {
        emit messagesFound(_candidateIds);
    }
    completed();
}

void QMessageServiceActionPrivate::reportMessagesCounted()
{
    if (_lastError == QMessageStore::NoError) {
        emit messagesCounted(_candidateIds.count());
    }
    completed();
}

#ifdef _WIN32_WCE
void QMessageServiceActionPrivate::messageUpdated(const QMessageId& id)
{
    if(id == m_bodyDownloadTarget)
    {
        bool isBodyDownloaded = !isPartiallyDownloaded(id);

        if(isBodyDownloaded)
        {
            unregisterUpdates();
            _state = isBodyDownloaded ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
            emit q_ptr->stateChanged(_state);
            _active = false;
        }
    }
}

#endif

QMessageServiceActionPrivate::QMessageServiceActionPrivate(QMessageServiceAction* parent)
    :q_ptr(parent),
     _active(false),
     _state(QMessageServiceAction::Pending),
     m_registeredUpdates(false),
     m_queryThread(0)
{
}

QMessageServiceActionPrivate::~QMessageServiceActionPrivate()
{
    qDeleteAll(m_obsoleteThreads);
    delete m_queryThread;
    QMessageStore::instance()->unregisterNotificationFilter(m_bodyDownloadFilterId);
}

static Lptstr createMCFRecipients(const QMessageAddressList& addressList, QMessageAddress::Type filterAddressType)
{
    QStringList temp;

    foreach(const QMessageAddress& a, addressList)
    {
        if(a.type() == filterAddressType)
            temp.append(a.recipient());
    }

    return temp.isEmpty() ? Lptstr(0) : LptstrFromQString(temp.join(";"));
}

bool QMessageServiceActionPrivate::send(const QMessage& message, bool showComposer)
{
#ifdef _WIN32_WCE
    if(showComposer)
    {
        MAILCOMPOSEFIELDS mcf;
        memset(&mcf,0,sizeof(mcf));

        Lptstr accountName = LptstrFromQString(QMessageAccount(message.parentAccountId()).name());
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

            to = createMCFRecipients(message.to(),QMessageAddress::Email);
            cc = createMCFRecipients(message.cc(),QMessageAddress::Email);
            bcc = createMCFRecipients(message.bcc(),QMessageAddress::Email);
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

            to = createMCFRecipients(message.to(),QMessageAddress::Phone);
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
        else
            qWarning() << "Unsupported message type";

       mcf.cbSize = sizeof(mcf);

       if(FAILED(MailComposeMessage(&mcf)))
       {
           qWarning() << "MailComposeMessage failed";
           return false;
       }
    }
    else
    {
#endif

    MapiSessionPtr mapiSession(MapiSession::createSession(&_lastError));
    if (_lastError != QMessageStore::NoError)
    {
        qWarning() << "Could not create session";
        return false;
    }

    QMessage outgoing(message);

    if(!outgoing.parentAccountId().isValid())
    {
        qWarning() << "No valid account set for message, attempting to use default for message type...";

        QMessageAccountId defaultAccountId = QMessageAccount::defaultAccount(message.type());
        if(!defaultAccountId.isValid())
        {
            _lastError = QMessageStore::ConstraintFailure;
            qWarning() << "No default account available for message type.";
            return false;
        }

        outgoing.setParentAccountId(defaultAccountId);
    }

    MapiStorePtr mapiStore = mapiSession->findStore(&_lastError, outgoing.parentAccountId(),false);

    if(mapiStore.isNull() || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to get store for the account";
        return false;
    }

    //try first to create message in outbox for store, failing that attempt draft

    MapiFolderPtr mapiFolder = mapiStore->findFolder(&_lastError,QMessage::OutboxFolder);

    if( mapiFolder.isNull() || _lastError != QMessageStore::NoError ) {
        qWarning() << "Unable to get outbox folder for outgoing store, trying drafts...";
        mapiFolder = mapiStore->findFolder(&_lastError,QMessage::DraftsFolder);
        if(mapiFolder.isNull() || _lastError != QMessageStore::NoError) {
            qWarning() << "Unable to get drafts folder for outgoing store";
            return false;
        }
    }

    IMessage* mapiMessage = mapiFolder->createMessage(&_lastError, outgoing, mapiSession, DontSavePropertyChanges);

    if(!mapiMessage || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to create MAPI message from source";
        mapiRelease(mapiMessage);
        return false;
    }

#ifndef _WIN32_WCE
    if(showComposer)
    {
        if(FAILED(mapiSession->showForm(mapiMessage,mapiFolder->folder(),mapiStore->store())))
        {
            qWarning() << "MAPI ShowForm failed.";
            _lastError = QMessageStore::FrameworkFault;
            mapiRelease(mapiMessage);
            return false;
        }
    }
    else
#endif
    {
        if(FAILED(mapiMessage->SubmitMessage(0)))
        {
            qWarning() << "MAPI SubmitMessage failed.";
            _lastError = QMessageStore::FrameworkFault;
            mapiRelease(mapiMessage);
            return false;
        }
    }

    mapiRelease(mapiMessage);

#ifdef _WIN32_WCE
    }
#endif

    return true;
}

bool QMessageServiceActionPrivate::show(const QMessageId& messageId)
{
#ifdef _WIN32_WCE
    MapiEntryId entryId = QMessageIdPrivate::entryId(messageId);
    LPENTRYID entryIdPtr(reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())));
    if(FAILED(MailDisplayMessage(entryIdPtr,entryId.length())))
    {
        qWarning() << "MailDisplayMessage failed";
        return false;
    }
    return true;

#else
    if(!messageId.isValid())
    {
        _lastError = QMessageStore::InvalidId;
        qWarning() << "Invalid message id";
        return false;
    }

    MapiSessionPtr mapiSession(MapiSession::createSession(&_lastError));
    if (_lastError != QMessageStore::NoError)
    {
        qWarning() << "Could not create seesion";
        return false;
    }

    //messageId -> IMapiStore
    //messageId -> IMapiFolder
    //messageId -> IMessage

    MapiEntryId entryId = QMessageIdPrivate::entryId(messageId);
    MapiRecordKey folderRecordKey = QMessageIdPrivate::folderRecordKey(messageId);
    MapiRecordKey storeRecordKey = QMessageIdPrivate::storeRecordKey(messageId);

    MapiStorePtr mapiStore = mapiSession->findStore(&_lastError,QMessageAccountIdPrivate::from(storeRecordKey));

    if(mapiStore.isNull() || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to get store for the message";
        return false;
    }

    MapiFolderPtr mapiFolder = mapiStore->openFolderWithKey(&_lastError,folderRecordKey);

    if( mapiFolder.isNull() || _lastError != QMessageStore::NoError ) {
        qWarning() << "Unable to get folder for the message";
        return false;
    }

    IMessage* mapiMessage = mapiFolder->openMessage(&_lastError,entryId);

    if(!mapiMessage || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to create MAPI message from source";
        mapiRelease(mapiMessage);
        return false;
    }

    if(FAILED(mapiSession->showForm(mapiMessage,mapiFolder->folder(),mapiStore->store())))
    {
        qWarning() << "MAPI ShowForm failed.";
        _lastError = QMessageStore::FrameworkFault;
        mapiRelease(mapiMessage);
        return false;
    }

    mapiRelease(mapiMessage);

    return true;
#endif
}

#ifdef _WIN32_WCE

bool QMessageServiceActionPrivate::isPartiallyDownloaded(const QMessageId& id, bool considerAttachments)
{
    MapiSessionPtr mapiSession(MapiSession::createSession(&_lastError));

    if (_lastError != QMessageStore::NoError)
    {
        qWarning() << "Could not create session";
        return false;
    }

    MapiStorePtr mapiStore = mapiSession->openStore(&_lastError,QMessageIdPrivate::storeRecordKey(id));

    if(mapiStore.isNull() || _lastError != QMessageStore::NoError) {
        qWarning() << "Unable to get store for the message id";
        return false;
    }

    IMessage* message = mapiStore->openMessage(&_lastError,QMessageIdPrivate::entryId(id));

    ULONG status = 0;
    if(!getMapiProperty(message,PR_MSG_STATUS,&status)) {
        qWarning() << "Unable to get message flags";
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

bool QMessageServiceActionPrivate::markForDownload(const QMessageId& id, bool includeAttachments)
{
    MapiSessionPtr mapiSession(MapiSession::createSession(&_lastError));

    if (_lastError != QMessageStore::NoError)
    {
        qWarning() << "Could not create session";
        return false;
    }

    MapiStorePtr mapiStore = mapiSession->openStore(&_lastError,QMessageIdPrivate::storeRecordKey(id));

    if(mapiStore.isNull() || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to get store for the message id";
        return false;
    }

    IMessage* message = mapiStore->openMessage(&_lastError,QMessageIdPrivate::entryId(id));

    ULONG status = 0;

    if(!getMapiProperty(message,PR_MSG_STATUS,&status))
    {
        qWarning() << "Unable to get message flags";
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
            qWarning() << "Could not mark the message for download!";
            return false;
        }

        mapiRelease(message);

        //perform the synchronization on the store

        QString transportName = mapiStore->transportName();
        if(transportName.isEmpty())
        {
            qWarning() << "Could not get transport name for mapi store";
            return false;
        }

    }
    return true;
}

bool QMessageServiceActionPrivate::synchronize(const QMessageAccountId& id)
{
    QMessageAccount account(id);
    if(FAILED(MailSyncMessages(LptstrFromQString(account.name()),MCF_ACCOUNT_IS_NAME | MCF_RUN_IN_BACKGROUND)))
    {
        qWarning() << "MailSyncMessages failed for account " << account.name();
        return false;
    }
    return true;
}

bool QMessageServiceActionPrivate::registerUpdates(const QMessageId& targetMessage)
{
    if(!m_registeredUpdates)
    {
        connect(QMessageStore::instance(),SIGNAL(messageUpdated(const QMessageId&, const QMessageStore::NotificationFilterIdSet&)),this,SLOT(messageUpdated(const QMessageId&)));
        m_bodyDownloadFilterId = QMessageStore::instance()->registerNotificationFilter(QMessageFilter());
        m_bodyDownloadTarget = targetMessage;
        m_registeredUpdates = true;
    }
    return m_registeredUpdates;
}

void QMessageServiceActionPrivate::unregisterUpdates()
{
    disconnect(QMessageStore::instance(),SIGNAL(messageUpdated(const QMessageId&, const QMessageStore::NotificationFilterIdSet&)),this,SLOT(messageUpdated(const QMessageId&)));
    QMessageStore::instance()->unregisterNotificationFilter(m_bodyDownloadFilterId);
    m_bodyDownloadFilterId = 0;
    m_registeredUpdates = false;
}

#endif

namespace {

class QueryThread : public QThread
{
    Q_OBJECT

    QMessageServiceActionPrivate *_parent;
    QMessageFilter _filter;
    QString _body;
    QMessageDataComparator::Options _options;
    QMessageOrdering _ordering;
    uint _limit;
    uint _offset;

public:
    QueryThread(QMessageServiceActionPrivate *parent, const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset)
        : QThread(),
          _parent(parent),
          _filter(filter),
          _body(body),
          _options(options),
          _ordering(ordering),
          _limit(limit),
          _offset(offset)
    {
        // Ensure that the main thread has instantiated the store before we access it from another thread
        (void)QMessageStore::instance();
    }

    void run()
    {
        // Ensure that this thread has initialized MAPI
        WinHelpers::MapiInitializationToken token(WinHelpers::initializeMapi());

        _parent->_candidateIds = QMessageStore::instance()->queryMessages(_filter, _body, _options, _ordering, _limit, _offset);
        _parent->_lastError = QMessageStore::instance()->lastError();
        emit completed();
    }

signals:
    void completed();
};

}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServiceActionPrivate(this))
{
    connect(d_ptr, SIGNAL(stateChanged(QMessageServiceAction::State)),
        this, SIGNAL(stateChanged(QMessageServiceAction::State)));
    connect(d_ptr, SIGNAL(messagesFound(const QMessageIdList&)),
        this, SIGNAL(messagesFound(const QMessageIdList&)));
    connect(d_ptr, SIGNAL(messagesCounted(int)),
        this, SIGNAL(messagesCounted(int)));
    connect(d_ptr, SIGNAL(progressChanged(uint, uint)),
        this, SIGNAL(progressChanged(uint, uint)));
}

QMessageServiceAction::~QMessageServiceAction()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset)
{
    return queryMessages(filter, QString(), QMessageDataComparator::Options(), ordering, limit, offset);
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset)
{
    if (d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = true;
    d_ptr->_lastError = QMessageStore::NoError;
    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);

    // Perform the query in another thread to keep the UI thread free
    QueryThread *query = new QueryThread(d_ptr, filter, body, options, ordering, limit, offset);
    connect(query, SIGNAL(completed()), d_ptr, SLOT(reportMatchingIds()), Qt::QueuedConnection);
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

bool QMessageServiceAction::countMessages(const QMessageFilter &filter)
{
    if (d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = true;
    d_ptr->_lastError = QMessageStore::NoError;
    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);

    // Perform the query in another thread to keep the UI thread free
    QueryThread *query = new QueryThread(d_ptr, filter, QString(), QMessageDataComparator::Options(), QMessageOrdering(), 0, 0);
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

bool QMessageServiceAction::send(QMessage &message)
{
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = true;

    d_ptr->_lastError = QMessageStore::NoError;

    bool result = d_ptr->send(message);

    d_ptr->_state = result ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);

    d_ptr->_active = false;

    return result;
}

bool QMessageServiceAction::compose(const QMessage &message)
{
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = true;

    d_ptr->_lastError = QMessageStore::NoError;

    bool result = d_ptr->send(message,true);

    d_ptr->_state = result ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;

    return result;
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
    Q_UNUSED(id);
    return true;
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
#ifdef _WIN32_WCE
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_lastError = QMessageStore::NoError;
    d_ptr->_active = true;

    QMessage message(id);

    bool result = true;

    if(message.type() == QMessage::Email) //already has body
    {
        if(d_ptr->isPartiallyDownloaded(id))
        {
            result = d_ptr->markForDownload(id,true);
            result &= d_ptr->registerUpdates(id);
            result &= d_ptr->synchronize(message.parentAccountId());
        }
    }

    return result;
#else
    Q_UNUSED(id);
    return false;
#endif
}

bool QMessageServiceAction::retrieve(const QMessageId &aMessageId, const QMessageContentContainerId& id)
{
    Q_UNUSED(aMessageId)
#ifdef _WIN32_WCE

    if(!aMessageId.isValid())
        return false;

    QMessage message(aMessageId);

    bool isBodyContainer = message.bodyId() == id;

    if(isBodyContainer)
        return retrieveBody(aMessageId);

    //TODO download message attachment programatically using MAPI impossible?
#endif

    return false;
}

bool QMessageServiceAction::show(const QMessageId& id)
{
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_lastError = QMessageStore::NoError;
    d_ptr->_active = true;

    bool result = d_ptr->show(id);

    d_ptr->_state = result ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;

    return result;
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id);
    return true;
}

QMessageServiceAction::State QMessageServiceAction::state() const
{
    return d_ptr->_state;
}

void QMessageServiceAction::cancelOperation()
{
    //NOOP
}

QMessageStore::ErrorCode QMessageServiceAction::lastError() const
{
    return d_ptr->_lastError;
}

#include <qmessageserviceaction_win.moc>

QTM_END_NAMESPACE
