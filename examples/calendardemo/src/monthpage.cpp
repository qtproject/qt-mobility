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

#include "monthpage.h"

#include "calendardemo.h"
#include <QtGui>
#include <qtorganizer.h>

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

    QComboBox *comboBox = new QComboBox(this);
    comboBox->addItems(QOrganizerItemManager::availableManagers());
    mainlayout->addRow("Backend:", comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(backendChanged(const QString &)));

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

#ifdef Q_WS_X11
    // Add push buttons for Maemo as it does not support soft keys
    QPushButton *addButton = new QPushButton("Add event", this);
    connect(addButton,SIGNAL(clicked()),this,SLOT(addNewEvent()));
    mainlayout->addRow("Options:", addButton);
#endif

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
    QAction* addEventAction = optionsMenu->addAction("Add event");
    connect(addEventAction, SIGNAL(triggered(bool)), this, SLOT(addNewEvent()));

    backendChanged(comboBox->currentText());
}

MonthPage::~MonthPage()
{

}

void MonthPage::backendChanged(const QString &managerName)
{
    // Nothing to change
    if (m_manager && m_manager->managerName() == managerName)
        return;

    // Out with the old
    delete m_manager;
    m_manager = 0;

    // In with the new
    QMap<QString, QString> parameters;
    m_manager = new QOrganizerItemManager(managerName, parameters, this);
    if (m_manager->error()) {
        QMessageBox::information(this, tr("Failed!"), QString("Failed to create manager!\n(error code %1)").arg(m_manager->error()));
        delete m_manager;
    }

    refresh();
}

void MonthPage::addNewEvent()
{
    QOrganizerEvent newEvent;
    QDateTime time(m_calendarWidget->selectedDate());
    newEvent.setStartDateTime(time);
    time = time.addSecs(60*30); // add 30 minutes to endtime
    newEvent.setEndDateTime(time);
    emit showEditPage(m_manager, newEvent);
}

void MonthPage::refresh()
{
    // fetch all current months items
    QDate start(m_calendarWidget->yearShown(), m_calendarWidget->monthShown(), 1);
    QDate end = start.addDays(start.daysInMonth());

    // Remove all previous formatting
    for (int i=0; i<start.daysInMonth(); i++) {
        QTextCharFormat cf = m_calendarWidget->dateTextFormat(start.addDays(i));
        cf.setFontItalic(false);
        cf.setFontUnderline(false);
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
    foreach (const QOrganizerItem &item, items)
    {
        QOrganizerEventTimeRange timeRange = item.detail<QOrganizerEventTimeRange>();
        if (!timeRange.isEmpty()) {
            dates << (timeRange.startDateTime().date());
            dates << (timeRange.endDateTime().date());
        }
        // TODO: other item types
    }

    // Mark all dates
    foreach (QDate date, dates) {
        QTextCharFormat cf = m_calendarWidget->dateTextFormat(date);
        cf.setFontItalic(true); // TODO: find a better way to mark dates
        m_calendarWidget->setDateTextFormat(date, cf);
    }

    refreshDayItems();
}

void MonthPage::refreshDayItems()
{
    // Update date
    QDate selectedDate = m_calendarWidget->selectedDate();
    m_dateLabel->setText(selectedDate.toString());

    // Clear items shown
    m_itemList->clear();

    // Find all items for today
    // TODO: refactor this when we have itemInstances() working properly
    QList<QOrganizerItem> items = m_manager->items();
    foreach (const QOrganizerItem &item, items)
    {
        QOrganizerEventTimeRange timeRange = item.detail<QOrganizerEventTimeRange>();
        if (!timeRange.isEmpty()) {
            if (timeRange.startDateTime().date() == selectedDate) {
                QString time = timeRange.startDateTime().time().toString("hh:mm");
                QListWidgetItem* listItem = new QListWidgetItem();
                listItem->setText(QString("%1-%2").arg(time).arg(item.displayLabel()));
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
    emit showDayPage(m_manager, date);
}

void MonthPage::openDay()
{
    emit showDayPage(m_manager, m_calendarWidget->selectedDate());
}

void MonthPage::itemDoubleClicked(QListWidgetItem *listItem)
{
    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(m_manager, organizerItem);
}

void MonthPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Calendar Demo");
    QWidget::showEvent(event);
}

