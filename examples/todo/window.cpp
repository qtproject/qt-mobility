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

#include "window.h"
#include "todoeditor.h"

//! [0]
Window::Window()
{
    setupGui();

    manager = new QOrganizerManager("memory");

    setWindowTitle(tr("ToDo Example"));
    refreshList();
}
//! [0]

Window::~Window()
{
    delete manager;
}

//! [1]
void Window::editNewTodo()
{
    QOrganizerTodo newTodo;
    newTodo.setPriority(QOrganizerItemPriority::HighPriority);
    newTodo.setStatus(QOrganizerTodoProgress::StatusNotStarted);
    QDateTime currentDateTime(calendarWidget->selectedDate(),
        QTime::currentTime());
    newTodo.setStartDateTime(currentDateTime);
    newTodo.setDueDateTime(currentDateTime.addSecs(60*60));

    todoEditor->editTodo(newTodo);

    stackedWidget->setCurrentWidget(todoEditor);
}
//! [1]

//! [2]
Q_DECLARE_METATYPE(QOrganizerTodo)

void Window::editTodo(QListWidgetItem *item)
{
    QVariant variant = item->data(Qt::UserRole);
    if (!variant.canConvert<QOrganizerTodo>())
        return;

    QOrganizerTodo todo = variant.value<QOrganizerTodo>();
    todoEditor->editTodo(todo);
    stackedWidget->setCurrentWidget(todoEditor);
}
//! [2]

//! [3]
void Window::saveTodo(QOrganizerTodo &todo)
{
    manager->saveItem(&todo);

    stackedWidget->setCurrentIndex(0);
    refreshList();
}
//! [3]

//! [4]
void Window::deleteTodo()
{
    QList<QListWidgetItem *> items = listWidget->selectedItems();
    if (!items.isEmpty()) {
        QVariant variant = items.at(0)->data(Qt::UserRole);
        if (variant.canConvert<QOrganizerTodo>()) {
            QOrganizerTodo theTodo = variant.value<QOrganizerTodo>();
            manager->removeItem(theTodo.id());
            refreshList();
        }
    }
}
//! [4] 

//! [5]
void Window::refreshList()
{
    listWidget->clear();

    QOrganizerItemSortOrder sortOrder;
    sortOrder.setDetailDefinitionName(QOrganizerTodoTime::DefinitionName,
        QOrganizerTodoTime::FieldDueDateTime);

    QList<QOrganizerItem> items =
        manager->items(QOrganizerItemFilter(), QList<QOrganizerItemSortOrder>() << sortOrder);
//! [5]
    if (items.isEmpty()) {
       new QListWidgetItem("<No Todos>", listWidget);
    }
//! [6]
    foreach(QOrganizerItem item, items) {
        if (item.type() == QOrganizerItemType::TypeTodo) {
            QOrganizerTodo todo = static_cast<QOrganizerTodo>(item);
            if (todo.startDateTime() >
                    QDateTime(calendarWidget->selectedDate(), QTime(23,59)) ||
                todo.dueDateTime() <
                    QDateTime(calendarWidget->selectedDate(), QTime(0, 0)))
                continue;            

            QString display = todo.startDateTime().toString("yy/MM/dd hh:mm") +
                "-" + todo.dueDateTime().toString("yy/MM/dd hh:mm") +
                " - "+ todo.displayLabel();

            QListWidgetItem *listItem =
                new QListWidgetItem(display, listWidget);
            listItem->setData(Qt::UserRole, QVariant::fromValue(todo));
        }
    }
}
//! [6]

void Window::setupGui()
{
    todoEditor = new TodoEditor;

    listWidget = new QListWidget;   
    stackedWidget = new QStackedWidget;
    newTodoButton = new QPushButton(tr("New Todo"));
    deletTodoButton = new QPushButton(tr("Delete Todo"));
    calendarWidget = new QCalendarWidget;

    connect(newTodoButton, SIGNAL(clicked()), this, SLOT(editNewTodo()));
    connect(todoEditor, SIGNAL(editingFinished(QOrganizerTodo &)),
            this, SLOT(saveTodo(QOrganizerTodo &)));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(editTodo(QListWidgetItem*)));
    connect(calendarWidget, SIGNAL(selectionChanged()),
            this, SLOT(refreshList()));
    connect(deletTodoButton, SIGNAL(clicked()), this, SLOT(deleteTodo()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(calendarWidget);
    mainLayout->addWidget(listWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(newTodoButton);
    buttonLayout->addWidget(deletTodoButton);
    mainLayout->addLayout(buttonLayout);

    QWidget *frontPage = new QWidget;
    frontPage->setLayout(mainLayout);

    stackedWidget->addWidget(frontPage);
    stackedWidget->addWidget(todoEditor);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(stackedWidget);

    setLayout(layout); 
}

