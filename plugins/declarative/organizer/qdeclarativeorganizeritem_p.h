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
    QDeclarativeOrganizerItemDetail* detail = property(detailType::DetailName).value<QDeclarativeOrganizerItemDetail*>(); \
    if (detail) \
        detail->setProperty(propertyName, propertyValue);

#define Q_DECLARATIVEORGANIZERITEMDETAIL_GET(detailType, propertyName, defaultValue) \
    QDeclarativeOrganizerItemDetail* detail = property(detailType::DetailName).value<QDeclarativeOrganizerItemDetail*>(); \
    if (detail) \
        return detail->property(propertyName); \
    return defaultValue;

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
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemType, "type", QOrganizerItemType::TypeEvent)
    }
    QString displayLabel() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemDisplayLabel, "label", QString())
    }

    void setDisplayLabel(const QString& label)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemDisplayLabel, "label", label)
    }

    QString description() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemDescription, "description", QString())
    }

    void setDescription(const QString& description)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemDescription, "description", description)
    }

    QString guid() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemGuid, "guid", QString())
    }
    void setGuid(const QString& guid)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemGuid, "guid", guid)
    }

signals:
    void valueChanged();
private:
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
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> recurrenceRules READ recurrenceRules NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> exceptionRules READ exceptionRules NOTIFY valueChanged)
    Q_PROPERTY(QVariantList recurrenceDates READ recurrenceDates WRITE setRecurrenceDates NOTIFY valueChanged)
    Q_PROPERTY(QVariantList exceptionDates  READ exceptionDates WRITE setExceptionDates NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "event");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "events");

    explicit QDeclarativeOrganizerEvent(QObject *parent = 0)
        :QDeclarativeOrganizerItem(parent)
    {

    }

    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "startDateTime", QDateTime())
    }

    void setEndDateTime(const QDateTime& endDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "endDateTime", endDateTime)
    }

    QDateTime endDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "endDateTime", QDateTime())
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", isTimeSpecified)
    }

    bool isTimeSpecified() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", false)
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> recurrenceRules()
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "recurrenceRules", QDeclarativeListProperty<QOrganizerItemRecurrenceRule>())
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> exceptionRules()
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "exceptionRules", QDeclarativeListProperty<QOrganizerItemRecurrenceRule>())
    }

    QVariantList recurrenceDates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "recurrenceDates", QVariantList())
    }

    void setRecurrenceDates(const QVariantList& dates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemRecurrence, "recurrenceDates", dates)
    }

    void setExceptionDates(const QVariantList& dates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemRecurrence, "exceptionDates", dates)
    }

    QVariantList exceptionDates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "exceptionDates", QVariantList())
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", priority)
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemPriority, "priority", QDeclarativeOrganizerItemPriority::MediumPriority)
    }

    QString locationName() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "locationName", QString())
    }
    void setLocationName(const QString& locationName)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "locationName", locationName)
    }

    QString locationAddress() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "address", QString())
    }
    void setLocationAddress(const QString& locationAddress)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "address", locationAddress)
    }

    QString locationGeoCoordinates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "geoLocation", QString())
    }

    void setLocationGeoCoordinates(const QString& locationCoordinates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET   (QDeclarativeOrganizerItemLocation, "geoLocation", locationCoordinates)
    }

signals:
    void valueChanged();

};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEvent::ItemName, "event");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEvent::ItemGroupName, "events");
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

    explicit QDeclarativeOrganizerEvent(QObject *parent = 0)
        :QDeclarativeOrganizerItem(parent)
    {

    }

    void setParentLocalId(uint parentId)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", parentId)
    }

    uint parentLocalId() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", 0)
    }
    void setOriginalDate(const QDate& date)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", date)
    }

    QDate originalDate() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", QDate())
    }

    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "startDateTime", QDateTime())
    }

    void setEndDateTime(const QDateTime& endDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "endDateTime", endDateTime)
    }

    QDateTime endDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "endDateTime", QDateTime())
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", isTimeSpecified)
    }

    bool isTimeSpecified() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerEventTimeRange, "isTimeSpecified", false)
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", priority)
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemPriority, "priority", QDeclarativeOrganizerItemPriority::MediumPriority)
    }

    QString locationName() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "locationName", QString())
    }
    void setLocationName(const QString& locationName)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "locationName", locationName)
    }

    QString locationAddress() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "address", QString())
    }
    void setLocationAddress(const QString& locationAddress)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemLocation, "address", locationAddress)
    }

    QString locationGeoCoordinates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemLocation, "geoLocation", QString())
    }

    void setLocationGeoCoordinates(const QString& locationCoordinates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET   (QDeclarativeOrganizerItemLocation, "geoLocation", locationCoordinates)
    }

signals:
    void valueChanged();
};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventOccurrence::ItemName, "eventOccurrence");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventOccurrence::ItemGroupName, "eventOccurrences");
QML_DECLARE_TYPE(QDeclarativeOrganizerEventOccurrence)

//journal
class QDeclarativeOrganizerJournal : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "journal");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "journals");
    explicit QDeclarativeOrganizerJournal(QObject *parent = 0)
        :QDeclarativeOrganizerItem(parent)
    {

    }
    void setDateTime(const QDateTime& dateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerJournalTimeRange, "entryDateTime", dateTime)
    }

    QDateTime dateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerJournalTimeRange, "entryDateTime", QDateTime())
    }
signals:
    void valueChanged();
};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerJournal::ItemName, "journal");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerJournal::ItemGroupName, "journals");
QML_DECLARE_TYPE(QDeclarativeOrganizerJournal)

//note
class QDeclarativeOrganizerNote : public QDeclarativeOrganizerItem
{
    Q_OBJECT
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "note");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "notes");

    explicit QDeclarativeOrganizerNote(QObject *parent = 0)
        :QDeclarativeOrganizerItem(parent)
    {
    }
signals:
    void valueChanged();
};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerNote::ItemName, "note");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerNote::ItemGroupName, "notes");
QML_DECLARE_TYPE(QDeclarativeOrganizerNote)

//todo
class QDeclarativeOrganizerTodo : public QDeclarativeOrganizerItem
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dueDateTime READ dueDateTime WRITE setDueDateTime NOTIFY valueChanged)
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> recurrenceRules READ recurrenceRules NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> exceptionRules READ exceptionRules NOTIFY valueChanged)
    Q_PROPERTY(QVariantList recurrenceDates READ recurrenceDates WRITE setRecurrenceDates NOTIFY valueChanged)
    Q_PROPERTY(QVariantList exceptionDates  READ exceptionDates WRITE setExceptionDates NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerItemPriority::Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_PROPERTY(int progressPercentage READ progressPercentage WRITE setProgressPercentage NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeOrganizerTodoProgress::Status status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)

public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "todo");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "todos");

    explicit QDeclarativeOrganizerTodo(QObject *parent = 0)
        :QDeclarativeOrganizerItem(parent)
    {
    }
    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QOrganizerTodoTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QOrganizerTodoTimeRange, "startDateTime", QDateTime())
    }

    void setDueDateTime(const QDateTime& dueDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QOrganizerTodoTimeRange, "dueDateTime", dueDateTime)
    }

    QDateTime dueDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QOrganizerTodoTimeRange, "dueDateTime", QDateTime())
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QOrganizerTodoTimeRange, "isTimeSpecified", isTimeSpecified)
    }

    bool isTimeSpecified() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QOrganizerTodoTimeRange, "isTimeSpecified", false)
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> recurrenceRules()
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "recurrenceRules", QDeclarativeListProperty<QOrganizerItemRecurrenceRule>())
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> exceptionRules()
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "exceptionRules", QDeclarativeListProperty<QOrganizerItemRecurrenceRule>())
    }

    QVariantList recurrenceDates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "recurrenceDates", QVariantList())
    }

    void setRecurrenceDates(const QVariantList& dates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemRecurrence, "recurrenceDates", dates)
    }

    void setExceptionDates(const QVariantList& dates)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemRecurrence, "exceptionDates", dates)
    }

    QVariantList exceptionDates() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemRecurrence, "exceptionDates", QVariantList())
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", priority)
    }

    void setProgressPercentage(int percentage)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "percentage", percentage)
    }

    int progressPercentage() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress, "percentage", 0)
    }

    void setStatus(QDeclarativeOrganizerTodoProgress::Status status)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "status", status)
    }

    QDeclarativeOrganizerTodoProgress::Status status() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "status", QDeclarativeOrganizerTodoProgress::StatusNotStarted)
    }

    void setFinishedDateTime(const QDateTime& finishedDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", finishedDateTime)
    }

    QDateTime finishedDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", QDateTime())
    }
signals:
    void valueChanged();
};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodo::ItemName, "todo");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodo::ItemGroupName, "todos");
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

    explicit QDeclarativeOrganizerTodoOccurrence(QObject *parent = 0)
        :QDeclarativeOrganizerItem(parent)
    {
    }
    void setStartDateTime(const QDateTime& startDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QOrganizerTodoTimeRange, "startDateTime", startDateTime)
    }

    QDateTime startDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QOrganizerTodoTimeRange, "startDateTime", QDateTime())
    }

    void setDueDateTime(const QDateTime& dueDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QOrganizerTodoTimeRange, "dueDateTime", dueDateTime)
    }

    QDateTime dueDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QOrganizerTodoTimeRange, "dueDateTime", QDateTime())
    }


    void setParentLocalId(uint parentId)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", parentId)
    }

    uint parentLocalId() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "parentLocalId", 0)
    }
    void setOriginalDate(const QDate& date)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", date)
    }

    QDate originalDate() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerItemInstanceOrigin, "originalDate", QDate())
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerItemPriority, "priority", priority)
    }

    void setProgressPercentage(int percentage)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "percentage", percentage)
    }

    int progressPercentage() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_GET(QDeclarativeOrganizerTodoProgress, "percentage", 0)
    }

    void setStatus(QDeclarativeOrganizerTodoProgress::Status status)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "status", status)
    }

    QDeclarativeOrganizerTodoProgress::Status status() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "status", QDeclarativeOrganizerTodoProgress::StatusNotStarted)
    }

    void setFinishedDateTime(const QDateTime& finishedDateTime)
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", finishedDateTime)
    }

    QDateTime finishedDateTime() const
    {
        Q_DECLARATIVEORGANIZERITEMDETAIL_SET(QDeclarativeOrganizerTodoProgress,  "finishedDateTime", QDateTime())
    }
signals:
    void valueChanged();
};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoOccurrence::ItemName, "todoOccurrence");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoOccurrence::ItemGroupName, "todoOccurrences");
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoOccurrence)
#endif // QDECLARATIVEORGANIZERITEM_H
