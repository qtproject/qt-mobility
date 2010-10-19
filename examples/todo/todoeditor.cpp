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

#include "todoeditor.h"

TodoEditor::TodoEditor()
{
    setupGui();
}

//! [1]
void TodoEditor::editTodo(const QOrganizerTodo &newTodo)
{
    todo = newTodo;

    subjectLineEdit->setText(todo.displayLabel());
    startDateEdit->setDateTime(todo.startDateTime());
    dueDateEdit->setDateTime(todo.dueDateTime());
    priorityCombo->setCurrentIndex(
        priorityCombo->findData(QVariant(todo.priority())));
    statusCombo->setCurrentIndex(
        statusCombo->findData(QVariant(todo.status())));
    descriptionTextEdit->setText(todo.description());
//! [1]

//! [2]
    if (!todo.details(QOrganizerItemVisualReminder::DefinitionName).isEmpty()){ 
        QOrganizerItemVisualReminder reminder =
            todo.detail<QOrganizerItemVisualReminder>();
        int seconds = reminder.secondsBeforeStart();
        alarmCombo->setCurrentIndex(seconds/(15*60));
    } else
        alarmCombo->setCurrentIndex(0);
}
//! [2]

//! [3]
void TodoEditor::updateSubject()
{
    todo.setDisplayLabel(subjectLineEdit->text());
}

void TodoEditor::updateDescription()
{
    todo.setDescription(descriptionTextEdit->toPlainText());
}
//! [3]

//! [4]
void TodoEditor::updateDates()
{
    QDateTime startTime = startDateEdit->dateTime();
    QDateTime dueDateTime = dueDateEdit->dateTime();

    todo.setStartDateTime(startTime);
    todo.setDueDateTime(dueDateTime);

    updateAlarm(alarmCombo->currentIndex());
}
//! [4]

//! [5]
void TodoEditor::updateStatus(int index)
{
    QOrganizerTodoProgress::Status status =
        (QOrganizerTodoProgress::Status) statusCombo->itemData(index).toInt();
    todo.setStatus(status);
}

void TodoEditor::updatePriority(int index)
{
    QOrganizerItemPriority::Priority priority =
        (QOrganizerItemPriority::Priority)
            priorityCombo->itemData(index).toInt();
    todo.setPriority(priority);
}
//! [5]

//! [6]
void TodoEditor::updateAlarm(int index)
{
    int seconds = index * (15*60);
    QDateTime dueDate = todo.dueDateTime();

    QOrganizerItemVisualReminder oldReminder =
        todo.detail(QOrganizerItemVisualReminder::DefinitionName);
    todo.removeDetail(&oldReminder);

    if (seconds == 0)
        return;

    QOrganizerItemVisualReminder reminder;
    reminder.setSecondsBeforeStart(seconds);
    
    todo.saveDetail(&reminder);
}
//! [6]

void TodoEditor::finishEditing()
{
    emit editingFinished(todo);
}

void TodoEditor::setupGui()
{
    startDateEdit = new QDateTimeEdit;
    dueDateEdit = new QDateTimeEdit;
    subjectLineEdit = new QLineEdit;
    descriptionTextEdit = new QTextEdit;
    doneButton = new QPushButton(tr("Done"));

    setupCombos();

    connect(startDateEdit, SIGNAL(editingFinished()),
        this, SLOT(updateDates()));
    connect(dueDateEdit, SIGNAL(editingFinished()), this, SLOT(updateDates()));
    connect(subjectLineEdit, SIGNAL(editingFinished()),
        this, SLOT(updateSubject()));
    connect(descriptionTextEdit, SIGNAL(textChanged()),
        this, SLOT(updateDescription()));
    connect(statusCombo, SIGNAL(activated(int)), this, SLOT(updateStatus(int)));
    connect(priorityCombo, SIGNAL(activated(int)),
        this, SLOT(updatePriority(int)));
    connect(alarmCombo, SIGNAL(activated(int)), this, SLOT(updateAlarm(int)));
    connect(doneButton, SIGNAL(clicked()), this, SLOT(finishEditing()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("Subject:")));
    layout->addWidget(subjectLineEdit);
    layout->addWidget(new QLabel(tr("Start Date:")));
    layout->addWidget(startDateEdit);
    layout->addWidget(new QLabel(tr("Due Date:")));
    layout->addWidget(dueDateEdit);
    layout->addWidget(new QLabel(tr("Status:")));
    layout->addWidget(statusCombo);
    layout->addWidget(new QLabel(tr("Priority:")));
    layout->addWidget(priorityCombo);
    layout->addWidget(new QLabel(tr("Alarm:")));
    layout->addWidget(alarmCombo);
    layout->addWidget(new QLabel(tr("Description")));
    layout->addWidget(descriptionTextEdit);
    layout->addWidget(doneButton);

    setLayout(layout);
}

//! [0]
void TodoEditor::setupCombos()
{
    priorityCombo = new QComboBox;
    priorityCombo->addItem("Unknown", QOrganizerItemPriority::UnknownPriority);
    priorityCombo->addItem("Highest", QOrganizerItemPriority::HighestPriority);
    priorityCombo->addItem("Extremely high",
        QOrganizerItemPriority::ExtremelyHighPriority);
    priorityCombo->addItem("Very high",
        QOrganizerItemPriority::VeryHighPriority);
    priorityCombo->addItem("High", QOrganizerItemPriority::HighPriority);
    priorityCombo->addItem("Medium", QOrganizerItemPriority::MediumPriority);
    priorityCombo->addItem("Low", QOrganizerItemPriority::LowPriority);
    priorityCombo->addItem("Very low", QOrganizerItemPriority::VeryLowPriority);
    priorityCombo->addItem("Extremely low",
        QOrganizerItemPriority::ExtremelyLowPriority);
    priorityCombo->addItem("Lowest", QOrganizerItemPriority::LowestPriority);

    statusCombo = new QComboBox;
    statusCombo->addItem("Not started",
        QOrganizerTodoProgress::StatusNotStarted);
    statusCombo->addItem("In progress", QOrganizerTodoProgress::StatusInProgress);
    statusCombo->addItem("Complete",
        QOrganizerTodoProgress::StatusComplete);

    alarmCombo = new QComboBox;
    QStringList alarmList;
    alarmList << "None" << "15 minutes" << "30 minutes" << "45 minutes"
              << "1 hour";
    alarmCombo->addItems(alarmList);
}
//! [0]

