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
#include <QObject>

extern "C" {

#include <glib.h>
#include <rtcom-eventlogger/eventlogger.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

};

QTM_BEGIN_NAMESPACE


class EventLoggerEngine
{
    
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

signals:

public slots:

private:
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
