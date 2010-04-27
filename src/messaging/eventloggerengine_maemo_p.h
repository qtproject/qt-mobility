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

#ifndef EVENTLOGGERENGINE_H
#define EVENTLOGGERENGINE_H
#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageservice.h"
#include "qmessage.h"
#include "qmessagemanager.h"
#include "maemohelpers_p.h"
#include "qmessagestore_p.h"
#include "qmessage_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qmessagefolderfilter_p.h"
#include <QThread>
#include <QObject>

extern "C" {

#include <glib.h>
#include <rtcom-eventlogger/eventlogger.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

};

QTM_BEGIN_NAMESPACE

class EventLoggerEngine;

class QueryThread : public QThread
{
    Q_OBJECT


public:
    QueryThread();
    void setArgs(EventLoggerEngine *parent, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset);
    void run();

 signals:
    void completed();
 public:
    EventLoggerEngine *_parent;
    QMessageFilter _filter;
    QString _body;
    QMessageDataComparator::MatchFlags _matchFlags;
    QMessageSortOrder _sortOrder;
    uint _limit;
    uint _offset;

};


class EventLoggerEngine:public QObject
{
    Q_OBJECT
    
public:

    explicit EventLoggerEngine(QObject *parent = 0);
    static EventLoggerEngine* instance();
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);
    QMessageManager::NotificationFilterId registerNotificationFilter(QMessageStorePrivate& aPrivateStore,const QMessageFilter &filter);
    QMessage message(const QMessageId& id);
    bool deleteMessage(const QMessageId& id);
    QMessage eventToMessage(RTComElEvent & ev);
    void notification(int eventId, QString servive,QMessageStorePrivate::NotificationType notificationType);
    static void new_event_cb(RTComEl *el,int event_id,
                             const char *local_uid,const char *remote_uid,const char *remote_ebook_uid,
                             const char *group_uid,const char *service,EventLoggerEngine *p);
    void newEvent(int event_id,
                  const char *local_uid,const char *remote_uid,const char *remote_ebook_uid,
                  const char *group_uid,const char *service);
    QMessageIdList filterAndOrderMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
				      QString body, QMessageDataComparator::MatchFlags matchFlags);

    bool filterMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
				      QString body, QMessageDataComparator::MatchFlags matchFlags);
<<<<<<< HEAD

    QMessageIdList m_ids;


=======

    QMessageIdList m_ids;


>>>>>>> 34ff85ad53564b6053a9bd702fe71a05be2f9da3
signals:
    void messagesFound(const QMessageIdList &,bool,bool);
    void stateChanged(QMessageService::State);
public slots:
    void reportMatchingIds();
    void messagesFound_(const QMessageIdList &ids);
    void completed();
private:
    bool active;
    QMessageService::State state;
    //    EventLoggerThread queryThread;
    QueryThread *queryThread;
    QMessageStorePrivate* ipMessageStorePrivate;
    void debugMessage(QMessage &message);
    RTComEl *el;
    DBusConnection   * dbus;
    bool iListenForNotifications;
    QMessageManager::NotificationFilterId _filterId;
    QMap<QMessageManager::NotificationFilterId, QMessageFilter> _filters;

};

QTM_END_NAMESPACE
#endif // EVENTLOGGERENGINE_H
