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
#include <QMap>
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

void usage()
{
    QString msg("Usage: [<options>] [<datafields>]\n"
                  " Where <options> limit the types of messages shown, valid choices: -all, -sms, -mms, -email, -instantmessage, -incoming, -read, -inboxfolder, -draftsfolder, -outboxfolder, -sentfolder, -trashfolder\n"
                  "         To join options by logical OR use +, for option, eg \"+sms +mms\" will return sms OR mms messages\n"
                  " Where <datafields> specify what fields to print from matched messages, valid choices are: subject, date, receivedDate, status, size, priority, to, cc, bcc, from, type, body, attachments");
    printMessage(msg);
}

// create message filter as requested by command line options
QMessageFilter createFilter(QStringList args)
{
    QMessageFilter filter(QMessageFilter::byStatus(QMessage::Incoming));
    QMessageFilter customfilter;
    bool setCustomFilter = false;
    QMap<QString, QMessageFilter> filterMap;
    filterMap.insert(QLatin1String("all"), QMessageFilter());
    filterMap.insert(QLatin1String("sms"), QMessageFilter::byType(QMessage::Sms));
    filterMap.insert(QLatin1String("mms"), QMessageFilter::byType(QMessage::Mms));
    filterMap.insert(QLatin1String("email"), QMessageFilter::byType(QMessage::Email));
    filterMap.insert(QLatin1String("instantmessage"), QMessageFilter::byType(QMessage::InstantMessage));
    filterMap.insert(QLatin1String("incoming"), QMessageFilter::byStatus(QMessage::Incoming));
    filterMap.insert(QLatin1String("read"), QMessageFilter::byStatus(QMessage::Read));
    filterMap.insert(QLatin1String("inboxfolder"), QMessageFilter::byStandardFolder(QMessage::InboxFolder));
    filterMap.insert(QLatin1String("draftsfolder"), QMessageFilter::byStandardFolder(QMessage::DraftsFolder));
    filterMap.insert(QLatin1String("outboxfolder"), QMessageFilter::byStandardFolder(QMessage::OutboxFolder));
    filterMap.insert(QLatin1String("sentfolder"), QMessageFilter::byStandardFolder(QMessage::SentFolder));
    filterMap.insert(QLatin1String("trashfolder"), QMessageFilter::byStandardFolder(QMessage::TrashFolder));

    // process command line options after the application name
    foreach (const QString &arg, args.mid(1)){
        QMap<QString, QMessageFilter>::const_iterator iterator = filterMap.find(arg.toLower().mid(1));
        if (iterator != filterMap.end()){
            setCustomFilter = true;
            // use AND logic when compounding filter?
            if (arg[0] == '-' )
                customfilter = customfilter & iterator.value();
            else
                customfilter = customfilter | iterator.value();
        }
    }

    if (setCustomFilter)
        filter = customfilter;

    return filter;
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    printMessage("Querying messages...");

    // determine what data options were requested and if help was requested
    // if no data options were provided default to "subject"
    QStringList dataOptions;
    QStringList validDataOptions = QStringList() << "subject" << "date" << "receivedDate" << "status"
                    << "size" << "priority" << "to" << "cc" << "bcc" << "from"
                    << "type" << "body" << "attachments";
    foreach (const QString &arg, app.arguments()){
        if (arg == "-help"){
            usage();
            exit(1);
        }
        if (validDataOptions.contains(arg))
            dataOptions.append(arg);
    }
    if (dataOptions.isEmpty())
        dataOptions.append("subject");

//! [setup-query]
    // Create filter to match required messages
    QMessageFilter filter = createFilter(app.arguments());
    // instead of the above use the following for a simple filter of all messages whose status field have the Incoming flag to be set, eg incoming emails
    //QMessageFilter filter(QMessageFilter::byStatus(QMessage::Incoming));

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

//! [generate-output]
            // Extract the requested data items from this message
            foreach (const QString &arg, dataOptions) {
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
                } else if (arg == "status") {
                    result.append(QString("0x%1").arg(QString::number(message.status())));
                } else if (arg == "attachments") {
                    QStringList fileNames;
                    foreach (const QMessageContentContainerId &id, message.attachmentIds()) {
                        fileNames.append(message.find(id).suggestedFileName());
                    }
                    result.append(fileNames.join(","));
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

