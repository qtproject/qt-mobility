/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "addressfinder.h"

#include <QComboBox>
#include <QDateTime>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QDebug>

AddressFinder::AddressFinder(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags),
      includePeriod(0),
      excludePeriod(0),
      addressList(0),
      messageList(0)
{
    excludePeriod = new QComboBox;

    includePeriod = new QComboBox;
    includePeriod->addItem(tr("Year"));
    includePeriod->addItem(tr("9 Months"));
    includePeriod->addItem(tr("6 Months"));
    includePeriod->addItem(tr("3 Months"));
    includePeriod->addItem(tr("Month"));

    includePeriodChanged(0);

    connect(includePeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(includePeriodChanged(int)));

    QGridLayout *filterayout = new QGridLayout;
    filterayout->addWidget(new QLabel(tr("Contacted within the last")), 0, 0);
    filterayout->addWidget(new QLabel(tr("But not the last")), 1, 0);
    filterayout->addWidget(includePeriod, 0, 1);
    filterayout->addWidget(excludePeriod, 1, 1);

    QGroupBox *inputGroup = new QGroupBox(tr("Find addresses"));
    inputGroup->setLayout(filterayout);

    QPushButton *searchButton = new QPushButton(tr("Search"));

    connect(searchButton, SIGNAL(clicked()), this, SLOT(searchMessages()));

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(inputGroup);
    inputLayout->addWidget(searchButton);

    addressList = new QListWidget;

    connect(addressList, SIGNAL(currentTextChanged(QString)), this, SLOT(addressSelected(QString)));

    QGroupBox *addressGroup = new QGroupBox(tr("Address"));
    addressGroup->setLayout(new QHBoxLayout);
    addressGroup->layout()->addWidget(addressList);

    messageList = new QListWidget;

    QGroupBox *messageGroup = new QGroupBox(tr("Messages"));
    messageGroup->setLayout(new QHBoxLayout);
    messageGroup->layout()->addWidget(messageList);

    QHBoxLayout *outputLayout = new QHBoxLayout;
    outputLayout->addWidget(addressGroup);
    outputLayout->addWidget(messageGroup);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(inputLayout);
    mainLayout->addLayout(outputLayout);
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

void AddressFinder::addressSelected(const QString &address)
{
    messageList->clear();

    foreach (const QString &message, addressMessages[address]) {
        messageList->addItem(new QListWidgetItem(message));
    }
}

void AddressFinder::searchMessages()
{
    // TODO: make this unnecessary:
    QMessageStore::instance();

    addressList->clear();
    messageList->clear();
    addressMessages.clear();

    QDateTime now(QDateTime::currentDateTime());

    QDateTime minimumDate(now);
    switch (includePeriod->currentIndex()) {
        case 0: minimumDate = minimumDate.addMonths(-12); break;
        case 1: minimumDate = minimumDate.addMonths(-9); break;
        case 2: minimumDate = minimumDate.addMonths(-6); break;
        case 3: minimumDate = minimumDate.addMonths(-3); break;
        case 4: minimumDate = minimumDate.addMonths(-1); break;
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

    QMessageFilterKey includeFilter(QMessageFilterKey::timeStamp(minimumDate, QMessageDataComparator::GreaterThanEqual));
    QMessageFilterKey excludeFilter(QMessageFilterKey::timeStamp(maximumDate, QMessageDataComparator::GreaterThanEqual));
    QMessageFilterKey outgoingFilter(QMessageFilterKey::status(QMessage::Incoming, QMessageDataComparator::Includes));

    // Find all outgoing messages that are within the exclusion range
    exclusionMessages = QMessageStore::instance()->queryMessages(~outgoingFilter & excludeFilter);

    // Find all outgoing messages that are within the inclusion range
    inclusionMessages = QMessageStore::instance()->queryMessages(~outgoingFilter & includeFilter & ~excludeFilter);

    if (!exclusionMessages.isEmpty() || !inclusionMessages.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(continueSearch()));
    }
}

void AddressFinder::continueSearch()
{
    if (!exclusionMessages.isEmpty()) {
        QMessageId id(exclusionMessages.takeFirst());
        const QMessage message(id);

        // All recipient addresses are to be excluded
        foreach (const QMessageAddress &address, message.to()) {
            excludedAddresses.insert(address.recipient());
        }
    } else if (!inclusionMessages.isEmpty()) {
        QMessageId id(inclusionMessages.takeFirst());
        const QMessage message(id);

        // Determine the properties of the message
        QString details(QString("[%1] %2").arg(message.date().toString()).arg(message.subject()));

        foreach (const QMessageAddress &address, message.to()) {
            QString recipient(address.recipient());
            if (!excludedAddresses.contains(recipient)) {
                // Link this message to this address
                QStringList &messages = addressMessages[recipient];
                if (messages.isEmpty()) {
                    addressList->addItem(recipient);
                }
                messages.append(details);
            }
        }
    }

    if (!exclusionMessages.isEmpty() || !inclusionMessages.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(continueSearch()));
    }
}

