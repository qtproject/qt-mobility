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
    m_alarmComboBox(0),
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
    QLabel *endTimeLabel = new QLabel("End time:", this);
    m_endTimeEdit = new QDateTimeEdit(this);
    QLabel *repeatLabel = new QLabel("Repeat:", this);
    QLabel *alarmLabel = new QLabel("Alarm:", this);
    QLabel *calendarLabel = new QLabel("Calendar:", this);
    m_alarmComboBox = new QComboBox(this);
    m_typeComboBox = new QComboBox(this);
    m_typeComboBox->addItem("None");
    m_typeComboBox->addItem("Daily");
    m_typeComboBox->addItem("Weekly");
    m_typeComboBox->addItem("Monthly");
    m_typeComboBox->addItem("Yearly");
    connect(m_typeComboBox, SIGNAL(currentIndexChanged(const QString&)), this, 
            SLOT(frequencyChanged(const QString&)));
    QStringList alarmList;
    alarmList	<< "None"
                << "0 minutes before"
                << "5 minutes before"
                << "15 minutes before"
                << "30 minutes before"
                << "1 hour before";
    m_alarmComboBox->addItems(alarmList);
    connect(m_alarmComboBox, SIGNAL(currentIndexChanged(const QString)), this,
                        SLOT(alarmIndexChanged(const QString)));

    m_endConditionComboBox = new QComboBox(this);
    m_endConditionComboBox->addItem("Forever");
    m_endConditionComboBox->addItem("Until a date");
    m_endConditionComboBox->addItem("For a number of occurrences");
    m_endConditionComboBox->setVisible(false);
    connect(m_endConditionComboBox, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(endConditionChanged(const QString&)));

    m_countSpinBox = new QSpinBox(this);
    m_countSpinBox->setRange(1, 100);
    m_countSpinBox->setSingleStep(1);
    m_countSpinBox->setVisible(false);
    connect(m_countSpinBox, SIGNAL(valueChanged(int)), this, SLOT(countChanged(int)));

    m_repeatUntilDate = new QDateEdit(this);
    m_repeatUntilDate->setVisible(false);
    connect(m_repeatUntilDate, SIGNAL(dateChanged(QDate)), this, SLOT(untilChanged(QDate)));

    m_calendarComboBox = new QComboBox(this);
    // the calendar names are not know here, fill the combo box later...

#ifndef Q_OS_SYMBIAN
    // Add push buttons for non-Symbian platforms as they do not support soft keys
    QHBoxLayout* hbLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("Save", this);
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
    scrollAreaLayout->addWidget(endTimeLabel);
    scrollAreaLayout->addWidget(m_endTimeEdit);
    scrollAreaLayout->addWidget(alarmLabel);
    scrollAreaLayout->addWidget(m_alarmComboBox);
    scrollAreaLayout->addWidget(repeatLabel);
    scrollAreaLayout->addWidget(m_typeComboBox);
    scrollAreaLayout->addWidget(m_endConditionComboBox);
    scrollAreaLayout->addWidget(m_countSpinBox);
    scrollAreaLayout->addWidget(m_repeatUntilDate);
    scrollAreaLayout->addWidget(calendarLabel);
    scrollAreaLayout->addWidget(m_calendarComboBox);
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
    
    QAction* saveSoftKey = new QAction("Save",this);
    saveSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(saveSoftKey);
    connect(saveSoftKey, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
    m_listOfEvents.clear();
}

EventEditPage::~EventEditPage()
{

}

void EventEditPage::eventChanged(QOrganizerManager *manager, const QOrganizerEvent &event)
{
    m_manager = manager;
    m_organizerEvent = event;
    m_subjectEdit->setText(event.displayLabel());
    m_startTimeEdit->setDateTime(event.startDateTime());
    m_endTimeEdit->setDateTime(event.endDateTime());
    QSet<QOrganizerRecurrenceRule> rrules(m_organizerEvent.recurrenceRules());
    // Check whether existing entry and if it is repeating.
    if (rrules.count() != 0) {
        QOrganizerRecurrenceRule rrule(rrules.values().at(0));
        QOrganizerRecurrenceRule::Frequency freq(rrule.frequency());
        switch (freq) {
        case QOrganizerRecurrenceRule::Daily:
            m_typeComboBox->setCurrentIndex(1);
            break;
        case QOrganizerRecurrenceRule::Weekly:
            m_typeComboBox->setCurrentIndex(2);
            break;
        case QOrganizerRecurrenceRule::Monthly:
            m_typeComboBox->setCurrentIndex(3);
            break;
        case QOrganizerRecurrenceRule::Yearly:
            m_typeComboBox->setCurrentIndex(4);
            break;
        case QOrganizerRecurrenceRule::Invalid:
            m_typeComboBox->setCurrentIndex(0); // No repeat
            return;
        }
        if (rrule.limitType() == QOrganizerRecurrenceRule::DateLimit) {
            m_endConditionComboBox->setCurrentIndex(1); // End date specified
            m_repeatUntilDate->setDate(rrule.limitDate());
        } else if (rrule.limitType() == QOrganizerRecurrenceRule::CountLimit) {
            m_endConditionComboBox->setCurrentIndex(2); // Count specified
            m_countSpinBox->setValue(rrule.limitCount());
        }
    } else {
        m_typeComboBox->setCurrentIndex(0); // No repeat
    }

    // set calendar selection
    m_calendarComboBox->clear();

    // resolve metadata field that contains calendar name (if any)
    QString calendarNameMetadataKey;
    m_collections = m_manager->collections();
    if (!m_collections.isEmpty()) {
        QOrganizerCollection firstCollection = m_collections[0];
        QVariantMap metadata = firstCollection.metaData();
        QList<QString> metaDataKeys = metadata.keys();
        foreach(QString key, metaDataKeys) {
            if (key.indexOf("name", 0, Qt::CaseInsensitive) != -1) {
                calendarNameMetadataKey = key;
                break;
            }
        }
    }
    int index = 0;
    int eventCalendarIndex = -1;
    foreach(QOrganizerCollection collection, m_collections) {
        // We currently have no way of stringifying ids
        //QString visibleName = "Calendar id = " + QString::number(collection.id().localId());
        QString visibleName = "Calendar " + QString::number(index);
        if (!calendarNameMetadataKey.isNull())
            visibleName = collection.metaData(calendarNameMetadataKey).toString();

        m_calendarComboBox->addItem(visibleName);
        if (collection.id().localId() == event.collectionId().localId())
            eventCalendarIndex = index;
        ++index;
    }

    if (eventCalendarIndex > -1) {
        m_calendarComboBox->setCurrentIndex(eventCalendarIndex);
        m_calendarComboBox->setEnabled(false); // when modifying existing events, the calendar can't be changed anymore
    }
    else {
        m_calendarComboBox->setEnabled(true);
    }
}

void EventEditPage::cancelClicked()
{
    emit showDayPage();
}

void EventEditPage::saveClicked()
{
    QDateTime start(m_startTimeEdit->dateTime());
    QDateTime end(m_endTimeEdit->dateTime());
    if (start > end) {
        QMessageBox::warning(this, "Failed!", "Start date is not before end date");
        return;
    }

    m_organizerEvent.setDisplayLabel(m_subjectEdit->text());
    m_organizerEvent.setStartDateTime(start);
    m_organizerEvent.setEndDateTime(end);
    m_listOfEvents.append(m_organizerEvent);
    if (m_calendarComboBox->currentIndex() > 0) {
        m_organizerEvent.setCollectionId(m_collections[m_calendarComboBox->currentIndex()].id());
    }
    m_manager->saveItem(&m_organizerEvent);
    if (m_manager->error())
        QMessageBox::warning(this, "Failed!", QString("Failed to save event!\n(error code %1)").arg(m_manager->error()));
    else
        emit showDayPage();
}

void EventEditPage::frequencyChanged(const QString& frequency)
{
    QOrganizerRecurrenceRule rrule;

    if (frequency != "None") {
        m_endConditionComboBox->setVisible(true);

        if (frequency == "Daily") {
            rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
        } else if (frequency == "Weekly") {
            rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
        } else if (frequency == "Monthly") {
            rrule.setFrequency(QOrganizerRecurrenceRule::Monthly);
        } else if (frequency == "Yearly") {
            rrule.setFrequency(QOrganizerRecurrenceRule::Yearly);
        }
        m_organizerEvent.setRecurrenceRule(rrule);
    } else {
        m_endConditionComboBox->setCurrentIndex(0);
        m_endConditionComboBox->setVisible(false);
    }
}

void EventEditPage::alarmIndexChanged(const QString time)
{
    QOrganizerItemVisualReminder reminder;
    reminder.setMessage(m_subjectEdit->text());

    if (time == "None") {
         QOrganizerItemVisualReminder fetchedReminder = m_organizerEvent.detail(QOrganizerItemVisualReminder::DefinitionName);
         m_organizerEvent.removeDetail(&fetchedReminder);
        return;
    } else if (time == "0 minutes before") {
        reminder.setSecondsBeforeStart(0);
    } else if (time == "5 minutes before") {
        reminder.setSecondsBeforeStart(5*60);
    } else if (time == "15 minutes before") {
        reminder.setSecondsBeforeStart(15*60);
    } else if (time == "30 minutes before") {
        reminder.setSecondsBeforeStart(30*60);
    } else if (time == "1 hour before") {
        reminder.setSecondsBeforeStart(60*60);
    }

    m_organizerEvent.saveDetail(&reminder);
}

void EventEditPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Edit event");
    QWidget::showEvent(event);
}

void EventEditPage::countChanged(int i)
{
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(m_organizerEvent.recurrenceRules().values().at(0).frequency());
    rrule.setLimit(i);
    m_organizerEvent.setRecurrenceRule(rrule);
}

void EventEditPage::untilChanged(QDate date)
{
    QOrganizerRecurrenceRule rrule;
    rrule.setFrequency(m_organizerEvent.recurrenceRules().values().at(0).frequency());
    rrule.setLimit(date);
    m_organizerEvent.setRecurrenceRule(rrule);
}

void EventEditPage::endConditionChanged(const QString& endCondition) {
    if (endCondition == "Forever") {
        m_countSpinBox->setVisible(false);
        m_repeatUntilDate->setVisible(false);
    } else if (endCondition == "Until a date") {
        setRepeatUntilField();
    } else if (endCondition == "For a number of occurrences") {
        setCountField();
    }
}

void EventEditPage::setCountField()
{
    m_countSpinBox->setVisible(true);
    m_repeatUntilDate->setVisible(false);
    m_countSpinBox->setValue(5);
    countChanged(5); // default value.
}

void EventEditPage::setRepeatUntilField()
{
    m_countSpinBox->setVisible(false);
    m_repeatUntilDate->setVisible(true);
    m_repeatUntilDate->setDate(m_endTimeEdit->date());
    untilChanged(m_endTimeEdit->date()); // default value.
}
