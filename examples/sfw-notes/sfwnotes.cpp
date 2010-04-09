/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>

#include "sfwnotes.h"

ToDoTool::ToDoTool(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    setupUi(this);
    #if defined(Q_OS_MAC)
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();
        dir.cd("PlugIns");
        QCoreApplication::addLibraryPath(dir.absolutePath());
    #endif

    serviceManager = new QServiceManager(this);

    registerExampleServices();

    setWindowTitle(tr("ToDoTool"));

    init();
}

ToDoTool::~ToDoTool()
{
    unregisterExampleServices();
}

void ToDoTool::soundAlarm(const QDateTime& alarm)
{
    QString message = notesManager->property("alarmMessage").toString();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Alert");
    msgBox.setText("ALARM SOUNDED!!!\n\n" +alarm.toString("yyyy-MM-dd HH:mm") + "\n\n" + message);
    msgBox.resize(200, msgBox.height());
    msgBox.exec();
}

void ToDoTool::init()
{
    bool ok;
    QString interfaceName = QInputDialog::getText(this, tr("ToDoTool"), tr("Specify Notes Manager interface:"),
                                                  QLineEdit::Normal, "com.nokia.qt.examples.NotesManager", &ok);
    if (ok) {
        QServiceInterfaceDescriptor desc = serviceManager->interfaceDefault(interfaceName);
        
        if (desc.isValid()) {
            QServiceManager mgr;
            notesManager = mgr.loadInterface(desc);

            if (notesManager)
                notesManager->setParent(this);

            currentNote = 1;
            searchWord = "";    
            refreshList();

            addButton->setEnabled(true);
            deleteButton->setEnabled(true);
            searchButton->setEnabled(true);
            nextButton->setEnabled(true);
            prevButton->setEnabled(true);

            QObject::connect(notesManager, SIGNAL(soundAlarm(QDateTime)), 
                            this, SLOT(soundAlarm(QDateTime)));
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("ToDoTool"));
            msgBox.setText("No valid default interface for:\n\n\"" + interfaceName + "\"");
            msgBox.exec();
        }
    }
}

void ToDoTool::registerExampleServices()
{
    QStringList exampleXmlFiles;
    exampleXmlFiles << "notesmanagerservice.xml";
    foreach (const QString &fileName, exampleXmlFiles) {
        QString path = QCoreApplication::applicationDirPath() + "/xmldata/" + fileName;
        serviceManager->addService(path);
    }
}

void ToDoTool::unregisterExampleServices()
{
    serviceManager->removeService("NotesManagerService");
}

void ToDoTool::refreshList()
{
        QMetaObject::invokeMethod(notesManager, "getNotes", 
                                  Q_RETURN_ARG(QList<QObject*>, ret),
                                  Q_ARG(QString, searchWord)); 

        totalNotes = ret.size();
    
        if (totalNotes < 1) { currentNote = 0; }
        else if (totalNotes > 0 && currentNote == 0) { currentNote = 1; }
        
        refreshNotes();
}

void ToDoTool::refreshNotes()
{
    countLabel->setText(QString::number(currentNote) + "/" + QString::number(totalNotes));
    
    if (currentNote == 0) {
        dateLabel->setText("");
        noteLabel->setText("Click + to add a note");
    }
    else {
        QDateTime alarm;
        QMetaObject::invokeMethod(ret[currentNote-1], "alarm", Q_RETURN_ARG(QDateTime, alarm));
        dateLabel->setText(alarm.toString("yyyy-MM-dd HH:mm"));

        QString note;
        QMetaObject::invokeMethod(ret[currentNote-1], "message", Q_RETURN_ARG(QString, note));
        noteLabel->setText(note);
    }
}

void ToDoTool::on_nextButton_clicked()
{
    if (currentNote < totalNotes) {
        currentNote++;
        refreshNotes();
    }
}

void ToDoTool::on_prevButton_clicked()
{
    if (currentNote > 1) {
        currentNote--;
        refreshNotes();
    }
}

void ToDoTool::on_addButton_clicked()
{
    // re-implement date-time input method

    bool ok;
    QString newNote = QInputDialog::getText(this, tr("ToDoTool"), 
                                            tr("Add a new note + alarm of format:\nnote#yyyy-mm-dd#hh:mm"), 
                                            QLineEdit::Normal, "", &ok);

    if (ok && !newNote.isEmpty()) {
        QStringList note = newNote.split(QRegExp("#"));
      
        if (note.size() == 3) {

            QStringList date = note.at(1).split("-");
            QStringList time = note.at(2).split(":");

            if (date.size() == 3 && time.size() == 2) {
                QDateTime alarm = QDateTime::fromString(note.at(1)+" "+note.at(2),"yyyy-MM-dd HH:mm");
                QMetaObject::invokeMethod(notesManager, "addNote",
                                          Q_ARG(QString, note.at(0)),
                                          Q_ARG(QDateTime, alarm));
            }
        } else {
                QMetaObject::invokeMethod(notesManager, "addNote",
                                          Q_ARG(QString, note.at(0)),
                                          Q_ARG(QDateTime, QDateTime::currentDateTime()));
        }

        refreshList();
    }
}

void ToDoTool::on_deleteButton_clicked()
{
    if (currentNote != 0) {
        QMessageBox msgBox;
        msgBox.setText("Confirm removing this note?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        
        if (msgBox.exec() == QMessageBox::Ok) {
            int index;
            QMetaObject::invokeMethod(ret[currentNote-1], "index", Q_RETURN_ARG(int, index));

            QMetaObject::invokeMethod(notesManager, "removeNote", Q_ARG(int, index));
            if (currentNote > 1)
                currentNote--;

            refreshList();
        }
    }
}

void ToDoTool::on_searchButton_clicked()
{
    bool ok;
    QString searchNote = QInputDialog::getText(this, tr("ToDoTool"), tr("Find a note:"),
                                               QLineEdit::Normal, "", &ok);
    if (ok) {
        if (searchNote.isEmpty())
            searchWord = "";
        else
            searchWord = searchNote;
        
        currentNote = 1;
        refreshList();
    }
}

