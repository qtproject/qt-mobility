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
          _store(QMailStore::instance()), 
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
                filteredIds = QMailStore::instance()->queryMessages(key).toSet();
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


