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
     m_scrollAreaLayout(0),
    m_manager(0),
    m_typeComboBox(0),
    m_subjectEdit(0)
{
    //create asynch request to save an item
	m_saveItemRequest = new QOrganizerItemSaveRequest(this);
    // Create widgets
    QLabel *subjectLabel = new QLabel("Subject:", this);
    m_subjectEdit = new QLineEdit(this);
    QLabel *startTimeLabel = new QLabel("Start time:", this);
    m_startTimeEdit = new QDateTimeEdit(this);
    QLabel *endTimeLabel = new QLabel("End time:", this);
    m_endTimeEdit = new QDateTimeEdit(this);
    QLabel *repeatLabel = new QLabel("Repeat:", this);
    m_typeComboBox = new QComboBox(this);
    QStringList repeatList;
    repeatList	<< "None"
				<< "Daily"
				<< "Weekly"
				<< "Monthly"
				<< "Yearly";
    m_typeComboBox->addItems(repeatList);
    connect(m_typeComboBox, SIGNAL(currentIndexChanged(const QString)), this, 
						SLOT(handleRepeatIndexChanged(const QString)));

#ifdef Q_WS_X11
    // Add push buttons for Maemo as it does not support soft keys
    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("Ok", this);
    connect(okButton,SIGNAL(clicked()),this,SLOT(saveOrNextClicked()));
    hbLayout->addWidget(okButton);
    QPushButton *cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelClicked()));
    hbLayout->addWidget(cancelButton);
#endif

    m_scrollAreaLayout = new QVBoxLayout();
    m_scrollAreaLayout->addWidget(subjectLabel);
    m_scrollAreaLayout->addWidget(m_subjectEdit);
    m_scrollAreaLayout->addWidget(startTimeLabel);
    m_scrollAreaLayout->addWidget(m_startTimeEdit);
    m_scrollAreaLayout->addWidget(endTimeLabel);
    m_scrollAreaLayout->addWidget(m_endTimeEdit);
    m_scrollAreaLayout->addWidget(repeatLabel);
    m_scrollAreaLayout->addWidget(m_typeComboBox);

#ifdef Q_WS_X11
    m_scrollAreaLayout->addLayout(hbLayout);
#endif

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *formContainer = new QWidget(scrollArea);
    formContainer->setLayout(m_scrollAreaLayout);
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
    int repeatIndex = 0;
    bool isRepeating = false;
    // Check whether existing entry and if it is repeating.
    if(m_organizerEvent.recurrenceRules().count() != 0) {
    	if (m_organizerEvent.recurrenceRules().at(0).frequency() == QOrganizerItemRecurrenceRule::Daily){
    		repeatIndex = 1;
    	} else if (m_organizerEvent.recurrenceRules().at(0).frequency() == QOrganizerItemRecurrenceRule::Weekly) {
    		repeatIndex = 2;
    	} else if (m_organizerEvent.recurrenceRules().at(0).frequency() == QOrganizerItemRecurrenceRule::Monthly) {
    		repeatIndex = 3;
    	} else if (m_organizerEvent.recurrenceRules().at(0).frequency() == QOrganizerItemRecurrenceRule::Yearly){
    		repeatIndex = 4;
    	}
    	isRepeating = true;
    }
    if(isRepeating){
    	m_typeComboBox->setCurrentIndex(repeatIndex);
    }
}


void EventEditPage::cancelClicked()
{
    emit showDayPage();
}

void EventEditPage::saveOrNextClicked()
{
    m_organizerEvent.setDisplayLabel(m_subjectEdit->text());
    m_organizerEvent.setStartDateTime(m_startTimeEdit->dateTime());
    m_organizerEvent.setEndDateTime(m_endTimeEdit->dateTime());
    m_listOfEvents.append(m_organizerEvent);
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
    		m_manager->saveItem(&m_organizerEvent);
    	} else {
    		// If multiple entries, save asynchronously.
    		m_saveItemRequest->setItems(m_listOfEvents);
    		m_saveItemRequest->setManager(m_manager);
    		m_saveItemRequest->start();
    	}
    	
    	if (m_manager->error())
    		QMessageBox::information(this, "Failed!", QString("Failed to save event!\n(error code %1)").arg(m_manager->error()));
    	else
    		emit showDayPage();
    }
}

void EventEditPage::handleRepeatIndexChanged(const QString frequency)
{
	QOrganizerItemRecurrenceRule rrule;
	QList<QOrganizerItemRecurrenceRule> listOfRRules;
	listOfRRules.clear();
	if (frequency != "None" && m_countFieldAdded == false) {
		QFormLayout *formLayout = new QFormLayout();
		QSpinBox *countSpinBox = new QSpinBox(this);
		countSpinBox->setRange(1, 100);
		countSpinBox->setSingleStep(1);
		countSpinBox->setValue(5);
		formLayout->addRow("Count", countSpinBox);
		formLayout->setFormAlignment(Qt::AlignBottom);
		m_scrollAreaLayout->addLayout(formLayout);
		if (frequency == "Daily") {
			rrule.setFrequency(QOrganizerItemRecurrenceRule::Daily);
		} else if (frequency == "Weekly") {
			rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
		} else if (frequency == "Monthly") {
			rrule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
		} else if (frequency == "Yearly") {
			rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
		}
		// If displaying for an existing entry then read count from event.
		if(m_organizerEvent.recurrenceRules().count() != 0) {
			if (m_organizerEvent.recurrenceRules().at(0).count() != 0){
				countSpinBox->setValue(m_organizerEvent.recurrenceRules().at(0).count());
			} else {
				rrule.setCount(5);
			}
		}
		m_countFieldAdded =true;
		listOfRRules.append(rrule);
		connect(countSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handleCountChanged(int)));
	} else {
		m_scrollAreaLayout->removeItem(m_scrollAreaLayout->itemAt(4));
		m_countFieldAdded = false;
	}
	m_organizerEvent.setRecurrenceRules(listOfRRules);
}

void EventEditPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Edit event");
    QWidget::showEvent(event);
}

void EventEditPage::handlemultipleEntriesToBeCreated(int numOfEntries)
{
	m_numOfEntiresToBeCreated = numOfEntries;
	if(m_numOfEntiresToBeCreated > 1) {
		m_multipleEntries = true; 
		//The option would be next instead of save inorder to create next entry.
		m_saveOrNextSoftKey->setText("Next");
	}
}
void EventEditPage::handleCountChanged(int i)
{
	QOrganizerItemRecurrenceRule rrule;
	QList<QOrganizerItemRecurrenceRule> listOfRRules;
	listOfRRules.clear();
	rrule.setFrequency(m_organizerEvent.recurrenceRules().at(0).frequency());
	rrule.setCount(i);
	listOfRRules.append(rrule);
	m_organizerEvent.setRecurrenceRules(listOfRRules);
}
