#include "eventloggerengine_maemo_p.h"
#include "telepathyengine_maemo_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

EventLoggerEngine::EventLoggerEngine(QObject *parent)
{
    qDebug() << "EventLoggerEngine::EventLoggerEngine";
    DBusError err=DBUS_ERROR_INIT;
    g_type_init();
    dbus = dbus_bus_get(DBUS_BUS_SESSION, &err); // Create dummy Dbus object and
    qDebug() << "Dummy Dbus object created";
    dbus_connection_setup_with_g_main (dbus, NULL); //add it to g_mainloop because eventlogger library expects that someone alse has added session bus to g_mainloop
    el=rtcom_el_new ();
    qDebug() << "rtcom_el_new() done";
    if(!RTCOM_IS_EL(el)) qDebug() << "EventLoggerEngine::EventLoggerEngine():Could't create RTComEl\n";



    g_signal_connect(G_OBJECT(el), "new-event", G_CALLBACK(new_event_cb),(void*)this);
}

void EventLoggerEngine::new_event_cb(RTComEl *el,int event_id,
                                    const char *local_uid,const char *remote_uid,const char *remote_ebook_uid,
                                    const char *group_uid,const char *service,EventLoggerEngine *p)
{
  printf("EventLoggerEngine::new_event(el=%p,event_id=%d local_uid=%s remote=uid=%s ebook=uid=%s group_uid=%s service=%s this=%p)\n",
         el,event_id,
         local_uid ? local_uid :"NULL",
         remote_uid ? remote_uid : "NULL",
         remote_ebook_uid,group_uid,service,p);
  p->newEvent(event_id, local_uid,remote_uid ,remote_ebook_uid,group_uid,service);
};

void EventLoggerEngine::newEvent(int event_id,
                                const char *local_uid,const char *remote_uid,const char *remote_ebook_uid,
                                const char *group_uid,const char *service)
{
    QString eventIds=QString::number(event_id);
    QMessageId id(eventIds);


    notification(event_id,service,QMessageStorePrivate::Added);
}

QMessageManager::NotificationFilterId EventLoggerEngine::registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                                                           const QMessageFilter &filter)
{
    iListenForNotifications = true;
    ipMessageStorePrivate = &aPrivateStore;

    int filterId = ++_filterId;
    _filters.insert(filterId, filter);
    return filterId;
}

void EventLoggerEngine::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    _filters.remove(notificationFilterId);
    if (_filters.count() == 0) {
        iListenForNotifications = false;
    }
}
QMessage EventLoggerEngine::getMessage(const QMessageId& id)
{

    QMessage message;

    qDebug() << "EventLoggerEngine::getMessage id=" << id.toString();

    RTComElEvent ev;
    bzero(&ev,sizeof(ev));
    RTComElQuery *q=rtcom_el_query_new(el);
    rtcom_el_query_prepare(q,"id",id.toString().toInt(),RTCOM_EL_OP_EQUAL,NULL);
    RTComElIter *iter=rtcom_el_get_events(el,q);
    g_object_unref(q);
    if(iter && rtcom_el_iter_first(iter))
    {
     gboolean res=rtcom_el_iter_get(iter,&ev);
     qDebug() << "EventLoggerEngine::getMessage res=" << res;
     if(res) {
         printf("got event id=%d service_id=%d event_typ_id=%d\n\
local_uid=%s local_name=%s\n\
remote_uid=%s remote_name=%s remote_ebook_uid=%s\n\
channel=%s free_text=%s group_uid=%s\n\
service=%s event_type=%s\n\
additional_text=%s icon_name=%s pango_markup=%s\n",
                    ev.fld_id,ev.fld_service_id,ev.fld_event_type_id,
                    ev.fld_local_uid,ev.fld_local_name,
                    ev.fld_remote_uid,ev.fld_remote_name,ev.fld_remote_ebook_uid,
                    ev.fld_channel,ev.fld_free_text,ev.fld_group_uid,
                    ev.fld_service,ev.fld_event_type,
                    ev.fld_additional_text,ev.fld_icon_name,ev.fld_pango_markup);
         if (!strcmp(ev.fld_service, "RTCOM_EL_SERVICE_SMS")) {
             message.setType(QMessage::Sms);
         } else if (!strcmp(ev.fld_service,"RTCOM_EL_SERVICE_CHAT")) {
             message.setType(QMessage::Xmpp);
         } else {
             message.setType(QMessage::NoType);  // Other type, as exampele voice Call
         };
         QMessageAccount account =  TelepathyEngine::instance()->account(QMessageAccountId(QString("/y/Account%1").arg(ev.fld_local_uid)));

         message.setParentAccountId(account.id());
         if (!ev.fld_is_read) {
             message.setStatus(QMessage::Read);
         };
         message.setPriority(QMessage::NormalPriority);
         message.setDate(QDateTime::fromTime_t(ev.fld_start_time));
         message.setReceivedDate(QDateTime::fromTime_t(ev.fld_start_time));
         if (!strcmp(ev.fld_event_type,"RTCOM_EL_EVENTTYPE_SMS_INBOUND") || !strcmp(ev.fld_event_type,"RTCOM_EL_EVENTTYPE_CHAT_INBOUND") )
             QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
         if (!strcmp(ev.fld_event_type,"RTCOM_EL_EVENTTYPE_SMS_OUTBOUND") ||!strcmp(ev.fld_event_type,"RTCOM_EL_EVENTTYPE_CHAT_OUTBOUND") )
             QMessagePrivate::setStandardFolder(message,QMessage::OutboxFolder);
         message.setFrom(QMessageAddress(QMessageAddress::Phone, QString(ev.fld_remote_uid)));
         QMessagePrivate::setSenderName(message, QString(ev.fld_remote_uid));
         QMessageAddressList messageAddresslist;
         messageAddresslist.append(QMessageAddress(QMessageAddress::Phone, QString(ev.fld_local_uid)));
         message.setTo(messageAddresslist);
         message.setBody(QString(ev.fld_free_text));
         qDebug() << "From:" << message.from().recipient() << "Text:" << message.textContent();
         QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
         privateMessage->_id = id;
         privateMessage->_modified = false;
     };
    };
    // rtcom_el_iter_next(iter);
    if(iter) g_object_unref(iter);
    debugMessage(message);

    return message;

}

void EventLoggerEngine::debugMessage(QMessage &message)
{
    qDebug() << "id:" << message.id().toString() << "type:" << message.type() << "size:" << message.size() << "status:" << message.status() << "priority:" << message.priority();
    qDebug() << "AccountId:" << message.parentAccountId().toString() << "StantardFolder" << message.standardFolder() << "parenFolderId:" << message.parentFolderId().toString();
    qDebug() << "Date:" << message.date() << "receivedDate:" << message.receivedDate() << "Subject:" << message.subject();
    qDebug() << "From:" << message.from().recipient();
    qDebug() << "To:" << (message.to().isEmpty() ? "**none**" : message.to().first().recipient());
    qDebug() << "Body:" <<message.textContent();
}


void EventLoggerEngine::notification(int eventId, QString service,QMessageStorePrivate::NotificationType notificationType)
{


    QMessageManager::NotificationFilterIdSet matchingFilters;
    qDebug() << "EventLoggerEngine::notification id=" << eventId;

    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    QMessage msg;
    bool messageRetrieved = false;

    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());
        qDebug() <<"Filter :";
        if (filter.isEmpty()) {
             qDebug() <<"Filter.isEmpty()";
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        } else {
            QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
            if (!messageRetrieved && (privateMessageFilter->_field == QMessageFilterPrivate::Type)) {
                qDebug() <<"Filter::Type";
                if (service == "RTCOM_EL_SERVICE_SMS") {
                    msg.setType(QMessage::Sms);
                } else if (service== "RTCOM_EL_SERVICE_CHAT") {
                    msg.setType(QMessage::Xmpp);
                } else {
                    msg.setType(QMessage::NoType);  // Other type, as eample voice calll
                }
            }
            else if (!messageRetrieved) {
                qDebug() <<"Filter :retrieving message";
                msg = getMessage(QMessageId(QString::number(eventId)));
                qDebug() <<"Filter :retrieved message";
                debugMessage(msg);
                if (msg.type() == QMessage::NoType) {
                    matchingFilters.clear();
                    break;
                } else {
                    messageRetrieved = true;
                }
            }
            qDebug() <<"Filter :filtering";
            if (privateMessageFilter->filter(msg)) {
                qDebug() <<"Filter :filtering match";
                matchingFilters.insert(it.key());
            }
        }
    }

qDebug() << "EventLoggerEngine::notification matchingFilters.count()=" << matchingFilters.count();
    if (matchingFilters.count() > 0) {
            ipMessageStorePrivate->messageNotification(notificationType,
                                                       QMessageId(QString::number(eventId)),
                                                       matchingFilters);
        }

}
QTM_END_NAMESPACE
