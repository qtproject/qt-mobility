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

#ifndef QDECLARATIVEORGANIZERITEMDETAIL_H
#define QDECLARATIVEORGANIZERITEMDETAIL_H

#include <QtDeclarative>

#include <QVariant>
#include "qorganizeritemdetail.h"
#include "qorganizeritemdetails.h"

QTM_USE_NAMESPACE;

class QDeclarativeOrganizerItemDetail : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString definitionName READ definitionName NOTIFY valueChanged)
    Q_PROPERTY(QStringList fields READ fields NOTIFY valueChanged)
    Q_PROPERTY(AccessConstraints accessConstraints READ accessConstraints NOTIFY valueChanged)

    Q_ENUMS(AccessConstraint)
    Q_FLAGS(AccessConstraints)

public:

    enum AccessConstraint {
        NoConstraint = QOrganizerItemDetail::NoConstraint,
        ReadOnly = QOrganizerItemDetail::ReadOnly,
        Irremovable = QOrganizerItemDetail::Irremovable
    };
    Q_DECLARE_FLAGS(AccessConstraints, AccessConstraint)

    explicit QDeclarativeOrganizerItemDetail(QObject* parent = 0);

    AccessConstraints accessConstraints() const;
    QString definitionName() const;

    QOrganizerItemDetail& detail();
    void setDetail(const QOrganizerItemDetail& detail);

    Q_INVOKABLE QStringList fields() const;
    Q_INVOKABLE QString value(const QString& key) const;
    Q_INVOKABLE bool setValue(const QString& key, const QVariant& value);
    Q_INVOKABLE bool removeValue(const QString& key);

signals:
    void valueChanged();

private:
    QOrganizerItemDetail m_detail;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetail)


//event time range detail
class QDeclarativeOrganizerEventTimeRange : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime NOTIFY valueChanged)
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "eventTimeRange");

    QDeclarativeOrganizerEventTimeRange(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setStartDateTime(const QDateTime& startDateTime) {detail().setValue(QOrganizerEventTimeRange::FieldStartDateTime, startDateTime);}
    QDateTime startDateTime() const {return detail().value<QDateTime>(QOrganizerEventTimeRange::FieldStartDateTime);}
    void setEndDateTime(const QDateTime& endDateTime) {detail().setValue(QOrganizerEventTimeRange::FieldEndDateTime, endDateTime);}
    QDateTime endDateTime() const {return detail().value<QDateTime>(QOrganizerEventTimeRange::FieldEndDateTime);}
    void setTimeSpecified(bool isTimeSpecified) {detail().setValue(QOrganizerEventTimeRange::FieldTimeSpecified, isTimeSpecified);}
    bool isTimeSpecified() {return detail().value<bool>(QOrganizerEventTimeRange::FieldTimeSpecified);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerEventTimeRange)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventTimeRange::DetailName, "eventTimeRange");

//comment detail
class QDeclarativeOrganizerItemComment : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "comment");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "comments");

    QDeclarativeOrganizerItemComment(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setComment(const QString& comment) {detail().setValue(QOrganizerItemComment::FieldComment, comment);}
    QString comment() const {return detail().value(QOrganizerItemComment::FieldComment);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemComment)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemComment::DetailName, "comment");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemComment::DetailGroupName, "comments");

//description detail
class QDeclarativeOrganizerItemDescription : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "description");

    QDeclarativeOrganizerItemDescription(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setDescription(const QString& description) {detail().setValue(QOrganizerItemDescription::FieldDescription, description);}
    QString description() const {return detail().value(QOrganizerItemDescription::FieldDescription);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDescription)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemDescription::DetailName, "description");

//display label detail
class QDeclarativeOrganizerItemDisplayLabel : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "label");
    QDeclarativeOrganizerItemDisplayLabel(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setLabel(const QString& label) {detail().setValue(QOrganizerItemDisplayLabel::FieldLabel, label);}
    QString label() const {return detail().value(QOrganizerItemDisplayLabel::FieldLabel);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDisplayLabel)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemDisplayLabel::DetailName, "label");

//guid detail
class QDeclarativeOrganizerItemGuid : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString guid READ guid WRITE setGuid NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "guid");
    QDeclarativeOrganizerItemGuid(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setGuid(const QString& guid) {detail().setValue(QOrganizerItemGuid::FieldGuid, guid);}
    QString guid() const {return detail().value(QOrganizerItemGuid::FieldGuid);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemGuid)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemGuid::DetailName, "guid");

//instance origin detail
class QDeclarativeQrganizerItemInstanceOrigin : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QOrganizerItemLocalId parentLocalId READ parentLocalId WRITE setParentLocalId NOTIFY valueChanged)
    Q_PROPERTY(QString originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)

public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "instanceOrigin");
    QDeclarativeQrganizerItemInstanceOrigin(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

    void setParentLocalId(QOrganizerItemLocalId parentId) {detail().setValue(QOrganizerItemInstanceOrigin::FieldParentLocalId, static_cast<int>(parentId));}
    QOrganizerItemLocalId parentLocalId() const {return static_cast<QOrganizerItemLocalId>(detail().variantValue(QOrganizerItemInstanceOrigin::FieldParentLocalId).toInt());}


    void setOriginalDate(const QDate& date) {detail().setValue(QOrganizerItemInstanceOrigin::FieldOriginalDate, date);}
    QDate originalDate() const {return detail().variantValue(QOrganizerItemInstanceOrigin::FieldOriginalDate).toDate();}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeQrganizerItemInstanceOrigin)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeQrganizerItemInstanceOrigin::DetailName, "instanceOrigin");

//location detail
class QDeclarativeOrganizerItemLocation : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString geoLocation READ geoLocation WRITE setGeoLocation NOTIFY valueChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY valueChanged)
    Q_PROPERTY(QString locationName READ locationName WRITE setLocationName NOTIFY valueChanged)

public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "location");
    QDeclarativeOrganizerItemLocation(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setGeoLocation(const QString& stringCoords) {detail().setValue(QOrganizerItemLocation::FieldGeoLocation, stringCoords);}
    QString geoLocation() const {return detail().value(QOrganizerItemLocation::FieldGeoLocation);}
    void setAddress(const QString& address) {detail().setValue(QOrganizerItemLocation::FieldAddress, address);}
    QString address() const {return detail().value(QOrganizerItemLocation::FieldAddress);}
    void setLocationName(const QString& locationName) {detail().setValue(QOrganizerItemLocation::FieldLocationName, locationName);}
    QString locationName() const {return detail().value(QOrganizerItemLocation::FieldLocationName);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemLocation)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemLocation::DetailName, "location");

//priority detail
class QDeclarativeOrganizerItemPriority : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_ENUMS(Priority)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "priority");
    enum Priority {
        UnknownPriority = QOrganizerItemPriority::UnknownPriority,
        HighestPriority = QOrganizerItemPriority::HighestPriority,
        ExtremelyHighPriority = QOrganizerItemPriority::ExtremelyHighPriority,
        VeryHighPriority = QOrganizerItemPriority::VeryHighPriority,
        HighPriority = QOrganizerItemPriority::HighestPriority,
        MediumPriority = QOrganizerItemPriority::MediumPriority,
        LowPriority = QOrganizerItemPriority::LowPriority,
        VeryLowPriority = QOrganizerItemPriority::VeryLowPriority,
        ExtremelyLowPriority = QOrganizerItemPriority::ExtremelyLowPriority,
        LowestPriority = QOrganizerItemPriority::LowestPriority
    };

    QDeclarativeOrganizerItemPriority(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setPriority(Priority priority) {detail().setValue(QOrganizerItemPriority::FieldPriority, static_cast<int>(priority));}
    Priority priority() const {return static_cast<Priority>(detail().value<int>(QOrganizerItemPriority::FieldPriority));}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemPriority)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemPriority::DetailName, "priority");

//recurrence detail
class QDeclarativeOrganizerItemRecurrence : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> recurrenceRules READ recurrenceRules NOTIFY recurrenceRulesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> exceptionRules READ exceptionRules NOTIFY exceptionRulesChanged)
    Q_PROPERTY(QVariantList recurrenceDates READ recurrenceDates WRITE setRecurrenceDates NOTIFY valueChanged)
    Q_PROPERTY(QVariantList exceptionDates  READ exceptionDates WRITE setExceptionDates NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "recurrence");
    QDeclarativeOrganizerItemRecurrence(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(recurrenceRulesChanged()), SLOT(_saveRecurrenceRules()));
        connect(this, SIGNAL(exceptionRulesChanged()), SLOT(_saveExceptionRules()));
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> recurrenceRules()
    {
        return QDeclarativeListProperty<QOrganizerItemRecurrenceRule>(this, m_recurrenceRules);
    }

    QDeclarativeListProperty<QOrganizerItemRecurrenceRule> exceptionRules()
    {
        return QDeclarativeListProperty<QOrganizerItemRecurrenceRule>(this, m_exceptionRules);
    }

    QVariantList recurrenceDates() const
    {
        return detail().variantValue(QOrganizerItemRecurrence::FieldRecurrenceRules).toList();
    }

    void setRecurrenceDates(const QVariantList& dates)
    {
        detail().setValue(QOrganizerItemRecurrence::FieldRecurrenceRules, saveList);
    }

    void setExceptionDates(const QVariantList& dates)
    {
        detail().setValue(QOrganizerItemRecurrence::FieldExceptionRules, saveList);
    }

    QVariantList exceptionDates() const
    {
        return detail().variantValue(QOrganizerItemRecurrence::FieldExceptionRules).toList();
    }

signals:
    void recurrenceRulesChanged();
    void exceptionRulesChanged();
    void valueChanged();

private slots:
    void _saveRecurrenceRules()
    {

        QVariantList saveList;
        for (int i=0; i < m_recurrenceRules.count(); i++) {
            saveList << m_recurrenceRules.at(i)->rule().variantValues();
        }
        detail().setValue(QOrganizerItemRecurrence::FieldRecurrenceRules, saveList);
        emit valueChanged();
    }

    void _saveExceptionRules()
    {
        QVariantList saveList;
        for (int i=0; i < m_exceptionRules.count(); i++) {
            saveList << m_exceptionRules.at(i)->rule().variantValues();
        }
        detail().setValue(QOrganizerItemRecurrence::FieldExceptionRules, saveList);
        emit valueChanged();
    }

private:
    QList<QDeclarativeOrganizerItemRecurrenceRule*>   m_recurrenceRules;
    QList<QDeclarativeOrganizerItemRecurrenceRule*>   m_exceptionRules;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemRecurrence)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemRecurrence::DetailName, "recurrence");

//reminder detail
class QDeclarativeOrganizerItemReminder : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(ReminderType reminderType READ reminderType NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime NOTIFY valueChanged)
    Q_PROPERTY(int timeDelta READ timeDelta WRITE setTimeDelta NOTIFY valueChanged)
    Q_PROPERTY(int repetitionCount READ repetitionCount WRITE setRepetitionCount NOTIFY valueChanged)
    Q_PROPERTY(int repetitionDelay READ repetitionDelay WRITE setRepetitionDelay NOTIFY valueChanged)
    Q_ENUMS(ReminderType)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "reminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "reminders");
    enum ReminderType {
        NoReminder = QOrganizerItemReminder::NoReminder,
        VisualReminder = QOrganizerItemReminder::VisualReminder,
        AudibleReminder = QOrganizerItemReminder::AudibleReminder,
        EmailReminder = QOrganizerItemReminder::EmailReminder
    };

    QDeclarativeOrganizerItemReminder(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

    ReminderType reminderType() const {return static_cast<ReminderType>(value<int>(QOrganizerItemReminder::FieldReminderType));}

    void setDateTime(const QDateTime& dateTime) {detail().setValue(QOrganizerItemReminder::FieldDateTime, dateTime);}
    QDateTime dateTime() const {return detail().value<QDateTime>(QOrganizerItemReminder::FieldDateTime);}
    void setTimeDelta(int secondsBefore) {detail().setValue(QOrganizerItemReminder::FieldTimeDelta, secondsBefore);}
    int timeDelta() const {return detail().value<int>(QOrganizerItemReminder::FieldTimeDelta);}

    void setRepetitionDelay(int delaySeconds) {detail().setValue(QOrganizerItemReminder::FieldRepetitionDelay, delaySeconds);}
    void setRepetitionCount(int count) {detail().setValue(QOrganizerItemReminder::FieldRepetitionCount, count); }
    int repetitionDelay() const {return detail().value<int>(QOrganizerItemReminder::FieldRepetitionDelay);}
    int repetitionCount() const {return detail().value<int>(QOrganizerItemReminder::FieldRepetitionCount);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemReminder)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemReminder::DetailName, "reminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemReminder::DetailGroupName, "reminders");

//audible reminder detail
class QDeclarativeOrganizerItemAudibleReminder : public QDeclarativeOrganizerItemReminder
{
    Q_OBJECT
    Q_PROPERTY(QUrl dataUrl READ dataUrl WRITE setDataUrl NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "audibleReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "audibleReminders");
    QDeclarativeOrganizerItemAudibleReminder(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setDataUrl(const QUrl& dataUrl) {detail().setValue(QOrganizerItemAudibleReminder::FieldDataUrl, dataUrl);}
    QUrl dataUrl() const {return detail().value<QUrl>(QOrganizerItemAudibleReminder::FieldDataUrl);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemAudibleReminder)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemAudibleReminder::DetailName, "audibleReminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemAudibleReminder::DetailGroupName, "audibleReminders");

//visual reminder detail
class QDeclarativeOrganizerItemVisualReminder : public QDeclarativeOrganizerItemReminder
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY valueChanged)
    Q_PROPERTY(QUrl dataUrl READ dataUrl WRITE setDataUrl NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "visualReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "visualReminders");
    QDeclarativeOrganizerItemVisualReminder(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

    // message and visual data to be displayed if visual notification.
    void setMessage(const QString& message) {detail().setValue(QOrganizerItemVisualReminder::FieldMessage, message);}
    QString message() const {return detail().value<QString>(QOrganizerItemVisualReminder::FieldMessage);}
    void setDataUrl(const QUrl& dataUrl) {detail().setValue(QOrganizerItemVisualReminder::FieldDataUrl, dataUrl);}
    QUrl dataUrl() const {return detail().value<QUrl>(QOrganizerItemVisualReminder::FieldDataUrl);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemVisualReminder)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemVisualReminder::DetailName, "visualReminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemVisualReminder::DetailGroupName, "visualReminders");

//email reminder detail
class QDeclarativeOrganizerItemEmailReminder : public QDeclarativeOrganizerItemReminder
{
    Q_OBJECT
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY valueChanged)
    Q_PROPERTY(QString body READ body WRITE setBody NOTIFY valueChanged)
    Q_PROPERTY(QStringList recipients READ recipients WRITE setRecipients NOTIFY valueChanged)
    Q_PROPERTY(QStringList attachments READ attachments WRITE setAttachments NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "emailReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "emailReminders");
    QDeclarativeOrganizerItemEmailReminder(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    QString subject() const {return detail().value(QOrganizerItemEmailReminder::FieldSubject);}
    void setSubject(const QString& subject) {detail().setValue(QOrganizerItemEmailReminder::FieldSubject, subject);}

    QString body() const {return detail().value(QOrganizerItemEmailReminder::FieldBody);}
    void setBody(const QString& body) {detail().setValue(QOrganizerItemEmailReminder::FieldBody, body);}

    QVariantList attachments() {return detail().value<QVariantList>(QOrganizerItemEmailReminder::FieldAttachments);}
    void setAttachments(const QVariantList& attachments) {detail().setValue(QOrganizerItemEmailReminder::FieldAttachments, attachments);}

    void setRecipients(const QStringList& recipients) {detail().setValue(QOrganizerItemEmailReminder::FieldRecipients, recipients);}
    QStringList recipients() const {return detail().value<QStringList>(QOrganizerItemEmailReminder::FieldRecipients);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemEmailReminder)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemEmailReminder::DetailName, "emailReminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemEmailReminder::DetailGroupName, "emailReminders");

//timestamp detail
class QDeclarativeOrganizerItemTimestamp : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified NOTIFY valueChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "timestamp");
    QDeclarativeOrganizerItemTimestamp(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setLastModified(const QDateTime& timestamp) {detail().setValue(QOrganizerItemTimestamp::FieldModificationTimestamp, timestamp);}
    QDateTime lastModified() const {return detail().value<QDateTime>(QOrganizerItemTimestamp::FieldModificationTimestamp);}
    void setCreated(const QDateTime& timestamp) {detail().setValue(QOrganizerItemTimestamp::FieldCreationTimestamp, timestamp);}
    QDateTime created() const {return detail().value<QDateTime>(QOrganizerItemTimestamp::FieldCreationTimestamp);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemTimestamp)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemTimestamp::DetailName, "timestamp");


//type detail
class QDeclarativeOrganizerItemType : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY valueChanged)
public:
    QDeclarativeOrganizerItemType(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setType(const QString& type) {detail().setValue(QOrganizerItemType::FieldType, type);}
    QString type() const {return detail().value(QOrganizerItemType::FieldType);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemType)

//journal time range detail
class QDeclarativeOrganizerJournalTimeRange : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime type READ entryDateTime WRITE setEntryDateTime NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "journalTimeRange");
    QDeclarativeOrganizerJournalTimeRange(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setEntryDateTime(const QDateTime& entryDateTime) {detail().setValue(QOrganizerJournalTimeRange::FieldEntryDateTime, entryDateTime);}
    QDateTime entryDateTime() const {return detail().value<QDateTime>(QOrganizerJournalTimeRange::FieldEntryDateTime);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerJournalTimeRange)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerJournalTimeRange::DetailName, "journalTimeRange");

//todo progress detail
class QDeclarativeOrganizerTodoProgress : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(int percentageComplete READ percentageComplete WRITE setPercentageComplete NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)
    Q_ENUMS(Status)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName,"todoProgress");

    QDeclarativeOrganizerTodoProgress(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setFinishedDateTime(const QDateTime& finishedDateTime) {detail().setValue(QOrganizerTodoProgress::FieldFinishedDateTime, finishedDateTime);}
    QDateTime finishedDateTime() const {return detail().value<QDateTime>(QOrganizerTodoProgress::FieldFinishedDateTime);}

    void setPercentageComplete(int percentage)
    {
        if (percentage >=0 && percentage <= 100)
            detail().setValue(QOrganizerTodoProgress::FieldPercentageComplete, percentage);
    }
    int percentageComplete() const {return detail().value<int>(QOrganizerTodoProgress::FieldPercentageComplete);}

    enum Status {
        StatusNotStarted = QOrganizerTodoProgress::StatusNotStarted,
        StatusInProgress = QOrganizerTodoProgress::StatusInProgress,
        StatusComplete = QOrganizerTodoProgress::StatusComplete
    };

    void setStatus(Status status) {detail().setValue(QOrganizerTodoProgress::FieldStatus, (int) status);}
    Status status() const {return (Status) detail().value<int>(QOrganizerTodoProgress::FieldStatus);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoProgress)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoProgress::DetailName, "todoProgress");

//todo time range detail
class QDeclarativeOrganizerTodoTimeRange : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dueDateTime READ dueDateTime WRITE setDueDateTime NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "todoTimeRange");
    QDeclarativeOrganizerTodoTimeRange(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setStartDateTime(const QDateTime& startDateTime) {detail().setValue(QOrganizerTodoTimeRange::FieldStartDateTime, startDateTime);}
    QDateTime startDateTime() const {return detail().value<QDateTime>(QOrganizerTodoTimeRange::FieldStartDateTime);}
    void setDueDateTime(const QDateTime& dueDateTime) {detail().setValue(QOrganizerTodoTimeRange::FieldDueDateTime, dueDateTime);}
    QDateTime dueDateTime() const {return detail().value<QDateTime>(QOrganizerTodoTimeRange::FieldDueDateTime);}
    void setTimeSpecified(bool isTimeSpecified) {detail().setValue(QOrganizerTodoTimeRange::FieldTimeSpecified, isTimeSpecified);}
    bool isTimeSpecified() {return detail().value<bool>(QOrganizerTodoTimeRange::FieldTimeSpecified);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoTimeRange)
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoTimeRange::DetailName, "todoTimeRange");
#endif // QDECLARATIVEORGANIZERITEMDETAIL_H



