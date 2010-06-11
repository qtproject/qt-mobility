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
        QOrganizerEventTimeRange timeRange = item.detail<QOrganizerEventTimeRange>();
        if (!timeRange.isEmpty()) {
            if (timeRange.startDateTime().date() == m_day) {
                QString time = timeRange.startDateTime().time().toString("hh:mm");
                QListWidgetItem* listItem = new QListWidgetItem();
                listItem->setText(QString("%1-%2").arg(time).arg(item.displayLabel()));
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
    QOrganizerEvent newEvent;
    QDateTime time(m_day);
    newEvent.setStartDateTime(time);
    time = time.addSecs(60*30); // add 30 minutes to endtime
    newEvent.setEndDateTime(time);
    emit showEditPage(m_manager, newEvent);
}

void DayPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle(m_day.toString("dddd"));
    QWidget::showEvent(event);
}

