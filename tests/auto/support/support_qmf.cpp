/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "support.h"
#include "qmfhelpers_p.h"

#include <qmailaccount.h>
#include <qmailaddress.h>
#include <qmailnamespace.h>
#include <qmailstore.h>

#include <QFileInfo>

// This function is private except to tst_QMailStore:
class tst_QMailStore 
{
public:
    static void clearMessageStore() { QMailStore::instance()->clearContent(); }
};

namespace Support {

void clearMessageStore()
{
    tst_QMailStore::clearMessageStore();
}

QMessageAccountId addAccount(const Parameters &params)
{
    QString name(params["name"]);
    QString fromAddress(params["fromAddress"]);

    if (!name.isEmpty()) {
        QMailAccount account;
        account.setName(name);
        account.setStatus(QMailAccount::Enabled, true);
        account.setStatus(QMailAccount::CanTransmit, true);
        account.setMessageType(QMailMessage::Email);

        if (!fromAddress.isEmpty()) {
            account.setFromAddress(QMailAddress(QString(), fromAddress));
        }

        // Ensure that we initialise to the current version for file storage
        const QString key("qtopiamailfile");

        QMailAccountConfiguration config;
        config.addServiceConfiguration(key);
        QMailAccountConfiguration::ServiceConfiguration &svcCfg(config.serviceConfiguration(key));
        svcCfg.setValue("servicetype", "storage");
        svcCfg.setValue("version", "101");

        if (!QMailStore::instance()->addAccount(&account, &config)) {
            qWarning() << "Unable to addAccount:" << name;
        } else {
            return QmfHelpers::convert(account.id());
        }
    }

    return QMessageAccountId();
}

QMessageFolderId addFolder(const Parameters &params)
{
    QString path(params["path"]);
    QString name(params["name"]);
    QString parentAccountName(params["parentAccountName"]);
    QString parentFolderPath(params["parentFolderPath"]);

    if (!path.isEmpty() && !parentAccountName.isEmpty()) {
        // Find the named account
        QMailAccountIdList accountIds(QMailStore::instance()->queryAccounts(QMailAccountKey::name(parentAccountName)));
        if (accountIds.count() == 1) {
            QMailFolder folder;
            folder.setPath(path);
            folder.setParentAccountId(accountIds.first());

            if (!name.isEmpty()) {
                folder.setDisplayName(name);
            }

            if (!parentFolderPath.isEmpty()) {
                QMailFolderKey key(QMailFolderKey::path(parentFolderPath) & QMailFolderKey::parentAccountId(folder.parentAccountId()));
                QMailFolderIdList folderIds(QMailStore::instance()->queryFolders(key));
                if (folderIds.count() == 1) {
                    folder.setParentFolderId(folderIds.first());
                } else {
                    qWarning() << "Unable to locate parent folder:" << parentFolderPath;
                }
            }

            if (!QMailStore::instance()->addFolder(&folder)) {
                qWarning() << "Unable to addFolder:" << path;
            } else {
                return QmfHelpers::convert(folder.id());
            }
        } else {
            qWarning() << "Unable to locate parent account:" << parentAccountName;
        }
    }

    return QMessageFolderId();
}

QMessageId addMessage(const Parameters &params)
{
    QString parentAccountName(params["parentAccountName"]);
    QString parentFolderPath(params["parentFolderPath"]);
    QString to(params["to"]);
    QString from(params["from"]);
    QString cc(params["cc"]);
    QString date(params["date"]);
    QString receivedDate(params["receivedDate"]);
    QString subject(params["subject"]);
    QString text(params["text"]);
    QString mimeType(params["mimeType"]);
    QString attachments(params["attachments"]);
    QString priority(params["priority"]);
    QString size(params["size"]);
    QString type(params["type"]);
    QString read(params["status-read"]);
    QString hasAttachments(params["status-hasAttachments"]);

    QMessageManager mgr;

    if (!to.isEmpty() && !from.isEmpty() && !date.isEmpty() && !subject.isEmpty() &&
        !parentAccountName.isEmpty() && !parentFolderPath.isEmpty()) {
        // Find the named account
        QMessageAccountIdList accountIds(mgr.queryAccounts(QMessageAccountFilter::byName(parentAccountName)));
        if (accountIds.count() == 1) {
            // Find the specified folder
            QMessageFolderFilter filter(QMessageFolderFilter::byPath(parentFolderPath, QMessageDataComparator::Equal) & QMessageFolderFilter::byParentAccountId(accountIds.first()));
            QMessageFolderIdList folderIds(mgr.queryFolders(filter));
            if (folderIds.count() == 1) {
                QMessage message;

                message.setParentAccountId(accountIds.first());

                QMessage::StatusFlags flags(0);
                if (read.toLower() == "true") {
                    flags |= QMessage::Read;
                }
                if (hasAttachments.toLower() == "true") {
                    flags |= QMessage::HasAttachments;
                }
                message.setStatus(flags);

                {
                    QMailMessage *msg(QmfHelpers::convert(&message));
                    msg->setParentFolderId(QmfHelpers::convert(folderIds.first()));
                    msg->setStatus(QMailMessage::Incoming, true);
                }

                QList<QMessageAddress> toList;
                foreach (const QString &addr, to.split(",", QString::SkipEmptyParts)) {
                    toList.append(QMessageAddress(QMessageAddress::Email, addr.trimmed()));
                }
                message.setTo(toList);

                QList<QMessageAddress> ccList;
                foreach (const QString &addr, cc.split(",", QString::SkipEmptyParts)) {
                    ccList.append(QMessageAddress(QMessageAddress::Email, addr.trimmed()));
                }
                if (!ccList.isEmpty()) {
                    message.setCc(ccList);
                }

                message.setFrom(QMessageAddress(QMessageAddress::Email, from));
                message.setSubject(subject);

                QDateTime dt(QDateTime::fromString(date, Qt::ISODate));
                dt.setTimeSpec(Qt::UTC);
                message.setDate(dt);

                if (type.isEmpty()) {
                    message.setType(QMessage::Email);
                } else {
                    if (type.toLower() == "mms") {
                        message.setType(QMessage::Mms);
                    } else if (type.toLower() == "sms") {
                        message.setType(QMessage::Sms);
                    } else if (type.toLower() == "instantmessage") {
                        message.setType(QMessage::InstantMessage);
                    } else {
                        message.setType(QMessage::Email);
                    }
                }

                if (!receivedDate.isEmpty()) {
                    QDateTime dt(QDateTime::fromString(receivedDate, Qt::ISODate));
                    dt.setTimeSpec(Qt::UTC);
                    message.setReceivedDate(dt);
                }

                if (!priority.isEmpty()) {
                    if (priority.toLower() == "high") {
                        message.setPriority(QMessage::HighPriority);
                    } else if (priority.toLower() == "low") {
                        message.setPriority(QMessage::LowPriority);
                    }
                }

                if (!text.isEmpty()) {
                    message.setBody(text, mimeType.toAscii());
                }

                if (!attachments.isEmpty()) {
                    message.appendAttachments(attachments.split("\n"));
                }

                {
                    // We need to add a size value for QMF
                    QMailMessage *msg(QmfHelpers::convert(&message));

                    if (!size.isEmpty()) {
                        msg->setSize(size.toUInt());
                    } else {
                        // Estimate the message size
                        msg->setSize(msg->indicativeSize() * 1024);
                    }
                }

                if (!mgr.addMessage(&message)) {
                    qWarning() << "Unable to addMessage:" << to << from << date << subject;
                } else {
                    return message.id();
                }
            } else {
                qWarning() << "Unable to locate parent folder:" << parentFolderPath;
            }
        } else {
            qWarning() << "Unable to locate parent account:" << parentAccountName;
        }
    } else {
        qWarning() << "Necessary information missing";
    }

    return QMessageId();
}

}

