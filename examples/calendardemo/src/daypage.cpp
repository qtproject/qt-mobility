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
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *dateLayout = new QHBoxLayout();
    m_dateLabel = new QLabel(this);
    m_dateLabel->setAlignment(Qt::AlignCenter);
    dateLayout->addWidget(m_dateLabel);
    dateLayout->addStretch();
#ifndef Q_OS_SYMBIAN
    QPushButton* backButton = new QPushButton("View Month",this);
    connect(backButton,SIGNAL(clicked()),this,SLOT(viewMonthClicked()));
    dateLayout->addWidget(backButton);
#endif
    mainLayout->addLayout(dateLayout);

    m_itemList = new QListWidget(this);
    mainLayout->addWidget(m_itemList);
    connect(m_itemList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(itemDoubleClicked(QListWidgetItem *)));

    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton* editEventButton = new QPushButton("Edit",this);
    connect(editEventButton,SIGNAL(clicked()),this,SLOT(editItem()));
    hbLayout->addWidget(editEventButton);
    QPushButton* removeEventButton = new QPushButton("Remove",this);
    connect(removeEventButton,SIGNAL(clicked()),this,SLOT(removeItem()));
    hbLayout->addWidget(removeEventButton);
    mainLayout->addLayout(hbLayout);

#ifdef Q_OS_SYMBIAN
    // Add softkey for symbian
    QAction* backSoftKey = new QAction("View Month", this);
    backSoftKey->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(backSoftKey);
    connect(backSoftKey, SIGNAL(triggered(bool)), this, SLOT(viewMonthClicked()));
#endif

    setLayout(mainLayout);
}

DayPage::~DayPage()
{

}

#ifdef Q_OS_SYMBIAN
void DayPage::setMenu(QMenu *menu)
{
    // Add softkey for symbian
    QAction* optionsSoftKey = new QAction("Options", this);
    optionsSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    optionsSoftKey->setMenu(menu);
    addAction(optionsSoftKey);
}
#endif

void DayPage::refresh()
{

    m_dateLabel->setText(m_day.toString());
    m_itemList->clear();

    // Today's item
    QList<QOrganizerItem> items = m_manager->items(QDateTime(m_day), QDateTime(m_day, QTime(23, 59, 59)));

    foreach (const QOrganizerItem &item, items)
    {
        QOrganizerEventTime eventTime = item.detail<QOrganizerEventTime>();
        if (!eventTime.isEmpty()) {
            QString time = eventTime.startDateTime().time().toString("hh:mm");
            QListWidgetItem* listItem = new QListWidgetItem();
            if (item.type() == QOrganizerItemType::TypeEventOccurrence)
                listItem->setText(QString("Event occurance:%1-%2").arg(time).arg(item.displayLabel()));
            else
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
    }

    if (m_itemList->count() == 0)
        m_itemList->addItem("(no entries)");
}

void DayPage::changeManager(QOrganizerManager *manager)
{
    m_manager = manager;
}

void DayPage::dayChanged(QDate date)
{
    m_day = date;
}

void DayPage::itemDoubleClicked(QListWidgetItem *listItem)
{
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(organizerItem);
}

void DayPage::viewMonthClicked()
{
    emit showMonthPage();
}

void DayPage::editItem()
{
    QListWidgetItem *listItem = m_itemList->currentItem();
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (!organizerItem.isEmpty())
        emit showEditPage(organizerItem);
}

void DayPage::removeItem()
{
    QListWidgetItem *listItem = m_itemList->currentItem();
    if (!listItem)
        return;

    QOrganizerItem organizerItem = listItem->data(ORGANIZER_ITEM_ROLE).value<QOrganizerItem>();
    if (organizerItem.isEmpty())
        return;

    if (organizerItem.type() == QOrganizerItemType::TypeEventOccurrence
        || organizerItem.type() == QOrganizerItemType::TypeTodoOccurrence) {
        // Here we could ask if the user wishes to remove only the occurrence (meaning we would
        // add an exception date to the parent item), or the parent item. The current
        // implementation is to remove the parent (including all the occurrences).
        m_manager->removeItem(organizerItem.detail<QOrganizerItemParent>().parentId());
    } else {
        m_manager->removeItem(organizerItem.id());
    }

    if (m_manager->error())
        QMessageBox::information(this, "Failed!", QString("Failed to remove item!\n(error code %1)").arg(m_manager->error()));
    else
        delete m_itemList->takeItem(m_itemList->currentRow());

    if (m_itemList->count() == 0)
        m_itemList->addItem("(no entries)");
}

void DayPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle(m_day.toString("dddd"));
    QWidget::showEvent(event);
}

