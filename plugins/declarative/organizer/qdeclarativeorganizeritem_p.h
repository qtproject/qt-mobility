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

#define Q_DECLARATIVEORGANIZERITEMDETAIL_SET(detailType, propertyName, propertyValue)  \
    QDeclarativeOrganizerItemDetail* detail = detailByDefinitionName(propertyName); \
    if (detail) \
        detail->setValue(propertyName, propertyValue);

#define Q_DECLARATIVEORGANIZERITEMDETAIL_GET(detailType, propertyName, returnValue) \
    QVariant returnValue; \
    QDeclarativeOrganizerItemDetail* detail = detailByDefinitionName(propertyName); \
    if (detail) \
        returnValue = detail->value(propertyName);



class QDeclarativeOrganizerItemMetaObject;
class QDeclarativeOrganizerItemDetail;
class QDeclarativeOrganizerItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> details READ details NOTIFY valueChanged);
    Q_PROPERTY (uint itemId READ itemId NOTIFY valueChanged)
    Q_PROPERTY (QString type READ type NOTIFY valueChanged)
    Q_PROPERTY (QString displayLabel READ displayLabel WRITE setDisplayLabel NOTIFY valueChanged)
    Q_PROPERTY (QString description READ description WRITE setDescription NOTIFY valueChanged)
    Q_PROPERTY (QString guid READ guid WRITE setGuid NOTIFY valueChanged)
    Q_CLASSINFO("DefaultProperty", "details")

public:
    explicit QDeclarativeOrganizerItem(QObject *parent = 0);
    explicit QDeclarativeOrganizerItem(const QOrganizerItem& item, const QMap<QString, QOrganizerItemDetailDefinition>& defs, QObject *parent = 0);
    ~QDeclarativeOrganizerItem();

    uint itemId() const;

    void setItem(const QOrganizerItem& c);
    QOrganizerItem item() const;

    void setDetailDefinitions(const QMap<QString, QOrganizerItemDetailDefinition>& defs);
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions() const;

    QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> details();

    Q_INVOKABLE QVariant detail(const QString& name);
    Q_INVOKABLE QVariant details(const QString& name);
    Q_INVOKABLE void addComment(const QString& comment);
    Q_INVOKABLE void clearComments();
    Q_INVOKABLE void clearDetails();

    QString type() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemType, "type", returnValue)
        if (returnValue.isNull())
            return QOrganizerItemType::TypeEvent;
        return returnValue.value<QString>();
    }
    QString displayLabel() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemDisplayLabel, "label", returnValue)
        return returnValue.value<QString>();
    }

    void setDisplayLabel(const QString& label)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemDisplayLabel, "label", label)
    }

    QString description() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemDescription, "description", returnValue)
        return returnValue.value<QString>();
    }

    void setDescription(const QString& description)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemDescription, "description", description)
    }

    QString guid() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemGuid, "guid", returnValue)
        return returnValue.value<QString>();
    }
    void setGuid(const QString& guid)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemGuid, "guid", guid)
    }

signals:
    void valueChanged();
protected:
    QDeclarativeOrganizerItemDetail* detailByDefinitionName(const QString& name) const;
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
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
    Q_PROPERTY(QString locationName READ locationName WRITE setLocationName NOTIFY valueChanged)
    Q_PROPERTY(QString locationAddress READ locationAddress WRITE setLocationAddress NOTIFY valueChanged)
    Q_PROPERTY(QString locationGeoCoordinates READ locationGeoCoordinates WRITE setLocationGeoCoordinates NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "event");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "events");

    explicit QDeclarativeOrganizerEvent(QObject *parent = 0);

    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "startDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setEndDateTime(const QDateTime& endDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "endDateTime", endDateTime)
    }

    QDateTime endDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "endDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", isTimeSpecified)
    }

    bool isTimeSpecified() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", returnValue)
        return returnValue.value<bool>();
    }

     void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", static_cast<int>(priority))
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemPriority, "priority", returnValue)
        if (returnValue.isNull())
            return QDeclarativeOrganizerItemPriority::MediumPriority;
        return static_cast<QDeclarativeOrganizerItemPriority::Priority>(returnValue.value<int>());
    }

    QString locationName() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "locationName", returnValue)
        return returnValue.value<QString>();
    }
    void setLocationName(const QString& locationName)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "locationName", locationName)
    }

    QString locationAddress() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "address", returnValue)
        return returnValue.value<QString>();
    }
    void setLocationAddress(const QString& locationAddress)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "address", locationAddress)
    }

    QString locationGeoCoordinates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "geoLocation", returnValue)
        return returnValue.value<QString>();
    }

    void setLocationGeoCoordinates(const QString& locationCoordinates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET   (QDeclarativeOrganizerItemLocation, "geoLocation", locationCoordinates)
    }

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
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
    Q_PROPERTY(QString locationName READ locationName WRITE setLocationName NOTIFY valueChanged)
    Q_PROPERTY(QString locationAddress READ locationAddress WRITE setLocationAddress NOTIFY valueChanged)
    Q_PROPERTY(QString locationGeoCoordinates READ locationGeoCoordinates WRITE setLocationGeoCoordinates NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(uint parentId READ parentLocalId WRITE setParentLocalId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "eventOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "eventOccurrences");

    explicit QDeclarativeOrganizerEventOccurrence(QObject *parent = 0);

    void setParentLocalId(uint parentId)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", parentId)
    }

    uint parentLocalId() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", returnValue)
        return returnValue.value<uint>();
    }
    void setOriginalDate(const QDate& date)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", date)
    }

    QDate originalDate() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", returnValue)
        return returnValue.value<QDate>();
    }

    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "startDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setEndDateTime(const QDateTime& endDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "endDateTime", endDateTime)
    }

    QDateTime endDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "endDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", isTimeSpecified)
    }

    bool isTimeSpecified() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", returnValue)
        return returnValue.value<bool>();
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", static_cast<int>(priority))
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemPriority, "priority", returnValue)
        if (returnValue.isNull())
            return QDeclarativeOrganizerItemPriority::MediumPriority;
        return static_cast<QDeclarativeOrganizerItemPriority::Priority>(returnValue.value<int>());
    }

    QString locationName() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "locationName", returnValue)
        return returnValue.value<QString>();
    }
    void setLocationName(const QString& locationName)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "locationName", locationName)
    }

    QString locationAddress() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "address", returnValue)
        return returnValue.value<QString>();
    }
    void setLocationAddress(const QString& locationAddress)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "address", locationAddress)
    }

    QString locationGeoCoordinates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "geoLocation", returnValue)
        return returnValue.value<QString>();
    }

    void setLocationGeoCoordinates(const QString& locationCoordinates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET   (QDeclarativeOrganizerItemLocation, "geoLocation", locationCoordinates)
    }

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
    void setDateTime(const QDateTime& dateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerJournalTimeRange, "entryDateTime", dateTime)
    }

    QDateTime dateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerJournalTimeRange, "entryDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }
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
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(int progressPercentage READ progressPercentage WRITE setProgressPercentage NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerTodoProgress::Status status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)

public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "todo");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "todos");

    explicit QDeclarativeOrganizerTodo(QObject *parent = 0);
    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoTimeRange, "startDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setDueDateTime(const QDateTime& dueDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoTimeRange, "dueDateTime", dueDateTime)
    }

    QDateTime dueDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoTimeRange, "dueDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoTimeRange, "isTimeSpecified", isTimeSpecified)
    }

    bool isTimeSpecified() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoTimeRange, "isTimeSpecified", returnValue)
        return returnValue.value<bool>();
    }

     void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", priority)
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemPriority, "priority", returnValue)
        if (returnValue.isNull())
            return QDeclarativeOrganizerItemPriority::MediumPriority;
        return static_cast<QDeclarativeOrganizerItemPriority::Priority>(returnValue.value<int>());
    }

    void setProgressPercentage(int percentage)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "percentage", percentage)
    }

    int progressPercentage() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress, "percentage", returnValue)
        return returnValue.value<int>();
    }

    void setStatus(QDeclarativeOrganizerTodoProgress::Status status)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "status", status)
    }

    QDeclarativeOrganizerTodoProgress::Status status() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress,  "status", returnValue)
        if (returnValue.isNull())
            return QDeclarativeOrganizerTodoProgress::StatusNotStarted;
        return static_cast<QDeclarativeOrganizerTodoProgress::Status>(returnValue.value<int>());
    }

    void setFinishedDateTime(const QDateTime& finishedDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", finishedDateTime)
    }

    QDateTime finishedDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }
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
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(int progressPercentage READ progressPercentage WRITE setProgressPercentage NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerTodoProgress::Status status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)
    Q_PROPERTY(uint parentId READ parentLocalId WRITE setParentLocalId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "todoOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "todoOccurrences");

    explicit QDeclarativeOrganizerTodoOccurrence(QObject *parent = 0);
    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoTimeRange, "startDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }

    void setDueDateTime(const QDateTime& dueDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoTimeRange, "dueDateTime", dueDateTime)
    }

    QDateTime dueDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoTimeRange, "dueDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }


    void setParentLocalId(uint parentId)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", parentId)
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", priority)
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemPriority, "priority", returnValue)
        if (returnValue.isNull())
            return QDeclarativeOrganizerItemPriority::MediumPriority;
        return static_cast<QDeclarativeOrganizerItemPriority::Priority>(returnValue.value<int>());
    }

    uint parentLocalId() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", returnValue)
        return returnValue.value<uint>();
    }
    void setOriginalDate(const QDate& date)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", date)
    }

    QDate originalDate() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", returnValue)
        return returnValue.value<QDate>();
    }

    void setProgressPercentage(int percentage)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "percentage", percentage)
    }

    int progressPercentage() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress, "percentage", returnValue)
        return returnValue.value<int>();
    }

    void setStatus(QDeclarativeOrganizerTodoProgress::Status status)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "status", status)
    }

    QDeclarativeOrganizerTodoProgress::Status status() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress,  "status", returnValue)
        if (returnValue.isNull())
            return QDeclarativeOrganizerTodoProgress::StatusNotStarted;
        return static_cast<QDeclarativeOrganizerTodoProgress::Status>(returnValue.value<int>());
    }

    void setFinishedDateTime(const QDateTime& finishedDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", finishedDateTime)
    }

    QDateTime finishedDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", returnValue)
        return returnValue.value<QDateTime>();
    }
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoOccurrence)
#endif // QDECLARATIVEORGANIZERITEM_H
