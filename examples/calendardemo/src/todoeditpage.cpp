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
#include "todoeditpage.h"

#include <QtGui>
#include <QComboBox>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

TodoEditPage::TodoEditPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    m_subjectEdit(0),
    m_startTimeEdit(0),
    m_dueTimeEdit(0),
    m_priorityEdit(0),
    m_statusEdit(0)
{
    // Create widgets
    QLabel *subjectLabel = new QLabel("Subject:", this);
    m_subjectEdit = new QLineEdit(this);
    QLabel *startTimeLabel = new QLabel("Start time:", this);
    m_startTimeEdit = new QDateTimeEdit(this);
    QLabel *dueTimeLabel = new QLabel("Due time:", this);
    m_dueTimeEdit = new QDateTimeEdit(this);    
    QLabel *priorityLabel = new QLabel("Priority:", this);
    m_priorityEdit = new QComboBox(this);
    QLabel *statusLabel = new QLabel("Status:", this);
    m_statusEdit = new QComboBox(this);

#ifdef Q_WS_X11
    // Add push buttons for Maemo as it does not support soft keys
    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("Ok", this);
    connect(okButton,SIGNAL(clicked()),this,SLOT(saveClicked()));
    hbLayout->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    hbLayout->addWidget(cancelButton);
#endif

    QVBoxLayout *scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->addWidget(subjectLabel);
    scrollAreaLayout->addWidget(m_subjectEdit);
    scrollAreaLayout->addWidget(startTimeLabel);
    scrollAreaLayout->addWidget(m_startTimeEdit);
    scrollAreaLayout->addWidget(dueTimeLabel);
    scrollAreaLayout->addWidget(m_dueTimeEdit);
    scrollAreaLayout->addWidget(priorityLabel);
    scrollAreaLayout->addWidget(m_priorityEdit);
    scrollAreaLayout->addWidget(statusLabel);
    scrollAreaLayout->addWidget(m_statusEdit);
#ifdef Q_WS_X11
    scrollAreaLayout->addLayout(hbLayout);
#endif

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *formContainer = new QWidget(scrollArea);
    formContainer->setLayout(scrollAreaLayout);
    scrollArea->setWidget(formContainer);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    // Add softkeys
    QAction* cancelSoftKey = new QAction("Cancel", this);
    cancelSoftKey->setSoftKeyRole(QAction::NegativeSoftKey);
    addAction(cancelSoftKey);
    connect(cancelSoftKey, SIGNAL(triggered(bool)), this, SLOT(cancelClicked()));
    
    QAction* saveSoftKey = new QAction("Save", this);
    saveSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(saveSoftKey);
    connect(saveSoftKey, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
    
    // Fill priority combo
    m_priorityEdit->addItem("Unknown", QVariant(QOrganizerItemPriority::UnknownPriority));
    m_priorityEdit->addItem("Highest", QVariant(QOrganizerItemPriority::HighestPriority));
    m_priorityEdit->addItem("Extremely high", QVariant(QOrganizerItemPriority::ExtremelyHighPriority));
    m_priorityEdit->addItem("Very high", QVariant(QOrganizerItemPriority::VeryHighPriority));
    m_priorityEdit->addItem("High", QVariant(QOrganizerItemPriority::HighPriority));
    m_priorityEdit->addItem("Medium", QVariant(QOrganizerItemPriority::MediumPriority));
    m_priorityEdit->addItem("Low", QVariant(QOrganizerItemPriority::LowPriority));
    m_priorityEdit->addItem("Very low", QVariant(QOrganizerItemPriority::VeryLowPriority));
    m_priorityEdit->addItem("Extremely low", QVariant(QOrganizerItemPriority::ExtremelyLowPriority));
    m_priorityEdit->addItem("Lowest", QVariant(QOrganizerItemPriority::LowestPriority));

    // Fill status combo
    m_statusEdit->addItem("Not started", QVariant(QOrganizerTodoProgress::StatusNotStarted));
    m_statusEdit->addItem("In progress", QVariant(QOrganizerTodoProgress::StatusInProgress));
    m_statusEdit->addItem("Complete", QVariant(QOrganizerTodoProgress::StatusComplete));
}

TodoEditPage::~TodoEditPage()
{

}

void TodoEditPage::todoChanged(QOrganizerItemManager *manager, const QOrganizerTodo &todo)
{
    m_manager = manager;
    m_organizerTodo = todo;
    m_subjectEdit->setText(todo.displayLabel());
    m_startTimeEdit->setDateTime(todo.startDateTime());
    m_dueTimeEdit->setDateTime(todo.dueDateTime());
    int index = m_priorityEdit->findData(QVariant(todo.priority()));
    m_priorityEdit->setCurrentIndex(index);
    index = m_priorityEdit->findData(QVariant(todo.status()));
    m_statusEdit->setCurrentIndex(index);
}


void TodoEditPage::cancelClicked()
{
    emit showDayPage();
}

void TodoEditPage::saveClicked()
{
    // Read data from page
    m_organizerTodo.setDisplayLabel(m_subjectEdit->text());
    m_organizerTodo.setStartDateTime(m_startTimeEdit->dateTime());
    m_organizerTodo.setDueDateTime(m_dueTimeEdit->dateTime());
    int index = m_priorityEdit->currentIndex();
    m_organizerTodo.setPriority((QOrganizerItemPriority::Priority) m_priorityEdit->itemData(index).toInt());
    
    index = m_statusEdit->currentIndex();
    QOrganizerTodoProgress::Status currentStatus = (QOrganizerTodoProgress::Status) m_statusEdit->itemData(index).toInt();
    QOrganizerTodoProgress oldStatus = m_organizerTodo.detail<QOrganizerTodoProgress>();
    m_organizerTodo.removeDetail(&oldStatus);
    if (currentStatus == QOrganizerTodoProgress::StatusComplete && oldStatus.status() != QOrganizerTodoProgress::StatusComplete)
        m_organizerTodo.setFinishedDateTime(QDateTime::currentDateTime());
    m_organizerTodo.setStatus(currentStatus);
    
    // Save
    m_manager->saveItem(&m_organizerTodo);
    if (m_manager->error())
        QMessageBox::information(this, "Failed!", QString("Failed to save todo!\n(error code %1)").arg(m_manager->error()));
    else
        emit showDayPage();
}

void TodoEditPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Edit todo");
    QWidget::showEvent(event);
}
