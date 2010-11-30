/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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
#include "eventoccurrenceeditpage.h"

#include <QtGui>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

EventOccurrenceEditPage::EventOccurrenceEditPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    scrollAreaLayout(0),
    m_typeComboBox(0),
    m_subjectEdit(0),
    m_countSpinBox(0),
    m_repeatUntilDate(0)
{
    //create asynch request to save an item
    m_saveItemRequest = new QOrganizerItemSaveRequest(this);
    // Create widgets
    QLabel *subjectLabel = new QLabel("Subject:", this);
    m_subjectEdit = new QLineEdit(this);
    QLabel *startTimeLabel = new QLabel("Start time:", this);
    m_startTimeEdit = new QDateTimeEdit(this);
    m_startTimeEdit->setDisplayFormat(QString("yyyy-MM-dd hh:mm:ss AP"));
    QLabel *endTimeLabel = new QLabel("End time:", this);
    m_endTimeEdit = new QDateTimeEdit(this);
    m_endTimeEdit->setDisplayFormat(QString("yyyy-MM-dd hh:mm:ss AP"));

#ifndef Q_OS_SYMBIAN
    // Add push buttons for Maemo as it does not support soft keys
    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("Ok", this);
    connect(okButton,SIGNAL(clicked()),this,SLOT(saveOrNextClicked()));
    hbLayout->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    hbLayout->addWidget(cancelButton);
#endif

    scrollAreaLayout = new QVBoxLayout();
    scrollAreaLayout->addWidget(subjectLabel);
    scrollAreaLayout->addWidget(m_subjectEdit);
    scrollAreaLayout->addWidget(startTimeLabel);
    scrollAreaLayout->addWidget(m_startTimeEdit);
    scrollAreaLayout->addWidget(endTimeLabel);
    scrollAreaLayout->addWidget(m_endTimeEdit);
    scrollAreaLayout->addStretch();

#ifndef Q_OS_SYMBIAN
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

    m_saveOrNextSoftKey = new QAction("Save",this);
    m_saveOrNextSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(m_saveOrNextSoftKey);
    connect(m_saveOrNextSoftKey, SIGNAL(triggered(bool)), this, SLOT(saveOrNextClicked()));
    m_countFieldAdded = false;
    m_multipleEntries = false;
    m_listOfEvents.clear();
}

EventOccurrenceEditPage::~EventOccurrenceEditPage()
{

}

void EventOccurrenceEditPage::eventOccurrenceChanged(QOrganizerManager *manager, const QOrganizerEventOccurrence &eventOccurrence)
{
    m_manager = manager;
    m_organizerEventOccurrence = eventOccurrence;
    m_subjectEdit->setText(eventOccurrence.displayLabel());
    m_startTimeEdit->setDateTime(eventOccurrence.startDateTime());
    m_endTimeEdit->setDateTime(eventOccurrence.endDateTime());
}

void EventOccurrenceEditPage::cancelClicked()
{
    emit showDayPage();
}

void EventOccurrenceEditPage::saveOrNextClicked()
{
    QDateTime start(m_startTimeEdit->dateTime());
    QDateTime end(m_endTimeEdit->dateTime());
    if (start > end) {
        QMessageBox::warning(this, "Failed!", "Start date is not before end date");
        return;
    }

    m_organizerEventOccurrence.setDisplayLabel(m_subjectEdit->text());
    m_organizerEventOccurrence.setStartDateTime(start);
    m_organizerEventOccurrence.setEndDateTime(end);
    m_listOfEvents.append(m_organizerEventOccurrence);
    if(m_numOfEntiresToBeCreated > 1){
        m_numOfEntiresToBeCreated--;
        if(m_numOfEntiresToBeCreated == 1){
                // For the last entry change the option to Save.
                m_saveOrNextSoftKey->setText("SaveAll");
        }
        // Clear subject field indicating its a new editor.
        m_subjectEdit->clear();
    } else {
        if(!m_multipleEntries){
                // If single entry
                m_manager->saveItem(&m_organizerEventOccurrence);
        } else {
                // If multiple entries, save asynchronously.
                m_saveItemRequest->setItems(m_listOfEvents);
                m_saveItemRequest->setManager(m_manager);
                m_saveItemRequest->start();
        }

        if (m_manager->error())
                QMessageBox::warning(this, "Failed!", QString("Failed to save event occurrence!\n(error code %1)").arg(m_manager->error()));
        else
                emit showDayPage();
    }
}

void EventOccurrenceEditPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Edit event occurence");
    QWidget::showEvent(event);
}

