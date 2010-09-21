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

#include <qdeclarative.h>
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
    Q_PROPERTY (QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> details READ details NOTIFY detailsChanged);
    Q_PROPERTY (uint itemId READ itemId NOTIFY itemIdChanged)
    Q_PROPERTY (QString type READ type NOTIFY typeChanged)
    Q_PROPERTY (QString displayLabel READ displayLabel WRITE setDisplayLabel NOTIFY typeChanged)
    Q_PROPERTY (QString description READ description WRITE setDescription NOTIFY typeChanged)
    Q_PROPERTY (QStringList comments READ comments WRITE addComment NOTIFY typeChanged)
    Q_PROPERTY (QString guid READ guid WRITE setGuid NOTIFY typeChanged)
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

    QString type() const;
    QString displayLabel() const;

    QString description() const;
    void setDescription(const QString& description);

    QStringList comments() const;
    void addComment(const QString& comment);
    Q_INVOKABLE void clearComments();

    QString guid() const;
    void setGuid(const QString& guid);
    QDeclarativeOrganizerItemMetaObject* metaObject() const;
signals:
    void detailsChanged();
    void iItemIdChanged();
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
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> recurrenceRules READ recurrenceRules NOTIFY recurrenceRulesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> exceptionRules READ exceptionRules NOTIFY exceptionRulesChanged)
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
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerEventTimeRange::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("startDateTime", startDateTime);
    }

    QDateTime startDateTime() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerEventTimeRange::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("startDateTime");
        return QDateTime();
    }

    void setEndDateTime(const QDateTime& endDateTime)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerEventTimeRange::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("endDateTime", endDateTime);
    }

    QDateTime endDateTime() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerEventTimeRange::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("endDateTime");
        return QDateTime();
    }

    void setTimeSpecified(bool isTimeSpecified)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerEventTimeRange::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("isTimeSpecified", isTimeSpecified);
    }

    bool isTimeSpecified() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerEventTimeRange::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("isTimeSpecified");
        return false;
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> recurrenceRules()
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("recurrenceRules");

        return QDeclarativeListProperty<QOrganizerItemRecurrenceRule>();
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> exceptionRules()
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("exceptionRules");

        return QDeclarativeListProperty<QOrganizerItemRecurrenceRule>();
    }

    QVariantList recurrenceDates() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("recurrenceDates");
        return QVariantList();
    }

    void setRecurrenceDates(const QVariantList& dates)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("recurrenceDates", dates);
    }

    void setExceptionDates(const QVariantList& dates)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("exceptionDates", dates);
    }

    QVariantList exceptionDates() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemRecurrence::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("exceptionDates");
        return QVariantList();
    }

    void setPriority(QDeclarativeOrganizerItemPriority::Priority priority)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemPriority::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("priority", priority);
    }

    QDeclarativeOrganizerItemPriority::Priority priority() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemPriority::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("priority");
        return QDeclarativeOrganizerItemPriority::MediumPriority;
    }

    QString locationName() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("locationName");
        return QString();
    }
    void setLocationName(const QString& locationName)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
           detail->setProperty("locationName", locationName);
    }

    QString locationAddress() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("address");
        return QString();
    }
    void setLocationAddress(const QString& locationAddress)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("address", locationAddress);
    }

    QString locationGeoCoordinates() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("geoLocation");
        return QString();
    }

    void setLocationGeoCoordinates(const QString& locationCoordinates)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("geoLocation", locationCoordinates);
    }

signals:
    void detailsChanged();

};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEvent::ItemName, "event");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEvent::ItemGroupName, "events");

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
public:
    Q_DECLARE_LATIN1_CONSTANT(ItemName, "eventOccurrence");
    Q_DECLARE_LATIN1_CONSTANT(ItemGroupName, "eventOccurrences");

    void setParentLocalId(const QOrganizerItemLocalId& parentId);
    QOrganizerItemLocalId parentLocalId() const;
    void setOriginalDate(const QDate& date);
    QDate originalDate() const;
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
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("address");
        return QString();
    }
    void setLocationAddress(const QString& locationAddress)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("address", locationAddress);
    }

    QString locationGeoCoordinates() const
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            return detail->property("geoLocation");
        return QString();
    }

    void setLocationGeoCoordinates(const QString& locationCoordinates)
    {
        QDeclarativeOrganizerItemDetail* detail = property(QDeclarativeOrganizerItemLocation::DetailName).value<QDeclarativeOrganizerItemDetail*>();
        if (detail)
            detail->setProperty("geoLocation", locationCoordinates);
    }

signals:
    void detailsChanged();
};
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventOccurrence::ItemName, "eventOccurrence");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventOccurrence::ItemGroupName, "eventOccurrences");

//journal
class Q_ORGANIZER_EXPORT QOrganizerJournal : public QOrganizerItem
{
public:
    Q_DECLARE_CUSTOM_ORGANIZER_ITEM(QOrganizerJournal, QOrganizerItemType::TypeJournal)

    // XXX TODO: research whether journal is a single point in time, or can cover a period of time...
    void setDateTime(const QDateTime& dateTime);
    QDateTime dateTime() const;
};

//note
class Q_ORGANIZER_EXPORT QOrganizerNote : public QOrganizerItem
{
public:
    Q_DECLARE_CUSTOM_ORGANIZER_ITEM(QOrganizerNote, QOrganizerItemType::TypeNote)
};

//todo
class Q_ORGANIZER_EXPORT QOrganizerTodo : public QOrganizerItem
{
public:
    Q_DECLARE_CUSTOM_ORGANIZER_ITEM(QOrganizerTodo, QOrganizerItemType::TypeTodo)

    // XXX TODO: some questions about "recurring periods and doneness"...
    void setStartDateTime(const QDateTime& dueDateTime);
    QDateTime startDateTime() const;
    void setDueDateTime(const QDateTime& dueDateTime);
    QDateTime dueDateTime() const;

    void setTimeSpecified(bool isTimeSpecified);
    bool isTimeSpecified() const;

    void setRecurrenceDates(const QList<QDate>& rdates);
    QList<QDate> recurrenceDates() const;
    void setRecurrenceRules(const QList<QOrganizerItemRecurrenceRule>& rrules);
    QList<QOrganizerItemRecurrenceRule> recurrenceRules() const;
    void setExceptionDates(const QList<QDate>& exdates);
    QList<QDate> exceptionDates() const;
    void setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& exrules);
    QList<QOrganizerItemRecurrenceRule> exceptionRules() const;

    void setPriority(QOrganizerItemPriority::Priority);
    QOrganizerItemPriority::Priority priority() const;

    void setProgressPercentage(int percentage);
    int progressPercentage() const;
    void setStatus(QOrganizerTodoProgress::Status status);
    QOrganizerTodoProgress::Status status() const;
    void setFinishedDateTime(const QDateTime& finishedDateTime);
    QDateTime finishedDateTime() const;
};

//todo occurrence
class Q_ORGANIZER_EXPORT QOrganizerTodoOccurrence : public QOrganizerItem
{
public:
    Q_DECLARE_CUSTOM_ORGANIZER_ITEM(QOrganizerTodoOccurrence, QOrganizerItemType::TypeTodoOccurrence)

    // XXX TODO: see discussion in qorganizertodo.h
    void setStartDateTime(const QDateTime& startDateTime);
    QDateTime startDateTime() const;
    void setDueDateTime(const QDateTime& dueDateTime);
    QDateTime dueDateTime() const;

    void setParentLocalId(const QOrganizerItemLocalId& parentId);
    QOrganizerItemLocalId parentLocalId() const;
    void setOriginalDate(const QDate& date);
    QDate originalDate() const;

    void setPriority(QOrganizerItemPriority::Priority);
    QOrganizerItemPriority::Priority priority() const;

    void setProgressPercentage(int percentage);
    int progressPercentage() const;
    void setStatus(QOrganizerTodoProgress::Status status);
    QOrganizerTodoProgress::Status status() const;
    void setFinishedDateTime(const QDateTime& finishedDateTime);
    QDateTime finishedDateTime() const;
};

QML_DECLARE_TYPE(QDeclarativeOrganizerEvent)
#endif // QDECLARATIVEORGANIZERITEM_H
