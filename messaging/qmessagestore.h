/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QMESSAGESTORE_H
#define QMESSAGESTORE_H
#include <QObject>
#include <qmessageglobal.h>
#include <qmessagefilter.h>
#include <qmessageordering.h>
#include <qmessagefolderordering.h>
#include <qmessageaccountordering.h>
#include <qmessage.h>
#include <qmessagefolder.h>
#include <qmessageaccount.h>

class QMessageStorePrivate;

class Q_MESSAGING_EXPORT QMessageStore : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QMessageStore)

public:
    enum RemovalOption
    {
        RemoveLocalCopyOnly = 1,
        RemoveOnOriginatingServer
    };

    enum ErrorCode
    {
        NoError = 0,
        InvalidId,
        ConstraintFailure,
        ContentInaccessible,
        NotYetImplemented,
        FrameworkFault,
        WorkingMemoryOverflow
    };

    QMessageStore::ErrorCode lastError() const;

    QMessageIdList queryMessages(const QMessageFilter &filter = QMessageFilter(), const QMessageOrdering &ordering = QMessageOrdering(), uint limit = 0, uint offset = 0) const;
    QMessageIdList queryMessages(const QString &body, const QMessageFilter &filter = QMessageFilter(), const QMessageOrdering &ordering = QMessageOrdering(), QMessageDataComparator::Options options = 0, uint limit = 0, uint offset = 0) const;
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter = QMessageFolderFilter(), const QMessageFolderOrdering &ordering = QMessageFolderOrdering(), uint limit = 0, uint offset = 0) const;
#endif
    QMessageAccountIdList queryAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter(), const QMessageAccountOrdering &ordering = QMessageAccountOrdering(), uint limit = 0, uint offset = 0) const;
    int countMessages(const QMessageFilter &filter = QMessageFilter()) const;
#ifdef QMESSAGING_OPTIONAL_FOLDER
    int countFolders(const QMessageFolderFilter &filter = QMessageFolderFilter()) const;
#endif
    int countAccounts(const QMessageAccountFilter &filter = QMessageAccountFilter()) const;

    bool addMessage(QMessage *m);
    bool updateMessage(QMessage *m);
    bool removeMessage(const QMessageId &id, RemovalOption option = RemoveOnOriginatingServer);
    bool removeMessages(const QMessageFilter &filter, RemovalOption option = RemoveOnOriginatingServer);

    QMessage message(const QMessageId &id) const;
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolder folder(const QMessageFolderId &id) const;
#endif
    QMessageAccount account(const QMessageAccountId &id) const;

    static QMessageStore* instance();
    
signals:
    void messagesAdded(const QMessageIdList &ids);
    void messagesRemoved(const QMessageIdList &ids);
    void messagesUpdated(const QMessageIdList &ids);

public slots:
    void startNotifications(const QMessageFilter &filter);
    void stopNotifications();

private:
    friend class QGlobalStaticDeleter<QMessageStore>;
    QMessageStore(QObject *parent = 0);
    virtual ~QMessageStore();

    QMessageStorePrivate *d_ptr;
};

#endif
