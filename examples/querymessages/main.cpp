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

#include <QCoreApplication>
#include <qmessagemanager.h>
#include <qdebug.h>
#ifdef Q_OS_SYMBIAN
#include <iostream>
#endif

QTM_USE_NAMESPACE

static void printMessage(const QString& msg)
{
#ifdef Q_OS_SYMBIAN
    std::cout << qPrintable(msg) << std::endl;
#else
    qDebug() << qPrintable(msg);
#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    printMessage("Querying messages...");

//! [setup-query]
    // Match all messages whose status field includes the Incoming flag
    QMessageFilter filter(QMessageFilter::byStatus(QMessage::Incoming));

    // Order the matching results by their reception timestamp, in descending order
    QMessageSortOrder sortOrder(QMessageSortOrder::byReceptionTimeStamp(Qt::DescendingOrder));
//! [setup-query]

//! [perform-query]
    // Acquire a handle to the message manager
    QMessageManager manager;

    // Find the matching message IDs, limiting our results to a managable number
    const int max = 100;
    const QMessageIdList matchingIds(manager.queryMessages(filter, sortOrder, max));
//! [perform-query]

    int n = 0;

//! [iterate-results]
    // Retrieve each message and print requested details
    foreach (const QMessageId &id, matchingIds) {
        QMessage message(manager.message(id));
//! [iterate-results]

        if (manager.error() == QMessageManager::NoError) {
            QStringList result;

            if (app.arguments().count() < 2) {
                // Default to printing only the subject
                result.append(message.subject());
            } else {
//! [generate-output]
                // Extract the requested data items from this message
                foreach (const QString &arg, app.arguments().mid(1)) {
                    if (arg == "subject") {
                        result.append(message.subject());
                    } else if (arg == "date") {
                        result.append(message.date().toLocalTime().toString());
//! [generate-output]
                    } else if (arg == "receivedDate") {
                        result.append(message.receivedDate().toLocalTime().toString());
                    } else if (arg == "size") {
                        result.append(QString::number(message.size()));
                    } else if (arg == "priority") {
                        result.append(message.priority() == QMessage::HighPriority ? "High" : (message.priority() == QMessage::LowPriority ? "Low" : "Normal"));
                    } else if ((arg == "to") || (arg == "cc") || (arg == "bcc")) {
                        QStringList addresses;
                        foreach (const QMessageAddress &addr, (arg == "to" ? message.to() : (arg == "cc" ? message.cc() : message.bcc()))) {
                            addresses.append(addr.addressee());
                        }
                        result.append(addresses.join(","));
                    } else if (arg == "from") {
                        result.append(message.from().addressee());
                    } else if (arg == "type") {
                        result.append(message.contentType() + '/' + message.contentSubType());
                    } else if (arg == "body") {
                        result.append(message.find(message.bodyId()).textContent());
                    } else if (arg == "attachments") {
                        QStringList fileNames;
                        foreach (const QMessageContentContainerId &id, message.attachmentIds()) {
                            fileNames.append(message.find(id).suggestedFileName());
                        }
                        result.append(fileNames.join(","));
                    }
                }
            }

//! [print-result]
            printMessage(QString::number(++n) + '\t' + result.join("\t"));
//! [print-result]
        }
    }

    if(matchingIds.isEmpty())
        printMessage("No matching messages!");

    return 0;
}

