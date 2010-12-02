/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file Is pre-release code and may not be distributed.
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
    Q_PROPERTY (QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> details READ details NOTIFY itemChanged)
    Q_PROPERTY (QString manager READ manager)
    Q_PROPERTY (QString itemId READ itemId)
    Q_PROPERTY (QString type READ type)
    Q_PROPERTY (QString displayLabel READ displayLabel WRITE setDisplayLabel NOTIFY itemChanged)
    Q_PROPERTY (QString description READ description WRITE setDescription NOTIFY itemChanged)
    Q_PROPERTY (QString guid READ guid WRITE setGuid NOTIFY itemChanged)
    Q_PROPERTY (bool modified READ modified)
    Q_ENUMS(OrganizerItemType)
    Q_CLASSINFO("DefaultProperty", "details")

public:
    enum OrganizerItemType {
        Event = 0,
        EventOccurrence,
        Todo,
        TodoOccurrence,
        Journal,
        Note,
        Customized = 100
    };

    explicit QDeclarativeOrganizerItem(QObject *parent = 0);
    explicit QDeclarativeOrganizerItem(const QOrganizerItem& item, const QMap<QString, QOrganizerItemDetailDefinition>& defs, QObject *parent = 0);
    ~QDeclarativeOrganizerItem();

    OrganizerItemType itemType() const;

    QString itemId() const;
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
    Q_INVOKABLE bool removeDetail(QDeclarativeOrganizerItemDetail* detail);

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
    void clearComments();
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
    Q_PROPERTY(QString parentId READ parentId WRITE setParentId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "eventOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "eventOccurrences");

    explicit QDeclarativeOrganizerEventOccurrence(QObject *parent = 0);

    void setParentId(const QString& parentId);
    QString parentId() const;

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
    Q_PROPERTY(QString parentId READ parentId WRITE setParentId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "todoOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "todoOccurrences");

    explicit QDeclarativeOrganizerTodoOccurrence(QObject *parent = 0);
    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;

    void setDueDateTime(const QDateTime& dueDateTime);
    QDateTime dueDateTime() const;
    QString parentId() const;
    void setParentId(const QString& parentId);

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
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoOccurrence)
#endif // QDECLARATIVEORGANIZERITEM_H
