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

#include "daypage.h"

#include "calendardemo.h"
#include <QtGui>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QOrganizerItem)

DayPage::DayPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    m_dateLabel(0),
    m_itemList(0),
    m_menuBar(0)
{
    QVBoxLayout *mainlayout = new QVBoxLayout(this);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    mainlayout->addWidget(m_dateLabel);

    m_itemList = new QListWidget(this);
    mainlayout->addWidget(m_itemList);
    connect(m_itemList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(itemDoubleClicked(QListWidgetItem *)));

#ifdef Q_WS_X11
    // Add push buttons for Maemo as it does not support soft keys
    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton* editEventButton = new QPushButton("Edit",this);
    connect(editEventButton,SIGNAL(clicked()),this,SLOT(editItem()));
    hbLayout->addWidget(editEventButton);
    QPushButton* removeEventButton = new QPushButton("Remove",this);
    connect(removeEventButton,SIGNAL(clicked()),this,SLOT(removeItem()));
    hbLayout->addWidget(removeEventButton);
    QPushButton* addEventButton = new QPushButton("Add Event",this);
    connect(addEventButton,SIGNAL(clicked()),this,SLOT(addNewEvent()));
    hbLayout->addWidget(addEventButton);
    QPushButton* backButton = new QPushButton("Back",this);
    connect(backButton,SIGNAL(clicked()),this,SLOT(backClicked()));
    hbLayout->addWidget(backButton);
    mainlayout->addLayout(hbLayout);
#endif

    setLayout(mainlayout);

    // Add softkeys
    QAction* backSoftKey = new QAction("Back", this);
    backSoftKey->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(backSoftKey);
    connect(backSoftKey, SIGNAL(triggered(bool)), this, SLOT(backClicked()));

    QAction* optionsSoftKey = new QAction("Options", this);
    optionsSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    optionsSoftKey->setMenu(new QMenu(this));
    addAction(optionsSoftKey);

    // Add actions to options menu
    QMenu *optionsMenu = optionsSoftKey->menu();
    QAction* editAction = optionsMenu->addAction("Edit");
    connect(editAction, SIGNAL(triggered(bool)), this, SLOT(editItem()));
    QAction* removeAction = optionsMenu->addAction("Remove");
    connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeItem()));
    QAction* addEventAction = optionsMenu->addAction("Add Event");
    connect(addEventAction, SIGNAL(triggered(bool)), this, SLOT(addNewEvent()));
    QAction* addTodoAction = optionsMenu->addAction("Add Todo");
    connect(addTodoAction, SIGNAL(triggered(bool)), this, SLOT(addNewTodo()));
}

DayPage::~DayPage()
{

}

void DayPage::refresh()
{

    m_dateLabel->setText(m_day.toString());
    m_itemList->clear();

    // TODO: refactor this when we have itemInstances() working properly
    QList<QOrganizerItem> items = m_manager->items();
    foreach (const QOrganizerItem &item, items)
    {
        QOrganizerEventTimeRange eventTimeRange = item.detail<QOrganizerEventTimeRange>();
        if (!eventTimeRange.isEmpty()) {
            if (eventTimeRange.startDateTime().date() == m_day) {
                QString time = eventTimeRange.startDateTime().time().toString("hh:mm");
                QListWidgetItem* listItem = new QListWidgetItem();
                listItem->setText(QString("Event:%1-%2").arg(time).arg(item.displayLabel()));
                QVariant data = QVariant::fromValue<QOrganizerItem>(item);
                listItem->setData(ORGANIZER_ITEM_ROLE, data);
                m_itemList->addItem(listItem);
            }
        }
        
        QOrganizerTodoTimeRange todoTimeRange = item.detail<QOrganizerTodoTimeRange>();
        if (!todoTimeRange.isEmpty()) {
            if (todoTimeRange.startDateTime().date() == m_day) {
                QString time = todoTimeRange.startDateTime().time().toString("hh:mm");
                QListWidgetItem* listItem = new QListWidgetItem();
                listItem->setText(QString("Todo:%1-%2").arg(time).arg(item.displayLabel()));
                QVariant data = QVariant::fromValue<QOrganizerItem>(item);
                listItem->setData(ORGANIZER_ITEM_ROLE, data);
                m_itemList->addItem(listItem);
            }
        }        
        
        // TODO: other item types
    }

    if (m_itemList->count() == 0)
        m_itemList->addItem("(no entries)");
}

void DayPage::dayChanged(QOrganizerItemManager *manager, QDate date)
{
    m_manager = manager;
    m_day = date;
}

void DayPage::itemDoubleClicked(QListWidgetItem *listItem)
{
    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(m_manager, organizerItem);
}

void DayPage::backClicked()
{
    emit showMonthPage();
}

void DayPage::editItem()
{
    QListWidgetItem *listItem = m_itemList->currentItem();
    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(m_manager, organizerItem);
}

void DayPage::removeItem()
{
    QListWidgetItem *listItem = m_itemList->currentItem();
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (organizerItem.isEmpty())
        return;

    m_manager->removeItem(organizerItem.localId());
    if (m_manager->error())
        QMessageBox::information(this, "Failed!", QString("Failed to remove item!\n(error code %1)").arg(m_manager->error()));
    else
        delete m_itemList->takeItem(m_itemList->currentRow());

    if (m_itemList->count() == 0)
        m_itemList->addItem("(no entries)");
}

void DayPage::addNewEvent()
{
    // TODO: move this to CalendarDemo::addNewEvent() slot
    QOrganizerEvent newEvent;
    QDateTime time(m_day);
    newEvent.setStartDateTime(time);
    time = time.addSecs(60*30); // add 30 minutes to end time
    newEvent.setEndDateTime(time);
    emit showEditPage(m_manager, newEvent);
}

void DayPage::addNewTodo()
{
    // TODO: move this to CalendarDemo::addNewTodo() slot
    QOrganizerTodo newTodo;
    QDateTime time(m_day);
    newTodo.setStartDateTime(time);
    time = time.addSecs(60*30); // add 30 minutes to due time
    newTodo.setDueDateTime(time);
    emit showEditPage(m_manager, newTodo);
}

void DayPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle(m_day.toString("dddd"));
    QWidget::showEvent(event);
}

