/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NOTESMANAGER_H
#define NOTESMANAGER_H

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QTimer>
#include <QSqlDatabase>
#include <QSqlQuery>


#ifdef DECLARATIVE     
#include <QtDeclarative/qdeclarativelist.h>
#include <QtDeclarative/qdeclarative.h>
#endif

#include "note.h"

class NotesManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime alarmTime READ getAlarmTime WRITE setAlarmTime NOTIFY soundAlarm)
    Q_PROPERTY(QString alarmMessage READ getAlarmMessage WRITE setAlarmMessage)
#ifdef DECLARATIVE     
    Q_PROPERTY(QDeclarativeListProperty<QObject> noteSet READ noteSet)
#endif

public:
    NotesManager(QObject *parent = 0);
    Q_INVOKABLE QList<QObject*> getNotes(const QString& search=QString());
#ifdef DECLARATIVE     
    QDeclarativeListProperty<QObject> noteSet();
#endif

public slots:
    void addNote(const QString &note, const QDateTime &alarm);
    void removeNote(int id);
    void setSearch(const QString &search);

private:
    QDateTime m_alarmTime;
    QString m_alarmMessage;
    QList<QObject *> m_notes;
    QString m_search;

    QDateTime getAlarmTime() const;
    void setAlarmTime(const QDateTime &alarm);
    
    QString getAlarmMessage() const;
    void setAlarmMessage(const QString &message);

    void nextAlarm();

private slots:
    void checkAlarm();

signals:
    void soundAlarm(const QDateTime &alarm);
};

#endif

