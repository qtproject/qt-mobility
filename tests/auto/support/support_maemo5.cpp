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

#include <QDebug>

#include <qmessage.h>

#include <glib.h>
#include <gconf/gconf-client.h>

#include "support.h"

// Constants
const char TST_ACCOUNTS_PATH[]         = "/apps/modest/accounts/";
const char TST_SERVER_ACCOUNTS_PATH[]  = "/apps/modest/server_accounts/";
const char TST_STORE_ACCOUNT_KEY[]     = "store_account";
const char TST_TRANSPORT_ACCOUNT_KEY[] = "transport_account";

// Maybe it would be better to just nuke everything?
#define NAME_COUNT 6
const char *TST_ACCOUNT_NAMES[NAME_COUNT] = {
    "testAccount",
    "Test Account #1",
    "Test Account #2",
    "Alter Ego",
    "Personal",
    "Work"
};

// Prints and clears an error
static void
printError (const char *fname, const int &line, GError **error);

// Removes test accounts
static void
clearTestAccounts ();

// Get a valid GConf key for account name
static QString
getAccountKey (const Support::Parameters &params, const bool &escape);

// Gets a valid account path for GConf from parameters
static QString
getAccountPath (const Support::Parameters &params, const bool &escape);

static void
printError (
    const char *fname,
    const int &line,
    GError **error)
{
    GError *err = NULL;

    if (error == NULL || *error == NULL) return;
    err = *error;

    qWarning() << "Error in " << fname << ", line: " << line << endl
               << "Domain: " << g_quark_to_string (err->domain) << endl
               << "Code: " << err->code << endl
               << "Message: " << err->message;
    g_clear_error (error);
}

// Clears test accounts
static void
clearTestAccounts ()
{
    GConfClient *client = NULL;
    GError *err = NULL;
    gboolean unset = FALSE;
    uint unset_flags = 0;
    const char *key = NULL;
    uint i;
    gchar *store_account = NULL;
    gchar *transport_account = NULL;

    client = gconf_client_get_default ();

    for (i = 0; i < NAME_COUNT; i++) {
        QString str, path;
        gchar *escaped;

        key = TST_ACCOUNT_NAMES[i];
        escaped = gconf_escape_key (key, -1);

        path = QString::fromUtf8 (TST_ACCOUNTS_PATH);
        path.append (escaped);

        str = path;
        str.append ("/");
        str.append (TST_STORE_ACCOUNT_KEY);

        qDebug() << "Fetching key value from: " << str;

        store_account = gconf_client_get_string (
            client, str.toLocal8Bit().data(), &err);

        if (err != NULL || store_account == NULL) {
            if (err != NULL)
                printError (__FUNCTION__, __LINE__, &err);
            qWarning() << "Failed to get GConf key value " << str;
        } else if (store_account) {
            gchar *temp = gconf_escape_key (store_account, -1);

            str = QString::fromUtf8 (TST_SERVER_ACCOUNTS_PATH);
            str.append (temp);

            qDebug() << "Unsetting value from: " << str;

            unset = gconf_client_recursive_unset (
                client,
                str.toLocal8Bit().data(),
                (GConfUnsetFlags)unset_flags,
                &err);

            if (err != NULL) {
                printError (__FUNCTION__, __LINE__, &err);
            }
            if (unset == FALSE) {
                qWarning() << "Failed to unset GConf dir " << str;
            }
            g_free (temp);
            temp = NULL;
        }

        str = path;
        str.append ("/");
        str.append (TST_TRANSPORT_ACCOUNT_KEY);

        qDebug() << "Fetching key value from: " << str;

        transport_account = gconf_client_get_string (
            client, str.toLocal8Bit().data(), &err);

        if (err != NULL || transport_account == NULL) {
            if (err != NULL)
                printError (__FUNCTION__, __LINE__, &err);
            qWarning() << "Failed to get GConf key value " << str;
        } else if (transport_account) {
            gchar *temp = gconf_escape_key (transport_account, -1);

            str = QString::fromUtf8 (TST_SERVER_ACCOUNTS_PATH);
            str.append (temp);

            qDebug() << "Unsetting value from: " << str;

            unset = gconf_client_recursive_unset (
                client,
                str.toLocal8Bit().data(),
                (GConfUnsetFlags)unset_flags,
                &err);

            if (err != NULL) {
                printError (__FUNCTION__, __LINE__, &err);
            }
            if (unset == FALSE) {
                qWarning() << "Failed to unset GConf dir " << str;
            }
            g_free (temp);
            temp = NULL;
        }

        qDebug() << "Unsetting account: " << path;

        unset = gconf_client_recursive_unset (
            client, path.toLocal8Bit().data(),
            (GConfUnsetFlags)unset_flags, &err);

        if (err != NULL) {
            printError (__FUNCTION__, __LINE__, &err);
        }
        if (unset == FALSE) {
            qWarning() << "Failed to unset GConf dir " << path;
        }

        // Reserved with GLIB methods, free with GLIB methods
        g_free (escaped);
        escaped = NULL;

        g_free (store_account);
        store_account = NULL;

        g_free (transport_account);
        transport_account = NULL;
    }
}

// Get a valid GConf key for account name
static QString
getAccountKey (const Support::Parameters &params, const bool &escape)
{
    QString key;
    gchar *escaped;

    key = params["name"];

    if (key.isEmpty() || key.isNull()) return key;

    // Modest compatability
    key.append ("ID");

    if (escape == false) return key;

    escaped = gconf_escape_key (key.toLocal8Bit().data(), -1);

    qDebug() << "Escaped key: " << key << " to " << escaped;

    key = escaped;
    g_free (escaped);
    escaped = NULL;

    return key;
}

// Get a valid GConf path for account name
static QString
getAccountPath (const Support::Parameters &params, const bool &escape)
{
    QString key, path;

    // Escape the incoming name
    key = getAccountKey (params, escape);

    if (key.isEmpty() || key.isNull()) return key;

    // Match the accountName to a GConf key (it's how the account ids are)
    // formed...
    path = QString::fromUtf8 (TST_ACCOUNTS_PATH);
    path.append (key);

    return path;
}

namespace Support {

// Clears test data, that we know about
void clearMessageStore()
{
    // TODO:
    // - Clear messages
    // - Clear folders

    clearTestAccounts ();
}

// Add, or modify an account
QMessageAccountId addAccount (const Parameters &params)
{
    QString accountPath, accountKey, escapedAccountKey, accountName, address;
    QString confKey, confValue;
    GConfClient *client;
    GError *err = NULL;
    gboolean valueSet = FALSE;

    qDebug() << params;

    // Get escaped path for account id
    accountPath = getAccountPath (params, true);

    // Invalid?
    if (accountPath.isEmpty ()) return QMessageAccountId ();
    if (accountPath.isNull ())  return QMessageAccountId ();

    client = gconf_client_get_default ();

    accountName       = params["name"];
    address           = params["fromAddress"];
    accountKey        = getAccountKey (params, false);
    escapedAccountKey = getAccountKey (params, true);

    // Display name
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("display_name");
    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        accountName.toLocal8Bit().data(),
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set display_name for " << accountName;
        return QMessageAccountId();
    }

    // Email
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("email");
    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        address.toLocal8Bit().data(),
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set email for " << accountName;
        return QMessageAccountId();
    }

    // Enabled
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("enabled");
    valueSet = gconf_client_set_bool (
        client,
        confKey.toLocal8Bit().data(),
        TRUE,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set enabled for " << accountName;
        return QMessageAccountId();
    }

    // Signature
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("signature");
    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set signature for " << accountName;
        return QMessageAccountId();
    }

    // Limit retrieve
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("limit-retrieve");
    valueSet = gconf_client_set_int (
        client,
        confKey.toLocal8Bit().data(),
        20,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set retrieve limit for " << accountName;
        return QMessageAccountId();
    }

    // Retrieve
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("retrieve");
    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "messages-and-attachments",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set retrieve value for " << accountName;
        return QMessageAccountId();
    }

    // Use specific SMTP
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("use_specific_smtp");
    valueSet = gconf_client_set_bool (
        client,
        confKey.toLocal8Bit().data(),
        FALSE,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set specific smtp for " << accountName;
        return QMessageAccountId();
    }

    // Should the account leave mails to server
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("leave_on_server");
    valueSet = gconf_client_set_bool (
        client,
        confKey.toLocal8Bit().data(),
        TRUE,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set leave on server for " << accountName;
        return QMessageAccountId();
    }

    // Full name
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("fullname");
    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        accountName.toLocal8Bit().data(),
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set fullname for " << accountName;
        return QMessageAccountId();
    }

    // Should the account use signature
    confKey = accountPath;
    confKey.append ("/");
    confKey.append ("use_signature");
    valueSet = gconf_client_set_bool (
        client,
        confKey.toLocal8Bit().data(),
        FALSE,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set signature use for " << accountName;
        return QMessageAccountId();
    }


    // Store account (aka where to fetch accounts)
    confKey = accountPath;
    confKey.append ("/");
    confKey.append (TST_STORE_ACCOUNT_KEY);

    confValue = accountKey;
    confValue.append ("_store");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        confValue.toLocal8Bit().data(),
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set store account for " << accountName;
        return QMessageAccountId();
    }

    // Used protocol
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("proto");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "pop",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set protocol for " << accountName;
        return QMessageAccountId();
    }

    // Port
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("port");

    valueSet = gconf_client_set_int (
        client,
        confKey.toLocal8Bit().data(),
        110,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set port for " << accountName;
        return QMessageAccountId();
    }

    // Hostname to use
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("hostname");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "pop.example.com",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set hostname for " << accountName;
        return QMessageAccountId();
    }

    // Username to use
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("username");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "tester",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set username on store for " << accountName;
        return QMessageAccountId();
    }

    // Authorization mechanism?
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("auth_mech");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "none",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set auth mechanism for " << accountName;
        return QMessageAccountId();
    }

    // Password to use
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("password");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "testerpassword",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set password on store for " << accountName;
        return QMessageAccountId();
    }

    // Security mechanism to use
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_store");
    confKey.append ("/");
    confKey.append ("security");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "none",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set security mechanism store for "
                   << accountName;
        return QMessageAccountId();
    }


    // Transport account (aka what to use to send)
    confKey = accountPath;
    confKey.append ("/");
    confKey.append (TST_TRANSPORT_ACCOUNT_KEY);

    confValue = accountKey;
    confValue.append ("_transport");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        confValue.toLocal8Bit().data(),
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport account for " << accountName;
        return QMessageAccountId();
    }

    // Protocol for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("proto");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "smtp",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport protocol for "
                   << accountName;
        return QMessageAccountId();
    }

    // Port for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("port");

    valueSet = gconf_client_set_int (
        client,
        confKey.toLocal8Bit().data(),
        25,
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport port for "
                   << accountName;
        return QMessageAccountId();
    }

    // Host for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("hostname");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "smtp.example.com",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport host for "
                   << accountName;
        return QMessageAccountId();
    }

    // Username for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("username");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "tester",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport username for "
                   << accountName;
        return QMessageAccountId();
    }

    // Password for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("password");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "testerpassword",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport password for "
                   << accountName;
        return QMessageAccountId();
    }

    // Authorization mechanism for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("auth_mech");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "none",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport auth mechanism for "
                   << accountName;
        return QMessageAccountId();
    }

    // Security mechanism for transport
    confKey = TST_SERVER_ACCOUNTS_PATH;
    confKey.append (escapedAccountKey);
    confKey.append ("_transport");
    confKey.append ("/");
    confKey.append ("security");

    valueSet = gconf_client_set_string (
        client,
        confKey.toLocal8Bit().data(),
        "none",
        &err);

    if (valueSet == FALSE || err != NULL) {
        if (err != NULL)
            printError (__FUNCTION__, __LINE__, &err);
        qWarning() << "Failed to set transport security for "
                   << accountName;
        return QMessageAccountId();
    }

    qDebug() << "Added account path " << accountPath << " with name "
             << accountName;

    return QMessageAccountId (accountPath);
}

// Add a folder, unless it already exists
QMessageFolderId addFolder(const Parameters &params)
{
    qDebug() << params;
    return QMessageFolderId();
}

// Add a message
QMessageId addMessage(const Parameters &params)
{
    qDebug() << params;
    return QMessageId();
}

}; // Namespace Support
