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
#include <qorganizerabstractrequest.h>

#include "monthpage.h"
#include "calendardemo.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QOrganizerItem)

MonthPage::MonthPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    m_calendarWidget(0),
    m_dateLabel(0),
    m_itemList(0),
    m_ignoreShowDayPageOnceFlag(false)
{
    // Create widgets
    QFormLayout *mainlayout = new QFormLayout(this);

    m_managerComboBox = new QComboBox(this);
    foreach (const QString& manager, QOrganizerManager::availableManagers()) {
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
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    m_itemList->setVisible(false);
#else
    mainlayout->addRow(m_itemList);
#endif
    connect(m_itemList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    setLayout(mainlayout);
}

// This is separate from the constructor so we can do it after connecting the signals
void MonthPage::init()
{
    backendChanged(m_managerComboBox->currentText());
    currentMonthChanged();
}

MonthPage::~MonthPage()
{

}

#ifdef Q_OS_SYMBIAN
void MonthPage::setMenu(QMenu *menu)
{
    // Add softkey for symbian
    QAction* optionsSoftKey = new QAction("Options", this);
    optionsSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    optionsSoftKey->setMenu(menu);
    addAction(optionsSoftKey);
}
#endif

void MonthPage::backendChanged(const QString &managerName)
{
    // Nothing to change
    if (m_manager && m_manager->managerName() == managerName)
        return;

    // Try creating a new manager
    QMap<QString, QString> parameters;
    QOrganizerManager* newManager = new QOrganizerManager(managerName, parameters, this);
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

void MonthPage::editItem()
{
    QListWidgetItem *listItem = m_itemList->currentItem();
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(organizerItem);
}

void MonthPage::removeItem()
{
    QListWidgetItem *listItem = m_itemList->currentItem();
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (organizerItem.isEmpty())
        return;

    m_manager->removeItem(organizerItem.id());
    refresh();
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
    QList<QOrganizerItem> items = m_manager->items(startDateTime, endDateTime);

    // Get dates for all items
    QList<QDate> dates;
    foreach (const QOrganizerItem &item, items)
    {
        QDate startDate;
        QDate endDate;

        QOrganizerEventTime eventTime = item.detail<QOrganizerEventTime>();
        if (!eventTime.isEmpty()) {
            startDate = eventTime.startDateTime().date();
            endDate = eventTime.endDateTime().date();
        } else {
            QOrganizerTodoTime todoTime = item.detail<QOrganizerTodoTime>();
            if (!todoTime.isEmpty()) {
                startDate = todoTime.startDateTime().date();
                endDate = todoTime.dueDateTime().date();
            } else {
                QOrganizerJournalTime journalTime = item.detail<QOrganizerJournalTime>();
                if (!journalTime.isEmpty())
                    startDate = endDate = journalTime.entryDateTime().date();
            }
        }

        if (!startDate.isNull()) {
            while (startDate <= endDate) {
                dates << startDate;
                startDate = startDate.addDays(1);
            }
        }
    }

    // Mark all dates which has events.
    QBrush brush;
    brush.setColor(Qt::green);
    foreach (QDate date, dates) {
        if (date == QDate())
            continue;
        QTextCharFormat cf = m_calendarWidget->dateTextFormat(date);
        cf.setFontItalic(true); // TODO: find a better way to mark dates
        cf.setBackground(brush);
        m_calendarWidget->setDateTextFormat(date, cf);
    }

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // As the day item list is not showed do not refresh
    // the day items in Maemo5 or Maemo6 to improve performance
    refreshDayItems();
#endif
}

void MonthPage::refreshDayItems()
{
    QDate selectedDate = m_calendarWidget->selectedDate();
    emit currentDayChanged(selectedDate);

    QDateTime startOfDay(selectedDate, QTime(0, 0, 0));
    QDateTime endOfDay(selectedDate, QTime(23, 59, 59));
    // Clear items shown
    m_itemList->clear();

    // Find all items for today
    QList<QOrganizerItem> items = m_manager->items(startOfDay, endOfDay);

    foreach (const QOrganizerItem &item, items)
    {
        QOrganizerEventTime eventTime = item.detail<QOrganizerEventTime>();
        if (!eventTime.isEmpty()) {
            QString time = eventTime.startDateTime().time().toString("hh:mm");
            QListWidgetItem* listItem = new QListWidgetItem();
            listItem->setText(QString("Event:%1-%2").arg(time).arg(item.displayLabel()));
            QVariant data = QVariant::fromValue<QOrganizerItem>(item);
            listItem->setData(ORGANIZER_ITEM_ROLE, data);
            m_itemList->addItem(listItem);
        }

        QOrganizerTodoTime todoTime = item.detail<QOrganizerTodoTime>();
        if (!todoTime.isEmpty()) {
            QString time = todoTime.startDateTime().time().toString("hh:mm");
            QListWidgetItem* listItem = new QListWidgetItem();
            listItem->setText(QString("Todo:%1-%2").arg(time).arg(item.displayLabel()));
            QVariant data = QVariant::fromValue<QOrganizerItem>(item);
            listItem->setData(ORGANIZER_ITEM_ROLE, data);
            m_itemList->addItem(listItem);
        }

        QOrganizerJournalTime journalTime = item.detail<QOrganizerJournalTime>();
        if (!journalTime.isEmpty()) {
            QString time = journalTime.entryDateTime().time().toString("hh:mm");
            QListWidgetItem* listItem = new QListWidgetItem();
            listItem->setText(QString("Journal:%1-%2").arg(time).arg(item.displayLabel()));
            QVariant data = QVariant::fromValue<QOrganizerItem>(item);
            listItem->setData(ORGANIZER_ITEM_ROLE, data);
            m_itemList->addItem(listItem);
        }

        // TODO: other item types
        // TODO: icons for event/todo/journal would be nice
    }
    if (m_itemList->count() == 0)
        m_itemList->addItem("(no entries)");
}


void MonthPage::currentMonthChanged()
{
    int month = m_calendarWidget->monthShown();
    int year = m_calendarWidget->yearShown();
    m_dateLabel->setText(QString("%1 %2").arg(QDate::longMonthName(month)).arg(year));
    refresh();
    m_ignoreShowDayPageOnceFlag = true;
}

void MonthPage::dayDoubleClicked(QDate date)
{
    if (!m_ignoreShowDayPageOnceFlag)
        emit showDayPage(date);
    else
        m_ignoreShowDayPageOnceFlag = false;
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

void MonthPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Calendar Demo");
    QWidget::showEvent(event);
}
