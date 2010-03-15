/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
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

#ifndef NOTE_H
#define NOTE_H

#include <QObject>
#include <QDateTime>
#include <QtDeclarative/qdeclarative.h>

class Note : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ index WRITE setIndex)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(QDateTime alarm READ alarm WRITE setAlarm)

public:
    Note(QObject *parent =0) : QObject(parent) {}
    ~Note() {}

public slots:
    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }

    QString message() const { return m_message; }
    void setMessage(const QString &message) { m_message = message; }

    QDateTime alarm() const { return m_alarm; }
    void setAlarm(const QDateTime &alarm) { m_alarm = alarm; }

private:
    int m_index;
    QString m_message;
    QDateTime m_alarm;
};

//QML_DECLARE_TYPE(Note)

class NoteSet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Note*>* noteset READ noteset WRITE setnoteset)

public:
    NoteSet(QObject *parent =0) : QObject(parent) {};
    ~NoteSet() {};

    QList<Note*> *noteset() { return &m_noteset; }
    void setnoteset(QList<Note*>* set) { m_noteset = *set; }

private:
    QList<Note*> m_noteset;
};

//QML_DECLARE_TYPE(NoteSet)

#endif

