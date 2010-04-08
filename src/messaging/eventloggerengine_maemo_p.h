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

    QMessageIdList m_ids;


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
