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

#include <QtCore>
#include <QDebug>

#include "notesmanager.h"

NotesManager::NotesManager(QObject *parent)
    : QObject(parent)
{
    m_search = "";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("todoDB");
    db.open();

    QSqlQuery create;
    create.exec("CREATE TABLE todolist(id INTEGER PRIMARY KEY, notes VARCHAR(255), date VARCHAR(255))");

    nextAlarm();

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(checkAlarm()));
    timer->start(1000);
}

void NotesManager::nextAlarm()
{
    QSqlQuery alarmQuery("SELECT * FROM todolist WHERE date > DATETIME('now', 'localtime') ORDER BY date");
    if (alarmQuery.next()) { 
        setAlarmTime(QDateTime::fromString(alarmQuery.value(2).toString(), "yyyy-MM-dd HH:mm:ss"));
        setAlarmMessage(alarmQuery.value(1).toString());
    }
}

void NotesManager::checkAlarm()
{
    QString currStr = QDateTime::currentDateTime().toString(Qt::ISODate);
    QDateTime curr = QDateTime::fromString(currStr, Qt::ISODate);

    if (getAlarmTime() == curr)
        emit soundAlarm(getAlarmTime());

    nextAlarm();
}

QDateTime NotesManager::getAlarmTime() const
{
    return m_alarmTime;
}

void NotesManager::setAlarmTime(const QDateTime& alarm)
{
    m_alarmTime = alarm;
}

QString NotesManager::getAlarmMessage() const
{
    return m_alarmMessage;
}

void NotesManager::setAlarmMessage(const QString& message)
{
    m_alarmMessage = message;
}

void NotesManager::addNote(const QString& note, const QDateTime& alarm)
{
    QString alert = alarm.toString("yyyy-MM-dd HH:mm:ss");
    QSqlQuery query("INSERT INTO todolist(notes, date) VALUES ('" + note + "', '" + alert + "')");
}

void NotesManager::removeNote(int id)
{
    QSqlQuery query("DELETE FROM todolist WHERE id='" + QString::number(id) + "'");
}

void NotesManager::setSearch(const QString& search)
{
    m_search = search;
}

QList<QObject*> NotesManager::getNotes(const QString& search)
{
    m_notes.clear();
    setSearch(search);

    QString queryString = "SELECT * FROM todolist";
    if (m_search != "") queryString += " WHERE notes LIKE '%" + m_search + "%'"; 
    queryString += " ORDER BY date";

    QSqlQuery query(queryString);
    while (query.next()) {
        Note *entry = new Note(this);
        entry->setIndex(query.value(0).toInt());
        entry->setMessage(query.value(1).toString());
        entry->setAlarm(QDateTime::fromString(query.value(2).toString(), "yyyy-MM-dd HH:mm:ss"));

        m_notes << entry;
    }
   
    return m_notes;
}

#ifdef DECLARATIVE
QDeclarativeListProperty<QObject> NotesManager::noteSet()
{
    m_notes = getNotes(m_search);
    return QDeclarativeListProperty<QObject>(this, m_notes);
}
#endif
