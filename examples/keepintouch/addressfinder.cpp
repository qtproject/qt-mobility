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

#include "addressfinder.h"
#include <qmessageaddress.h>
#include <QComboBox>
#include <QDateTime>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QMenuBar>
#include <QTabWidget>

namespace {

QString simpleEmailAddress(const QString &recipient)
{
    QString name;
    QString addressOnly;

    QMessageAddress::parseEmailAddress(recipient, &name, &addressOnly);

    return addressOnly;
}

QString simpleEmailAddress(const QMessageAddress &address)
{
    return simpleEmailAddress(address.recipient());
}

}

AddressFinder::AddressFinder(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      tabWidget(0),
      includePeriod(0),
      excludePeriod(0),
      searchAction(0),
      searchButton(0),
      addressList(0),
      messageCombo(0)
 {
    setupUi();

    connect(&serviceAction, SIGNAL(stateChanged(QMessageServiceAction::State)), this, SLOT(stateChanged(QMessageServiceAction::State)));
    connect(&serviceAction, SIGNAL(messagesFound(QMessageIdList)), this, SLOT(messagesFound(QMessageIdList)));
}

AddressFinder::~AddressFinder()
{
}

void AddressFinder::includePeriodChanged(int selected)
{
    // Only allow smaller periods to be excluded
    excludePeriod->clear();

    switch (selected) {
        case 0: excludePeriod->insertItem(0, "9 Months"); // fall through:
        case 1: excludePeriod->insertItem(0, "6 Months");
        case 2: excludePeriod->insertItem(0, "3 Months");
        case 3: excludePeriod->insertItem(0, "Month");
        case 4: excludePeriod->insertItem(0, "Week");
        default: break;
    }

    excludePeriod->setCurrentIndex(0);
}

//! [address-selected]
void AddressFinder::addressSelected(const QString &address)
{
    messageCombo->clear();

    QString addressOnly(simpleEmailAddress(address));

    // Add the subject of each message to this address to the message pane
    typedef QPair<QString, QMessageId> MessageDetails;
    foreach (const MessageDetails &message, addressMessages[addressOnly]) {
        messageCombo->addItem(message.first);
    }
}
//! [address-selected]

void AddressFinder::searchMessages()
{
    setSearchActionEnabled(false);

    addressList->clear();
    messageCombo->clear();
    excludedAddresses.clear();
    addressMessages.clear();

//! [create-date-range]
    QDateTime now(QDateTime::currentDateTime());

    // Determine the dates that demarcate the selected range
    QDateTime minimumDate(now);
    switch (includePeriod->currentIndex()) {
        case 0: minimumDate = minimumDate.addMonths(-12); break;
        case 1: minimumDate = minimumDate.addMonths(-9); break;
        case 2: minimumDate = minimumDate.addMonths(-6); break;
        case 3: minimumDate = minimumDate.addMonths(-3); break;
        case 4: minimumDate = minimumDate.addMonths(-1); break;
        case 5: minimumDate = minimumDate.addDays(-7); break;
        default: break;
    }

    QDateTime maximumDate(now);
    switch (excludePeriod->currentIndex()) {
        case 0: maximumDate = maximumDate.addDays(-7); break;
        case 1: maximumDate = maximumDate.addMonths(-1); break;
        case 2: maximumDate = maximumDate.addMonths(-3); break;
        case 3: maximumDate = maximumDate.addMonths(-6); break;
        case 4: maximumDate = maximumDate.addMonths(-9); break;
        default: break;
    }
//! [create-date-range]

//! [create-simple-filters]
    // We will include addresses contacted following the minimum date
    QMessageFilter includeFilter(QMessageFilter::byTimeStamp(minimumDate, QMessageDataComparator::GreaterThanEqual));

    // We will exclude addresses contacted following the maximum date
    QMessageFilter excludeFilter(QMessageFilter::byTimeStamp(maximumDate, QMessageDataComparator::GreaterThanEqual));
//! [create-simple-filters]

    // Not sure why reception timestamp is relevant to sent messages...
    includeFilter |= QMessageFilter::byReceptionTimeStamp(minimumDate, QMessageDataComparator::GreaterThanEqual);
    excludeFilter |= QMessageFilter::byReceptionTimeStamp(maximumDate, QMessageDataComparator::GreaterThanEqual);

    QMessageFilter exclusionFilter;

//! [create-composite-filters]
    // We only want to match messages that we sent
    QMessageFilter sentFilter(QMessageFilter::byStandardFolder(QMessage::SentFolder));

    // Create the filter needed to locate messages whose address we will exclude
    exclusionFilter = (sentFilter & excludeFilter);

    // Create the filter needed to locate messages to search for addresses to include
    inclusionFilter = (sentFilter & includeFilter & ~excludeFilter);
//! [create-composite-filters]

//! [begin-search]
    // Start the search for messages containing addresses to exclude
    serviceAction.queryMessages(exclusionFilter);
//! [begin-search]
}

//! [handle-search-result]
void AddressFinder::stateChanged(QMessageServiceAction::State s)
{
    if (s == QMessageServiceAction::Successful) {
        if (!inclusionFilter.isEmpty()) {
            // Now find the included messages
            serviceAction.queryMessages(inclusionFilter);

            // Clear the inclusion filter to indicate that we have searched for it
            inclusionFilter = QMessageFilter();
        } else {
            // We have found the exclusion and inclusion message sets
            if (!inclusionMessages.isEmpty()) {
                // Begin processing the message sets
                QTimer::singleShot(0, this, SLOT(continueSearch()));
//! [handle-search-result]
            } else {
                searchAction->setEnabled(true);
#ifdef USE_SEARCH_BUTTON
                searchButton->setEnabled(true);
#endif
            }
        }
    } else if (s == QMessageServiceAction::Failed) {
        qWarning() << "Search failed!";
        setSearchActionEnabled(true);
    }
}

//! [accumulate-matches]
void AddressFinder::messagesFound(const QMessageIdList &ids)
{
    // Add these IDs to the relevant set
    if (!inclusionFilter.isEmpty()) {
        exclusionMessages << ids;
    } else {
        inclusionMessages << ids;
    }
}
//! [accumulate-matches]

//! [continue-search]
void AddressFinder::continueSearch()
{
    if (!exclusionMessages.isEmpty()) {
        // Take the first message whose addreses we should exclude
        QMessageId id(exclusionMessages.takeFirst());
        const QMessage message(id);

        // All recipient addresses are to be excluded
        foreach (const QMessageAddress &address, message.to() + message.cc() + message.bcc()) {
            excludedAddresses.insert(simpleEmailAddress(address));
        }
    } else if (!inclusionMessages.isEmpty()) {
        // Take the first message to inspect for suitable addresses
        QMessageId id(inclusionMessages.takeFirst());
        const QMessage message(id);

        QString details;

        // For each recipient of this message
        foreach (const QMessageAddress &address, message.to() + message.cc() + message.bcc()) {
            QString addressOnly(simpleEmailAddress(address));

            // Ignore recipients whose addresses we have added to the exclusion set
            if (!excludedAddresses.contains(addressOnly)) {
                // Link this message to this address
                QList<QPair<QString, QMessageId> > &messageList(addressMessages[addressOnly]);
                if (messageList.isEmpty()) {
                    // Add the recipient to our visible list of addresses to keep in touch with
                    addressList->addItem(address.recipient());
                }

                if (details.isEmpty()) {
                    // Determine the properties of the message
                    details = QString("[%1] %2").arg(message.date().toString("MMM d")).arg(message.subject());
                }
                messageList.append(qMakePair(details, id));
            }
        }
    }

    if (!exclusionMessages.isEmpty() || !inclusionMessages.isEmpty()) {
        // There are more messages to process
        QTimer::singleShot(0, this, SLOT(continueSearch()));
    } else {
        // We're finished our search
        setSearchActionEnabled(true);
#ifndef USE_SEARCH_BUTTON
        tabChanged(1);
#endif

        if (addressList->currentItem()) {
            // Select the first address automatically
            addressSelected(addressList->currentItem()->text());
        }
    }
}
//! [continue-search]

#ifndef USE_SEARCH_BUTTON
void AddressFinder::tabChanged(int index)
{
    QWidget* currentTab = tabWidget->currentWidget();
    QAction* action = 0;
    if(currentTab && !currentTab->actions().isEmpty())
        action = currentTab->actions().first();
    menuBar()->setDefaultAction(action);

    Q_UNUSED(index)
}
#endif

void AddressFinder::setupUi()
{
    setWindowTitle(tr("Keep In Touch"));

#ifndef USE_SEARCH_BUTTON
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
#else
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
#endif

    QGroupBox *inputGroup = new QGroupBox(tr("Find addresses"));
    inputGroup->setAlignment(Qt::AlignLeft);
#ifndef USE_SEARCH_BUTTON
    tabWidget->addTab(inputGroup,"Search");
#else
    centralLayout->addWidget(inputGroup);
#endif

    QGridLayout *filterLayout = new QGridLayout(inputGroup);

    QLabel *includeLabel = new QLabel(tr("Contacted this"));
    filterLayout->addWidget(includeLabel, 0, 0);
    filterLayout->setAlignment(includeLabel, Qt::AlignRight);

    QLabel *excludeLabel = new QLabel(tr("But not last"));
    filterLayout->addWidget(excludeLabel, 1, 0);
    filterLayout->setAlignment(excludeLabel, Qt::AlignRight);

    includePeriod = new QComboBox;
    filterLayout->addWidget(includePeriod, 0, 1);
    includePeriod ->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    includePeriod->addItem(tr("Year"));
    includePeriod->addItem(tr("9 Months"));
    includePeriod->addItem(tr("6 Months"));
    includePeriod->addItem(tr("3 Months"));
    includePeriod->addItem(tr("Month"));
    includePeriod->addItem(tr("Week"));
    connect(includePeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(includePeriodChanged(int)));

    excludePeriod = new QComboBox;
    excludePeriod ->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filterLayout->addWidget(excludePeriod, 1, 1);

#ifdef USE_SEARCH_BUTTON
    searchButton = new QPushButton(tr("Search"));
    searchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    filterLayout->addWidget(searchButton,0,2,2,1,Qt::AlignVCenter | Qt::AlignHCenter);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(searchMessages()), Qt::QueuedConnection);
#endif

    addressList = new QListWidget(this);
    connect(addressList, SIGNAL(currentTextChanged(QString)), this, SLOT(addressSelected(QString)));

#ifndef USE_SEARCH_BUTTON
    QWidget* resultsWidget = new QWidget(this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsWidget);
    tabWidget->addTab(resultsWidget,"Results");
#else
    QVBoxLayout* resultsLayout = centralLayout;
#endif

    QGroupBox *addressGroup = new QGroupBox(tr("Addresses"));
    addressGroup->setAlignment(Qt::AlignLeft);
    addressGroup->setLayout(new QVBoxLayout);
    addressGroup->layout()->addWidget(addressList);
    resultsLayout->addWidget(addressGroup);

    QGroupBox *messageGroup = new QGroupBox(tr("Messages"));
    messageGroup->setAlignment(Qt::AlignLeft);

    QVBoxLayout *groupLayout = new QVBoxLayout;

    messageCombo = new QComboBox;
    connect(messageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(messageIndexChanged(int)));

    groupLayout->addWidget(messageCombo);

    showButton = new QPushButton(tr("Show..."));
    showButton->setEnabled(false);
    connect(showButton, SIGNAL(clicked()), this, SLOT(showMessage()));

    forwardButton = new QPushButton(tr("Forward..."));
    forwardButton->setEnabled(false);
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forwardMessage()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(showButton);
    buttonLayout->addWidget(forwardButton);

    groupLayout->addLayout(buttonLayout);

    messageGroup->setLayout(groupLayout);
    resultsLayout->addWidget(messageGroup);

    searchAction = new QAction("Search",this);
    inputGroup->addAction(searchAction);
    connect(searchAction,SIGNAL(triggered()),this,SLOT(searchMessages()),Qt::QueuedConnection);
    QAction* quitAction = menuBar()->addAction("Quit");
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

    includePeriodChanged(0);
#ifndef USE_SEARCH_BUTTON
    tabChanged(0);
#endif
}

void AddressFinder::setSearchActionEnabled(bool val)
{
    searchAction->setEnabled(val);
#ifdef USE_SEARCH_BUTTON
    searchButton->setEnabled(val);
#endif
}

void AddressFinder::messageIndexChanged(int index)
{
    bool messageSelected(index != -1);
    showButton->setEnabled(messageSelected);
    forwardButton->setEnabled(messageSelected);
}

void AddressFinder::showMessage()
{
    int index = messageCombo->currentIndex();
    if (index != -1) {
        // Find the address currently selected
        QString selectedAddress(simpleEmailAddress(addressList->currentItem()->text()));

        // Show the message selected
        QMessageId &messageId((addressMessages[selectedAddress])[index].second);
        serviceAction.show(messageId);
    }
}

void AddressFinder::forwardMessage()
{
    int index = messageCombo->currentIndex();
    if (index != -1) {
        // Find the address currently selected
        QString selectedAddress(simpleEmailAddress(addressList->currentItem()->text()));

        // Create a message which forwards the selected message to the same recipient
        QMessageId &messageId((addressMessages[selectedAddress])[index].second);
        
        QMessage original(messageId);
        QMessage fwd(original.createResponseMessage(QMessage::Forward));
        fwd.setTo(original.to());

        serviceAction.compose(fwd);
    }
}
