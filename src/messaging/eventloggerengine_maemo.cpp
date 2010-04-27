#include "eventloggerengine_maemo_p.h"
#include "telepathyengine_maemo_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE


Q_GLOBAL_STATIC(EventLoggerEngine,eventLoggerEngine);


EventLoggerEngine* EventLoggerEngine::instance()
{
    return eventLoggerEngine();
}


EventLoggerEngine::EventLoggerEngine(QObject *parent)
{
  Q_UNUSED(parent);
  //    qDebug() << "EventLoggerEngine::EventLoggerEngine";
    DBusError err=DBUS_ERROR_INIT;
    g_type_init();
    dbus = dbus_bus_get(DBUS_BUS_SESSION, &err); // Create dummy Dbus object and
    dbus_connection_setup_with_g_main (dbus, NULL); //add it to g_mainloop because eventlogger library expects that someone alse has added session bus to g_mainloop
    el=rtcom_el_new ();
    if(!RTCOM_IS_EL(el)) qDebug() << "EventLoggerEngine::EventLoggerEngine():Could't create RTComEl\n";



    g_signal_connect(G_OBJECT(el), "new-event", G_CALLBACK(new_event_cb),(void*)this);
}

void EventLoggerEngine::new_event_cb(RTComEl *el,int event_id,
                                    const char *local_uid,const char *remote_uid,const char *remote_ebook_uid,
                                    const char *group_uid,const char *service,EventLoggerEngine *p)
{
  p->newEvent(event_id, local_uid,remote_uid ,remote_ebook_uid,group_uid,service);
};

void EventLoggerEngine::newEvent(int event_id,
                                const char *local_uid,const char *remote_uid,const char *remote_ebook_uid,
                                const char *group_uid,const char *service)
{
  Q_UNUSED(local_uid); Q_UNUSED(remote_uid);Q_UNUSED(remote_ebook_uid);
  Q_UNUSED(group_uid);Q_UNUSED(service);
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
QMessage EventLoggerEngine::eventToMessage(RTComElEvent & ev)
{

    QMessage message;

    if (!strcmp(ev.fld_service, "RTCOM_EL_SERVICE_SMS")) {
        message.setType(QMessage::Sms);
    } else if (!strcmp(ev.fld_service,"RTCOM_EL_SERVICE_CHAT")) {
        message.setType(QMessage::InstantMessage);
    } else {
        message.setType(QMessage::NoType);  // Other type, as exampele voice Call
    };

    message.setParentAccountId(QMessageAccountId(QString("y/Account/%1").arg(ev.fld_local_uid)));

    if (!ev.fld_is_read) {
        message.setStatus(QMessage::Read);
    };
    message.setPriority(QMessage::NormalPriority);
    message.setDate(QDateTime::fromTime_t(ev.fld_start_time));
    message.setReceivedDate(QDateTime::fromTime_t(ev.fld_start_time));
    if (ev.fld_outgoing) QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
    else
      QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
    //    qDebug() << "event_type:"  << ev.fld_event_type << ev.fld_event_type_id << "Outgoing:" << ev.fld_outgoing << " Folder:" << message.standardFolder();
    message.setFrom(QMessageAddress(QMessageAddress::Phone, QString(ev.fld_remote_uid)));
    QMessagePrivate::setSenderName(message, QString(ev.fld_remote_uid));
    QMessageAddressList messageAddresslist;
    messageAddresslist.append(QMessageAddress(QMessageAddress::Phone, QString(ev.fld_local_uid)));
    message.setTo(messageAddresslist);
    message.setBody(QString(ev.fld_free_text));
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(QString::number(ev.fld_id));
    privateMessage->_modified = false;
    // qDebug() << "id:" << message.id().toString() << "From:" << message.from().addressee() << "Text:" << message.textContent();
    return message;

}


bool EventLoggerEngine::deleteMessage(const QMessageId& id)
{
  int status=rtcom_el_delete_event(el,id.toString().toInt(),NULL);
  return status==0;
}

QMessage EventLoggerEngine::message(const QMessageId& id)
{

    QMessage message;

    // qDebug() << "EventLoggerEngine::getMessage id=" << id.toString();

    RTComElEvent ev;
    bzero(&ev,sizeof(ev));
    RTComElQuery *q=rtcom_el_query_new(el);
    rtcom_el_query_prepare(q,"id",id.toString().toInt(),RTCOM_EL_OP_EQUAL,NULL);
    RTComElIter *iter=rtcom_el_get_events(el,q);
    g_object_unref(q);
    if(iter && rtcom_el_iter_first(iter))
    {
     gboolean res=rtcom_el_iter_get_full(iter,&ev);
     if(res) {
#if 0
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
#endif
         if (!strcmp(ev.fld_service, "RTCOM_EL_SERVICE_SMS")) {
             message.setType(QMessage::Sms);
         } else if (!strcmp(ev.fld_service,"RTCOM_EL_SERVICE_CHAT")) {
             message.setType(QMessage::InstantMessage);
         } else {
             message.setType(QMessage::NoType);  // Other type, as exampele voice Call
         };
         //QMessageAccount account =  TelepathyEngine::instance()->account(QMessageAccountId(QString("/y/Account%1").arg(ev.fld_local_uid)));

         message.setParentAccountId(QMessageAccountId(QString("/y/Account/%1").arg(ev.fld_local_uid)));
         if (!ev.fld_is_read) {
             message.setStatus(QMessage::Read);
         };
         message.setPriority(QMessage::NormalPriority);
         message.setDate(QDateTime::fromTime_t(ev.fld_start_time));
         message.setReceivedDate(QDateTime::fromTime_t(ev.fld_start_time));
	 if (ev.fld_outgoing) QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
	 else
	   QMessagePrivate::setStandardFolder(message,QMessage::InboxFolder);
         message.setFrom(QMessageAddress(QMessageAddress::Phone, QString(ev.fld_remote_uid)));
         QMessagePrivate::setSenderName(message, QString(ev.fld_remote_uid));
         QMessageAddressList messageAddresslist;
         messageAddresslist.append(QMessageAddress(QMessageAddress::Phone, QString(ev.fld_local_uid)));
         message.setTo(messageAddresslist);
         message.setBody(QString(ev.fld_free_text));
         QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
         privateMessage->_id = id;
         privateMessage->_modified = false;
	 //  qDebug() << "id:" << message.id().toString() << "From:" << message.from().addressee() << "Text:" << message.textContent();
     };
    };
    if(iter) g_object_unref(iter);
    //    debugMessage(message);

    return message;

}

void EventLoggerEngine::debugMessage(QMessage &message)
{
    qDebug() << "id:" << message.id().toString() << "type:" << message.type() << "size:" << message.size() << "status:" << message.status() << "priority:" << message.priority();
    qDebug() << "AccountId:" << message.parentAccountId().toString() << "StantardFolder" << message.standardFolder() << "parenFolderId:" << message.parentFolderId().toString();
    qDebug() << "Date:" << message.date() << "receivedDate:" << message.receivedDate() << "Subject:" << message.subject();
    qDebug() << "From:" << message.from().addressee();
    qDebug() << "To:" << (message.to().isEmpty() ? "**none**" : message.to().first().addressee());
    qDebug() << "Body:" <<message.textContent();
}


void EventLoggerEngine::notification(int eventId, QString service,QMessageStorePrivate::NotificationType notificationType)
{


    QMessageManager::NotificationFilterIdSet matchingFilters;
    // qDebug() << "EventLoggerEngine::notification id=" << eventId;

    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    QMessage msg;
    bool messageRetrieved = false;

    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());
        if (filter.isEmpty()) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        } else {
            QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
            if (!messageRetrieved && (privateMessageFilter->_field == QMessageFilterPrivate::Type)) {
                if (service == "RTCOM_EL_SERVICE_SMS") {
                    msg.setType(QMessage::Sms);
                } else if (service== "RTCOM_EL_SERVICE_CHAT") {
                    msg.setType(QMessage::InstantMessage);
                } else {
                    msg.setType(QMessage::NoType);  // Other type, as eample voice calll
                }
            }
            else if (!messageRetrieved) {
                msg = this->message(QMessageId(QString::number(eventId)));
                if (msg.type() == QMessage::NoType) {
                    matchingFilters.clear();
                    break;
                } else {
                    messageRetrieved = true;
                }
            }
            if (privateMessageFilter->filter(msg)) {
                matchingFilters.insert(it.key());
            }
        }
    }

    if (matchingFilters.count() > 0) {
            ipMessageStorePrivate->messageNotification(notificationType,
                                                       QMessageId(QString::number(eventId)),
                                                       matchingFilters);
        }

}

#if 0
QMessageIdList EventLoggerEngine::filterAndOrderMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
                                                    QString body, QMessageDataComparator::MatchFlags matchFlags)
{
    filterAndOrderMessages(filte, sortOrder, body, matchFlags);
}
#endif

QMessageIdList EventLoggerEngine::filterAndOrderMessages(const QMessageFilter &filter,
                                                    const QMessageSortOrder& sortOrder,
                                                    QString body,
                                                    QMessageDataComparator::MatchFlags matchFlags)
{
  Q_UNUSED(body);
  Q_UNUSED(matchFlags);
  Q_UNUSED(sortOrder);
    QMessageId fId;  // Filtering id
    //    QMessageType fType;
    QDate fDate;
    RTComElEvent ev;
    QMessage message;
    
    const char *services[]={"RTCOM_EL_SERVICE_CHAT","RTCOM_EL_SERVICE_SMS", NULL };

    QMessageIdList idList;

    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filter);
#if 0
    if ((filters.count() == 1) && (pf->_field == QMessageFilterPrivate::None) && (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList
            return idList;
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages

            }
        }
    // Pre-filtering setup
    switch (pf->_field) {
    case QMessageFilterPrivate::Id:
    case QMessageFilterPrivate::ParentAccountId:
        {
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageAccountId
            iNumberOfHandledFilters++;
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                QMessageAccount messageAccount = TelepathyEngine::instance()->account(QMessageAccountId(QString("/y/Account%1").arg(pf->_value.toString())));
            }
        }
       }
    }
#endif
    RTComElQuery *q=rtcom_el_query_new(el);
    rtcom_el_query_prepare(q,"service", services, RTCOM_EL_OP_IN_STRV, NULL);
    RTComElIter *iter=rtcom_el_get_events(el,q);
    g_object_unref(q);
    if(iter && rtcom_el_iter_first(iter))
       do {
         bzero(&ev,sizeof(ev));
         if(rtcom_el_iter_get_full(iter,&ev))
	   {
	     message=eventToMessage(ev);
	     // debugMessage(message);
	     if (pf->filter(message)) {
	       //   qDebug() <<"Filter :filtering match" << message.id().toString();
	       //matchingFilters.insert(it.key());
	       idList.append(message.id());
	     };
	   };
       }
       while( rtcom_el_iter_next(iter));
#if 0
    foreach(const QMessageId& id, idList) {
         	  qDebug() << "id=" << id.toString();
        }
#endif
    return idList;
}

QTM_END_NAMESPACE
