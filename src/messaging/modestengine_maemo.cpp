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
#include "modestengine_maemo_p.h"
#include "maemohelpers_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessagefolder_p.h"
#include "qmessageservice.h"
#include "qmessageservice_maemo_p.h"
#include "qmessagecontentcontainer_maemo_p.h"
#include <QUrl>
#include <QtDBus>
#include <dbus/dbus.h>
#include <libmodest-dbus-client/libmodest-dbus-api.h>

#include <QDebug>

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

// Specific priority settings to translate to modest priorities
#define MODESTENGINE_HIGH_PRIORITY             2
#define MODESTENGINE_NORMAL_PRIORITY           0
#define MODESTENGINE_LOW_PRIORITY              1

Q_GLOBAL_STATIC(ModestEngine,modestEngine);

ModestEngine::ModestEngine()
{
    g_type_init();
    m_gconfclient = gconf_client_get_default();
    if (!m_gconfclient) {
        qWarning("qtmessaging: could not get gconf client");
    } else {
        updateEmailAccounts();
    }

    // Setup DBus Interface for Modest
    m_ModestDBusInterface = new QDBusInterface(MODEST_DBUS_SERVICE,
                                               MODEST_DBUS_OBJECT,
                                               MODEST_DBUS_IFACE,
                                               QDBusConnection::sessionBus(),
                                               this);

    qDBusRegisterMetaType< ModestStringMap >();
    qDBusRegisterMetaType< ModestStringMapList >();
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
                QString accountId = QString::fromUtf8(escaped_account_id);
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
    QMessageAccountIdList accountIds;

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
    QMessageAccount account = this->account(accountId);

    QString userName;
    QString hostName;
    QString port;
    QString protocol;

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", account.id().toString().toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
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
        QFileInfo fileInfo = QFileInfo(accountRootFolder(accountId)+"/Inbox");
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
    QMessageAccount account = this->account(accountId);

    gchar* store_account_key = g_strconcat(MODESTENGINE_ACCOUNT_NAMESPACE, "/", account.id().toString().toUtf8().data(), "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
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

QMessageFolderIdList ModestEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder,
                                                uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    QMessageFolderIdList folderIds;

    //QDBusMessage msg = m_ModestDBusInterface->call(MODEST_DBUS_METHOD_GET_FOLDERS);

    QFileInfoList localFolders = this->localFolders();
    QString localRootFolder = this->localRootFolder();

    foreach (QMessageAccount value, iAccounts) {
        QMessageAccountId accountId = value.id();
        QString rootFolder = accountRootFolder(accountId);
qWarning() << "rootfolder : " << rootFolder;
        EmailProtocol protocol = accountEmailProtocol(accountId);
        QFileInfoList folders = this->accountFolders(accountId);

        for (int i=0; i < folders.count(); i++) {
            QString filePath = folders[i].absoluteFilePath();
            QString protocolStr;
            if (protocol == ModestEngine::EmailProtocolPop3) {
                protocolStr = "P_";
            } else if (protocol == ModestEngine::EmailProtocolIMAP) {
                protocolStr = "I_";
            }
            QString id = "MO_"+protocolStr+accountId.toString()+"&"+filePath.right(filePath.size()-rootFolder.size()-1);
            folderIds.append(QMessageFolderId(id));
qWarning() << "id : " << id;
        }

        for (int i=0; i < localFolders.count(); i++) {
            QString filePath = localFolders[i].absoluteFilePath();
            QString id = "MO_L_"+accountId.toString()+"&"+filePath.right(filePath.size()-localRootFolder.size()-1);
            folderIds.append(QMessageFolderId(id));
qWarning() << "id : " << id;
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
    // Take 'MO_' & account type string ("P_") away from the beginning of the id string
    QString idString = id.toString().remove(0,5);
    int endOfAccountId = idString.indexOf('&');
    QString accountId = idString.left(endOfAccountId);
    QString folder = idString.right(idString.length()-idString.lastIndexOf('&')-1);
    QMessageFolderId parentId;
    QString name;
    if (folder.lastIndexOf('/') == -1) {
        name = folder;
        QChar protocolChar = id.toString().at(3);
        if (protocolChar == 'P') {
            if (name == "cache") {
                name = "Inbox";
            }
        }
    } else {
        name = folder.right(folder.length()-folder.lastIndexOf('/')-1);
        QString folderIdString = id.toString();
        QChar protocolChar = folderIdString.at(3);
        if (protocolChar == 'I') {
            parentId = folderIdString.left(folderIdString.lastIndexOf("/subfolders/"));
        } else if (protocolChar == 'L') {
            parentId = folderIdString.left(folderIdString.lastIndexOf('/'));
        }
    }
    //TODO: path handling
    return QMessageFolderPrivate::from(id, QMessageAccountId(accountId), parentId, name, name);
}

bool ModestEngine::sendEmail(QMessage &message)
{
    QDBusInterface qtmPlugin(
        MODESTENGINE_QTM_PLUGIN_NAME,
        MODESTENGINE_QTM_PLUGIN_PATH,
        MODESTENGINE_QTM_PLUGIN_NAME,
        QDBusConnection::sessionBus());
    QMessageAddressList addresses;
    QMessageAddress address;
    QMessageContentContainerIdList attachmentIds;
    QMessageContentContainerId bodyId;
    QMessageContentContainer body;
    QString value;
    QMessageAccountId accountId;
    QMessageAccount account;
    ModestStringMap senderInfo;
    ModestStringMap recipients;
    ModestStringMap messageData;
    ModestStringMapList attachments;
    ModestStringMapList images;
    uint priority = 0;
    ModestStringMap headers;
    QMessage::StatusFlags messageStatus;

    qDebug() << __PRETTY_FUNCTION__ << "Sending message";

    // XXX: Room for convenience function here...
    accountId = message.parentAccountId();
    if (accountId.isValid() == false) {
        qWarning () << "Account ID is invalid";
        return false;
    }

    senderInfo["account-name"] = accountId.toString();

    address = message.from();
    value = address.recipient();

    if (value.isEmpty() == false && value.isNull() == false) {
        senderInfo["from"] = value;
    }

    qDebug() << "Digging \"to\" field";

    addresses = message.to();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.recipient();
        } else {
            value.append (",");
            value.append (address.recipient());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["to"] = value;
    }

    qDebug() << "Digging \"cc\" field";

    addresses = message.cc();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.recipient();
        } else {
            value.append (",");
            value.append (address.recipient());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["cc"] = value;
    }

    qDebug() << "Digging \"bcc\" field";

    addresses = message.bcc();
    value.clear();
    for (int i = 0; i < addresses.length(); i++) {
        address = addresses[i];

        if (value.isEmpty()) {
            value = address.recipient();
        } else {
            value.append (",");
            value.append (address.recipient());
        }
    }

    if (value.isEmpty() == false && value.isNull() == false) {
        recipients["bcc"] = value;
    }

    qDebug() << "Digging \"subject\" field";

    value = message.subject();

    if (value.isEmpty() == false && value.isNull() == false) {
        messageData["subject"] = value;
    }

    qDebug() << "Digging body content";

    bodyId = message.bodyId();
    if (bodyId.isValid()) {
        body = message.find (bodyId);
    } else {
        body = message;
    }

    value = body.contentType();
    qDebug() << value;

    if (value == "text") {
        QString key, data;
        bool hasContent = false;

        value = body.contentSubType();
        qDebug() << value;

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

    qDebug() << "Sending D-BUS message";

    QDBusPendingCall call = qtmPlugin.asyncCall (
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
                mailString += list.at(i).recipient();
            } else {
                mailString += QString("%2C%20");
                mailString += list.at(i).recipient();
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

QMessage ModestEngine::message(const QMessageId &id) const
{
    return QMessage();
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body,
                                 QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
                                 uint limit, uint offset) const
{
    return false;
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return false;
}

bool ModestEngine::countMessages(QMessageService& messageService, const QMessageFilter &filter)
{
    return false;
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

#include "moc_modestengine_maemo_p.cpp"

QTM_END_NAMESPACE
