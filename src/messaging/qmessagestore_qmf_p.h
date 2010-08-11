/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qmobilityglobal.h>
#include <qmailstore.h>
#include "qmfhelpers_p.h"
#include "qmessagestore.h"

QTM_BEGIN_NAMESPACE

class QMessageStorePrivate : public QObject
{
    Q_OBJECT

public:
    QMessageStorePrivate() 
        : QObject(), 
          _store(QmfHelpers::mailStoreInstance()), 
          _error(QMessageManager::NoError), 
          _notify(true),
          _filterId(0)
    {
    }

    QMailStore *_store;
    QMessageManager::Error _error;
    bool _notify;
    QMessageManager::NotificationFilterId _filterId;
    QMap<QMessageManager::NotificationFilterId, QMailMessageKey> _filters;

    static QMailStore *convert(QMessageStore *store);
    static QMailStore *convert(QMessageManager &manager);

    Q_SCOPED_STATIC_DECLARE(QMessageStore,storeInstance);

    static void registerMessageStatus(QMailStore *store, const QString &field);
    static void createNonexistentFolder(QMailStore *store, const QString &path, quint64 status);
    
public slots:
    void messagesAdded(const QMailMessageIdList &ids);
    void messagesRemoved(const QMailMessageIdList &ids);
    void messagesUpdated(const QMailMessageIdList &ids);

signals:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

private:
    void processFilters(const QMailMessageIdList &ids, void (QMessageStorePrivate::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &))
    {
        QMap<QMailMessageId, QMessageManager::NotificationFilterIdSet> matches;

        // Copy the filter map to protect against modification during traversal
        QMap<QMessageManager::NotificationFilterId, QMailMessageKey> filters(_filters);
        QMap<QMessageManager::NotificationFilterId, QMailMessageKey>::const_iterator it = filters.begin(), end = filters.end();
        for ( ; it != end; ++it) {
            const QMailMessageKey &key(it.value());

            QSet<QMailMessageId> filteredIds;
            if (!key.isEmpty()) {
                // Empty key matches all messages; otherwise get the filtered set
                filteredIds = _store->queryMessages(key).toSet();
            }

            foreach (const QMailMessageId &id, ids) {
                if (key.isEmpty() || filteredIds.contains(id)) {
                    matches[id].insert(it.key());
                }
            }
        }

        QMap<QMailMessageId, QMessageManager::NotificationFilterIdSet>::const_iterator mit = matches.begin(), mend = matches.end();
        for ( ; mit != mend; ++mit) {
            emit (this->*signal)(QmfHelpers::convert(mit.key()), mit.value());
        }
    }
};

QTM_END_NAMESPACE


