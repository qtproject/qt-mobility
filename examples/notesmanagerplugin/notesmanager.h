/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

