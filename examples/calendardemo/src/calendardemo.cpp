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

#include "calendardemo.h"

#include "monthpage.h"
#include "daypage.h"
#include "eventeditpage.h"
#include "todoeditpage.h"
#include "journaleditpage.h"
#include "eventoccurrenceeditpage.h"
#include "addcalendarpage.h"
#include "editcalendarspage.h"
#ifdef BUILD_VERSIT
#include "qversitreader.h"
#include "qversitwriter.h"
#include "qversitorganizerimporter.h"
#include "qversitorganizerexporter.h"
#endif
#include <QtGui>
#include <qtorganizer.h>

QTM_USE_NAMESPACE

CalendarDemo::CalendarDemo(QWidget *parent)
    :QMainWindow(parent),
    m_stackedWidget(0),
    m_monthPage(0),
    m_dayPage(0),
    m_eventEditPage(0),
    m_eventOccurrenceEditPage(0)
{
    m_stackedWidget = new QStackedWidget(this);

    m_dayPage = new DayPage(m_stackedWidget);
    m_monthPage = new MonthPage(m_stackedWidget);
    m_eventEditPage = new EventEditPage(m_stackedWidget);
    m_todoEditPage = new TodoEditPage(m_stackedWidget);
    m_journalEditPage = new JournalEditPage(m_stackedWidget);
    m_eventOccurrenceEditPage = new EventOccurrenceEditPage(m_stackedWidget);
    m_addCalendarPage = new AddCalendarPage(m_stackedWidget);
    m_editCalendarsPage = new EditCalendarsPage(m_stackedWidget);

    //qRegisterMetaType<QOrganizerManager>("QOrganizerManager");
    qRegisterMetaType<QOrganizerItem>("QOrganizerItem");
    qRegisterMetaType<QOrganizerItemId>("QOrganizerItemId");
    qRegisterMetaType<QOrganizerCollection>("QOrganizerCollection");
    qRegisterMetaType<QOrganizerAbstractRequest::State>("QOrganizerAbstractRequest::State");

    connect(m_monthPage, SIGNAL(showDayPage(QDate)), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(showEditPage(const QOrganizerItem &)), this, SLOT(activateEditPage(const QOrganizerItem &)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(addNewEvent()), this, SLOT(addNewEvent()), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(addNewTodo()), this, SLOT(addNewTodo()), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(managerChanged(QOrganizerManager*)), this, SLOT(changeManager(QOrganizerManager*)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(managerChanged(QOrganizerManager*)), m_dayPage, SLOT(changeManager(QOrganizerManager*)), Qt::QueuedConnection);
    connect(m_monthPage, SIGNAL(currentDayChanged(QDate)), this, SLOT(updateSelectedDay(QDate)));
    connect(m_dayPage, SIGNAL(showMonthPage()), this, SLOT(activateMonthPage()), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(showEditPage(const QOrganizerItem &)), this, SLOT(activateEditPage(const QOrganizerItem &)), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(addNewEvent()), this, SLOT(addNewEvent()), Qt::QueuedConnection);
    connect(m_dayPage, SIGNAL(addNewTodo()), this, SLOT(addNewTodo()), Qt::QueuedConnection);
    connect(m_eventEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_todoEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_journalEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_eventOccurrenceEditPage, SIGNAL(showDayPage()), this, SLOT(activateDayPage()), Qt::QueuedConnection);
    connect(m_addCalendarPage, SIGNAL(showPreviousPage()), this, SLOT(activatePreviousPage()), Qt::QueuedConnection);
    connect(m_editCalendarsPage, SIGNAL(showAddCalendarPage(QOrganizerManager*,QOrganizerCollection*)), this, SLOT(editExistingCalendar(QOrganizerManager*,QOrganizerCollection*)), Qt::QueuedConnection);
    connect(m_editCalendarsPage, SIGNAL(showPreviousPage()), this, SLOT(activateMonthPage()), Qt::QueuedConnection);
    connect(m_editCalendarsPage, SIGNAL(addClicked()), this, SLOT(addCalendar()), Qt::QueuedConnection);

    // Connect to the save and remove request status change signals
    connect(&m_saveReq, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
            this, SLOT(saveReqStateChanged(QOrganizerAbstractRequest::State)));
    connect(&m_remReq, SIGNAL(stateChanged(QOrganizerAbstractRequest::State)),
            this, SLOT(removeReqStateChanged(QOrganizerAbstractRequest::State)));

    m_monthPage->init();
    
    m_stackedWidget->addWidget(m_monthPage);
    m_stackedWidget->addWidget(m_dayPage);
    m_stackedWidget->addWidget(m_eventEditPage);
    m_stackedWidget->addWidget(m_todoEditPage);
    m_stackedWidget->addWidget(m_journalEditPage);
    m_stackedWidget->addWidget(m_eventOccurrenceEditPage);
    m_stackedWidget->addWidget(m_addCalendarPage);
    m_stackedWidget->addWidget(m_editCalendarsPage);
    m_stackedWidget->setCurrentIndex(0);

    setCentralWidget(m_stackedWidget);
    buildMenu();

    activateMonthPage();
}



CalendarDemo::~CalendarDemo()
{

}

void CalendarDemo::buildMenu()
{
    // Build Options menu
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6) || defined(Q_OS_WINCE)
    // These platforms need their menu items added directly to the menu bar.
    QMenuBar *optionsMenu = menuBar();
#else
    QMenu *optionsMenu = new QMenu("&Options", this);
    #ifndef Q_OS_SYMBIAN
    // We add the options menu to the softkey manually later
    menuBar()->addMenu(optionsMenu);
    #endif
#endif
    // Add editing options in the menu for Symbian (other platforms get buttons)
    QOrganizerManager defaultManager;
    QStringList supportedItemTypes = defaultManager.supportedItemTypes();
    if (supportedItemTypes.contains(QOrganizerItemType::TypeEvent)) {
        QAction* addEventAction = optionsMenu->addAction("Add E&vent");
        connect(addEventAction, SIGNAL(triggered(bool)), this, SLOT(addNewEvent()));
    }
    if (supportedItemTypes.contains(QOrganizerItemType::TypeTodo)) {
        QAction* addTodoAction = optionsMenu->addAction("Add &Todo");
        connect(addTodoAction, SIGNAL(triggered(bool)), this, SLOT(addNewTodo()));
    }
    if (supportedItemTypes.contains(QOrganizerItemType::TypeJournal)) {
        QAction* addJournalAction = optionsMenu->addAction("Add &Journal");
        connect(addJournalAction, SIGNAL(triggered(bool)), this, SLOT(addNewJournal()));
    }
    optionsMenu->addSeparator();
    QAction* editAction = optionsMenu->addAction("&Edit");
    connect(editAction, SIGNAL(triggered(bool)), this, SLOT(editItem()));
    QAction* removeAction = optionsMenu->addAction("&Remove");
    connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeItem()));
    optionsMenu->addSeparator();
    m_switchViewAction = optionsMenu->addAction("&Open Day");
    connect(m_switchViewAction, SIGNAL(triggered(bool)), this, SLOT(switchView()));
    optionsMenu->addSeparator();
    QAction* addHugeEntries = optionsMenu->addAction("Add Test Events");
    connect(addHugeEntries, SIGNAL(triggered(bool)), this, SLOT(addEvents()));
    QAction* importItems = optionsMenu->addAction("&Import Items...");
    connect(importItems, SIGNAL(triggered(bool)), this, SLOT(importItems()));
    QAction* exportItems = optionsMenu->addAction("Ex&port Items...");
    connect(exportItems, SIGNAL(triggered(bool)), this, SLOT(exportItems()));
    QAction* deleteAllEntries = optionsMenu->addAction("Delete All Items");
    connect(deleteAllEntries, SIGNAL(triggered(bool)), this, SLOT(deleteAllEntries()));
    QAction* addCalendar = optionsMenu->addAction("New calendar");
    connect(addCalendar, SIGNAL(triggered(bool)), this, SLOT(addCalendar()));
    QAction* editCalendar = optionsMenu->addAction("Edit calendars");
    connect(editCalendar, SIGNAL(triggered(bool)), this, SLOT(editCalendar()));

#ifdef Q_OS_SYMBIAN
    // add the menu to the softkey for these pages
    m_monthPage->setMenu(optionsMenu);
    m_dayPage->setMenu(optionsMenu);
#endif
}

void CalendarDemo::activateMonthPage()
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
    menuBar()->setVisible(true);
#endif
    m_monthPage->refresh();
    m_stackedWidget->setCurrentWidget(m_monthPage);
    m_switchViewAction->setText("&Open Day");
}

void CalendarDemo::activateDayPage()
{
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
    menuBar()->setVisible(true);
#endif
    m_dayPage->refresh();
    m_stackedWidget->setCurrentWidget(m_dayPage);
    m_switchViewAction->setText("View &Month");
}

void CalendarDemo::activateEditPage(const QOrganizerItem &item)
{
    m_previousItem = item;
#if !(defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
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
    else if (item.type() == QOrganizerItemType::TypeJournal) {
        QOrganizerJournal journal = static_cast<QOrganizerJournal>(item);
        m_dayPage->dayChanged(journal.dateTime().date()); // edit always comes back to day page
        m_journalEditPage->journalChanged(m_manager, journal);
        m_stackedWidget->setCurrentWidget(m_journalEditPage);
    }
    else if (item.type() == QOrganizerItemType::TypeEventOccurrence) {
        QOrganizerEventOccurrence eventOccurrence = static_cast<QOrganizerEventOccurrence>(item);
        m_dayPage->dayChanged(eventOccurrence.startDateTime().date()); // edit always comes back to day page
        m_eventOccurrenceEditPage->eventOccurrenceChanged(m_manager, eventOccurrence);
        m_stackedWidget->setCurrentWidget(m_eventOccurrenceEditPage);
    }
    // TODO:
    //else if (item.type() == QOrganizerItemType::TypeNote)
}

void CalendarDemo::activatePreviousPage()
{
    if (m_previousPage == m_stackedWidget->indexOf(m_monthPage))
        activateMonthPage();
    else if (m_previousPage == m_stackedWidget->indexOf(m_dayPage))
        activateDayPage();
    else if (m_previousPage == m_stackedWidget->indexOf(m_editCalendarsPage))
        editCalendar();
    else
        activateEditPage(m_previousItem);
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

void CalendarDemo::addNewJournal()
{
    QOrganizerJournal newJournal;
    QDateTime time(m_currentDate);
    newJournal.setDateTime(time);
    activateEditPage(newJournal);
}

void CalendarDemo::switchView()
{
    if (m_stackedWidget->currentWidget() == m_dayPage) {
        activateMonthPage();
    } else if (m_stackedWidget->currentWidget() == m_monthPage) {
        activateDayPage();
    }
}

void CalendarDemo::editItem()
{
    if (m_stackedWidget->currentWidget() == m_dayPage) {
        m_dayPage->editItem();
    } else if (m_stackedWidget->currentWidget() == m_monthPage) {
        m_monthPage->editItem();
    }
}

void CalendarDemo::removeItem()
{
    if (m_stackedWidget->currentWidget() == m_dayPage) {
        m_dayPage->removeItem();
    } else if (m_stackedWidget->currentWidget() == m_monthPage) {
        m_monthPage->removeItem();
    }
}

void CalendarDemo::addEvents()
{
    QList<QOrganizerItem> items;
    
    // Create a large number of events asynchronously
    QOrganizerCollectionId defaultCollectionId = m_manager->defaultCollection().id();
    for(int index=0 ; index <  100 ; index++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeEvent);
        item.setDescription(QString("Event %1").arg(index));
        item.setDisplayLabel(QString("Subject for event %1").arg(index + 1));
        
        // Set the start date to index to add events to next 5000 days
        QOrganizerEventTime timeRange;
        timeRange.setStartDateTime(QDateTime::currentDateTime().addDays(index));
        timeRange.setEndDateTime(QDateTime::currentDateTime().addDays(index).addSecs(60 * 60));
        item.saveDetail(&timeRange);

        item.setCollectionId(defaultCollectionId);
        
        items.append(item);
    }
    
    // Now create a save request and execute it
    m_saveReq.setItems(items);
    m_saveReq.setManager(m_manager);
    m_saveReq.start();
}

void CalendarDemo::importItems()
{
#ifdef BUILD_VERSIT
    if (!m_manager) {
        qWarning() << "No manager selected; cannot import";
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this,
       tr("Select iCalendar file"), ".", tr("iCalendar files (*.ics)"));
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    if (!file.isReadable()) {
        qWarning() << "File is not readable";
        return;
    }
    QVersitReader reader;
    reader.setDevice(&file);
    if (!reader.startReading() || !reader.waitForFinished()) {
        qWarning() << "Read failed, " << reader.error();
    }
    QVersitOrganizerImporter importer;
    foreach (const QVersitDocument& document, reader.results()) {
        if (!importer.importDocument(document)) {
            qWarning() << "Import failed, " << importer.errorMap();
            continue;
        }
        QList<QOrganizerItem> items = importer.items();
        QList<QOrganizerItem>::iterator it = items.begin();
        while (it != items.end()) {
            *it = m_manager->compatibleItem(*it);
            it++;
        }
        m_manager->saveItems(&items);
    }
    m_monthPage->refresh();
    m_dayPage->refresh();
#endif
}

void CalendarDemo::exportItems()
{
#ifdef BUILD_VERSIT
    if (!m_manager) {
        qWarning() << "No manager selected; cannot export";
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save iCalendar"),
                                                    "./calendar.ics",
                                                    tr("iCalendar files (*.ics)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    if (!file.isWritable()) {
        qWarning() << "File is not writable";
        return;
    }
    QList<QOrganizerItem> items(m_manager->itemsForExport());
    QVersitOrganizerExporter exporter;
    if (!exporter.exportItems(items)) {
        qWarning() << "Export failed, " << exporter.errorMap();
        return;
    }
    QVersitDocument document = exporter.document();
    QVersitWriter writer;
    writer.setDevice(&file);
    if (!writer.startWriting(QList<QVersitDocument>() << document)
        || !writer.waitForFinished()) {
        qWarning() << "Write failed, " << writer.error();
    }
#endif
}

void CalendarDemo::deleteAllEntries()
{
    // Fetch all the entries
    QList<QOrganizerItemId> ids = m_manager->itemIds();
    
    if(ids.count()) {
        m_remReq.setItemIds(ids);
        m_remReq.setManager(m_manager);
        m_remReq.start();
    }
}

void CalendarDemo::addCalendar()
{
    // Get default collection
    QOrganizerCollection defaultCollection = m_manager->defaultCollection();

    QOrganizerCollection newCollection = defaultCollection;
    newCollection.setId(QOrganizerCollectionId()); // reset collection id
#if defined(Q_WS_MAEMO_5)
    newCollection.setMetaData("Name", "New calendar");
#endif
    m_addCalendarPage->calendarChanged(m_manager, newCollection);

    m_previousPage = m_stackedWidget->currentIndex();
    m_stackedWidget->setCurrentWidget(m_addCalendarPage);
}

void CalendarDemo::editCalendar()
{
    m_editCalendarsPage->showPage(m_manager);
    m_previousPage = m_stackedWidget->currentIndex();
    m_stackedWidget->setCurrentWidget(m_editCalendarsPage);
}

void CalendarDemo::editExistingCalendar(QOrganizerManager *manager, QOrganizerCollection* calendar)
{
    m_addCalendarPage->calendarChanged(manager, *calendar);
    m_previousPage = m_stackedWidget->currentIndex();
    m_stackedWidget->setCurrentWidget(m_addCalendarPage);
}

void CalendarDemo::saveReqStateChanged(QOrganizerAbstractRequest::State reqState)
{
    if(QOrganizerAbstractRequest::ActiveState == reqState) {
        // Request started. Show a progress or wait dialog
        m_progressDlg = new QProgressDialog("Saving events..", "Cancel", 100, 100, this);
        connect(m_progressDlg, SIGNAL(canceled()), &m_saveReq, SLOT(cancel()));
        m_progressDlg->show();
    } else if (QOrganizerAbstractRequest::FinishedState == reqState ||
               QOrganizerAbstractRequest::CanceledState == reqState) {
        // Request finished or cancelled. Stop showing the progress dialog and refresh
        m_progressDlg->hide();
        m_monthPage->refresh();
        m_dayPage->refresh();
    }
}

void CalendarDemo::removeReqStateChanged(QOrganizerAbstractRequest::State reqState)
{
    if(QOrganizerAbstractRequest::ActiveState == reqState) {
        // Request started. Show a progress or wait dialog
        m_progressDlg = new QProgressDialog("Removing events..", "Cancel", 100, 100, this);
        connect(m_progressDlg, SIGNAL(canceled()), &m_remReq, SLOT(cancel()));
        m_progressDlg->show();
    } else if (QOrganizerAbstractRequest::FinishedState == reqState ||
               QOrganizerAbstractRequest::CanceledState == reqState) {
        // Request finished or cancelled. Stop showing the progress dialog and refresh
        m_progressDlg->hide();
        m_monthPage->refresh();
        m_dayPage->refresh();
    }
}

void CalendarDemo::changeManager(QOrganizerManager *manager)
{
    m_manager = manager;
}

void CalendarDemo::updateSelectedDay(const QDate& date)
{
    m_dayPage->dayChanged(date);
    m_currentDate = date;
}
