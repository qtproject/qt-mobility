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
    QString displayName(params["displayName"]);
    QString parentAccountName(params["parentAccountName"]);
    QString parentFolderPath(params["parentFolderPath"]);

    if (!path.isEmpty() && !parentAccountName.isEmpty()) {
        // Find the named account
        QMailAccountIdList accountIds(QMailStore::instance()->queryAccounts(QMailAccountKey::name(parentAccountName)));
        if (accountIds.count() == 1) {
            QMailFolder folder;
            folder.setPath(path);
            folder.setParentAccountId(accountIds.first());

            if (!displayName.isEmpty()) {
                folder.setDisplayName(displayName);
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

    if (!to.isEmpty() && !from.isEmpty() && !date.isEmpty() && !subject.isEmpty() &&
        !parentAccountName.isEmpty() && !parentFolderPath.isEmpty()) {
        // Find the named account
        QMailAccountIdList accountIds(QMailStore::instance()->queryAccounts(QMailAccountKey::name(parentAccountName)));
        if (accountIds.count() == 1) {
            // Find the specified folder
            QMailFolderKey key(QMailFolderKey::path(parentFolderPath) & QMailFolderKey::parentAccountId(accountIds.first()));
            QMailFolderIdList folderIds(QMailStore::instance()->queryFolders(key));
            if (folderIds.count() == 1) {
                QMailMessage message;

                message.setStatus(QMailMessage::LocalOnly, true);

                message.setParentAccountId(accountIds.first());
                message.setParentFolderId(folderIds.first());

                message.setTo(QMailAddress::fromStringList(to));
                message.setFrom(QMailAddress(from));
                message.setSubject(subject);

                QDateTime dt(QDateTime::fromString(date, Qt::ISODate));
                dt.setTimeSpec(Qt::UTC);
                message.setDate(QMailTimeStamp(dt));

                if (type.isEmpty()) {
                    message.setMessageType(QMailMessage::Email);
                } else {
                    if (type.toLower() == "mms") {
                        message.setMessageType(QMailMessage::Mms);
                    } else if (type.toLower() == "sms") {
                        message.setMessageType(QMailMessage::Sms);
                    } else if (type.toLower() == "xmpp") {
                        message.setMessageType(QMailMessage::Instant);
                    } else {
                        message.setMessageType(QMailMessage::Email);
                    }
                }

                if (!receivedDate.isEmpty()) {
                    QDateTime dt(QDateTime::fromString(receivedDate, Qt::ISODate));
                    dt.setTimeSpec(Qt::UTC);
                    message.setReceivedDate(QMailTimeStamp(dt));
                }

                if (!priority.isEmpty()) {
                    if (priority.toLower() == "high") {
                        message.setStatus(QmfHelpers::highPriorityMask(), true);
                    } else if (priority.toLower() == "low") {
                        message.setStatus(QmfHelpers::lowPriorityMask(), true);
                    }
                }

                if (!text.isEmpty()) {
                    QMailMessageContentType ct(mimeType.toAscii());

                    QByteArray charset(QMessage::preferredCharsetFor(text));
                    if (charset.isEmpty()) {
                        charset = "UTF-8";
                    }
                    ct.setCharset(charset);

                    if (!attachments.isEmpty()) {
                        // Add the body as the first of multiple parts
                        message.setMultipartType(QMailMessage::MultipartMixed);

                        QMailMessageContentDisposition cd(QMailMessageContentDisposition::Inline);
                        cd.setSize(text.length());

                        QMailMessagePart textPart(QMailMessagePart::fromData(text, cd, ct, QMailMessageBody::Base64));
                        message.appendPart(textPart);
                    } else {
                        message.setBody(QMailMessageBody::fromData(text, ct, QMailMessageBody::Base64));
                        message.setStatus(QMailMessage::ContentAvailable, true);
                    }
                }

                if (read.toLower() == "true") {
                    message.setStatus(QMailMessage::Read, true);
                }

                if (hasAttachments.toLower() == "true") {
                    message.setStatus(QMailMessage::HasAttachments, true);
                }

                if (!attachments.isEmpty()) {
                    foreach (const QString &fileName, attachments.split("\n")) {
                        QString mimeType(QMail::mimeTypeFromFileName(fileName));
                        if (!mimeType.isEmpty()) {
                            QFileInfo fi(fileName);

                            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Attachment);
                            cd.setFilename(fi.fileName().toAscii());
                            cd.setSize(fi.size());

                            QMailMessageContentType ct(mimeType.toAscii());
                            QMailMessagePart part(QMailMessagePart::fromFile(fileName, cd, ct, QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
                            message.appendPart(part);
                        }
                    }
                }

                Parameters::const_iterator it = params.begin(), end = params.end();
                for ( ; it != end; ++it) {
                    if (it.key().startsWith("custom-")) {
                        message.setCustomField(it.key().mid(7), it.value());
                    }
                }

                if (!size.isEmpty()) {
                    message.setSize(size.toUInt());
                } else {
                    // Estimate the message size
                    message.setSize(message.indicativeSize() * 1024);
                }

                if (!QMailStore::instance()->addMessage(&message)) {
                    qWarning() << "Unable to addMessage:" << to << from << date << subject;
                } else {
                    return QmfHelpers::convert(message.id());
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

