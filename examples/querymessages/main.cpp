/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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

