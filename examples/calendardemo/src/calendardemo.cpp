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

#include "calendardemo.h"

#include "monthpage.h"
#include "daypage.h"
#include "eventeditpage.h"
#include <QtGui>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

CalendarDemo::CalendarDemo(QWidget *parent)
    :QMainWindow(parent),
    m_manager(0),
    m_stackedWidget(0),
    m_monthPage(0),
    m_dayPage(0),
    m_eventEditPage(0)
{
    m_stackedWidget = new QStackedWidget(this);

    m_monthPage = new MonthPage(m_stackedWidget);
    m_dayPage = new DayPage(m_stackedWidget);
    m_eventEditPage = new EventEditPage(m_stackedWidget);

    //qRegisterMetaType<QOrganizerItemManager>("QOrganizerItemManager");
    qRegisterMetaType<QOrganizerItem>("QOrganizerItem");

    connect(m_monthPage, SIGNAL(showDayPage(QOrganizerItemManager *, QDate)), this, SLOT(activateNewDayPage(QOrganizerItemManager *, QDate)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(showEditPage(QOrganizerItemManager *, const QOrganizerItem &)), this, SLOT(activateEditPage(QOrganizerItemManager *, const QOrganizerItem &)), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(showMonthPage()), this, SLOT(activateMonthPage()), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(showEditPage(QOrganizerItemManager *, const QOrganizerItem &)), this, SLOT(activateEditPage(QOrganizerItemManager *, const QOrganizerItem &)), Qt::QueuedConnection);
    connect(m_eventEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);

    m_stackedWidget->addWidget(m_monthPage);
    m_stackedWidget->addWidget(m_dayPage);
    m_stackedWidget->addWidget(m_eventEditPage);
    m_stackedWidget->setCurrentIndex(0);

    setCentralWidget(m_stackedWidget);
}

CalendarDemo::~CalendarDemo()
{

}

void CalendarDemo::activateMonthPage()
{
    m_monthPage->refresh();
    m_stackedWidget->setCurrentWidget(m_monthPage);
}

void CalendarDemo::activateDayPage()
{
    m_dayPage->refresh();
    m_stackedWidget->setCurrentWidget(m_dayPage);
}

void CalendarDemo::activateNewDayPage(QOrganizerItemManager *manager, QDate date)
{
    m_dayPage->dayChanged(manager, date);
    m_dayPage->refresh();
    m_stackedWidget->setCurrentWidget(m_dayPage);
}

void CalendarDemo::activateEditPage(QOrganizerItemManager *manager, const QOrganizerItem &item)
{
    if (item.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent event = static_cast<QOrganizerEvent>(item);
        m_dayPage->dayChanged(manager, event.startDateTime().date()); // edit always comes back to day page
        m_eventEditPage->eventChanged(manager, event);
        m_stackedWidget->setCurrentWidget(m_eventEditPage);
    }
    // TODO:
    //else if (item.type() == QOrganizerItemType::TypeJournal)
    //else if (item.type() == QOrganizerItemType::TypeNote)
    //else if (item.type() == QOrganizerItemType::TypeTodo)
}
