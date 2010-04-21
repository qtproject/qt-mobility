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
#include "modestengine_maemo_p.h"
#include "maemohelpers_p.h"
#include "qmessage_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessagefolder_p.h"
#include "qmessagestore_p.h"
#include "qmessageservice_maemo_p.h"
#include "qmessagecontentcontainer_maemo_p.h"
#include <QUrl>
#include <QtDBus>
#include <QFileSystemWatcher>
#include <QTimer>

#include <dbus/dbus.h>
#include <QDebug>

#include </usr/include/libmodest-dbus-client-1.0/libmodest-dbus-client/libmodest-dbus-api.h>
#include </usr/include/sys/inotify.h>
#include </usr/include/sys/ioctl.h>
#include </usr/include/sys/fcntl.h>

// Marshall the ModestStringMap data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument,
                          const QtMobility::ModestStringMap &map)
{
    QtMobility::ModestStringMap::const_iterator iter;

    argument.beginMap (QVariant::String, QVariant::String);
    for (iter = map.constBegin(); iter != map.constEnd(); iter++) {
        argument.beginMapEntry();
        argument << iter.key() << iter.value();
        argument.endMapEntry();
    }
    argument.endMap();

    return argument;
}

// Retrieve the ModestStringMap data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument,
                                QtMobility::ModestStringMap &map)
{
    map.clear();

    argument.beginMap();
    while (!argument.atEnd()) {
        QString key, value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        map[key] = value;
    }
    argument.endMap();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const QtMobility::ModestUnreadMessageDBusStruct &unreadMessage)
{
    argument.beginStructure();
    argument << unreadMessage.timeStamp;
    argument << unreadMessage.subject;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                QtMobility::ModestUnreadMessageDBusStruct &unreadMessage)
{
    argument.beginStructure();
    argument >> unreadMessage.timeStamp;
    argument >> unreadMessage.subject;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument,
                          const QtMobility::ModestAccountsUnreadMessagesDBusStruct &unreadMessages)
{
    argument.beginStructure();
    argument << unreadMessages.accountId;
    argument << unreadMessages.accountName;
    argument << unreadMessages.accountProtocol;
    argument << unreadMessages.unreadCount;
    argument << unreadMessages.unreadMessages;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
                                QtMobility::ModestAccountsUnreadMessagesDBusStruct &unreadMessages)
{
    argument.beginStructure();
    argument >> unreadMessages.accountId;
    argument >> unreadMessages.accountName;
    argument >> unreadMessages.accountProtocol;
    argument >> unreadMessages.unreadCount;
    argument >> unreadMessages.unreadMessages;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QtMobility::ModestMessage &message)
{
    argument.beginStructure();
    argument << message.id;
    argument << message.subject;
    argument << message.folder;
    argument << message.sender;
    argument << message.size;
    argument << message.hasAttachment;
    argument << message.isUnread;
    argument << message.timeStamp;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QtMobility::ModestMessage &message)
{
    argument.beginStructure();
    argument >> message.id;
    argument >> message.subject;
    argument >> message.folder;
    argument >> message.sender;
    argument >> message.size;
    argument >> message.hasAttachment;
    argument >> message.isUnread;
    argument >> message.timeStamp;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const QtMobility::MessagingModestMimePart &mimePart)
{
    argument.beginStructure();
    argument << mimePart.mimeType;
    argument << mimePart.isAttachment;
    argument << mimePart.fileName;
    argument << mimePart.contentId;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QtMobility::MessagingModestMimePart &mimePart)
{
    argument.beginStructure();
    argument >> mimePart.mimeType;
    argument >> mimePart.isAttachment;
    argument >> mimePart.fileName;
    argument >> mimePart.contentId;
    argument.endStructure();
    return argument;
}

QTM_BEGIN_NAMESPACE

/* configuration key definitions for modest */
#define MODESTENGINE_ACCOUNT_NAMESPACE         "/apps/modest/accounts"
#define MODESTENGINE_SERVER_ACCOUNT_NAMESPACE  "/apps/modest/server_accounts"
#define MODESTENGINE_DEFAULT_ACCOUNT           "/apps/modest/default_account"
#define MODESTENGINE_ACCOUNT_ENABLED           "enabled"
#define MODESTENGINE_ACCOUNT_DISPLAY_NAME      "display_name"
#define MODESTENGINE_ACCOUNT_FULLNAME          "fullname"
#define MODESTENGINE_ACCOUNT_EMAIL             "email"
#define MODESTENGINE_ACCOUNT_STORE_ACCOUNT     "store_account"
#define MODESTENGINE_ACCOUNT_TRANSPORT_ACCOUNT "transport_account"
#define MODESTENGINE_ACCOUNT_PROTOCOL          "proto"
#define MODESTENGINE_ACCOUNT_USERNAME          "username"
#define MODESTENGINE_ACCOUNT_HOSTNAME          "hostname"
#define MODESTENGINE_ACCOUNT_PORT              "port"

// The modest engine has a new plugin, we need service names for it
#define MODESTENGINE_QTM_PLUGIN_PATH           "/com/nokia/Qtm/Modest/Plugin"
#define MODESTENGINE_QTM_PLUGIN_NAME           "com.nokia.Qtm.Modest.Plugin"

typedef enum {
    MODEST_DBUS_SEARCH_SUBJECT   = (1 << 0),
    MODEST_DBUS_SEARCH_SENDER    = (1 << 1),
    MODEST_DBUS_SEARCH_RECIPIENT = (1 << 2),
    MODEST_DBUS_SEARCH_SIZE      = (1 << 3),
    MODEST_DBUS_SEARCH_BODY      = (1 << 6)
} ModestDBusSearchFlags;

// Specific priority settings to translate to modest priorities
#define MODESTENGINE_HIGH_PRIORITY             2
#define MODESTENGINE_NORMAL_PRIORITY           0
#define MODESTENGINE_LOW_PRIORITY              1

Q_GLOBAL_STATIC(ModestEngine,modestEngine);

ModestEngine::ModestEngine()
 : m_queryIds(0)
{
    qWarning() << "ModestEngine::ModestEngine Starting to initialize";
    g_type_init();
    m_gconfclient = gconf_client_get_default();
    if (!m_gconfclient) {
        qWarning("qtmessaging: could not get gconf client");
    } else {
        updateEmailAccounts();
    }

    qWarning() << "ModestEngine::ModestEngine Connecting to Modest DBus Interface";
    // Setup DBus Interface for Modest
    m_ModestDBusInterface = new QDBusInterface(MODEST_DBUS_SERVICE,
                                               MODEST_DBUS_OBJECT,
                                               MODEST_DBUS_IFACE,
                                               QDBusConnection::sessionBus(),
                                               this);
    if (m_ModestDBusInterface->isValid()) {
        qWarning() << "ModestEngine::ModestEngine Connected to Modest DBus Interface";
    } else {
        qWarning() << "ModestEngine::ModestEngine Unable to connect to Modest DBus Interface";
    }

    // Get notifications of Incoming Messages
    m_ModestDBusInterface->connection().connect(MODEST_DBUS_SERVICE,
                                                MODEST_DBUS_OBJECT,
                                                MODEST_DBUS_IFACE,
                                                MODEST_DBUS_SIGNAL_FOLDER_UPDATED,
                                                this, SLOT(folderUpdatedSlot(QDBusMessage)));

    // Get notifications of message Read/Unread state changes
    m_ModestDBusInterface->connection().connect(MODEST_DBUS_SERVICE,
                                                MODEST_DBUS_OBJECT,
                                                MODEST_DBUS_IFACE,
                                                MODEST_DBUS_SIGNAL_MSG_READ_CHANGED,
                                               this, SLOT(messageReadChangedSlot(QDBusMessage)));

    qWarning() << "ModestEngine::ModestEngine Connecting to Qt Mobility Modest Plugin DBus Interface";
    // Setup Qt Mobility Modest Plugin based DBus Interface for Modest
    m_QtmPluginDBusInterface = new QDBusInterface(MODESTENGINE_QTM_PLUGIN_NAME,
                                                  MODESTENGINE_QTM_PLUGIN_PATH,
                                                  MODESTENGINE_QTM_PLUGIN_NAME,
                                                  QDBusConnection::sessionBus(),
                                                  this);

    if (m_QtmPluginDBusInterface->isValid()) {
        qWarning() << "ModestEngine::ModestEngine Connected to Qt Mobility Modest Plugin DBus Interface";
    } else {
        qWarning() << "ModestEngine::ModestEngine Unable to connect to Qt Mobility Modest Plugin DBus Interface";
    }

    qDBusRegisterMetaType< ModestStringMap >();
    qDBusRegisterMetaType< ModestStringMapList >();

    qRegisterMetaType<INotifyWatcher::FileNotification>();

    qRegisterMetaType<ModestUnreadMessageDBusStruct>();
    qRegisterMetaType<ModestAccountsUnreadMessagesDBusStruct>();
    qDBusRegisterMetaType<ModestMessage>();

    qRegisterMetaType<MessagingModestMimePart>();


    connect(&m_MailFoldersWatcher, SIGNAL(fileChanged(int, QString, uint)),
            this, SLOT(fileChangedSlot(int, QString, uint)));

    watchAllKnownEmailFolders();

    // Get latest messages from each account
    // => This ensures that notifications of all incoming messages will be sent
    int messagesPerAccount = 1;
    QDBusPendingCall pendingCall = m_ModestDBusInterface->asyncCall(MODEST_DBUS_METHOD_GET_UNREAD_MESSAGES,
                                                                    messagesPerAccount);
    QDBusPendingCallWatcher* pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall);
    pendingCallWatcher->setProperty("setOnlyDates", true);
    connect(pendingCallWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(pendingGetUnreadMessagesFinishedSlot(QDBusPendingCallWatcher*)));

    qWarning() << "ModestEngine::ModestEngine Initialized successfully";
}

ModestEngine::~ModestEngine()
{
    g_object_unref(m_gconfclient);
    m_gconfclient=NULL;
}

ModestEngine* ModestEngine::instance()
{
    return modestEngine();
}

MessagingModestMessage ModestEngine::messageFromModest(const QString& accountId, const QString &folderId, const QString& messageId) const
{
    MessagingModestMessage modestMessage;

    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return modestMessage;
    }

    QDBusPendingCall pendingCall = m_QtmPluginDBusInterface->asyncCall("GetMessage",
                                                                       accountId,
                                                                       folderId,
                                                                       messageId);
    QDBusPendingCallWatcher pendingCallWatcher(pendingCall);
    pendingCallWatcher.waitForFinished();

    QDBusMessage msg = pendingCallWatcher.reply();

    if (msg.type() == QDBusMessage::ReplyMessage) {
        modestMessage.id = messageId;
        modestMessage.accountId = accountId;
        modestMessage.folderId = folderId;
        modestMessage.url = msg.arguments()[0].toString();
        modestMessage.mimeType = msg.arguments()[1].toString();
        modestMessage.from = msg.arguments()[2].toString();
        modestMessage.to = msg.arguments()[3].toString();
        modestMessage.cc = msg.arguments()[4].toString();
        modestMessage.bcc = msg.arguments()[5].toString();
        modestMessage.replyTo = msg.arguments()[6].toString();
        modestMessage.subject = msg.arguments()[7].toString();
        modestMessage.dateReceived = msg.arguments()[8].toLongLong();
        modestMessage.dateSent = msg.arguments()[9].toLongLong();
        modestMessage.size = msg.arguments()[10].toLongLong();
        modestMessage.flags = static_cast<MessagingModestMessageFlags>(msg.arguments()[11].toUInt());
        modestMessage.priority = static_cast<MessagingModestMessagePriority>(msg.arguments()[12].toUInt());

        QVariant variant = msg.arguments()[13];
        QDBusArgument argument = variant.value<QDBusArgument>();
        argument >> modestMessage.mimeParts;
    } else {
        modestMessage.dateReceived = 0;
        modestMessage.dateSent = 0;
        modestMessage.size = 0;
        modestMessage.flags = MessagingModestMessageNotDefined;
        modestMessage.priority = MessagingModestMessagePriorityNotDefined;
    }

    return modestMessage;
}

void ModestEngine::updateEmailAccounts() const
{
    iDefaultEmailAccountId = QMessageAccountId();
    iAccounts.clear();

    GSList *accounts;
    GError *error = NULL;

    accounts = gconf_client_all_dirs(m_gconfclient, MODESTENGINE_ACCOUNT_NAMESPACE, &error);
    if (error) {
#ifdef MESSAGING_DEBUG
        qDebug("qtmessaging: failed to list '%s': '%s'", MODESTENGINE_ACCOUNT_NAMESPACE, error->message);
#endif
        g_error_free(error);
    } else {
        gchar *default_account_id = gconf_client_get_string(m_gconfclient, MODESTENGINE_DEFAULT_ACCOUNT, &error);
        if (error) {
            qWarning("qtmessaging: failed to get '%s': %s", MODESTENGINE_DEFAULT_ACCOUNT, error->message);
            g_error_free(error);
        }

        const size_t prefix_len = strlen(MODESTENGINE_ACCOUNT_NAMESPACE) + 1;

        GSList *iter = accounts;
        while (iter) {
            if (!(iter->data)) {
                iter = iter->next;
                continue;
            }

            const gchar* account_key = (const gchar*)iter->data;
            // account_key = /apps/modest/server_accounts/<account id>
            // => take account id from account_key & unescape account id
            gchar* unescaped_account_id = gconf_unescape_key(account_key+prefix_len, strlen(account_key)-prefix_len);

            gboolean account_ok = FALSE;
            // Check if account is enabled
            if (account_key) {
                gchar* key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_ENABLED, NULL);
                account_ok = gconf_client_get_bool(m_gconfclient, key, NULL);
                g_free(key);
            }

            // Check if account store is defined
            if (account_ok) {
                gchar* key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
                gchar* server_account_name = gconf_client_get_string(m_gconfclient, key, NULL);
                if (server_account_name) {
                    gchar* escaped_server_account_name = gconf_escape_key(server_account_name, strlen(server_account_name));
                    g_free(server_account_name);
                    gchar* store_account_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_server_account_name, NULL);
                    if (!gconf_client_dir_exists(m_gconfclient, store_account_key, NULL)) {
                        account_ok = FALSE;
                    }
                    g_free(store_account_key);
                    g_free(escaped_server_account_name);
                }
                g_free(key);
            }

            // Check if account transport is defined
            if (account_ok) {
                gchar* key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_TRANSPORT_ACCOUNT, NULL);
                gchar* server_account_name = gconf_client_get_string(m_gconfclient, key, NULL);
                if (server_account_name) {
                    gchar* escaped_server_account_name = gconf_escape_key(server_account_name, strlen(server_account_name));
                    g_free(server_account_name);
                    gchar* transport_account_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_server_account_name, NULL);
                    if (!gconf_client_dir_exists(m_gconfclient, transport_account_key, NULL)) {
                        account_ok = FALSE;
                    }
                    g_free(transport_account_key);
                    g_free(escaped_server_account_name);
                }
                g_free(key);
            }

            if (account_ok) {
                gchar* escaped_account_id = gconf_escape_key(unescaped_account_id, strlen(unescaped_account_id));
                QString accountId = "MO_"+QString::fromUtf8(escaped_account_id);
                g_free(escaped_account_id);

                gchar* name_key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_DISPLAY_NAME, NULL);
                gchar* account_name = gconf_client_get_string(m_gconfclient, name_key, NULL);
                QString accountName = QString::fromUtf8(account_name);
                g_free(account_name);
                g_free(name_key);

                gchar* email_key = g_strconcat(account_key, "/", MODESTENGINE_ACCOUNT_EMAIL, NULL);
                gchar* email = gconf_client_get_string(m_gconfclient, email_key, NULL);
                QString accountAddress = QString::fromUtf8(email);
                g_free(email);
                g_free(email_key);

                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                                                       accountName,
                                                                       QMessageAddress(QMessageAddress::Email, accountAddress),
                                                                       QMessage::Email);
                iAccounts.insert(accountId, account);

                // Check if newly added account is default account
                if (!strncmp(default_account_id, unescaped_account_id, strlen(default_account_id))) {
                    iDefaultEmailAccountId = accountId;
                }
            }

            g_free(unescaped_account_id);
            g_free(iter->data);
            iter->data = NULL;
            iter = g_slist_next(iter);
        }
        // strings were freed in while loop
        // => it's enough to just free accounts list
        g_slist_free(accounts);
        g_free(default_account_id);
    }
}

QMessageAccountIdList ModestEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                                  uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    Q_UNUSED(sortOrder)
    Q_UNUSED(limit)
    Q_UNUSED(offset)

    QMessageAccountIdList accountIds;

    if (!m_QtmPluginDBusInterface->isValid()) {
        return accountIds;
    }

    updateEmailAccounts();
    foreach (QMessageAccount value, iAccounts) {
        accountIds.append(value.id());
    }

    MessagingHelper::filterAccounts(accountIds, filter);
    isFiltered = true;

    isSorted = false;

    return accountIds;
}

int ModestEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    bool isFiltered, isSorted;
    return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageAccount ModestEngine::account(const QMessageAccountId &id) const
{
    updateEmailAccounts();
    return iAccounts[id.toString()];
}

QMessageAccountId ModestEngine::defaultAccount() const
{
    updateEmailAccounts();
    return iDefaultEmailAccountId;
}

QFileInfoList ModestEngine::localFolders() const
{
    QDir dir(localRootFolder());
    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    QFileInfoList fileInfoList = dir.entryInfoList();
    appendLocalSubFolders(fileInfoList, 0);
    return fileInfoList;
}

void ModestEngine::appendLocalSubFolders(QFileInfoList& fileInfoList, int startIndex) const
{
    int endIndex = fileInfoList.count();
    for (int i=startIndex; i < endIndex; i++) {
        QDir dir(fileInfoList[i].absoluteFilePath());
        if (dir.exists()) {
            dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
            QFileInfoList dirs = dir.entryInfoList();
            for (int j = 0; j < dirs.count(); j++) {
                QString fileName = dirs[j].fileName();
                if (fileName != "cur" && fileName != "new" && fileName != "tmp") {
                    fileInfoList.append(dirs[j]);
                }
            }
        }
    }
    if (fileInfoList.count() > endIndex) {
        appendLocalSubFolders(fileInfoList, endIndex);
    }
}

void ModestEngine::appendIMAPSubFolders(QFileInfoList& fileInfoList, int startIndex) const
{
    int endIndex = fileInfoList.count();
    for (int i=startIndex; i < endIndex; i++) {
        QDir dir(fileInfoList[i].absoluteFilePath()+QString("/subfolders"));
        if (dir.exists()) {
            dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
            fileInfoList.append(dir.entryInfoList());
        }
    }
    if (fileInfoList.count() > endIndex) {
        appendIMAPSubFolders(fileInfoList, endIndex);
    }
}

QString ModestEngine::localRootFolder() const
{
    return QDir::home().absolutePath()+QString("/.modest/local_folders");
}

QString ModestEngine::accountRootFolder(QMessageAccountId& accountId) const
{
    QString modestAccountId = escapeString(modestAccountIdFromAccountId(accountId));

    QString userName;
    QString hostName;
    QString port;
    QString protocol;

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* username_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_USERNAME, NULL);
    gchar* account_username = gconf_client_get_string(m_gconfclient, username_key, NULL);
    userName = QString::fromUtf8(account_username);
    g_free(account_username);
    g_free(username_key);

    gchar* hostname_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_HOSTNAME, NULL);
    gchar* account_hostname = gconf_client_get_string(m_gconfclient, hostname_key, NULL);
    hostName = QString::fromUtf8(account_hostname);
    g_free(account_hostname);
    g_free(hostname_key);

    gchar* port_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PORT, NULL);
    gint account_port = gconf_client_get_int(m_gconfclient, port_key, NULL);
    port = QString::number(account_port);
    g_free(port_key);

    gchar* protocol_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PROTOCOL, NULL);
    gchar* account_protocol = gconf_client_get_string(m_gconfclient, protocol_key, NULL);
    protocol = QString::fromUtf8(account_protocol);
    g_free(account_protocol);
    g_free(protocol_key);

    g_free(escaped_store_account_name);

    if (protocol == "pop") {
        return QDir::home().absolutePath()+"/.modest/cache/mail/"+protocol+"/"+userName+"__"+hostName+"_"+port;
    } else if (protocol == "imap") {
        return QDir::home().absolutePath()+"/.modest/cache/mail/"+protocol+"/"+userName+"__"+hostName+"_"+port+"/folders";
    }
    return QString();
}

QFileInfoList ModestEngine::accountFolders(QMessageAccountId& accountId) const
{
    QFileInfoList fileInfoList;

    EmailProtocol protocol = accountEmailProtocol(accountId);

    if (protocol == ModestEngine::EmailProtocolPop3) {
        QFileInfo fileInfo = QFileInfo(accountRootFolder(accountId)+"/cache");
        fileInfoList.append(fileInfo);
    } else if (protocol == ModestEngine::EmailProtocolIMAP) {
        QDir dir(accountRootFolder(accountId));
        dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        fileInfoList = dir.entryInfoList();
        appendIMAPSubFolders(fileInfoList, 0);
    }

    return fileInfoList;
}

ModestEngine::EmailProtocol ModestEngine::accountEmailProtocol(QMessageAccountId& accountId) const
{
    EmailProtocol protocol = EmailProtocolUnknown;

    QString modestAccountId = escapeString(modestAccountIdFromAccountId(accountId));

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* protocol_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PROTOCOL, NULL);
    gchar* account_protocol = gconf_client_get_string(m_gconfclient, protocol_key, NULL);
    if (QString("pop") == account_protocol) {
        protocol = EmailProtocolPop3;
    } else if (QString("imap") == account_protocol) {
        protocol = EmailProtocolIMAP;
    }
    g_free(account_protocol);
    g_free(protocol_key);

    g_free(escaped_store_account_name);

    return protocol;
}

QString ModestEngine::accountEmailProtocolAsString(const QMessageAccountId& accountId) const
{
    QString protocol;

    QString modestAccountId = escapeString(modestAccountIdFromAccountId(accountId));

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* protocol_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_PROTOCOL, NULL);
    gchar* account_protocol = gconf_client_get_string(m_gconfclient, protocol_key, NULL);
    protocol = QString::fromUtf8(account_protocol);
    g_free(account_protocol);
    g_free(protocol_key);

    g_free(escaped_store_account_name);

    return protocol;
}

QString ModestEngine::accountUsername(QMessageAccountId& accountId) const
{
    QString username;

    QString modestAccountId = escapeString(modestAccountIdFromAccountId(accountId));

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* username_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_USERNAME, NULL);
    gchar* account_username = gconf_client_get_string(m_gconfclient, username_key, NULL);
    username = QString::fromUtf8(account_username);
    g_free(account_username);
    g_free(username_key);

    g_free(escaped_store_account_name);

    return username;
}

QString ModestEngine::accountHostname(QMessageAccountId& accountId) const
{
    QString host;

    QString modestAccountId = escapeString(modestAccountIdFromAccountId(accountId));

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", modestAccountId.toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
    gchar* store_account_name = gconf_client_get_string(m_gconfclient, store_account_key, NULL);
    g_free(store_account_key);
    gchar* escaped_store_account_name = gconf_escape_key(store_account_name, strlen(store_account_name));
    g_free(store_account_name);

    gchar* host_key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", escaped_store_account_name, "/", MODESTENGINE_ACCOUNT_HOSTNAME, NULL);
    gchar* account_host = gconf_client_get_string(m_gconfclient, host_key, NULL);
    host = QString::fromUtf8(account_host);
    g_free(account_host);
    g_free(host_key);

    g_free(escaped_store_account_name);

    return host;
}

QMessageFolderIdList ModestEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder,
                                                uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    Q_UNUSED(sortOrder)
    Q_UNUSED(limit)
    Q_UNUSED(offset)

    QMessageFolderIdList folderIds;

    if (!m_QtmPluginDBusInterface->isValid()) {
        return folderIds;
    }

    updateEmailAccounts();

    if (iAccounts.isEmpty()) {
        return folderIds;
    }

    QFileInfoList localFolders = this->localFolders();
    QString localRootFolder = this->localRootFolder();

    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accountId = value.id();
        QString rootFolder = accountRootFolder(accountId);
        QFileInfoList folders = this->accountFolders(accountId);

        for (int i=0; i < folders.count(); i++) {
            QString filePath = folders[i].absoluteFilePath();
            QString id = accountId.toString()+"&"+accountEmailProtocolAsString(accountId)+"&"+filePath.right(filePath.size()-rootFolder.size()-1);
            id = id.remove("/subfolders");
            folderIds.append(QMessageFolderId(id));
        }

        // Each account sees local folders as account folders
        for (int i=0; i < localFolders.count(); i++) {
            QString filePath = localFolders[i].absoluteFilePath();
            QString id = accountId.toString()+"&"+"maildir"+"&"+filePath.right(filePath.size()-localRootFolder.size()-1);
            folderIds.append(QMessageFolderId(id));
        }
    }

    MessagingHelper::filterFolders(folderIds, filter);
    isFiltered = true;
    isSorted = false;
    return folderIds;
}

int ModestEngine::countFolders(const QMessageFolderFilter &filter) const
{
    bool isFiltered, isSorted;
    return queryFolders(filter, QMessageFolderSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageFolder ModestEngine::folder(const QMessageFolderId &id) const
{
    QString idString = id.toString();
    int endOfAccountId = idString.indexOf('&');
    int endOfProtocolString = idString.lastIndexOf('&');
    QString accountId = idString.left(endOfAccountId);
    QString protocolString = idString.mid(endOfAccountId+1, endOfProtocolString-endOfAccountId-1);
    QString folder = idString.right(idString.length()-idString.lastIndexOf('&')-1);
    QMessageFolderId parentId;
    QString name;
    if (folder.lastIndexOf('/') == -1) {
        // Folder does not have subfolders
        name = folder;
        if ((protocolString == "pop") && (name == "cache")) {
            name = "Inbox";
        }
    } else {
        // Folder has subfolders
        name = folder.right(folder.length()-folder.lastIndexOf('/')-1);
        parentId = idString.left(idString.lastIndexOf('/'));
    }
    return QMessageFolderPrivate::from(id, QMessageAccountId(accountId), parentId, name, folder);
}

void ModestEngine::watchAllKnownEmailFolders()
{
    QFileInfoList localFolders = this->localFolders();

    // Changes in local Email folders can be monitored using directory
    // monitoring. <=> All messages are stored as individual files.
    for (int i=0; i < localFolders.count(); i++) {
        m_MailFoldersWatcher.addDirectory(localFolders[i].absoluteFilePath()+"/cur", IN_MOVED_TO | IN_DELETE);
    }

    // Monitor changes also in root folder to see if new folders are added
    m_MailFoldersWatcher.addDirectory(localRootFolder(), IN_CREATE);

    // Changes in remote Email folders must be monitored using file monitoring.
    // That's because message headers are stored into summary.mmap file (and summary.mmap
    // file maybe the only place that contains message information).
    // => summary.mmap files will be monitored
    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accountId = value.id();
        QString rootFolder = accountRootFolder(accountId);
        EmailProtocol protocol = accountEmailProtocol(accountId);
        QFileInfoList folders = this->accountFolders(accountId);

        for (int i=0; i < folders.count(); i++) {
            if (protocol == ModestEngine::EmailProtocolPop3) {
                QString folder = folders[i].absoluteFilePath();
                m_MailFoldersWatcher.addDirectory(folder, IN_CREATE);
            } else if (protocol == ModestEngine::EmailProtocolIMAP) {
                m_MailFoldersWatcher.addDirectory(folders[i].absoluteFilePath(), IN_MOVED_TO | IN_DELETE);
            }
        }

    }
}

void ModestEngine::fileChangedSlot(int watchDescriptor, QString filePath, uint events)
{
    Q_UNUSED(watchDescriptor)

    if (events & IN_CREATE) {
        if (QFileInfo(filePath).isDir()) {
            // New directory was added
            // => Start watching new folder
            QString newDirPath = QString(filePath.toUtf8());
            m_MailFoldersWatcher.addDirectory(newDirPath + "/cur");
        }
    }

    int filenameBegin = filePath.lastIndexOf('/')+1;
    QString fileName = filePath.mid(filenameBegin,filePath.lastIndexOf('.')-filenameBegin);
    if (!fileName.endsWith("summary")) {
        if (events & (IN_MOVED_TO | IN_CREATE)) {
            if (events != (IN_MOVED_TO | IN_MOVED_FROM)) {
                notification(messageIdFromModestMessageFilePath(filePath), ModestEngine::Added);
            }
        } else if (events & IN_DELETE) {
            notification(messageIdFromModestMessageFilePath(filePath), ModestEngine::Removed);
        }
    }
}

bool ModestEngine::sendEmail(QMessage &message)
{
    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return false;
    }

    ModestStringMap senderInfo;
    ModestStringMap recipients;
    ModestStringMap messageData;
    ModestStringMapList attachments;
    ModestStringMapList images;
    uint priority = 0;
    ModestStringMap headers;

    senderInfo = getModestSenderInfo(message);

    if (senderInfo.isEmpty()) {
        return false;
    }

    recipients = getModestRecipients(message);

    if (recipients.isEmpty()) {
        return false;
    }

    messageData = getModestMessageData(message);
    attachments = getModestAttachments(message);
    images = getModestImages(message);
    priority = getModestPriority(message);
    headers = getModestHeaders(message);

    qDebug() << "Sending D-BUS message";

    QDBusPendingCall call = m_QtmPluginDBusInterface->asyncCall (
            "SendEmail",
            QVariant::fromValue (senderInfo),
            QVariant::fromValue (recipients),
            QVariant::fromValue (messageData),
            QVariant::fromValue (attachments),
            QVariant::fromValue (images),
            priority,
            QVariant::fromValue (headers));

    qDebug() << "Message sent";

    if (call.isError()) {
        qWarning() << "Call failed! " << call.error();
        return false;
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher (call, this);

    connect (watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
             this, SLOT(sendEmailCallEnded(QDBusPendingCallWatcher*)));

    return true;
}

bool ModestEngine::composeEmail(const QMessage &message)
{
    QString mailString("mailto:");
    QList<QMessageAddress> list(message.to());
    if (!list.empty()){
        for (int i = 0; i < list.size(); ++i) {
            if (i == 0) {
                mailString += list.at(i).addressee();
            } else {
                mailString += QString("%2C%20");
                mailString += list.at(i).addressee();
            }
        }
    }

    bool firstParameter = true;

    QString subject = message.subject();
    if (!subject.isEmpty()) {
        if (firstParameter) {
            mailString += QString("?");
            firstParameter = false;
        } else {
            mailString += QString("&");
        }
        mailString += QString("subject=");
        mailString += subject;
    }

    QString body = message.textContent();
    if (!body.isEmpty()) {
        if (firstParameter) {
            mailString += QString("?");
            firstParameter = false;
        } else {
            mailString += QString("&");
        }
        mailString += QString("body=");
        mailString += QString(body);
    }

    QUrl url(mailString);

    QDBusInterface dbusif(MODEST_DBUS_SERVICE, MODEST_DBUS_OBJECT, MODEST_DBUS_IFACE);
    QDBusMessage msg = dbusif.call(MODEST_DBUS_METHOD_MAIL_TO, url.toString());

    return true;
}

bool ModestEngine::showMessage(const QMessageId &id)
{
    QMessage msg = message(id, false);
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(msg);
    if (privateMessage->_url.isEmpty()) {
        return false;
    } else {
        m_ModestDBusInterface->call(MODEST_DBUS_METHOD_OPEN_MESSAGE,
                                    privateMessage->_url);
    }
    return true;
}

bool ModestEngine::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id) // Modest does not offer Account specific updates

    m_ModestDBusInterface->call(MODEST_DBUS_METHOD_SEND_RECEIVE);
    return true;
}

QMessage ModestEngine::message(const QMessageId &id, bool useCache) const
{
    if (useCache) {
        QMessage message = m_messageCache.value(id.toString());
        if (message.type() != QMessage::NoType) {
            return message;
        }
    }

    QString modestAccountId = modestAccountIdFromMessageId(id);
    QString modestFolderId = modestFolderIdFromMessageId(id);
    QString modestMessageId = modestMessageIdFromMessageId(id);

    MessagingModestMessage modestMessage = messageFromModest(modestAccountId,
                                                             modestFolderId,
                                                             modestMessageId);

    if (modestMessage.flags & MessagingModestMessageDeleted) {
        return QMessage();
    }

    if (modestMessage.accountId.isEmpty()) {
        return QMessage();
    }

    return messageFromModestMessage(modestMessage, accountIdFromMessageId(id));
}

QMessage::StandardFolder ModestEngine::standardFolderFromModestFolderId(const QString& modestFolderId) const
{
    if (!QString::compare(modestFolderId, "INBOX", Qt::CaseInsensitive)) {
        return QMessage::InboxFolder;
    } else if (!QString::compare(modestFolderId, "drafts", Qt::CaseInsensitive)) {
        return QMessage::DraftsFolder;
    } else if (!QString::compare(modestFolderId, "sent", Qt::CaseInsensitive)) {
        return QMessage::SentFolder;
    }

    return QMessage::DraftsFolder;
}

QString ModestEngine::modestFolderIdFromStandardFolder(QMessage::StandardFolder standardFolder) const
{
    switch (standardFolder) {
    case QMessage::InboxFolder:  return "INBOX";
    case QMessage::DraftsFolder: return "drafts";
    case QMessage::SentFolder:   return "sent";
    case QMessage::OutboxFolder: return "outbox";
    case QMessage::TrashFolder:  return "thrash";
    }
    return "drafts";
}

QMessage ModestEngine::messageFromModestMessage(const MessagingModestMessage& modestMessage,
                                                QMessageAccountId accountId) const
{
    QMessage message;
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    QMessageContentContainerPrivate* container = QMessagePrivate::containerImplementation(message);

    // Type
    message.setType(QMessage::Email);

    // Parent Account Id
    QMessageAccountId parentAccountId;
    if (accountId.isValid()) {
        parentAccountId = accountId;
    } else {
        parentAccountId = realAccountId(modestMessage);
    }
    message.setParentAccountId(parentAccountId);

    // Parent Folder Id
    QMessageFolderId folderId;
    if (modestMessage.accountId == "local_folders") {
        folderId = folderIdFromModestFolderId(parentAccountId, true, modestMessage.folderId);
    } else {
        // Since Message is not in local folder, message status can be set to Incoming
        privateMessage->_status = privateMessage->_status | QMessage::Incoming;
        folderId = folderIdFromModestFolderId(parentAccountId, false, modestMessage.folderId);
    }
    privateMessage->_parentFolderId = folderId;

    // Message Id
    QMessageId messageId = QMessageId(folderId.toString()+"/"+modestMessage.id);
    privateMessage->_id = messageId;

    // Dates
    message.setDate(QDateTime::fromTime_t(modestMessage.dateSent));
    message.setReceivedDate(QDateTime::fromTime_t(modestMessage.dateReceived));

    // Priority
    switch (modestMessage.priority) {
    case QtMobility::MessagingModestMessageHighPriority:
        message.setPriority(QMessage::HighPriority);
        break;
    case QtMobility::MessagingModestMessageNormalPriority:
        message.setPriority(QMessage::NormalPriority);
        break;
    case QtMobility::MessagingModestMessageLowPriority:
        message.setPriority(QMessage::LowPriority);
        break;
    case QtMobility::MessagingModestMessageSuspendedPriority:
        message.setPriority(QMessage::NormalPriority);
        break;
    }

    // Standard Folder
    QMessagePrivate::setStandardFolder(message,
                                       standardFolderFromModestFolderId(modestMessage.folderId));

    // Body & Attachments handling
    for (int i=0; i < modestMessage.mimeParts.count(); i++) {
        if (!modestMessage.mimeParts[i].isAttachment) {
            // Body
            QByteArray fullMimeType = modestMessage.mimeParts[i].mimeType.toUtf8();
            QString contentId = modestMessage.mimeParts[i].contentId;
            QByteArray fileName = modestMessage.mimeParts[i].fileName.toAscii();

            QByteArray mainType("text");
            QByteArray subType("plain");
            QByteArray charset;

            int index = fullMimeType.indexOf("/");
            if (index != -1) {
                mainType = fullMimeType.left(index).trimmed();

                subType = fullMimeType.mid(index + 1).trimmed();
                index = subType.indexOf(";");
                if (index != -1) {
                    QString remainder = subType.mid(index + 1);
                    subType = subType.left(index).trimmed();

                    QRegExp charsetPattern("charset=(\\S+)");
                    index = charsetPattern.indexIn(remainder);
                    if (index != -1) {
                        charset = charsetPattern.cap(1).toLatin1();
                    }
                }
            }

            if (charset.isEmpty()) {
                charset = "UTF-8";
            }

            QMessageContentContainerId existingBodyId(message.bodyId());
            if (existingBodyId.isValid()) {
                if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
                    // The body content is in the message itself
                    container->_containingMessageId = messageId.toString();
                    container->_attachmentId = contentId;
                    container->_name = fileName;
                    container->_type = mainType;
                    container->_subType = subType;
                    container->_charset = charset;
                    container->_size = 0;
                    container->_available = true;
                } else {
                    // The body content is in the first attachment
                    QMessageContentContainerPrivate *attachmentContainer(QMessageContentContainerPrivate::implementation(*container->attachment(existingBodyId)));
                    attachmentContainer->_containingMessageId = messageId.toString();
                    attachmentContainer->_attachmentId = contentId;
                    attachmentContainer->_name = fileName;
                    attachmentContainer->_type = mainType;
                    attachmentContainer->_subType = subType;
                    attachmentContainer->_charset = charset;
                    attachmentContainer->_size = 0;
                    attachmentContainer->_available = true;
                }
            } else {
                if (container->_attachments.isEmpty()) {
                    // Put the content directly into the message
                    container->_containingMessageId = messageId.toString();
                    container->_attachmentId = contentId;
                    container->_name = fileName;
                    container->_type = mainType;
                    container->_subType = subType;
                    container->_charset = charset;
                    container->_size = 0;
                    container->_available = true;
                    privateMessage->_bodyId = QMessageContentContainerPrivate::bodyContentId();
                } else {
                    // Add the body as the first attachment
                    QMessageContentContainer newBody;
                    QMessageContentContainerPrivate *attachmentContainer = QMessageContentContainerPrivate::implementation(newBody);
                    attachmentContainer->_containingMessageId = messageId.toString();
                    attachmentContainer->_attachmentId = contentId;
                    attachmentContainer->_name = fileName;
                    attachmentContainer->_type = mainType;
                    attachmentContainer->_subType = subType;
                    attachmentContainer->_charset = charset;
                    attachmentContainer->_size = 0;
                    attachmentContainer->_available = true;
                    privateMessage->_bodyId = container->prependContent(newBody);
                }
            }
        } else {
            // Attachment
            QString fullMimeType = modestMessage.mimeParts[i].mimeType;
            QString contentId = modestMessage.mimeParts[i].contentId;
            int slashIndex = fullMimeType.indexOf('/');
            QByteArray mimeType = fullMimeType.left(slashIndex).toAscii();
            QByteArray mimeSubType = fullMimeType.mid(slashIndex+1).toAscii();
            // TODO: Attachment size
            QByteArray fileName = modestMessage.mimeParts[i].fileName.toAscii();
            fileName = fileName.mid(fileName.lastIndexOf('/')+1);
            QString msgId = messageId.toString();
            QMessageContentContainer attachment =
                    QMessageContentContainerPrivate::from(msgId,
                                                          contentId,
                                                          fileName,
                                                          mimeType,
                                                          mimeSubType,
                                                          0);
            appendAttachmentToMessage(message, attachment);
        }
    }

    // From
    if (modestMessage.from.size() > 0) {
        message.setFrom(QMessageAddress(QMessageAddress::Email, modestMessage.from));
        QMessagePrivate::setSenderName(message, modestMessage.from);
    }

    // To
    if (modestMessage.to.size() > 0) {
        QMessageAddressList toAddresses;
        foreach (const QString &element, modestMessage.to.split(",", QString::SkipEmptyParts)) {
            QMessageAddress addr;
            addr.setType(QMessageAddress::Email);
            addr.setAddressee(element.trimmed());
            toAddresses.append(addr);
        }
        message.setTo(toAddresses);
    }

    // Cc
    if (modestMessage.cc.size() > 0) {
        QMessageAddressList ccAddresses;
        foreach (const QString &element, modestMessage.cc.split(",", QString::SkipEmptyParts)) {
            QMessageAddress addr;
            addr.setType(QMessageAddress::Email);
            addr.setAddressee(element.trimmed());
            ccAddresses.append(addr);
        }
        message.setCc(ccAddresses);
    }

    // Bcc
    if (modestMessage.bcc.size() > 0) {
        QMessageAddressList bccAddresses;
        foreach (const QString &element, modestMessage.bcc.split(",", QString::SkipEmptyParts)) {
            QMessageAddress addr;
            addr.setType(QMessageAddress::Email);
            addr.setAddressee(element.trimmed());
            bccAddresses.append(addr);
        }
        message.setBcc(bccAddresses);
    }

    // Subject
    message.setSubject(modestMessage.subject);

    // Size
    privateMessage->_size = modestMessage.size;

    // Read Status
    if (modestMessage.flags & MessagingModestMessageSeen) {
        privateMessage->_status = privateMessage->_status | QMessage::Read;
    }

    // Message MIME type
    QString fullMimeType = modestMessage.mimeType;
    int slashIndex = fullMimeType.indexOf('/');
    QByteArray mimeType = fullMimeType.left(slashIndex).toAscii();
    QByteArray mimeSubType = fullMimeType.mid(slashIndex+1).toAscii();
    container->_type = mimeType.data();
    container->_subType = mimeSubType.data();

    // Modest specific url
    privateMessage->_url = modestMessage.url;

    // Modified flag
    privateMessage->_modified = false;

    return message;
}

void ModestEngine::appendAttachmentToMessage(QMessage& message, QMessageContentContainer& attachment) const
{
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    QMessageContentContainerPrivate* container = QMessagePrivate::containerImplementation(message);

    if (container->_attachments.isEmpty()) {
        QMessageContentContainerId existingBodyId(message.bodyId());
        if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
            // The body content is in the message itself - move it to become the first attachment
            QMessageContentContainer newBody(message);
            QMessageContentContainerPrivate::implementation(newBody)->setDerivedMessage(0);

            container->setContentType("multipart", "mixed", "");
            privateMessage->_bodyId = container->prependContent(newBody);
        } else {
            // This message is now multipart
            container->setContentType("multipart", "mixed", "");
        }

        container->_available = true;
    }

    container->appendContent(attachment);

    bool haveAttachments = !container->_attachments.isEmpty();
    message.setStatus(QMessage::HasAttachments,haveAttachments);

    privateMessage->_modified = true;
}

bool ModestEngine::addMessage(QMessage &message)
{
    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return false;
    }

    QString modestFolder;
    ModestStringMap senderInfo;
    ModestStringMap recipients;
    ModestStringMap messageData;
    ModestStringMapList attachments;
    ModestStringMapList images;
    uint priority = 0;
    ModestStringMap headers;

    qDebug() << __PRETTY_FUNCTION__;

    senderInfo = getModestSenderInfo (message);
    recipients = getModestRecipients (message);
    messageData = getModestMessageData (message);
    attachments = getModestAttachments (message);
    images = getModestImages (message);
    priority = getModestPriority (message);
    headers = getModestHeaders (message);

    QString accountName;
    if (message.parentFolderId().isValid()) {
        modestFolder = modestFolderIdFromFolderId (message.parentFolderId());
        accountName = modestAccountIdFromFolderId(message.parentFolderId());
    } else {
        modestFolder = modestFolderIdFromStandardFolder(message.standardFolder());
        if (message.standardFolder() == QMessage::DraftsFolder) {
            accountName = "local_folders";
        } else {
            accountName = modestAccountIdFromAccountId(message.parentAccountId());
        }
    }
    senderInfo["account-name"] = accountName;

    QDBusPendingCall pendingCall = m_QtmPluginDBusInterface->asyncCall (
            "AddMessage",
            QVariant::fromValue (modestFolder),
            QVariant::fromValue (senderInfo),
            QVariant::fromValue (recipients),
            QVariant::fromValue (messageData),
            QVariant::fromValue (attachments),
            QVariant::fromValue (images),
            priority,
            QVariant::fromValue (headers));

    if (pendingCall.isError()) {
        qWarning() << "DBus call failed! " << pendingCall.error();
        return false;
    }

    QDBusPendingCallWatcher pendingCallWatcher(pendingCall);
    pendingCallWatcher.waitForFinished();
    QDBusMessage msg = pendingCallWatcher.reply();
    if (msg.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Failed to add message via modest: " << msg.errorMessage();
        return false;
    }

    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    QString messageId;
    if (message.parentFolderId().isValid()) {
        messageId = message.parentFolderId().toString()+"/"+msg.arguments()[0].toString();
    } else if (accountName == "local_folders") {
        messageId = message.parentAccountId().toString()+
                    "&maildir&"+modestFolder+"/"+msg.arguments()[0].toString();
    } else {
        messageId = message.parentAccountId().toString()+"&"+
                    accountEmailProtocolAsString(message.parentAccountId())+
                    "&"+modestFolder+"/"+msg.arguments()[0].toString();
    }
    privateMessage->_id = QMessageId(messageId);

    return true;
}

bool ModestEngine::updateMessage(QMessage &message)
{
    Q_UNUSED(message) // TODO:

    return false;
}

bool ModestEngine::removeMessage(const QMessageId &id, QMessageManager::RemovalOption option)
{
    Q_UNUSED(option) // TODO:

    QMessage msg = message(id, false);
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(msg);
    if (privateMessage->_url.isEmpty()) {
        return false;
    } else {
        m_ModestDBusInterface->call(MODEST_DBUS_METHOD_DELETE_MESSAGE,
                                    privateMessage->_url);
        // Make sure that there will instant notification about removed message
        notification(id, ModestEngine::Removed);
    }
    return true;
}

bool ModestEngine::filterMessage(const QMessage& message, QMessageFilterPrivate::SortedMessageFilterList filterList, int start) const
{
    if (filterList.count() > start) {
        for (int j=start; j < filterList.count(); j++) {
            QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filterList[j]);
            if (!pf->filter(message)) {
                return false;
            }
        }
    }
    return true;
}

QMessageIdList ModestEngine::queryMessagesSync(const QMessageFilter &filter, const QMessageSortOrder &sortOrder,
                                               uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    QMessageIdList ids;

    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return ids;
    }

    QMessageServicePrivate* privateService = QMessageServicePrivate::implementation(m_service);
    if (privateService->queryMessages(m_service, filter, sortOrder, limit, offset,
                                      QMessageServicePrivate::EnginesToCallModest)) {
        QObject::connect(&m_service, SIGNAL(stateChanged(QMessageService::State)),
                         this, SLOT(stateChanged(QMessageService::State)));

        m_eventLoop.exec();

        isSorted = m_isSorted;
        isFiltered = m_isFiltered;
        ids = m_ids;
        m_ids.clear();
    }

    return ids;
}

QMessageIdList ModestEngine::queryMessagesSync(const QMessageFilter &filter, const QString &body,
                                               QMessageDataComparator::MatchFlags matchFlags,
                                               const QMessageSortOrder &sortOrder, uint limit, uint offset,
                                               bool &isFiltered, bool &isSorted) const
{
    QMessageIdList ids;

    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return ids;
    }

    QMessageServicePrivate* privateService = QMessageServicePrivate::implementation(m_service);
    if (privateService->queryMessages(m_service, filter, body, matchFlags,
                                      sortOrder, limit, offset,
                                      QMessageServicePrivate::EnginesToCallModest)) {
        QObject::connect(&m_service, SIGNAL(stateChanged(QMessageService::State)),
                         this, SLOT(stateChanged(QMessageService::State)));

        m_eventLoop.exec();

        isSorted = m_isSorted;
        isFiltered = m_isFiltered;
        ids = m_ids;
        m_ids.clear();
    }

    return ids;
}

int ModestEngine::countMessagesSync(const QMessageFilter &filter) const
{
    int count;

    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return 0;
    }

    QMessageServicePrivate* privateService = QMessageServicePrivate::implementation(m_service);
    if (privateService->countMessages(m_service, filter, QMessageServicePrivate::EnginesToCallModest)) {
        QObject::connect(&m_service, SIGNAL(stateChanged(QMessageService::State)),
                         this, SLOT(stateChanged(QMessageService::State)));

        m_eventLoop.exec();

        count = m_count;
    }

    return count;
}

void ModestEngine::stateChanged(QMessageService::State newState)
{
    if (newState == QMessageService::FinishedState) {
        QMessageServicePrivate* privateService = QMessageServicePrivate::implementation(m_service);

        m_ids = privateService->_ids;
        m_isSorted = privateService->_sorted;
        m_isFiltered = privateService->_filtered;
        m_count = privateService->_count;

        m_eventLoop.quit();
    }
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return queryMessages(messageService, filter, QString(), 0, sortOrder, limit, offset);
}

bool ModestEngine::countMessages(QMessageService& messageService, const QMessageFilter &filter)
{
    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return false;
    }

    m_pendingMessageQueries.append(MessageQueryInfo());

    MessageQueryInfo &queryInfo = m_pendingMessageQueries[m_pendingMessageQueries.count()-1];

    queryInfo.filter = filter;
    queryInfo.limit = 0;
    queryInfo.offset = 0;
    queryInfo.privateService = QMessageServicePrivate::implementation(messageService);
    queryInfo.currentFilterListIndex = 0;
    queryInfo.handledFiltersCount = 0;
    queryInfo.isQuery = false;
    queryInfo.returnWithSingleShot = false;

    if (!startQueryingAndFilteringMessages(m_pendingMessageQueries[m_pendingMessageQueries.count()-1])) {
        QMessageServicePrivate::implementation(messageService)->setFinished(false);
        m_pendingMessageQueries.removeAt(m_pendingMessageQueries.count()-1);
        return false;
    }

    return true;
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body,
                                 QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
                                 uint limit, uint offset) const
{
    if (!m_QtmPluginDBusInterface->isValid() || iAccounts.isEmpty()) {
        return false;
    }

    m_pendingMessageQueries.append(MessageQueryInfo());

    MessageQueryInfo &queryInfo = m_pendingMessageQueries[m_pendingMessageQueries.count()-1];

    queryInfo.body = body;
    queryInfo.matchFlags = matchFlags;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.limit = limit;
    queryInfo.offset = offset;
    queryInfo.privateService = QMessageServicePrivate::implementation(messageService);
    queryInfo.currentFilterListIndex = 0;
    queryInfo.handledFiltersCount = 0;
    queryInfo.isQuery = true;
    queryInfo.returnWithSingleShot = false;

    if (!startQueryingAndFilteringMessages(m_pendingMessageQueries[m_pendingMessageQueries.count()-1])) {
        m_pendingMessageQueries.removeAt(m_pendingMessageQueries.count()-1);
        if (m_pendingMessageQueries.count() == 0) {
            // This was last query in pending queries queue
            // => Disconnect from "GetHeaders" request related DBus signals
            // Note: Disconnecting signals is done to optimize signal handling
            //       <=> Disconnecting prevents unnecessary handling of signals
            //           which have been sent from other applications using
            //           Qt Mobility Messaging API
            m_QtmPluginDBusInterface->connection().disconnect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                              MODESTENGINE_QTM_PLUGIN_PATH,
                                                              MODESTENGINE_QTM_PLUGIN_NAME,
                                                              "HeadersReceived",
                                                              (ModestEngine*)this,
                                                              SLOT(searchMessagesHeadersReceivedSlot(QDBusMessage)));

            m_QtmPluginDBusInterface->connection().disconnect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                              MODESTENGINE_QTM_PLUGIN_PATH,
                                                              MODESTENGINE_QTM_PLUGIN_NAME,
                                                              "HeadersFetched",
                                                              (ModestEngine*)this,
                                                              SLOT(searchMessagesHeadersFetchedSlot(QDBusMessage)));
        }
        return false;
    }

    return true;
}

bool ModestEngine::startQueryingAndFilteringMessages(MessageQueryInfo &msgQueryInfo) const
{
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(msgQueryInfo.filter);
    if (pf->_filterList.count() == 0) {
        if ((pf->_field == QMessageFilterPrivate::None) &&
            (pf->_filterList.count() == 0) &&
            (pf->_notFilter)) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList
            msgQueryInfo.ids.clear();
            msgQueryInfo.returnWithSingleShot = true;
            QTimer::singleShot(0, (ModestEngine*)this, SLOT(returnQueryResultsSlot()));
            return true;
        } else {
            // One single filter to be handled
            QMessageFilter newFilter;
            QMessageFilterPrivate* pfNew = QMessageFilterPrivate::implementation(newFilter);
            pfNew->_filterList.append(QMessageFilterPrivate::SortedMessageFilterList());
            pfNew->_filterList[0] << msgQueryInfo.filter;
            msgQueryInfo.filter = newFilter;
        }
    }

    return queryAndFilterMessages(msgQueryInfo);
}

void ModestEngine::returnQueryResultsSlot()
{
    for (int i=m_pendingMessageQueries.count()-1; i >= 0; i--) {
        if (m_pendingMessageQueries[i].returnWithSingleShot) {
            if (m_pendingMessageQueries[i].isQuery) {
                m_pendingMessageQueries[i].privateService->messagesFound(m_pendingMessageQueries[i].ids, true, true);
            } else {
                m_pendingMessageQueries[i].privateService->messagesCounted(m_pendingMessageQueries[i].ids.count());
            }
            m_pendingMessageQueries.removeAt(i);
        }
    }
}

bool ModestEngine::queryAndFilterMessages(MessageQueryInfo &msgQueryInfo) const
{
    QStringList accountIds;
    QStringList folderUris;
    QDateTime startTimeStamp;
    QDateTime endTimeStamp;
    QDateTime startReceptionTimeStamp;
    QDateTime endReceptionTimeStamp;

    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(msgQueryInfo.filter);

    // Filters have been sorted into matrix of filters:
    // - there is AND operation between every filter in one row
    // - there is OR operation between every row
    // => Every row can be handled individually
    // => Final result set can be created by combining result sets
    //    from individual row based queries
    QMessageFilterPrivate::SortedMessageFilterList filters = pf->_filterList[msgQueryInfo.currentFilterListIndex];

    msgQueryInfo.realAccountId = QString();

    int handledFiltersCount = 0;
    pf = QMessageFilterPrivate::implementation(filters[handledFiltersCount]);
    if ((filters.count() == 1) &&
        (pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => this query results empty QMessageIdList
            // => Skip to next query round
            int index = -1;
            for (int i=0; i < m_pendingMessageQueries.count(); i++) {
                if (m_pendingMessageQueries[i].queryId == msgQueryInfo.queryId) {
                    index = i;
                    break;
                }
            }
            if (index >= 0) {
                handleQueryFinished(index);
            }
            return true;
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages
            handledFiltersCount++;
        }
    }

    bool handled = true;
    while (handledFiltersCount < filters.count() && handled) {
        handled = false;
        pf = QMessageFilterPrivate::implementation(filters[handledFiltersCount]);
        switch (pf->_field) {
        case QMessageFilterPrivate::Id:
            break;
        case QMessageFilterPrivate::ParentAccountId:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                if (accountIds.count()) {
                    // Only one account/one query can be used
                    break;
                }
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    if (pf->_value.toString().length() > 0) {
                        accountIds.append(modestAccountIdFromAccountId(pf->_value.toString()));
                        // Local folders are not account specific
                        // => Make sure that account specific messages are searched from local_folders account
                        accountIds.append("local_folders");
                        handled = true;
                    }
                }
            }
            break;
        }
        case QMessageFilterPrivate::ParentFolderId:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    if (pf->_value.toString().length() > 0) {
                        folderUris.append(modestFolderIdFromFolderId(pf->_value.toString()));
                        if (accountIds.count() == 0) {
                            accountIds.append(modestAccountIdFromFolderId(pf->_value.toString()));

                            // Note: Even though local folders belong to "local_folders" account
                            //       inside Modest, local folders can belong to any "real"
                            //       POP3 or IMAP account in Qt Mobility Messaging side
                            //       <=> Qt Mobility Messaging does not have "Local Folders"
                            //           account
                            // If folder is local folder, "local_folders" accountId will be
                            // added to accountIds list to enable correct filtering inside
                            // Modest Plugin
                            accountIds.append("local_folders");

                            // realAccountId will contain id of Qt Mobility Messaging account
                            // (AccountId can be found from the beginning of folderId string)
                            // <=> realAccountId will not ever be "local_folders"
                            msgQueryInfo.realAccountId = accountIdFromFolderId(pf->_value.toString()).toString();
                        }
                        handled = true;
                    }
                }
            }
            break;
            }
        case QMessageFilterPrivate::AncestorFolderIds:
            break;
        case QMessageFilterPrivate::Type:
            break;
        case QMessageFilterPrivate::StandardFolder:
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(pf->_value.toInt());
                    if (standardFolder == QMessage::SentFolder) {
                        // Possible "real" accountIds can be ignored in this phase
                        // because messages are searched from "local_folders" account
                        // <=> Actual account filtering is done when search from Modest
                        //     has finished
                        accountIds.clear();

                        accountIds.append("local_folders");
                        folderUris.append("sent");
                    } else if (standardFolder == QMessage::DraftsFolder) {
                        // Possible "real" accountIds can be ignored in this phase
                        // because messages are searched from "local_folders" account
                        // <=> Actual account filtering is done when search from Modest
                        //     has finished
                        accountIds.clear();

                        accountIds.append("local_folders");
                        folderUris.append("drafts");
                    }
                    handled = true;
                }
            }
            break;
        case QMessageFilterPrivate::ParentAccountIdFilter:
            break;
        case QMessageFilterPrivate::ParentFolderIdFilter:
            break;
        case QMessageFilterPrivate::TimeStamp:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    endTimeStamp = pf->_value.toDateTime();
                    startTimeStamp = pf->_value.toDateTime();
                    handled = true;
                }
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::LessThan) {
                    endTimeStamp = pf->_value.toDateTime();
                    handled = true;
                } else if (cmp == QMessageDataComparator::LessThanEqual) {
                    endTimeStamp = pf->_value.toDateTime();
                    handled = true;
                } else if (cmp == QMessageDataComparator::GreaterThan) {
                    startTimeStamp = pf->_value.toDateTime();
                    handled = true;
                } if (cmp == QMessageDataComparator::GreaterThanEqual) {
                    startTimeStamp = pf->_value.toDateTime();
                    handled = true;
                }
            }
            break;
            }
        case QMessageFilterPrivate::ReceptionTimeStamp:
            {
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    endReceptionTimeStamp = pf->_value.toDateTime();
                    startReceptionTimeStamp = pf->_value.toDateTime();
                    handled = true;
                }
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Relation) {
                QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::LessThan) {
                    endReceptionTimeStamp = pf->_value.toDateTime();
                    handled = true;
                } else if (cmp == QMessageDataComparator::LessThanEqual) {
                    endReceptionTimeStamp = pf->_value.toDateTime();
                    handled = true;
                } else if (cmp == QMessageDataComparator::GreaterThan) {
                    startReceptionTimeStamp = pf->_value.toDateTime();
                    handled = true;
                } if (cmp == QMessageDataComparator::GreaterThanEqual) {
                    startReceptionTimeStamp = pf->_value.toDateTime();
                    handled = true;
                }
            }
            break;
            }
        case QMessageFilterPrivate::Sender:
            break;
        case QMessageFilterPrivate::Recipients:
            break;
        case QMessageFilterPrivate::Subject:
            break;
        case QMessageFilterPrivate::Status:
            break;
        case QMessageFilterPrivate::Priority:
            break;
        case QMessageFilterPrivate::Size:
            break;
        case QMessageFilterPrivate::None:
            break;
        }
        handledFiltersCount++;
    }

    msgQueryInfo.handledFiltersCount = 0; // Do filtering also for filters which has been handled above

    return searchMessages(msgQueryInfo, accountIds, folderUris, msgQueryInfo.body, startTimeStamp,
                          endTimeStamp, startReceptionTimeStamp, endReceptionTimeStamp);
}

bool ModestEngine::searchMessages(MessageQueryInfo &msgQueryInfo, const QStringList& accountIds,
                                  const QStringList& folderUris, const QString& body,
                                  const QDateTime& startTimeStamp, const QDateTime& endTimeStamp,
                                  const QDateTime& startReceptionTimeStamp, const QDateTime& endReceptionTimeStamp) const
{
    Q_UNUSED(body) //TODO: Body search

    qulonglong sDate = 0;
    if (startTimeStamp.isValid() && startReceptionTimeStamp.isValid()) {
        if (startTimeStamp < startReceptionTimeStamp) {
            sDate = startTimeStamp.toTime_t();
        } else {
            sDate = startReceptionTimeStamp.toTime_t();
        }
    } else {
        if (startTimeStamp.isValid()) {
            sDate = startTimeStamp.toTime_t();
        } else if (startReceptionTimeStamp.isValid()) {
            sDate = startReceptionTimeStamp.toTime_t();
        }
    }


    qulonglong eDate = 0;
    if (endTimeStamp.isValid() && endReceptionTimeStamp.isValid()) {
        if (endTimeStamp > endReceptionTimeStamp) {
            eDate = endTimeStamp.toTime_t();
        } else {
            eDate = endReceptionTimeStamp.toTime_t();
        }
    } else {
        if (endTimeStamp.isValid()) {
            eDate = endTimeStamp.toTime_t();
        } else if (endReceptionTimeStamp.isValid()) {
            eDate = endReceptionTimeStamp.toTime_t();
        }
    }

    if (m_pendingMessageQueries.count() == 1) {
        // This is the first query in pending queries queue
        // => connect to signals
        m_QtmPluginDBusInterface->connection().connect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                       MODESTENGINE_QTM_PLUGIN_PATH,
                                                       MODESTENGINE_QTM_PLUGIN_NAME,
                                                       "HeadersReceived",
                                                       (ModestEngine*)this,
                                                       SLOT(searchMessagesHeadersReceivedSlot(QDBusMessage)));

        m_QtmPluginDBusInterface->connection().connect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                       MODESTENGINE_QTM_PLUGIN_PATH,
                                                       MODESTENGINE_QTM_PLUGIN_NAME,
                                                       "HeadersFetched",
                                                       (ModestEngine*)this,
                                                       SLOT(searchMessagesHeadersFetchedSlot(QDBusMessage)));
    }

    QDBusMessage reply = m_QtmPluginDBusInterface->call("GetHeaders",
                                                        accountIds,
                                                        folderUris,
                                                        sDate,
                                                        eDate,
                                                        false);
    if (reply.type() != QDBusMessage::ErrorMessage) {
        m_messageCache.clear();
        msgQueryInfo.queryId = reply.arguments().takeFirst().toInt();
    } else {
        return false;
    }

    return true;
}

void ModestEngine::searchMessagesHeadersReceivedSlot(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    int queryId = arguments.takeFirst().toInt();

    int index = -1;
    for (int i=0; i < m_pendingMessageQueries.count(); i++) {
        if (m_pendingMessageQueries[i].queryId == queryId) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        // Received DBus Message is not reply for the DBus query
        // that was sent from this process/instance of modest engine
        // => Continue waiting
        return;
    }

    MessageQueryInfo &queryInfo = m_pendingMessageQueries[index];

    QString reportedAccountId = arguments.takeFirst().toString();
    QString accountId;
    if (!queryInfo.realAccountId.isEmpty()) {
        // Search was done to Modest "local_folders" account
        // => Correct Qt Mobility Messaging AccountId
        //    can be taken realAccountId field
        // => Transform Messaging AccountId to Modest accountId
        //    by removing "MO_" from the beginning of accountId string
        accountId = queryInfo.realAccountId;
        accountId.remove(0,3);
    } else {
        accountId = reportedAccountId.remove("_store");
    }

    QString folderId = arguments.takeFirst().toString();
    QVariant variant = arguments.takeFirst();
    QDBusArgument argument = variant.value<QDBusArgument>();
    QList<QMap<QString, QVariant> > messages;
    argument >> messages;

    QMessageFilterPrivate::SortedMessageFilterList filters;
    int firstUnhandledFilterIndex = 0;

    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(queryInfo.filter);
    if (pf->_filterList.count() == 0) {
        filters.append(queryInfo.filter);
    } else {
        if (queryInfo.handledFiltersCount < pf->_filterList[queryInfo.currentFilterListIndex].count()) {
            filters = pf->_filterList[queryInfo.currentFilterListIndex];
        }
    }
    firstUnhandledFilterIndex = queryInfo.handledFiltersCount;

    for (int i=0; i < messages.count(); i++) {
        MessagingModestMessage modestMessage;
        modestMessage.accountId = accountId;
        modestMessage.folderId = folderId;
        modestMessage.dateReceived = 0;
        modestMessage.dateSent = 0;
        modestMessage.size = 0;
        modestMessage.flags = MessagingModestMessageNotDefined;
        modestMessage.priority = MessagingModestMessagePriorityNotDefined;

        QMapIterator<QString, QVariant> j(messages[i]);
        while (j.hasNext()) {
            j.next();
            if (j.key() == "url") {
                modestMessage.url = j.value().toString();
            } else if (j.key() == "message-uid") {
                modestMessage.id = j.value().toString();
            } else if (j.key() == "from") {
                modestMessage.from = j.value().toString();
            } else if (j.key() == "to") {
                modestMessage.to = j.value().toString();
            } else if (j.key() == "cc") {
                modestMessage.cc = j.value().toString();
            } else if (j.key() == "bcc") {
                modestMessage.bcc = j.value().toString();
            } else if (j.key() == "replyto") {
                modestMessage.replyTo = j.value().toString();
            } else if (j.key() == "subject") {
                modestMessage.subject = j.value().toString();
            } else if (j.key() == "date-received") {
                modestMessage.dateReceived = j.value().toLongLong();
            } else if (j.key() == "date-sent") {
                modestMessage.dateSent = j.value().toLongLong();
            } else if (j.key() == "size") {
                modestMessage.size = j.value().toLongLong();
            } else if (j.key() == "flags") {
                modestMessage.flags = static_cast<MessagingModestMessageFlags>(j.value().toUInt());
            } else if (j.key() == "priority") {
                modestMessage.priority = static_cast<MessagingModestMessagePriority>(j.value().toUInt());
            }
        }

        QMessage message = messageFromModestMessage(modestMessage);
        if (reportedAccountId == "local_folders") {
            QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
            QString id = privateMessage->_id.toString();
            QString newProtocol = "maildir";
            replaceProtocol(id, newProtocol);
            privateMessage->_id = QMessageId(id);
            id = privateMessage->_parentFolderId.toString();
            replaceProtocol(id, newProtocol);
            privateMessage->_parentFolderId = QMessageFolderId(id);
        }
        if (filterMessage(message, filters, 0)) {
            if (!queryInfo.ids.contains(message.id())) {
                if (m_messageCache.size() < maxCacheSize) {
                    m_messageCache.insert(message.id().toString(), message);
                }
                queryInfo.ids.append(message.id());
            }
        }
    }
}

void ModestEngine::searchMessagesHeadersFetchedSlot(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    int queryId = arguments.takeFirst().toInt();

    int index = -1;
    for (int i=0; i < m_pendingMessageQueries.count(); i++) {
        if (m_pendingMessageQueries[i].queryId == queryId) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        // Received DBus Message is not reply for the DBus query
        // that was sent from this process/instance of modest engine
        // => Continue waiting
        return;
    }

    handleQueryFinished(index);
}

void ModestEngine::handleQueryFinished(int index) const
{
    MessageQueryInfo &queryInfo = m_pendingMessageQueries[index];

    queryInfo.currentFilterListIndex++;
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(queryInfo.filter);
    if (queryInfo.currentFilterListIndex < pf->_filterList.count()) {
        if (queryAndFilterMessages(queryInfo)) {
            // Continue searching
            return;
        }
    }

    MessagingHelper::orderMessages(queryInfo.ids, queryInfo.sortOrder);
    MessagingHelper::applyOffsetAndLimitToMessageIdList(queryInfo.ids, queryInfo.limit, queryInfo.offset);

    // Search finished
    if (queryInfo.isQuery) {
        queryInfo.privateService->messagesFound(queryInfo.ids, true, true);
    } else {
        queryInfo.privateService->messagesCounted(queryInfo.ids.count());
    }
    m_pendingMessageQueries.removeAt(index);

    if (m_pendingMessageQueries.count() == 0) {
        // This was last query in pending queries queue
        // => Disconnect from "GetHeaders" request related DBus signals
        // Note: Disconnecting signals is done to optimize signal handling
        //       <=> Disconnecting prevents unnecessary handling of signals
        //           which have been sent from other applications using
        //           Qt Mobility Messaging API
        m_QtmPluginDBusInterface->connection().disconnect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                          MODESTENGINE_QTM_PLUGIN_PATH,
                                                          MODESTENGINE_QTM_PLUGIN_NAME,
                                                          "HeadersReceived",
                                                          (ModestEngine*)this,
                                                          SLOT(searchMessagesHeadersReceivedSlot(QDBusMessage)));

        m_QtmPluginDBusInterface->connection().disconnect(MODESTENGINE_QTM_PLUGIN_NAME,
                                                          MODESTENGINE_QTM_PLUGIN_PATH,
                                                          MODESTENGINE_QTM_PLUGIN_NAME,
                                                          "HeadersFetched",
                                                          (ModestEngine*)this,
                                                          SLOT(searchMessagesHeadersFetchedSlot(QDBusMessage)));
    }
}

void
ModestEngine::sendEmailCallEnded(QDBusPendingCallWatcher *watcher)
{
    if (watcher->isError ()) {
        // TODO: Emit a failure
        qWarning() << "Failed to send email via modest: " << watcher->error();
    } else {
        // TODO: Emit a success (or put to outbox)
        qDebug() << "Message should be outboxed now...";
    }
}

void
ModestEngine::addMessageCallEnded(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError ()) {
        // TODO: Emit a failure
        qWarning() << "Failed to add message via modest: " << reply.error();
    } else {
        QString id = reply.argumentAt<0>();
        // TODO: Emit a success ... with message's id?
        qDebug() << "Message with id" << id << "should be added now";
    }
}

void ModestEngine::folderUpdatedSlot(QDBusMessage msg)
{
    QList<QVariant> arguments = msg.arguments();
    QString modestAccountId = arguments.takeFirst().toString();
    QString modestFolderId = arguments.takeFirst().toString();
    QMessageFolderId updatedFolderId;

    if (modestAccountId == "local_folders") {
        updatedFolderId = QMessageFolderId("MO_LOCAL&maildir&"+modestFolderId);
    } else {
        QMessageAccountId accountId = QMessageAccountId("MO_"+escapeString(modestAccountId));
        QString protocol = accountEmailProtocolAsString(accountId);
        if ((protocol == "pop") && (modestFolderId == "INBOX")) {
            modestFolderId = "cache";
        }
        updatedFolderId = QMessageFolderId(accountId.toString()+"&"+protocol+"&"+modestFolderId);
    }

    int messagesPerAccount = 5;
    QDBusPendingCall pendingCall = m_ModestDBusInterface->asyncCall(MODEST_DBUS_METHOD_GET_UNREAD_MESSAGES,
                                                                    messagesPerAccount);
    QDBusPendingCallWatcher* pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall);
    pendingCallWatcher->setProperty("folderId", updatedFolderId.toString());
    connect(pendingCallWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(pendingGetUnreadMessagesFinishedSlot(QDBusPendingCallWatcher*)));
}

void ModestEngine::pendingGetUnreadMessagesFinishedSlot(QDBusPendingCallWatcher* pendingCallWatcher)
{
    QDBusMessage msg = pendingCallWatcher->reply();
    QVariant variant = msg.arguments().takeFirst();
    QDBusArgument argument = variant.value<QDBusArgument>();
    QList<ModestAccountsUnreadMessagesDBusStruct> accountsWithUnreadMessages;
    argument >> accountsWithUnreadMessages;

    bool setOnlyDates = false;
    if (pendingCallWatcher->property("setOnlyDates").isValid()) {
        setOnlyDates = true;
    }
    QMessageFolderId folderId;
    if (pendingCallWatcher->property("folderId").isValid()) {
        folderId = QMessageFolderId(pendingCallWatcher->property("folderId").toString());
    }

    for (int i=0; i < accountsWithUnreadMessages.count(); i++) {
        QDateTime newLatestTimeStamp;
        QDateTime latestTimeStamp = accountsLatestTimestamp.take(accountsWithUnreadMessages[i].accountId);
        for (int j=0; j < accountsWithUnreadMessages[i].unreadMessages.count(); j++) {
            ModestUnreadMessageDBusStruct unreadMessage = accountsWithUnreadMessages[i].unreadMessages[j];
            QDateTime time = QDateTime::fromTime_t(unreadMessage.timeStamp);
            if (time > newLatestTimeStamp) {
                newLatestTimeStamp = time;
            }
            if (!setOnlyDates) {
                if (time > latestTimeStamp) {
                    searchNewMessages(unreadMessage.subject, QString(), time, time, MODEST_DBUS_SEARCH_SUBJECT, 0);
                }
            }
        }
        if (newLatestTimeStamp.isValid()) {
            accountsLatestTimestamp.remove(accountsWithUnreadMessages[i].accountId);
            accountsLatestTimestamp.insert(accountsWithUnreadMessages[i].accountId, newLatestTimeStamp);
        } else {
            accountsLatestTimestamp.insert(accountsWithUnreadMessages[i].accountId, QDateTime::currentDateTime());
        }
    }
}

void ModestEngine::searchNewMessages(const QString& searchString, const QString& folderToSearch,
                                  const QDateTime& startDate, const QDateTime& endDate,
                                  int searchflags, uint minimumMessageSize) const
{
    qlonglong sDate = 0;
    if (startDate.isValid()) {
        sDate = startDate.toTime_t();
    }
    qlonglong eDate = 0;
    if (endDate.isValid()) {
        eDate = endDate.toTime_t();
    }

    QDBusInterface interface(MODEST_DBUS_SERVICE, MODEST_DBUS_OBJECT, MODEST_DBUS_IFACE);
    QDBusPendingCall pendingCall = interface.asyncCall(MODEST_DBUS_METHOD_SEARCH,
                                                       searchString,
                                                       folderToSearch,
                                                       sDate,
                                                       eDate,
                                                       searchflags,
                                                       minimumMessageSize);
    QDBusPendingCallWatcher* pendingCallWatcher = new QDBusPendingCallWatcher(pendingCall);
    connect(pendingCallWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(pendingSearchFinishedSlot(QDBusPendingCallWatcher*)));
}

void ModestEngine::pendingSearchFinishedSlot(QDBusPendingCallWatcher* pendingCallWatcher)
{
    QDBusMessage msg = pendingCallWatcher->reply();
    QVariant variant = msg.arguments().takeFirst();
    QDBusArgument argument = variant.value<QDBusArgument>();
    QList<ModestMessage> messages;
    argument >> messages;

    for (int i=0; i < messages.count(); i++) {
        notification(messageIdFromModestMessageId(messages[i].id), ModestEngine::Added);
    }
}

void ModestEngine::messageReadChangedSlot(QDBusMessage msg)
{
    QString changedMessageId = msg.arguments().takeFirst().toString();
    notification(messageIdFromModestMessageId(changedMessageId), ModestEngine::Updated);
}

QMessageManager::NotificationFilterId ModestEngine::registerNotificationFilter(QMessageStorePrivate& messageStore,
                                                                               const QMessageFilter &filter,
                                                                               QMessageManager::NotificationFilterId id)
{
    m_messageStore = &messageStore;

    int filterId = id;
    if (filterId == 0) {
        filterId = ++m_filterId;
    }
    m_filters.insert(filterId, filter);
    return filterId;
}

void ModestEngine::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    m_filters.remove(notificationFilterId);
}

QByteArray ModestEngine::getMimePart (const QMessageId &id, const QString &attachmentId)
{
    QByteArray result;

    QString modestAccountId = modestAccountIdFromMessageId(id);
    QString modestFolderId  = modestFolderIdFromMessageId(id);
    QString modestMessageId = modestMessageIdFromMessageId(id);

    QString filePath, mimeType;
    int mimeSize = -1;
    bool expunge = false, isAttachment = false;

    QDBusPendingReply<QString, QString, int, bool, bool> reply =
            m_QtmPluginDBusInterface->asyncCall(
                    "GetMimePart",
                    QVariant::fromValue(modestAccountId),
                    QVariant::fromValue(modestFolderId),
                    QVariant::fromValue(modestMessageId),
                    QVariant::fromValue(attachmentId));

    reply.waitForFinished();

    if (reply.isError()) {
        return result;
    }

    filePath = reply.argumentAt<0>();
    mimeType = reply.argumentAt<1>();
    mimeSize = reply.argumentAt<2>();
    isAttachment = reply.argumentAt<3>();
    expunge = reply.argumentAt<4>();

    if (filePath.isEmpty()) {
        qWarning() << "Received empty file path!";
        return result;
    }

    QFile file(filePath);

    QIODevice::OpenMode openMode = QIODevice::ReadOnly;
    if (expunge) {
        openMode = QIODevice::ReadWrite;
    }
    
    if (file.open(openMode) == false) {
        qWarning() << "Failed to open file" << filePath << ": "
                << file.errorString();
        return result;
    }

    result = file.readAll();

    if (expunge) {
        file.remove();
    } else {
        file.close();
    }

    return result;
}

void ModestEngine::notification(const QMessageId& messageId, NotificationType notificationType) const
{
    QMessageId realMessageId = messageId;
    QString modestAccountId = modestAccountIdFromMessageId(messageId);
    QString modestFolderId = modestFolderIdFromMessageId(messageId);
    QString modestMessageId = modestMessageIdFromMessageId(messageId);

    // Make sure that there will not be many Added or Removed notifications
    // in a row for the same message
    // Make also sure that there will not be updated notification for a
    // message that has already been notified to be removed.
    QString searchId;
    if (notificationType == ModestEngine::Added) {
        searchId = "A"+modestAccountId+modestFolderId+modestMessageId;
    } else {
        searchId = "D"+modestAccountId+modestFolderId+modestMessageId;
    }
    if (!m_latestAddOrRemoveNotifications.contains(searchId)) {
        if ((notificationType == ModestEngine::Added) || (notificationType == ModestEngine::Removed)) {
            // Only Added & Removed notification will be checked
            if (m_latestAddOrRemoveNotifications.count() > 10) {
                // Remove the oldest notification from the beginning of the list
                m_latestAddOrRemoveNotifications.removeFirst();
            }
            // Append new notification
            m_latestAddOrRemoveNotifications.append(searchId);
        }
    } else {
        // Add or Remove notification for the message was already handled!
        // => Skip unwanted notification
        return;
    }

    QMessageManager::NotificationFilterIdSet matchingFilters;

    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(m_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    QMessage message;
    bool messageRetrieved = false;
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());

        if (!messageRetrieved) {
            MessagingModestMessage modestMessage = messageFromModest(modestAccountId,
                                                                     modestFolderId,
                                                                     modestMessageId);

            if (modestMessage.accountId.isEmpty()) {
                return;
            }

            if (modestMessage.flags & MessagingModestMessageDeleted) {
                notificationType = ModestEngine::Removed;
            }
            if (modestMessage.size == 0) {
                notificationType = ModestEngine::Removed;
            }

            message = messageFromModestMessage(modestMessage);
            realMessageId =message.id();
            messageRetrieved = true;
        }

        if (filter.isEmpty()) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        } else {
            if (message.type() == QMessage::NoType) {
                continue;
            }
            QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
            if (privateMessageFilter->filter(message)) {
                matchingFilters.insert(it.key());
            }
        }
    }

    if (matchingFilters.count() > 0) {
        if (notificationType == ModestEngine::Added) {
            m_messageStore->messageNotification(QMessageStorePrivate::Added, realMessageId, matchingFilters);
        } else if (notificationType == ModestEngine::Updated) {
            m_messageStore->messageNotification(QMessageStorePrivate::Updated, realMessageId, matchingFilters);
        } else if (notificationType == ModestEngine::Removed) {
            m_messageStore->messageNotification(QMessageStorePrivate::Removed, realMessageId, matchingFilters);
        }
    }
}

QMessageAccountId ModestEngine::accountIdFromModestMessageId(const QString& modestMessageId) const
{
    // Modest messageId format is following:
    //   <email protocol>://<username>@<hostname>:<port>...
    QMessageAccountId accountId;

    int usernameBegin = modestMessageId.indexOf("//")+2;
    int usernameEnd = modestMessageId.indexOf("@");
    QString username = QUrl::fromPercentEncoding(modestMessageId.mid(usernameBegin, usernameEnd-usernameBegin).toUtf8());
    int hostnameBegin = usernameEnd+1;
    int hostnameEnd = modestMessageId.indexOf(':',hostnameBegin);
    QString hostname = QUrl::fromPercentEncoding(modestMessageId.mid(hostnameBegin, hostnameEnd-hostnameBegin).toUtf8());

    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accId = value.id();
        if ((accountUsername(accId) == username) &&
            (accountHostname(accId) == hostname)) {
            accountId = accId;
        }
    }

    if (!accountId.isValid()) {
        if (modestMessageId.left(modestMessageId.indexOf(':')) == "maildir") {
            accountId = QMessageAccountId("MO_LOCAL");
        }
    }

    return accountId;
}

QMessageFolderId ModestEngine::folderIdFromModestMessageId(const QString& modestMessageId,
                                                           const QMessageAccountId accountId) const
{
    // Modest messageId format is following:
    //   <email protocol>://<username>@<hostname>:<port>...
    QMessageFolderId folderId;
    QString folderIdAsString;

    if (!accountId.isValid()) {
        folderIdAsString = accountIdFromModestMessageId(modestMessageId).toString();
    } else {
        folderIdAsString = accountId.toString();
    }

    int protocolEnd = modestMessageId.indexOf(':');
    QString protocol = modestMessageId.left(protocolEnd);
    folderIdAsString += "&" + protocol;
    if (protocol == "pop") {
        folderIdAsString += "&cache";
    } else if (protocol == "imap") {
        int pathBegin = modestMessageId.indexOf('/',modestMessageId.lastIndexOf(':'))+1;
        int pathEnd = modestMessageId.lastIndexOf('/');
        folderIdAsString += "&" + modestMessageId.mid(pathBegin, pathEnd-pathBegin);
    } else if (protocol == "maildir") {
        int pathBegin = modestMessageId.indexOf('#')+1;
        int pathEnd = modestMessageId.lastIndexOf('/');
        folderIdAsString += "&" + modestMessageId.mid(pathBegin, pathEnd-pathBegin);
    }
    folderId = QMessageFolderId(QUrl::fromPercentEncoding(folderIdAsString.toUtf8()));

    return folderId;
}

QString ModestEngine::modestAccountIdFromAccountId(const QMessageAccountId& accountId) const
{
    // Just remove "MO_" prefix from the beginning of id string
    return accountId.toString().remove(0,3);
}

QString ModestEngine::modestFolderIdFromFolderId(const QMessageFolderId& folderId) const
{
    QString modestFolderId;

    QString folderIdString = folderId.toString();
    int protocolBegin = folderIdString.indexOf('&');
    int protocolEnd = folderIdString.lastIndexOf('&');

    modestFolderId = folderIdString.mid(protocolEnd+1);
    QString protocol = folderIdString.mid(protocolBegin+1,protocolEnd-protocolBegin-1);
    if ((protocol == "pop") && (modestFolderId == "cache")) {
        modestFolderId = "INBOX";
    }

    return modestFolderId;
}

QString ModestEngine::modestFolderUriFromFolderId(const QMessageFolderId& folderId) const
{
    Q_UNUSED(folderId) //TODO:
    return QString();
}

QString ModestEngine::modestAccountIdFromMessageId(const QMessageId& messageId,
                                                   bool checkProtocol) const
{
    QString id = messageId.toString();
    int protocolBegin = id.indexOf('&');
    int protocolEnd = id.lastIndexOf('&');
    QString protocol = id.mid(protocolBegin+1,protocolEnd-protocolBegin-1);
    if (checkProtocol && protocol == "maildir") {
        return "local_folders";
    }

    return unescapeString(id.left(protocolBegin).remove(0,3));
}

QMessageAccountId ModestEngine::accountIdFromFolderId(const QMessageFolderId& folderId) const
{
    QString id = folderId.toString();
    int protocolBegin = id.indexOf('&');
    return QMessageAccountId(id.left(protocolBegin));
}

QMessageAccountId ModestEngine::accountIdFromMessageId(const QMessageId& messageId) const
{
    QString id = messageId.toString();
    int protocolBegin = id.indexOf('&');
    return QMessageAccountId(id.left(protocolBegin));
}

QString ModestEngine::modestAccountIdFromFolderId(const QMessageFolderId& folderId,
                                                  bool checkProtocol) const
{
    QString id = folderId.toString();
    int protocolBegin = id.indexOf('&');
    int protocolEnd = id.lastIndexOf('&');
    QString protocol = id.mid(protocolBegin+1,protocolEnd-protocolBegin-1);
    if (checkProtocol && protocol == "maildir") {
        return "local_folders";
    }

    return unescapeString(id.left(protocolBegin).remove(0,3));
}

QString ModestEngine::modestFolderIdFromMessageId(const QMessageId& messageId) const
{
    QString id = messageId.toString();

    int protocolBegin = id.indexOf('&');
    int protocolEnd = id.lastIndexOf('&');
    int folderEnd = id.lastIndexOf('/');

    QString protocol = id.mid(protocolBegin+1,protocolEnd-protocolBegin-1);
    id = id.mid(protocolEnd+1,folderEnd-protocolEnd-1);
    if ((protocol == "pop") && (id == "cache")) {
        id = "INBOX";
    }

    return id;
}

void ModestEngine::replaceProtocol(QString& id, const QString& newProtocol) const
{
    int protocolBegin = id.indexOf('&');
    int protocolEnd = id.lastIndexOf('&');
    id.remove(protocolBegin+1,protocolEnd-protocolBegin-1);
    id.insert(protocolBegin+1, newProtocol);
}

QMessageAccountId ModestEngine::realAccountId(const MessagingModestMessage& modestMessage) const
{
    QMessageAccountId accountId;

    if (modestMessage.accountId == "local_folders") {
        QString accountIdString;
        // Message is in local foldar, but message can be linked
        // to actual account using 'From', 'To', 'Cc' or 'Bcc' fields
        foreach (QMessageAccount value, iAccounts) {
            QMessageAccountPrivate* privAccount = QMessageAccountPrivate::implementation(value);
            if (modestMessage.from.contains(privAccount->_address.addressee())) {
                accountIdString = value.id().toString();
                break;
            } else if (modestMessage.to.contains(privAccount->_address.addressee())) {
                accountIdString = value.id().toString();
                break;
            } else if (modestMessage.cc.contains(privAccount->_address.addressee())) {
                accountIdString = value.id().toString();
                break;
            } else if (modestMessage.bcc.contains(privAccount->_address.addressee())) {
                accountIdString = value.id().toString();
                break;
            }
        }
        if (!accountIdString.isEmpty()) {
            accountId = QMessageAccountId(accountIdString);
        }
    } else {
        accountId = accountIdFromModestAccountId(modestMessage.accountId);
    }

    return accountId;
}

QString ModestEngine::modestMessageIdFromMessageId(const QMessageId& messageId) const
{
    QString id = messageId.toString();
    return id.mid(id.lastIndexOf('/')+1);
}

QMessageAccountId ModestEngine::accountIdFromModestAccountId(const QString& accountId) const
{
    // Just add "MO_" prefix to the beginning of id string & escape created Id
    return QMessageAccountId(escapeString("MO_"+accountId));
}

QMessageFolderId ModestEngine::folderIdFromModestFolderId(const QMessageAccountId& accountId,
                                                          bool isLocalFolder,
                                                          const QString& modestFolderId) const
{
    // Format: <accountId>&<email protocol>&<Modest folderId>
    QMessageFolderId folderId;

    if (isLocalFolder) {
         folderId = QMessageFolderId(accountId.toString()+"&maildir&"+modestFolderId);
    } else {
        QString protocol = accountEmailProtocolAsString(accountId);
        if ((protocol == "pop") && (modestFolderId == "INBOX")) {
            folderId = QMessageFolderId(accountId.toString()+"&"+protocol+"&cache");
        } else {
            folderId = QMessageFolderId(accountId.toString()+"&"+protocol+"&"+modestFolderId);
        }
    }

    return folderId;
}

QMessageId ModestEngine::messageIdFromModestMessageId(const QString& messageId) const
{
    QString messageIdString = folderIdFromModestMessageId(messageId).toString();
    int idPartBegin = messageId.lastIndexOf('/');
    messageIdString += messageId.mid(idPartBegin, idPartBegin-messageId.length());
    return QMessageId(messageIdString);
}

QMessageId ModestEngine::messageIdFromModestMessageFilePath(const QString& messageFilePath) const
{
    QString messageIdString;

    QString filePath = messageFilePath;
    QString localRootFolder = this->localRootFolder();
    if (filePath.startsWith(localRootFolder)) {
        messageIdString = "MO_LOCAL&maildir&";
        filePath.remove(0,localRootFolder.length()+1);
        filePath.remove("/cur");
        messageIdString += filePath.left(filePath.lastIndexOf('!'));
    } else {
        foreach (QMessageAccount value, iAccounts) {
            QMessageAccountId accountId = value.id();
            QString rootFolder = accountRootFolder(accountId);
            if (filePath.startsWith(rootFolder)) {
                QString protocol = accountEmailProtocolAsString(accountId);
                messageIdString = accountId.toString()+"&"+protocol+"&";
                filePath.remove(0,rootFolder.length()+1);
                filePath.remove("/subfolders");
                messageIdString += filePath.left(filePath.lastIndexOf('.'));
                if (protocol == "pop") {
                    QDir dir(messageFilePath);
                    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
                    QFileInfoList dirs = dir.entryInfoList();
                    if (dirs.count() > 0) {
                        QString fileName = dirs[0].fileName();
                        // Remove folder that contains actual message
                        messageIdString = messageIdString.left(messageIdString.lastIndexOf('/')+1);
                        // Add message name
                        messageIdString = messageIdString+fileName;
                    }
                }
            }
        }
    }

    return QMessageId(messageIdString);
}

QString ModestEngine::unescapeString(const QString& string)
{
    QString unescapedString;

    QByteArray str = string.toUtf8();
    gchar* unescaped_string = gconf_unescape_key(str.data(), str.length());
    unescapedString = QString::fromUtf8(unescaped_string);
    g_free(unescaped_string);

    return unescapedString;
}

QString ModestEngine::escapeString(const QString& string)
{
    QString escapedString;

    QByteArray str = unescapeString(string).toUtf8();
    gchar* escaped_string = gconf_escape_key(str.data(), str.length());
    escapedString = QString::fromUtf8(escaped_string);
    g_free(escaped_string);

    return escapedString;
}

INotifyWatcher::INotifyWatcher()
{
    // Initialize inotify instance
    // => returned file descriptor is associated with
    //    a new inotify event queue
    // O_CLOEXEC flag makes sure that file descriptor
    //           is closed if execution is transfered
    //           from this process to a new program
    //           (Check more info from 'execve' documentation)
#ifdef IN_CLOEXEC
    m_inotifyFileDescriptor = inotify_init1(IN_CLOEXEC);
#else
    m_inotifyFileDescriptor = inotify_init();
    if (m_inotifyFileDescriptor >= 0) {
        ::fcntl(m_inotifyFileDescriptor, F_SETFD, FD_CLOEXEC);
    }
#endif
    if (m_inotifyFileDescriptor >= 0) {
        // Change thread affinity for this object to this
        // thread.
        // => Event processing (for this objects events) will
        //    be done in this thread
        moveToThread(this);
    }
}

INotifyWatcher::~INotifyWatcher()
{
    // Tell the thread's event loop to exit
    // => thread returns from the call to exec()
    exit();

    // Wait until this thread has finished execution
    // <=> waits until thread returns from run()
    wait();

    clear();

    // Close file descriptor that's referring to inotify instance
    // => The underlying inotify object and its resources are freed
    ::close(m_inotifyFileDescriptor);
}

void INotifyWatcher::clear()
{
    // Remove all watches from inotify instance watch list
    QMapIterator<int, QString> i(m_dirs);
    while (i.hasNext()) {
        inotify_rm_watch(m_inotifyFileDescriptor, i.next().key());
    }
    m_dirs.clear();

    QMapIterator<int, QString> j(m_files);
    while (j.hasNext()) {
        inotify_rm_watch(m_inotifyFileDescriptor, j.next().key());
    }
    m_files.clear();
}

void INotifyWatcher::run()
{
    // Start listening inotify
    QSocketNotifier socketNotifier(m_inotifyFileDescriptor, QSocketNotifier::Read, this);
    connect(&socketNotifier, SIGNAL(activated(int)), SLOT(notifySlot()));

    // Enter the thread event loop
    (void) exec();
}

int INotifyWatcher::addFile(const QString& path, uint eventsToWatch)
{
    int watchDescriptor = 0;
    QMutexLocker locker(&m_mutex);

    if (m_inotifyFileDescriptor >= 0) {
        int watchDescriptor = 0;
        if (eventsToWatch == 0) {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                0 | IN_ATTRIB
                                                  | IN_MODIFY
                                                  | IN_MOVE
                                                  | IN_MOVE_SELF
                                                  | IN_DELETE_SELF);
        } else {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                eventsToWatch);
        }
        if (watchDescriptor > 0) {
            m_files.insert(watchDescriptor, path);
        } else {
            watchDescriptor = 0;
        }
    }

    // Start thread (if thread is not already running)
    start();

    return watchDescriptor;
}

int INotifyWatcher::addDirectory(const QString& path, uint eventsToWatch)
{
    int watchDescriptor = 0;
    QMutexLocker locker(&m_mutex);

    if (m_inotifyFileDescriptor >= 0) {
        int watchDescriptor = 0;
        if (eventsToWatch == 0) {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                0 | IN_ATTRIB
                                                  | IN_MOVE
                                                  | IN_CREATE
                                                  | IN_DELETE
                                                  | IN_DELETE_SELF);
        } else {
            watchDescriptor = inotify_add_watch(m_inotifyFileDescriptor,
                                                QFile::encodeName(path),
                                                eventsToWatch);
        }
        if (watchDescriptor > 0) {
            m_dirs.insert(watchDescriptor, path);
        } else {
            watchDescriptor = 0;
        }
    }

    // Start thread (if thread is not already running)
    start();

    return watchDescriptor;
}

QStringList INotifyWatcher::directories() const
{
    return m_dirs.values();
}

QStringList INotifyWatcher::files() const
{
    return m_dirs.values();
}

void INotifyWatcher::notifySlot()
{
    QMutexLocker locker(&m_mutex);

    int bufferSize = 0;
    ioctl(m_inotifyFileDescriptor, FIONREAD, (char*) &bufferSize);
    QVarLengthArray<char, 4096> buffer(bufferSize);
    bufferSize = read(m_inotifyFileDescriptor, buffer.data(), bufferSize);
    const char* at = buffer.data();
    const char* const end = at + bufferSize;

    QMap<int, INotifyEvent> eventsForWatchedFile;
    QMap<QString, INotifyEvent> eventsForFileInWatchedDirectory;
    while (at < end) {
        const inotify_event *event = reinterpret_cast<const inotify_event *>(at);
        if (m_files.contains(event->wd)) {
            // File event handling
            if (eventsForWatchedFile.contains(event->wd)) {
                // There is already unhandled event for this file in queue
                // => Mask is ORed to existing event
                eventsForWatchedFile[event->wd].mask |= event->mask;
            } else {
                // There is no event for this file in queue
                // => New event is created
                INotifyEvent inotifyEvent;
                inotifyEvent.watchDescriptor = event->wd;
                inotifyEvent.mask = event->mask;
                inotifyEvent.fileName = QString::fromAscii(event->name, event->len);
                eventsForWatchedFile.insert(event->wd, inotifyEvent);
            }
        } else {
            // Directory event handling
            QString changeForFileInDirectory = QString::fromAscii(event->name, event->len);
            // Remove unnecessary postfix (starting with '!') from the file name
            changeForFileInDirectory = changeForFileInDirectory.left(changeForFileInDirectory.lastIndexOf('!'));
            if (!changeForFileInDirectory.isEmpty()) {
                QString eventId = QString::number(event->wd)+changeForFileInDirectory;
                if (eventsForFileInWatchedDirectory.contains(eventId)) {
                    // There is already unhandled event for this file in queue
                    // => Mask is ORed to existing event
                    eventsForFileInWatchedDirectory[eventId].mask |= event->mask;
                } else {
                    // There is no event for this file in queue
                    // => New event is created
                    INotifyEvent inotifyEvent;
                    inotifyEvent.watchDescriptor = event->wd;
                    inotifyEvent.mask = event->mask;
                    inotifyEvent.fileName = QString::fromAscii(event->name, event->len);
                    eventsForFileInWatchedDirectory.insert(eventId, inotifyEvent);
                }
            }
        }
        at += sizeof(inotify_event) + event->len;
    }

    QMap<int, INotifyEvent>::const_iterator it = eventsForWatchedFile.constBegin();
    while (it != eventsForWatchedFile.constEnd()) {
        INotifyEvent event = *it;
        QString file = m_files.value(event.watchDescriptor);
        if (!file.isEmpty()) {
            emit fileChanged(event.watchDescriptor, file, event.mask);
        }
        ++it;
    }

    QMap<QString, INotifyEvent>::const_iterator jt = eventsForFileInWatchedDirectory.constBegin();
    while (jt != eventsForFileInWatchedDirectory.constEnd()) {
        INotifyEvent event = *jt;
        QString file = m_dirs.value(event.watchDescriptor)+"/"+event.fileName;
        emit fileChanged(event.watchDescriptor, file, event.mask);
        ++jt;
    }
}

ModestStringMap ModestEngine::getModestSenderInfo(QMessage &message)
{
    QMessageAddress address;
    ModestStringMap senderInfo;
    QMessageAccountId accountId;
    QString value;

    accountId = message.parentAccountId();
    if (accountId.isValid() == false) {
        qWarning () << "Account ID is invalid";
        return ModestStringMap();
    }

    senderInfo["account-name"] = unescapeString(modestAccountIdFromAccountId(accountId));

    QMessageAccount messageAccount = account(accountId);
    QMessageAccountPrivate* privAccount = QMessageAccountPrivate::implementation(messageAccount);
    address = privAccount->_address;
    value = address.addressee();

    if (value.isEmpty() == false && value.isNull() == false) {
        senderInfo["from"] = value;
    }

    return senderInfo;
}

ModestStringMap ModestEngine::getModestRecipients(QMessage &message)
{
    QMessageAddressList addresses;
    QMessageAddress address;
    ModestStringMap recipients;
    QString value;

    addresses = message.to();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.addressee();
        } else {
            value.append (",");
            value.append (address.addressee());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["to"] = value;
    }

    addresses = message.cc();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.addressee();
        } else {
            value.append (",");
            value.append (address.addressee());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["cc"] = value;
    }

    addresses = message.bcc();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.addressee();
        } else {
            value.append (",");
            value.append (address.addressee());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["bcc"] = value;
    }

    return recipients;
}

ModestStringMap ModestEngine::getModestMessageData(QMessage &message)
{
    QMessageContentContainerId bodyId;
    QMessageContentContainer body;
    ModestStringMap messageData;
    QString value;

    value = message.subject();

    if (value.isEmpty() == false && value.isNull() == false) {
        messageData["subject"] = value;
    }

    bodyId = message.bodyId();
    if (bodyId.isValid()) {
        body = message.find (bodyId);
    } else {
        body = message;
    }

    value = body.contentType();

    if (value == "text") {
        QString key, data;
        bool hasContent = false;

        value = body.contentSubType();

        if ((hasContent = body.isContentAvailable()) == true) {
            data = body.textContent();
        }

        if (value == "plain") {
            key = "plain-body";
        } else if (value == "html") {
            key = "html-body";
        }

        if (key.isEmpty() == false && key.isNull() == false && hasContent) {
            messageData[key] = data;
        }
    }

    return messageData;
}

ModestStringMapList ModestEngine::getModestAttachments(QMessage &message)
{
    QMessageContentContainerIdList attachmentIds;
    ModestStringMapList attachments;
    QMessage::StatusFlags messageStatus;
    QString value;

    messageStatus = message.status();

    if (messageStatus & QMessage::HasAttachments) {
        attachmentIds = message.attachmentIds();

        foreach (QMessageContentContainerId identifier, attachmentIds) {
            ModestStringMap attachmentData;
            QMessageContentContainer attachmentCont;

            if (identifier.isValid() == false) continue;

            attachmentCont = message.find (identifier);

            if (attachmentCont.isContentAvailable () == false) continue;

            attachmentData.clear();

            value = attachmentCont.contentType();

            if (value.isEmpty() == false) {
                value.append("/");
                value.append (attachmentCont.contentSubType());
                attachmentData["mime-type"] = value;

                qDebug() << "mime-type: " << value;
            }

            value = QMessageContentContainerPrivate::attachmentFilename (
                attachmentCont);

            if (value.isEmpty() == false) {
                attachmentData["filename"] = value;
                qDebug() << "filename: " << value;
            }

            qDebug() << "Charset: " << attachmentCont.contentCharset();
            qDebug() << "Headers: " << attachmentCont.headerFields();

            if (attachmentData.isEmpty() == false) {
                attachmentData["content-id"] = identifier.toString();
                attachments.append (attachmentData);
            }
        }
    }

    return attachments;
}

ModestStringMapList ModestEngine::getModestImages(QMessage &message)
{
    Q_UNUSED(message);
    // Don't know if this even makes sense. Modest expects inlined images
    // to be in a separate list, but that doesn't make much sense?
    return ModestStringMapList();
}

uint ModestEngine::getModestPriority(QMessage &message)
{
    uint priority = 0;

    switch (message.priority()) {
    case QMessage::HighPriority:
        priority = MODESTENGINE_HIGH_PRIORITY;
        break;

    default:
    case QMessage::NormalPriority:
        priority = MODESTENGINE_NORMAL_PRIORITY;
        break;

    case QMessage::LowPriority:
        priority = MODESTENGINE_LOW_PRIORITY;
        break;
    }

    return priority;
}

ModestStringMap ModestEngine::getModestHeaders(QMessage &message)
{
    Q_UNUSED(message);
    return ModestStringMap(); // stub
}

void ModestEngine::clearHeaderCache()
{
    m_messageCache.clear();
}

#include "moc_modestengine_maemo_p.cpp"

QTM_END_NAMESPACE
