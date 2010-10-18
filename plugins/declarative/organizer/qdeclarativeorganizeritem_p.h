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

#ifndef QDECLARATIVEORGANIZERITEM_H
#define QDECLARATIVEORGANIZERITEM_H

#include "qdeclarative.h"
#include <QDeclarativeExtensionPlugin>
#include <QDeclarativeListProperty>

#include "qorganizeritems.h"
#include "qorganizeritemdetaildefinition.h"
#include "qdeclarativeorganizeritemdetail_p.h"

QTM_USE_NAMESPACE;

class QDeclarativeOrganizerItemMetaObject;
class QDeclarativeOrganizerItemDetail;
class QDeclarativeOrganizerItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> details READ details NOTIFY itemChanged);
    Q_PROPERTY (QString manager READ manager)
    Q_PROPERTY (uint itemId READ itemId)
    Q_PROPERTY (QString type READ type)
    Q_PROPERTY (QString displayLabel READ displayLabel WRITE setDisplayLabel NOTIFY itemChanged)
    Q_PROPERTY (QString description READ description WRITE setDescription NOTIFY itemChanged)
    Q_PROPERTY (QString guid READ guid WRITE setGuid NOTIFY itemChanged)
    Q_PROPERTY (bool modified READ modified)

    Q_CLASSINFO("DefaultProperty", "details")

public:
    explicit QDeclarativeOrganizerItem(QObject *parent = 0);
    explicit QDeclarativeOrganizerItem(const QOrganizerItem& item, const QMap<QString, QOrganizerItemDetailDefinition>& defs, QObject *parent = 0);
    ~QDeclarativeOrganizerItem();

    uint itemId() const;
    QString manager() const;
    bool modified() const;

    void setItem(const QOrganizerItem& c);
    QOrganizerItem item() const;

    void setDetailDefinitions(const QMap<QString, QOrganizerItemDetailDefinition>& defs);
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions() const;

    QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> details();

    Q_INVOKABLE QVariant detail(const QString& name);
    Q_INVOKABLE QVariant details(const QString& name);
    Q_INVOKABLE void addComment(const QString& comment);
    Q_INVOKABLE void clearComments();

    QString type() const;
    QString displayLabel() const;
    void setDisplayLabel(const QString& label);
    QString description() const;
    void setDescription(const QString& description);
    QString guid() const;
    void setGuid(const QString& guid);

public slots:
    void save();
    void clearDetails();
signals:
    void itemChanged();
private slots:
    void setModified();
protected:
    QDeclarativeOrganizerItemMetaObject* d;
    friend class QDeclarativeOrganizerItemMetaObject;
};

QML_DECLARE_TYPE(QDeclarativeOrganizerItem)

//event
class QDeclarativeOrganizerEvent : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime NOTIFY valueChanged)
    Q_PROPERTY(bool allDay READ isAllDay WRITE setAllDay NOTIFY valueChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::PriorityType priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemRecurrence* recurrence READ recurrence NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "event");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "events");

    explicit QDeclarativeOrganizerEvent(QObject *parent = 0);

    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;
    void setEndDateTime(const QDateTime& endDateTime);
    QDateTime endDateTime() const;
    void setAllDay(bool isAllDay);
    bool isAllDay() const;
    void setPriority(QDeclarativeOrganizerItemPriority::PriorityType priority);
    QDeclarativeOrganizerItemPriority::PriorityType priority() const;
    QString location() const;
    void setLocation(const QString& loc);

    QDeclarativeOrganizerItemRecurrence* recurrence();

signals:
    void valueChanged();
private:
    QOrganizerEvent* m_event;

};
QML_DECLARE_TYPE(QDeclarativeOrganizerEvent)

//event occurrence
class QDeclarativeOrganizerEventOccurrence : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime NOTIFY valueChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::PriorityType priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(uint parentId READ parentId WRITE setParentId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "eventOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "eventOccurrences");

    explicit QDeclarativeOrganizerEventOccurrence(QObject *parent = 0);

    void setParentId(uint parentId);
    uint parentId() const;

    void setOriginalDate(const QDate& date);

    QDate originalDate() const;
    void setStartDateTime(const QDateTime& startDateTime);

    QDateTime startDateTime() const;

    void setEndDateTime(const QDateTime& endDateTime);
    QDateTime endDateTime() const;

    void setPriority(QDeclarativeOrganizerItemPriority::PriorityType priority);
    QDeclarativeOrganizerItemPriority::PriorityType priority() const;

    QString location() const;
    void setLocation(const QString& loc);

signals:
    void valueChanged();
private:
    QOrganizerEventOccurrence* m_eo;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerEventOccurrence)

//journal
class QDeclarativeOrganizerJournal : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "journal");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "journals");
    explicit QDeclarativeOrganizerJournal(QObject *parent = 0);
    void setDateTime(const QDateTime& dateTime);
    QDateTime dateTime() const;
signals:
    void valueChanged();
private:
    QOrganizerJournal* m_journal;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerJournal)

//note
class QDeclarativeOrganizerNote : public QDeclarativeOrganizerItem
{
    Q_OBJECT
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "note");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "notes");

    explicit QDeclarativeOrganizerNote(QObject *parent = 0);
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerNote)

//todo
class QDeclarativeOrganizerTodo : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dueDateTime READ dueDateTime WRITE setDueDateTime NOTIFY valueChanged)
    Q_PROPERTY(bool isAllDay READ isAllDay WRITE setAllDay NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::PriorityType priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(int progressPercentage READ progressPercentage WRITE setProgressPercentage NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerTodoProgress::StatusType status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemRecurrence* recurrence READ recurrence NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "todo");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "todos");

    explicit QDeclarativeOrganizerTodo(QObject *parent = 0);
    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;

    void setDueDateTime(const QDateTime& dueDateTime);

    QDateTime dueDateTime() const;

    void setAllDay(bool isAllDay);
    bool isAllDay() const;

    void setPriority(QDeclarativeOrganizerItemPriority::PriorityType priority);
    QDeclarativeOrganizerItemPriority::PriorityType priority() const;
    void setProgressPercentage(int percentage);
    int progressPercentage() const;
    void setStatus(QDeclarativeOrganizerTodoProgress::StatusType status);
    QDeclarativeOrganizerTodoProgress::StatusType status() const;

    void setFinishedDateTime(const QDateTime& finishedDateTime);
    QDateTime finishedDateTime() const;
    QDeclarativeOrganizerItemRecurrence* recurrence();

signals:
    void valueChanged();
private:
    QOrganizerTodo* m_todo;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodo)

//todo occurrence
class QDeclarativeOrganizerTodoOccurrence : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dueDateTime READ dueDateTime WRITE setDueDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::PriorityType priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(int progressPercentage READ progressPercentage WRITE setProgressPercentage NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerTodoProgress::StatusType status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)
    Q_PROPERTY(uint parentId READ parentId WRITE setParentId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "todoOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "todoOccurrences");

    explicit QDeclarativeOrganizerTodoOccurrence(QObject *parent = 0);
    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;

    void setDueDateTime(const QDateTime& dueDateTime);
    QDateTime dueDateTime() const;
    uint parentId() const;
    void setParentId(uint parentId);

    void setPriority(QDeclarativeOrganizerItemPriority::PriorityType priority);

    QDeclarativeOrganizerItemPriority::PriorityType priority() const;


    void setOriginalDate(const QDate& date);
    QDate originalDate() const;

    void setProgressPercentage(int percentage);
    int progressPercentage() const;

    void setStatus(QDeclarativeOrganizerTodoProgress::StatusType status);

    QDeclarativeOrganizerTodoProgress::StatusType status() const;
    void setFinishedDateTime(const QDateTime& finishedDateTime);
    QDateTime finishedDateTime() const;
signals:
    void valueChanged();
private:
    QOrganizerTodoOccurrence* m_to;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoOccurrence)
#endif // QDECLARATIVEORGANIZERITEM_H
