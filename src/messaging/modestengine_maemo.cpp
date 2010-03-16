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
#include "qmessageservice.h"
#include <libmodest-dbus-client/libmodest-dbus-client.h>
#include <libmodest-dbus-client/libmodest-dbus-api.h>
#include <QUrl>
#include <QtDBus/QtDBus>
#include <dbus/dbus.h>

#include <QDebug>

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
        gchar *default_account = gconf_client_get_string(m_gconfclient, MODESTENGINE_DEFAULT_ACCOUNT, &error);
        if (error) {
            qWarning("qtmessaging: failed to get '%s': %s", MODESTENGINE_DEFAULT_ACCOUNT, error->message);
            g_error_free(error);
        }

        GSList *iter = accounts;
        while (iter) {
            if (!(iter->data)) {
                iter = iter->next;
                continue;
            }

            const gchar* account_name_key = (const gchar*)iter->data;

            gboolean account_ok = FALSE;
            // Check if account is enabled
            if (account_name_key) {
                gchar* key = g_strconcat(account_name_key, "/", MODESTENGINE_ACCOUNT_ENABLED, NULL);
                account_ok = gconf_client_get_bool(m_gconfclient, key, NULL);
                g_free(key);
            }
            // Check if account store is defined
            if (account_ok) {
                gchar* key = g_strconcat(account_name_key, "/", MODESTENGINE_ACCOUNT_STORE_ACCOUNT, NULL);
                gchar* server_account_name = gconf_client_get_string(m_gconfclient, key, NULL);
                if (server_account_name) {
                    gchar* key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", server_account_name, NULL);
                    if (!gconf_client_dir_exists(m_gconfclient, key, NULL)) {
                        account_ok = FALSE;
                    }
                    g_free(server_account_name);
                }
                g_free(key);
            }
            // Check if account transport is defined
            if (account_ok) {
                gchar* key = g_strconcat(account_name_key, "/", MODESTENGINE_ACCOUNT_TRANSPORT_ACCOUNT, NULL);
                gchar* server_account_name = gconf_client_get_string(m_gconfclient, key, NULL);
                if (server_account_name) {
                    gchar* key = g_strconcat(MODESTENGINE_SERVER_ACCOUNT_NAMESPACE, "/", server_account_name, NULL);
                    if (!gconf_client_dir_exists(m_gconfclient, key, NULL)) {
                        account_ok = FALSE;
                    }
                    g_free(server_account_name);
                }
                g_free(key);
            }

            if (account_ok) {
                QString accountId = QString::fromUtf8(account_name_key);
                gchar* name_key = g_strconcat(account_name_key, "/", MODESTENGINE_ACCOUNT_DISPLAY_NAME, NULL);
                QString accountName = QString::fromUtf8(gconf_client_get_string(m_gconfclient, name_key, NULL));
                g_free(name_key);
                gchar* email_key = g_strconcat(account_name_key, "/", MODESTENGINE_ACCOUNT_EMAIL, NULL);
                QString accountAddress = QString::fromUtf8(gconf_client_get_string(m_gconfclient, email_key, NULL));
                g_free(email_key);
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                                                       accountName,
                                                                       QMessageAddress(QMessageAddress::Email, accountAddress),
                                                                       QMessage::Email);
                iAccounts.insert(accountId, account);

                if (strncmp(account_name_key, default_account, strlen(default_account))) {
                    iDefaultEmailAccountId = accountId;
                }
            }

            g_free(iter->data);
            iter->data = NULL;
            iter = g_slist_next(iter);
        }
        // strings were freed in while loop
        // => it's enough to just free accounts list
        g_slist_free(accounts);
        g_free(default_account);
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

QMessageAccountId ModestEngine::defaultAccount(QMessage::Type type) const
{
    updateEmailAccounts();
    return iDefaultEmailAccountId;
}

bool ModestEngine::sendEmail(QMessage &message)
{
    return composeEmail(message);
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

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return true;
}

bool ModestEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return true;
}

bool ModestEngine::countMessages(QMessageService& messageService, const QMessageFilter &filter)
{
    return true;
}


QTM_END_NAMESPACE
