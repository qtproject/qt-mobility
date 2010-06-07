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
#include "eventeditpage.h"

#include <QtGui>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

EventEditPage::EventEditPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    m_typeComboBox(0),
    m_subjectEdit(0)
{
    // Create widgets
    QLabel *subjectLabel = new QLabel("Subject:", this);
    m_subjectEdit = new QLineEdit(this);
    QLabel *startTimeLabel = new QLabel("Start time:", this);
    m_startTimeEdit = new QDateTimeEdit(this);
    QLabel *endTimeLabel = new QLabel("End time:", this);
    m_endTimeEdit = new QDateTimeEdit(this);

    QVBoxLayout *scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->addWidget(subjectLabel);
    scrollAreaLayout->addWidget(m_subjectEdit);
    scrollAreaLayout->addWidget(startTimeLabel);
    scrollAreaLayout->addWidget(m_startTimeEdit);
    scrollAreaLayout->addWidget(endTimeLabel);
    scrollAreaLayout->addWidget(m_endTimeEdit);

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
}

EventEditPage::~EventEditPage()
{

}

void EventEditPage::eventChanged(QOrganizerItemManager *manager, const QOrganizerEvent &event)
{
    m_manager = manager;
    m_organizerEvent = event;
    m_subjectEdit->setText(event.displayLabel());
    m_startTimeEdit->setDateTime(event.startDateTime());
    m_endTimeEdit->setDateTime(event.endDateTime());
}


void EventEditPage::cancelClicked()
{
    emit showDayPage();
}

void EventEditPage::saveClicked()
{
    m_organizerEvent.setDisplayLabel(m_subjectEdit->text());
    m_organizerEvent.setStartDateTime(m_startTimeEdit->dateTime());
    m_organizerEvent.setEndDateTime(m_endTimeEdit->dateTime());

    m_manager->saveItem(&m_organizerEvent);
    if (m_manager->error())
        QMessageBox::information(this, "Failed!", QString("Failed to save event!\n(error code %1)").arg(m_manager->error()));
    else
        emit showDayPage();
}

void EventEditPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Edit event");
    QWidget::showEvent(event);
}
