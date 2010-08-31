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

#include <QtGui>
#include <qtorganizer.h>
#include <qorganizeritemabstractrequest.h>

#include "monthpage.h"
#include "calendardemo.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QOrganizerItem)

MonthPage::MonthPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    m_calendarWidget(0),
    m_dateLabel(0),
    m_itemList(0)
{
    // Create widgets
    QFormLayout *mainlayout = new QFormLayout(this);

    m_managerComboBox = new QComboBox(this);
    foreach (const QString& manager, QOrganizerItemManager::availableManagers()) {
        if (manager != "invalid" && manager != "skeleton")
            m_managerComboBox->addItem(manager);
    }
    mainlayout->addRow("Backend:", m_managerComboBox);
    connect(m_managerComboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(backendChanged(const QString &)));

    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    mainlayout->addRow(m_dateLabel);

    m_calendarWidget = new QCalendarWidget(this);
    m_calendarWidget->setGridVisible(true);
    m_calendarWidget->setHorizontalHeaderFormat(QCalendarWidget::SingleLetterDayNames);
    m_calendarWidget->setHeaderVisible(false);
    mainlayout->addRow(m_calendarWidget);
    connect(m_calendarWidget, SIGNAL(selectionChanged()), this, SLOT(refreshDayItems()));
    connect(m_calendarWidget, SIGNAL(currentPageChanged(int, int)), this, SLOT(currentMonthChanged()));
    connect(m_calendarWidget, SIGNAL(activated(QDate)), this, SLOT(dayDoubleClicked(QDate)));

    m_itemList = new QListWidget(this);
    m_itemList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_itemList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_itemList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_itemList->setFocusPolicy(Qt::NoFocus);
    mainlayout->addRow(m_itemList);
    connect(m_itemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    setLayout(mainlayout);

    // Add options softkey
    QAction* optionsSoftKey = new QAction("Options", this);
    optionsSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    optionsSoftKey->setMenu(new QMenu(this));
    addAction(optionsSoftKey);

    // Add actions to options menu
    QMenu *optionsMenu = optionsSoftKey->menu();
    QAction* openAction = optionsMenu->addAction("Open");
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openDay()));
    QAction* addEventAction = optionsMenu->addAction("Add Event");
    connect(addEventAction, SIGNAL(triggered(bool)), this, SIGNAL(addNewEvent()));
    QAction* addTodoAction = optionsMenu->addAction("Add Todo");
    connect(addTodoAction, SIGNAL(triggered(bool)), this, SIGNAL(addNewTodo()));
    QAction* addHugeEntires = optionsMenu->addAction("Add large no. of events");
    connect(addHugeEntires, SIGNAL(triggered(bool)), this, SLOT(addEvents()));
    QAction* deleteAllEntires = optionsMenu->addAction("Delete all entries");
    connect(deleteAllEntires, SIGNAL(triggered(bool)), this, SLOT(deleteAllEntries()));
    
    // Connect to the save and remove request status change signals
    connect(&m_saveReq, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)),
            this, SLOT(saveReqStateChanged(QOrganizerItemAbstractRequest::State)));
    connect(&m_remReq, SIGNAL(stateChanged(QOrganizerItemAbstractRequest::State)),
            this, SLOT(removeReqStateChanged(QOrganizerItemAbstractRequest::State)));
}

// This is separate from the constructor so we can do it after connecting the signals
void MonthPage::init()
{
    backendChanged(m_managerComboBox->currentText());
    refresh();
}

MonthPage::~MonthPage()
{

}

void MonthPage::backendChanged(const QString &managerName)
{
    // Nothing to change
    if (m_manager && m_manager->managerName() == managerName)
        return;

    // Try creating a new manager
    QMap<QString, QString> parameters;
    QOrganizerItemManager* newManager = new QOrganizerItemManager(managerName, parameters, this);
    if (!newManager || newManager->error()) {
        QMessageBox::information(this, tr("Failed!"), QString("Failed to create manager"));
        delete newManager;
        m_managerComboBox->setCurrentIndex(m_managerComboBox->findText(m_manager->managerName()));
    } else {
        // Success: Replace the old one
        delete m_manager;
        m_manager = newManager;
        emit managerChanged(m_manager);
        refresh();
    }
}

void MonthPage::addEvents()
{
    QList<QOrganizerItem> items;
    
    // Create a large number of events asynchronously
    for(int index=0 ; index <  100 ; index++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeEvent);
        item.setDescription(QString("Event %1").arg(index));
        item.setDisplayLabel(QString("Subject for event %1").arg(index + 1));
        
        // Set the start date to index to add events to next 5000 days
        QOrganizerEventTimeRange timeRange;
        timeRange.setStartDateTime(QDateTime::currentDateTime().addDays(index));
        item.saveDetail(&timeRange);
        
        items.append(item);
    }
    
    // Now create a save request and execute it
    m_saveReq.setItems(items);
    m_saveReq.setManager(m_manager);
    m_saveReq.start();
}

void MonthPage::deleteAllEntries()
{
    // Fetch all the entries
    QList<QOrganizerItemLocalId> ids = m_manager->itemIds();
    
    if(ids.count()) {
        m_remReq.setItemIds(ids);
        m_remReq.setManager(m_manager);
        m_remReq.start();
    }
}

void MonthPage::refresh()
{
    // fetch all current months items
    QDate start(m_calendarWidget->yearShown(), m_calendarWidget->monthShown(), 1);
    QDate end = start.addDays(start.daysInMonth());
    QDateTime startDateTime(start, QTime(0, 0, 0, 0));
    QDateTime endDateTime(end, QTime(23, 59, 59, 0));
    // Remove all previous formatting
    for (int i=0; i<start.daysInMonth(); i++) {
        QTextCharFormat cf = m_calendarWidget->dateTextFormat(start.addDays(i));
        cf.setFontItalic(false);
        cf.setFontUnderline(false);
        cf.clearBackground();
        m_calendarWidget->setDateTextFormat(start.addDays(i), cf);
    }

    // Underline current date
    QTextCharFormat cf = m_calendarWidget->dateTextFormat(QDate::currentDate());
    cf.setFontUnderline(true);
    m_calendarWidget->setDateTextFormat(QDate::currentDate(), cf);

    // TODO: switch to item instances when theres a backed
    QList<QOrganizerItem> items = m_manager->items();

    // Get dates for all items
    QList<QDate> dates;
    QList<QOrganizerItem> instanceList;
    instanceList.clear();
    foreach (const QOrganizerItem &item, items)
    {   
        // Get the instances of the item for that month and collect those dates.
        instanceList.append(m_manager->itemInstances(item, startDateTime, endDateTime));
        for (int count = 0; count < instanceList.count() ; count++) {
            QOrganizerEventTimeRange eventTimeRange = instanceList.at(count).detail<QOrganizerEventTimeRange>();
            QDate startDate(eventTimeRange.startDateTime().date());
            QDate endDate(eventTimeRange.endDateTime().date());
            while (startDate <= endDate) {
                dates << (eventTimeRange.startDateTime().date());
                startDate = startDate.addDays(1);
            }
        }

        QOrganizerTodoTimeRange todoTimeRange = item.detail<QOrganizerTodoTimeRange>();
        if (!todoTimeRange.isEmpty()) {
            dates << (todoTimeRange.startDateTime().date());
            //dates << (todoTimeRange.dueDateTime().date());
        }
        instanceList.clear();
        
        // TODO: other item types
    }

    // Mark all dates which has events.
    QBrush brush;
    brush.setColor(Qt::green);
    foreach (QDate date, dates) {
        QTextCharFormat cf = m_calendarWidget->dateTextFormat(date);
        cf.setFontItalic(true); // TODO: find a better way to mark dates
        cf.setBackground(brush);
        m_calendarWidget->setDateTextFormat(date, cf);
    }

    refreshDayItems();
}

void MonthPage::refreshDayItems()
{
    QDate selectedDate = m_calendarWidget->selectedDate();
    emit currentDayChanged(selectedDate);

    // Update date
    m_dateLabel->setText(selectedDate.toString());
    QDateTime startOfDay(selectedDate, QTime(0, 0, 0));
    QDateTime endOfDay(selectedDate, QTime(23, 59, 59));
    // Clear items shown
    m_itemList->clear();

    // Find all items for today
    // TODO: refactor this when we have itemInstances() working properly
    QList<QOrganizerItem> items = m_manager->items();
    QList<QOrganizerItem> instanceList;
    foreach (const QOrganizerItem &item, items)
    {
        instanceList.clear();
        instanceList.append(m_manager->itemInstances(item, startOfDay, endOfDay));
        if(instanceList.count() > 0) {
            QOrganizerEventTimeRange eventTimeRange = item.detail<QOrganizerEventTimeRange>();
            QString time = eventTimeRange.startDateTime().time().toString("hh:mm");
            QListWidgetItem* listItem = new QListWidgetItem();
            listItem->setText(QString("Event:%1-%2").arg(time).arg(item.displayLabel()));
            QVariant data = QVariant::fromValue<QOrganizerItem>(item);
            listItem->setData(ORGANIZER_ITEM_ROLE, data);
            m_itemList->addItem(listItem);
        }

        QOrganizerTodoTimeRange todoTimeRange = item.detail<QOrganizerTodoTimeRange>();
        if (!todoTimeRange.isEmpty()) {
            if (todoTimeRange.startDateTime().date() == selectedDate) {
                QString time = todoTimeRange.startDateTime().time().toString("hh:mm");
                QListWidgetItem* listItem = new QListWidgetItem();
                listItem->setText(QString("Todo:%1-%2").arg(time).arg(item.displayLabel()));
                QVariant data = QVariant::fromValue<QOrganizerItem>(item);
                listItem->setData(ORGANIZER_ITEM_ROLE, data);
                m_itemList->addItem(listItem);
            }
        }
        // TODO: other item types
        // TODO: icons for event/todo/journal would be nice
    }
    if (m_itemList->count() == 0)
        m_itemList->addItem("(no entries)");
}


void MonthPage::currentMonthChanged()
{
    refresh();
}

void MonthPage::dayDoubleClicked(QDate date)
{
    emit showDayPage(date);
}

void MonthPage::openDay()
{
    emit showDayPage(m_calendarWidget->selectedDate());
}

void MonthPage::itemDoubleClicked(QListWidgetItem *listItem)
{
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(organizerItem);
}

void MonthPage::saveReqStateChanged(QOrganizerItemAbstractRequest::State reqState)
{
    if(QOrganizerItemAbstractRequest::ActiveState == reqState) {
        // Request started. Show a progress or wait dialog
        m_progressDlg = new QProgressDialog("Saving events..", "Cancel", 100, 100, this);
        connect(m_progressDlg, SIGNAL(canceled()), &m_saveReq, SLOT(cancel()));
        m_progressDlg->show();
    } else if (QOrganizerItemAbstractRequest::FinishedState == reqState ||
               QOrganizerItemAbstractRequest::CanceledState == reqState) {
        // Request finished or cancelled. Stop showing the progress dialog and refresh
        m_progressDlg->hide();
        refresh();
    }
}

void MonthPage::removeReqStateChanged(QOrganizerItemAbstractRequest::State reqState)
{
    if(QOrganizerItemAbstractRequest::ActiveState == reqState) {
        // Request started. Show a progress or wait dialog
        m_progressDlg = new QProgressDialog("Removing events..", "Cancel", 100, 100, this);
        connect(m_progressDlg, SIGNAL(canceled()), &m_remReq, SLOT(cancel()));
        m_progressDlg->show();
    } else if (QOrganizerItemAbstractRequest::FinishedState == reqState ||
               QOrganizerItemAbstractRequest::CanceledState == reqState) {
        // Request finished or cancelled. Stop showing the progress dialog and refresh
        m_progressDlg->hide();
        refresh();
    }
}

void MonthPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Calendar Demo");
    QWidget::showEvent(event);
}
