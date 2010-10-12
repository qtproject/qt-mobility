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
#include "journaleditpage.h"

#include <QtGui>
#include <QComboBox>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

JournalEditPage::JournalEditPage(QWidget *parent)
    :QWidget(parent),
    m_manager(0),
    m_subjectEdit(0),
    m_timeEdit(0),
    m_alarmComboBox(0)
{
    // Create widgets
    QLabel *subjectLabel = new QLabel("Subject:", this);
    m_subjectEdit = new QLineEdit(this);
    QLabel *startTimeLabel = new QLabel("Time:", this);
    m_timeEdit = new QDateTimeEdit(this);
    QLabel *alarmLabel = new QLabel("Alarm:", this);
    m_alarmComboBox = new QComboBox(this);
    QLabel *calendarLabel = new QLabel("Calendar:", this);

    QStringList alarmList;
    alarmList  << "None"
                << "0 minutes before"
                << "5 minutes before"
                << "15 minutes before"
                << "30 minutes before"
                << "1 hour before";
    m_alarmComboBox->addItems(alarmList);
    connect(m_alarmComboBox, SIGNAL(currentIndexChanged(const QString)), this,
                        SLOT(handleAlarmIndexChanged(const QString)));

    m_calendarComboBox = new QComboBox(this);
    // the calendar names are not know here, fill the combo box later...

#ifndef Q_OS_SYMBIAN
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
    scrollAreaLayout->addWidget(m_timeEdit);
    scrollAreaLayout->addWidget(alarmLabel);
    scrollAreaLayout->addWidget(m_alarmComboBox);
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

    QAction* saveSoftKey = new QAction("Save", this);
    saveSoftKey->setSoftKeyRole(QAction::PositiveSoftKey);
    addAction(saveSoftKey);
    connect(saveSoftKey, SIGNAL(triggered(bool)), this, SLOT(saveClicked()));
}

JournalEditPage::~JournalEditPage()
{

}

void JournalEditPage::journalChanged(QOrganizerManager *manager, const QOrganizerJournal &journal)
{
    m_manager = manager;
    m_organizerJournal = journal;
    m_subjectEdit->setText(journal.displayLabel());
    m_timeEdit->setDateTime(journal.dateTime());

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
    int journalCalendarIndex = -1;
    foreach(QOrganizerCollection collection, m_collections) {
        // We currently have no way of stringifying ids
        // QString visibleName = "Calendar id = " + QString::number(collection.id().localId());
        QString visibleName = "Calendar " + QString::number(index);
        if (!calendarNameMetadataKey.isNull())
            visibleName = collection.metaData(calendarNameMetadataKey).toString();

        m_calendarComboBox->addItem(visibleName);
        if (collection.id().localId() == journal.collectionId().localId())
            journalCalendarIndex = index;
        ++index;
    }

    if (journalCalendarIndex > -1) {
        m_calendarComboBox->setCurrentIndex(journalCalendarIndex);
        m_calendarComboBox->setEnabled(false); // when modifying existing events, the calendar can't be changed anymore
    }
    else {
        m_calendarComboBox->setEnabled(true);
    }
}


void JournalEditPage::cancelClicked()
{
    emit showDayPage();
}

void JournalEditPage::saveClicked()
{
    // Read data from page
    m_organizerJournal.setDisplayLabel(m_subjectEdit->text());
    m_organizerJournal.setDateTime(m_timeEdit->dateTime());

    // Save
    if (m_calendarComboBox->currentIndex() > -1) {
        m_organizerJournal.setCollectionId(m_collections[m_calendarComboBox->currentIndex()].id());
    }
    m_manager->saveItem(&m_organizerJournal);
    if (m_manager->error())
        QMessageBox::warning(this, "Failed!", QString("Failed to save journal!\n(error code %1)").arg(m_manager->error()));
    else
        emit showDayPage();
}

void JournalEditPage::showEvent(QShowEvent *event)
{
    window()->setWindowTitle("Edit journal");
    QWidget::showEvent(event);
}

void JournalEditPage::handleAlarmIndexChanged(const QString time)
{
    QOrganizerItemVisualReminder reminder;
    reminder.setMessage(m_subjectEdit->text());

    if (time == "None") {
         QOrganizerItemVisualReminder fetchedReminder = m_organizerJournal.detail(QOrganizerItemVisualReminder::DefinitionName);
         m_organizerJournal.removeDetail(&fetchedReminder);
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

    m_organizerJournal.saveDetail(&reminder);
}

