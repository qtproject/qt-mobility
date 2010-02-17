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
#include <QDebug>

#include <qservicemanager.h>
#include <qserviceinterfacedescriptor.h>

#include "todotool.h"

Q_DECLARE_METATYPE(QServiceInterfaceDescriptor)

ToDoTool::ToDoTool(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    setupUi(this);

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
    QString message = notesObject->property("message").toString();

    QMessageBox msgBox;
    msgBox.setWindowTitle("Alert");
    msgBox.setText(alarm.toString() + "\n\n" + message);
    msgBox.exec();
}

void ToDoTool::init()
{
    QServiceManager mgr;
    QServiceFilter filter;
    filter.setServiceName("NotesManagerService");
    QList<QServiceInterfaceDescriptor> list = serviceManager->findInterfaces(filter);

    notesObject = mgr.loadInterface(list[0]);

    if (notesObject)
        notesObject->setParent(this);

    {
        currentNote = 1;
        searchWord = "";    
        refreshList();

        QObject::connect(notesObject, SIGNAL(soundAlarm(QDateTime)), 
                         this, SLOT(soundAlarm(QDateTime)));
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
    serviceManager->removeService("DatabaseManagerService");
}

void ToDoTool::refreshList()
{
        QMetaObject::invokeMethod(notesObject, "getNotes", 
                                  Q_RETURN_ARG(QList<Note>, ret),
                                  Q_ARG(QString, searchWord)); 

        totalNotes = ret.size();
        
        if (totalNotes < 1)
            currentNote = 0;
        
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
        dateLabel->setText(ret[currentNote-1].alert.toString());
        noteLabel->setText(ret[currentNote-1].message);
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
                                            QLineEdit::Normal, QDir::home().dirName(), &ok);
    if (ok && !newNote.isEmpty()) {
        QStringList note = newNote.split(QRegExp("#"));
      
        if (note.size() == 3) {

            QStringList date = note.at(1).split("-");
            QStringList time = note.at(2).split(":");

            if (date.size() == 3 && time.size() == 2) {
                QDateTime alarm = QDateTime::fromString(note.at(1)+" "+note.at(2),"yyyy-MM-dd HH:mm");
                QMetaObject::invokeMethod(notesObject, "addNote",
                                          Q_ARG(QString, note.at(0)),
                                          Q_ARG(QDateTime, alarm));
                refreshList();
            }
        } else {
                QMetaObject::invokeMethod(notesObject, "addNote",
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
        msgBox.setWindowTitle("ToDoTool");
        msgBox.setText("Are you sure you want to remove this note item?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        
        if (msgBox.exec() == QMessageBox::Ok) {
            int index = ret[currentNote-1].index;

            QMetaObject::invokeMethod(notesObject, "removeNote", Q_ARG(int, index));
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
                                               QLineEdit::Normal, QDir::home().dirName(), &ok);
    if (ok) {
        if (searchNote.isEmpty())
            searchWord = "";
        else
            searchWord = searchNote;
        
        currentNote = 1;
        refreshList();
    }
}

