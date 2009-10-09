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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "addressfinder.h"
#include "addresshelper.h"
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

void parseAddress(const QString& addressText, QString *name, QString *address)
{
    QString suffix;
    QString input = addressText.trimmed();

    if (!addressText.isEmpty()) {
        QString input = addressText.trimmed();

        // See whether this address is a group
        if (qContainsGroupSpecifier(input)) {
            QRegExp groupFormat("(.*):(.*);");
            if (groupFormat.indexIn(input) != -1) {
                *name = groupFormat.cap(1).trimmed();
                *address = groupFormat.cap(2).trimmed();
            }
        } else {
            qParseMailbox(input, *name, *address, suffix);
            *address = address->toLower();
        }
    } else {
        *name = *address = QString();
    }
}

AddressFinder::AddressFinder(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      includePeriod(0),
      excludePeriod(0),
      searchAction(0),
      searchButton(0),
	  addressCombo(0),
      messageList(0),
	  tabWidget(0)
{
	setupUi();

	connect(&service, SIGNAL(stateChanged(QMessageServiceAction::State)), this, SLOT(stateChanged(QMessageServiceAction::State)));
    connect(&service, SIGNAL(messagesFound(QMessageIdList)), this, SLOT(messagesFound(QMessageIdList)));
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
        case 5: excludePeriod->insertItem(0, "Second");
        default: break;
    }

    excludePeriod->setCurrentIndex(0);
}

void AddressFinder::addressSelected(const QString &address)
{
    QString name;
    QString addressOnly;
    parseAddress(address, &name, &addressOnly);
    messageList->clear();

    foreach (const QString &message, addressMessages[addressOnly]) {
        messageList->addItem(new QListWidgetItem(message));
    }
}

void AddressFinder::searchMessages()
{
    setSearchActionEnabled(false);

	addressCombo->clear();
    messageList->clear();
    excludedAddresses.clear();
    includedAddresses.clear();
    addressMessages.clear();

    QDateTime now(QDateTime::currentDateTime());

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
        case 0: maximumDate = maximumDate.addSecs(-1); break;
        case 1: maximumDate = maximumDate.addDays(-7); break;
        case 2: maximumDate = maximumDate.addMonths(-1); break;
        case 3: maximumDate = maximumDate.addMonths(-3); break;
        case 4: maximumDate = maximumDate.addMonths(-6); break;
        case 5: maximumDate = maximumDate.addMonths(-9); break;
        default: break;
    }

    QMessageFilter includeFilter(QMessageFilter::byTimeStamp(minimumDate, QMessageDataComparator::GreaterThanEqual));
    QMessageFilter excludeFilter(QMessageFilter::byTimeStamp(maximumDate, QMessageDataComparator::GreaterThanEqual));
    // Would be faster and more accurate to just examine the sent folder, outgoingFilter also includes drafts.
    QMessageFilter outgoingFilter(QMessageFilter::byStatus(QMessage::Incoming, QMessageDataComparator::Excludes));

    // Search for messages containing addresses to exclude
    service.queryMessages(outgoingFilter & excludeFilter);

    // Create the filter needed to locate messages to search for addresses
    inclusionFilter = (outgoingFilter & includeFilter & ~excludeFilter);
}

void AddressFinder::stateChanged(QMessageServiceAction::State s)
{
    if (s == QMessageServiceAction::Successful) {
        if (!inclusionFilter.isEmpty()) {
            // Now find the included messages
            service.queryMessages(inclusionFilter);
            inclusionFilter = QMessageFilter();
        } else {
            // We have found the message sets to process
            if (!inclusionMessages.isEmpty()) {
                QTimer::singleShot(0, this, SLOT(continueSearch()));
            } else {
                searchAction->setEnabled(true);
                searchButton->setEnabled(true);
            }
        }
    } else if (s == QMessageServiceAction::Failed) {
        qWarning() << "Search failed!";
        setSearchActionEnabled(true);
    }
}

void AddressFinder::messagesFound(const QMessageIdList &ids)
{
    if (!inclusionFilter.isEmpty()) {
        exclusionMessages << ids;
    } else {
        inclusionMessages << ids;
    }
}

void AddressFinder::continueSearch()
{
    QString name;
    QString addressOnly;
    if (!exclusionMessages.isEmpty()) {
        QMessageId id(exclusionMessages.takeFirst());
        const QMessage message(id);

        // All recipient addresses are to be excluded
        foreach (const QMessageAddress &address, message.to() + message.cc() + message.bcc()) {
            parseAddress(address.recipient(), &name, &addressOnly);
            if (!excludedAddresses.contains(addressOnly))
                qDebug() << "Exclude" << addressOnly;
            excludedAddresses.insert(addressOnly);
        }
    } else if (!inclusionMessages.isEmpty()) {
        QMessageId id(inclusionMessages.takeFirst());
        const QMessage message(id);

        // Determine the properties of the message
        QString details(QString("[%1] %2").arg(message.date().toString("MMM d")).arg(message.subject()));

        foreach (const QMessageAddress &address, message.to() + message.cc() + message.bcc()) {
            QString recipient(address.recipient());
            parseAddress(address.recipient(), &name, &addressOnly);
            if (!includedAddresses.contains(addressOnly))
                qDebug() << "Include" << addressOnly;
            includedAddresses.insert(addressOnly);
            if (!excludedAddresses.contains(addressOnly)) {
                // Link this message to this address
                qDebug() << "Exclude" << addressOnly;
                QStringList &messages = addressMessages[addressOnly];
                if (messages.isEmpty()) {
					addressCombo->addItem(recipient);
                }
                messages.append(details);
            }
        }
    }

    if (!exclusionMessages.isEmpty() || !inclusionMessages.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(continueSearch()));
    } else {
        setSearchActionEnabled(true);
#ifdef _WIN32_WCE
		tabChanged(1);
#endif
        addressSelected(addressCombo->currentText());
    }
}

#ifdef _WIN32_WCE
void AddressFinder::tabChanged(int index)
{
	QWidget* currentTab = tabWidget->currentWidget();
	QAction* action = 0;
	if(currentTab && !currentTab->actions().isEmpty())
		action = currentTab->actions().first();
    menuBar()->setDefaultAction(action);
}
#endif

void AddressFinder::setupUi()
{
	setWindowTitle(tr("Keep In Touch"));

#ifdef _WIN32_WCE
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
#ifdef _WIN32_WCE
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

#ifndef _WIN32_WCE
    searchButton = new QPushButton(tr("Search"));
    searchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	filterLayout->addWidget(searchButton,0,2,2,1,Qt::AlignVCenter | Qt::AlignHCenter);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(searchMessages()), Qt::QueuedConnection);
#endif

	addressCombo = new QComboBox(this);
    connect(addressCombo, SIGNAL(activated(QString)), this, SLOT(addressSelected(QString)));

#ifdef _WIN32_WCE
	QWidget* resultsWidget = new QWidget(this);
	QVBoxLayout* resultsLayout = new QVBoxLayout(resultsWidget);
	tabWidget->addTab(resultsWidget,"Results");
#else
    QVBoxLayout* resultsLayout = centralLayout;
#endif

    QGroupBox *addressGroup = new QGroupBox(tr("Address"));
    addressGroup->setAlignment(Qt::AlignLeft);
    addressGroup->setLayout(new QVBoxLayout);
	addressGroup->layout()->addWidget(addressCombo);
	resultsLayout->addWidget(addressGroup);

    QGroupBox *messageGroup = new QGroupBox(tr("Messages"));
    messageGroup->setAlignment(Qt::AlignLeft);
    messageGroup->setLayout(new QHBoxLayout);
	resultsLayout->addWidget(messageGroup);

	messageList = new QListWidget;
    messageList->setSelectionMode(QAbstractItemView::NoSelection);
	messageGroup->layout()->addWidget(messageList);

    searchAction = new QAction("Search",this);
	inputGroup->addAction(searchAction);
    connect(searchAction,SIGNAL(triggered()),this,SLOT(searchMessages()),Qt::QueuedConnection);
    QAction* quitAction = menuBar()->addAction("Quit");
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

	includePeriodChanged(0);
#ifdef _WIN32_WCE
	tabChanged(0);
#endif
}

void AddressFinder::setSearchActionEnabled(bool val)
{
    searchAction->setEnabled(val);
#ifndef _WIN32_WCE
    searchButton->setEnabled(val);
#endif
}
