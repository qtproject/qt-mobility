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
#include "todoeditpage.h"
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
    m_dayPage = new DayPage(this, m_stackedWidget);
    m_eventEditPage = new EventEditPage(m_stackedWidget);
    m_todoEditPage = new TodoEditPage(m_stackedWidget);

    //qRegisterMetaType<QOrganizerItemManager>("QOrganizerItemManager");
    qRegisterMetaType<QOrganizerItem>("QOrganizerItem");

    connect(m_monthPage, SIGNAL(showDayPage(QDate)), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(showEditPage(const QOrganizerItem &)), this, SLOT(activateEditPage(const QOrganizerItem &)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(addNewEvent()), this, SLOT(addNewEvent()), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(addNewTodo()), this, SLOT(addNewTodo()), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(managerChanged(QOrganizerItemManager*)), this, SLOT(changeManager(QOrganizerItemManager*)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(managerChanged(QOrganizerItemManager*)), m_dayPage, SLOT(changeManager(QOrganizerItemManager*)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(currentDayChanged(QDate)), this, SLOT(updateSelectedDay(QDate)));
    connect(m_dayPage, SIGNAL(showMonthPage()), this, SLOT(activateMonthPage()), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(showEditPage(const QOrganizerItem &)), this, SLOT(activateEditPage(const QOrganizerItem &)), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(addNewEvent()), this, SLOT(addNewEvent()), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(addNewTodo()), this, SLOT(addNewTodo()), Qt::QueuedConnection);
    connect(m_eventEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_todoEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);

    m_monthPage->init();
    
    m_stackedWidget->addWidget(m_monthPage);
    m_stackedWidget->addWidget(m_dayPage);
    m_stackedWidget->addWidget(m_eventEditPage);
    m_stackedWidget->addWidget(m_todoEditPage);
    m_stackedWidget->setCurrentIndex(0);

    setCentralWidget(m_stackedWidget);
#ifdef Q_WS_MAEMO_5
    setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
#endif
    buildMenu();
}



CalendarDemo::~CalendarDemo()
{

}

void CalendarDemo::buildMenu()
{
    // Build Options menu
#if defined(Q_WS_MAEMO_5) || defined(Q_OS_WINCE)
    // These platforms need their menu items added directly to the menu bar.
    QMenuBar *optionsMenu = menuBar();
#else
    QMenu *optionsMenu = new QMenu("Options", this);
    #ifndef Q_OS_SYMBIAN
    // We add the options menu to the softkey manually later
    menuBar()->addMenu(optionsMenu);
    #endif
#endif
#ifdef Q_OS_SYMBIAN
    // Add editing options in the menu for Symbian (other platforms get buttons)
    QAction* editAction = optionsMenu->addAction("Edit");
    connect(editAction, SIGNAL(triggered(bool)), this, SLOT(editItem()));
    QAction* removeAction = optionsMenu->addAction("Remove");
    connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeItem()));
#endif
    QAction* addEventAction = optionsMenu->addAction("Add Event");
    connect(addEventAction, SIGNAL(triggered(bool)), this, SLOT(addNewEvent()));
    QAction* addTodoAction = optionsMenu->addAction("Add Todo");
    connect(addTodoAction, SIGNAL(triggered(bool)), this, SLOT(addNewTodo()));

#ifdef Q_OS_SYMBIAN
    // Add softkeys for symbian
    QAction* backSoftKey = new QAction("View Month", this);
    backSoftKey->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(backSoftKey);
    connect(backSoftKey, SIGNAL(triggered(bool)), this, SLOT(viewMonthClicked()));

    QAction* optionsSoftKey = new QAction("Options", this);
    optionsSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    optionsSoftKey->setMenu(optionsMenu);
    addAction(optionsSoftKey);
#endif
}

void CalendarDemo::activateMonthPage()
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(true);
#endif
    m_monthPage->refresh();
    m_stackedWidget->setCurrentWidget(m_monthPage);
}

void CalendarDemo::activateDayPage()
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(true);
#endif
    m_dayPage->refresh();
    m_stackedWidget->setCurrentWidget(m_dayPage);
}

void CalendarDemo::activateEditPage(const QOrganizerItem &item)
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5))
    menuBar()->setVisible(false);
#endif
    if (item.type() == QOrganizerItemType::TypeEvent) {
        QOrganizerEvent event = static_cast<QOrganizerEvent>(item);
        m_dayPage->dayChanged(event.startDateTime().date()); // edit always comes back to day page
        m_eventEditPage->eventChanged(m_manager, event);
        m_stackedWidget->setCurrentWidget(m_eventEditPage);
    }
    else if (item.type() == QOrganizerItemType::TypeTodo) {
        QOrganizerTodo todo = static_cast<QOrganizerTodo>(item);
        m_dayPage->dayChanged(todo.startDateTime().date()); // edit always comes back to day page
        m_todoEditPage->todoChanged(m_manager, todo);
        m_stackedWidget->setCurrentWidget(m_todoEditPage);
    }
    // TODO:
    //else if (item.type() == QOrganizerItemType::TypeJournal)
    //else if (item.type() == QOrganizerItemType::TypeNote)
}

void CalendarDemo::addNewEvent()
{
    QOrganizerEvent newEvent;
    QDateTime time(m_currentDate);
    newEvent.setStartDateTime(time);
    time = time.addSecs(60*30); // add 30 minutes to end time
    newEvent.setEndDateTime(time);
    activateEditPage(newEvent);
}

void CalendarDemo::addNewTodo()
{
    QOrganizerTodo newTodo;
    QDateTime time(m_currentDate);
    newTodo.setStartDateTime(time);
    time = time.addSecs(60*30); // add 30 minutes to due time
    newTodo.setDueDateTime(time);
    activateEditPage(newTodo);
}

void CalendarDemo::changeManager(QOrganizerItemManager *manager)
{
    m_manager = manager;
}

void CalendarDemo::updateSelectedDay(const QDate& date)
{
    m_dayPage->dayChanged(date);
    m_currentDate = date;
}
