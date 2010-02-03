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
const char TST_ACCOUNTS_PATH[]         = "/apps/modest/accounts";
const char TST_SERVER_ACCOUNTS_PATH[]  = "/apps/modest/server_accounts/";
const char TST_STORE_ACCOUNT_KEY[]     = "store_account";
const char TST_TRANSPORT_ACCOUNT_KEY[] = "transport_account";

// Global to remember the account ids that have been added on this run...
QList<QString> myAccountNames;

// Prints and clears an error
void
printError (const char *fname, const int &line, GError **error);

// Removes test accounts
void
clearTestAccounts ();

void
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
void
clearTestAccounts ()
{
    GConfClient *client = NULL;
    GSList *dirs = NULL, *iter = NULL;
    GError *err = NULL;
    gboolean unset = FALSE;
    uint unset_flags = 0;
    const char *key = NULL;

    // The client returns GLIB types, so let's use GLIB types
    gchar *store_account = NULL;
    gchar *transport_account = NULL;

    client = gconf_client_get_default ();

    dirs = gconf_client_all_dirs (client, TST_ACCOUNTS_PATH, &err);

    if (dirs == NULL || err != NULL) {
        if (err != NULL) {
            printError (__FUNCTION__, __LINE__, &err);
        }
        qWarning() << "Failed to list dirs in " << TST_ACCOUNTS_PATH;
        return;
    }

    for (iter = dirs; iter != NULL; iter = iter->next) {
        QString str;

        key = static_cast<const char *>(iter->data);

        str = QString::fromUtf8 (key);

        // If we don't have the account remembered, don't bother in removing
        // It?
        if (myAccountNames.contains (str) == false) continue;

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

//             unset = gconf_client_recursive_unset (
//                 client,
//                 str.toLocal8Bit.data(),
//                 (GConfUnsetFlags)unset_flags,
//                 &err);

            if (err != NULL) {
                printError (__FUNCTION__, __LINE__, &err);
            }
            if (unset == FALSE) {
                qWarning() << "Failed to unset GConf dir " << str;
            }
            g_free (temp);
            temp = NULL;
        }

        str = QString::fromUtf8 (key);
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
            str.append (transport_account);

            qDebug() << "Unsetting value from: " << str;

//             unset = gconf_client_recursive_unset (
//                 client,
//                 str.toLocal8Bit.data(),
//                 (GConfUnsetFlags)unset_flags,
//                 &err);

            if (err != NULL) {
                printError (__FUNCTION__, __LINE__, &err);
            }
            if (unset == FALSE) {
                qWarning() << "Failed to unset GConf dir " << str;
            }
            g_free (temp);
            temp = NULL;
        }

        qDebug() << "Unsetting account: " << key;

//         unset = gconf_client_recursive_unset (
//             client, escaped, (GConfUnsetFlags)unset_flags, &err);

        if (err != NULL) {
            printError (__FUNCTION__, __LINE__, &err);
        }
        if (unset == FALSE) {
            qWarning() << "Failed to unset GConf dir " << key;
        }

        // Remove the name from remembered list
        myAccountNames.removeOne (QString::fromUtf8 (key));

        // Reserved with GLIB methods, free with GLIB methods
        g_free (store_account);
        store_account = NULL;

        g_free (transport_account);
        transport_account = NULL;
    }

    if (dirs != NULL) {
        g_slist_foreach (dirs, (GFunc)g_free, NULL);
        g_slist_free (dirs);
    }
}

namespace Support {

void clearMessageStore()
{
    // TODO:
    // - Clear messages
    // - Clear folders

    clearTestAccounts ();
}

QMessageAccountId addAccount(const Parameters &params)
{
    QString accountName;

    accountName = params["name"];

    // Invalid?
    if (accountName.isEmpty ()) return QMessageAccountId ();
    if (accountName.isNull ()) return QMessageAccountId ();

    // Match the accountName to a GConf key (it's how the account ids are)
    // formed...
    accountName.prepend ("/");
    accountName.prepend (TST_ACCOUNTS_PATH);

    // We've done this already
    if (myAccountNames.contains (accountName) == true)
        return QMessageAccountId (accountName);

    // Actually add the account
    myAccountNames.append (accountName);
    qDebug() << "Added account name " << accountName;

    return QMessageAccountId (accountName);
}

QMessageFolderId addFolder(const Parameters &params)
{
    return QMessageFolderId();
}

QMessageId addMessage(const Parameters &params)
{
    return QMessageId();
}

};
