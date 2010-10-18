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
#include "qdeclarativeorganizeritemrecurrencerule_p.h"

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

    explicit QDeclarativeOrganizerItemDetail(QObject* parent = 0)
        :QObject(parent)
    {

    }

    AccessConstraints accessConstraints() const
    {
        AccessConstraints acs;
        acs &= 0xFFFFFFFF;
        acs &= m_detail.accessConstraints();
        return acs;
    }

    QString definitionName() const
    {
        return m_detail.definitionName();
    }

    QOrganizerItemDetail detail()
    {
        return m_detail;
    }

    void setDetail(const QOrganizerItemDetail& detail)
    {
        m_detail = detail;
    }

    Q_INVOKABLE QStringList fields() const
    {
        return m_detail.variantValues().keys();
    }
    Q_INVOKABLE QVariant value(const QString& key) const
    {
        return m_detail.variantValue(key);
    }

    Q_INVOKABLE bool setValue(const QString& key, const QVariant& value)
    {
        return m_detail.setValue(key, value);
    }

    Q_INVOKABLE bool removeValue(const QString& key)
    {
        return m_detail.removeValue(key);
    }

signals:
    void valueChanged();

protected:
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

    QDeclarativeOrganizerEventTimeRange(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setStartDateTime(const QDateTime& startDateTime) {m_detail.setValue(QOrganizerEventTimeRange::FieldStartDateTime, startDateTime);}
    QDateTime startDateTime() const {return m_detail.value<QDateTime>(QOrganizerEventTimeRange::FieldStartDateTime);}
    void setEndDateTime(const QDateTime& endDateTime) {m_detail.setValue(QOrganizerEventTimeRange::FieldEndDateTime, endDateTime);}
    QDateTime endDateTime() const {return m_detail.value<QDateTime>(QOrganizerEventTimeRange::FieldEndDateTime);}
    void setTimeSpecified(bool isTimeSpecified) {m_detail.setValue(QOrganizerEventTimeRange::FieldTimeSpecified, isTimeSpecified);}
    bool isTimeSpecified() {return m_detail.value<bool>(QOrganizerEventTimeRange::FieldTimeSpecified);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerEventTimeRange)

//comment detail
class QDeclarativeOrganizerItemComment : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "comment");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "comments");

    QDeclarativeOrganizerItemComment(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setComment(const QString& comment) {m_detail.setValue(QOrganizerItemComment::FieldComment, comment);}
    QString comment() const {return m_detail.value(QOrganizerItemComment::FieldComment);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemComment)

//description detail
class QDeclarativeOrganizerItemDescription : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "description");

    QDeclarativeOrganizerItemDescription(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setDescription(const QString& description) {m_detail.setValue(QOrganizerItemDescription::FieldDescription, description);}
    QString description() const {return m_detail.value(QOrganizerItemDescription::FieldDescription);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDescription)

//display label detail
class QDeclarativeOrganizerItemDisplayLabel : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "label");
    QDeclarativeOrganizerItemDisplayLabel(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setLabel(const QString& label) {m_detail.setValue(QOrganizerItemDisplayLabel::FieldLabel, label);}
    QString label() const {return m_detail.value(QOrganizerItemDisplayLabel::FieldLabel);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDisplayLabel)

//guid detail
class QDeclarativeOrganizerItemGuid : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString guid READ guid WRITE setGuid NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "guid");
    QDeclarativeOrganizerItemGuid(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setGuid(const QString& guid) {m_detail.setValue(QOrganizerItemGuid::FieldGuid, guid);}
    QString guid() const {return m_detail.value(QOrganizerItemGuid::FieldGuid);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemGuid)

//instance origin detail
class QDeclarativeOrganizerItemInstanceOrigin : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(uint parentLocalId READ parentLocalId WRITE setParentLocalId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)

public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "instanceOrigin");
    QDeclarativeOrganizerItemInstanceOrigin(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

    void setParentLocalId(uint parentId)
    {
        m_detail.setValue(QOrganizerItemInstanceOrigin::FieldParentLocalId, parentId);
    }
    uint parentLocalId() const
    {
        return m_detail.variantValue(QOrganizerItemInstanceOrigin::FieldParentLocalId).toInt();
    }


    void setOriginalDate(const QDate& date) {m_detail.setValue(QOrganizerItemInstanceOrigin::FieldOriginalDate, date);}
    QDate originalDate() const {return m_detail.variantValue(QOrganizerItemInstanceOrigin::FieldOriginalDate).toDate();}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemInstanceOrigin)

//location detail
class QDeclarativeOrganizerItemLocation : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString geoLocation READ geoLocation WRITE setGeoLocation NOTIFY valueChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY valueChanged)
    Q_PROPERTY(QString locationName READ locationName WRITE setLocationName NOTIFY valueChanged)

public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "location");
    QDeclarativeOrganizerItemLocation(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setGeoLocation(const QString& stringCoords) {m_detail.setValue(QOrganizerItemLocation::FieldGeoLocation, stringCoords);}
    QString geoLocation() const {return m_detail.value(QOrganizerItemLocation::FieldGeoLocation);}
    void setAddress(const QString& address) {m_detail.setValue(QOrganizerItemLocation::FieldAddress, address);}
    QString address() const {return m_detail.value(QOrganizerItemLocation::FieldAddress);}
    void setLocationName(const QString& locationName) {m_detail.setValue(QOrganizerItemLocation::FieldLocationName, locationName);}
    QString locationName() const {return m_detail.value(QOrganizerItemLocation::FieldLocationName);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemLocation)

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
        HighPriority = QOrganizerItemPriority::HighPriority,
        MediumPriority = QOrganizerItemPriority::MediumPriority,
        LowPriority = QOrganizerItemPriority::LowPriority,
        VeryLowPriority = QOrganizerItemPriority::VeryLowPriority,
        ExtremelyLowPriority = QOrganizerItemPriority::ExtremelyLowPriority,
        LowestPriority = QOrganizerItemPriority::LowestPriority
    };

    QDeclarativeOrganizerItemPriority(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setPriority(Priority priority) {m_detail.setValue(QOrganizerItemPriority::FieldPriority, static_cast<int>(priority));}
    Priority priority() const {return static_cast<Priority>(m_detail.value<int>(QOrganizerItemPriority::FieldPriority));}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemPriority)

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
    QDeclarativeOrganizerItemRecurrence(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(recurrenceRulesChanged()), SLOT(_saveRecurrenceRules()));
        connect(this, SIGNAL(exceptionRulesChanged()), SLOT(_saveExceptionRules()));
    }

    QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> recurrenceRules()
    {
        return QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule>(this, m_recurrenceRules);
    }

    QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> exceptionRules()
    {
        return QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule>(this, m_exceptionRules);
    }

    QVariantList recurrenceDates() const
    {
        return m_detail.variantValue(QOrganizerItemRecurrence::FieldRecurrenceRules).toList();
    }

    void setRecurrenceDates(const QVariantList& dates)
    {
        m_detail.setValue(QOrganizerItemRecurrence::FieldRecurrenceRules, dates);
    }

    void setExceptionDates(const QVariantList& dates)
    {
        m_detail.setValue(QOrganizerItemRecurrence::FieldExceptionRules, dates);
    }

    QVariantList exceptionDates() const
    {
        return m_detail.variantValue(QOrganizerItemRecurrence::FieldExceptionRules).toList();
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
        m_detail.setValue(QOrganizerItemRecurrence::FieldRecurrenceRules, saveList);
        emit valueChanged();
    }

    void _saveExceptionRules()
    {
        QVariantList saveList;
        for (int i=0; i < m_exceptionRules.count(); i++) {
            saveList << m_exceptionRules.at(i)->rule().variantValues();
        }
        m_detail.setValue(QOrganizerItemRecurrence::FieldExceptionRules, saveList);
        emit valueChanged();
    }

private:
    QList<QDeclarativeOrganizerItemRecurrenceRule*>   m_recurrenceRules;
    QList<QDeclarativeOrganizerItemRecurrenceRule*>   m_exceptionRules;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemRecurrence)

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

    QDeclarativeOrganizerItemReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

    ReminderType reminderType() const
    {
        return  static_cast<ReminderType>(m_detail.value<int>(QOrganizerItemReminder::FieldReminderType));
    }

    void setDateTime(const QDateTime& dateTime) {m_detail.setValue(QOrganizerItemReminder::FieldDateTime, dateTime);}
    QDateTime dateTime() const {return m_detail.value<QDateTime>(QOrganizerItemReminder::FieldDateTime);}
    void setTimeDelta(int secondsBefore) {m_detail.setValue(QOrganizerItemReminder::FieldTimeDelta, secondsBefore);}
    int timeDelta() const {return m_detail.value<int>(QOrganizerItemReminder::FieldTimeDelta);}

    void setRepetitionDelay(int delaySeconds) {m_detail.setValue(QOrganizerItemReminder::FieldRepetitionDelay, delaySeconds);}
    void setRepetitionCount(int count) {m_detail.setValue(QOrganizerItemReminder::FieldRepetitionCount, count); }
    int repetitionDelay() const {return m_detail.value<int>(QOrganizerItemReminder::FieldRepetitionDelay);}
    int repetitionCount() const {return m_detail.value<int>(QOrganizerItemReminder::FieldRepetitionCount);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemReminder)

//audible reminder detail
class QDeclarativeOrganizerItemAudibleReminder : public QDeclarativeOrganizerItemReminder
{
    Q_OBJECT
    Q_PROPERTY(QUrl dataUrl READ dataUrl WRITE setDataUrl NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "audibleReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "audibleReminders");
    QDeclarativeOrganizerItemAudibleReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemReminder(parent)
    {
    }
    void setDataUrl(const QUrl& dataUrl) {m_detail.setValue(QOrganizerItemAudibleReminder::FieldDataUrl, dataUrl);}
    QUrl dataUrl() const {return m_detail.value<QUrl>(QOrganizerItemAudibleReminder::FieldDataUrl);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemAudibleReminder)

//visual reminder detail
class QDeclarativeOrganizerItemVisualReminder : public QDeclarativeOrganizerItemReminder
{
    Q_OBJECT
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY valueChanged)
    Q_PROPERTY(QUrl dataUrl READ dataUrl WRITE setDataUrl NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "visualReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "visualReminders");
    QDeclarativeOrganizerItemVisualReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemReminder(parent)
    {
    }

    // message and visual data to be displayed if visual notification.
    void setMessage(const QString& message) {m_detail.setValue(QOrganizerItemVisualReminder::FieldMessage, message);}
    QString message() const {return m_detail.value<QString>(QOrganizerItemVisualReminder::FieldMessage);}
    void setDataUrl(const QUrl& dataUrl) {m_detail.setValue(QOrganizerItemVisualReminder::FieldDataUrl, dataUrl);}
    QUrl dataUrl() const {return m_detail.value<QUrl>(QOrganizerItemVisualReminder::FieldDataUrl);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemVisualReminder)

//email reminder detail
class QDeclarativeOrganizerItemEmailReminder : public QDeclarativeOrganizerItemReminder
{
    Q_OBJECT
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY valueChanged)
    Q_PROPERTY(QString body READ body WRITE setBody NOTIFY valueChanged)
    Q_PROPERTY(QStringList recipients READ recipients WRITE setRecipients NOTIFY valueChanged)
    Q_PROPERTY(QVariantList attachments READ attachments WRITE setAttachments NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "emailReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "emailReminders");
    QDeclarativeOrganizerItemEmailReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemReminder(parent)
    {
    }
    QString subject() const {return m_detail.value(QOrganizerItemEmailReminder::FieldSubject);}
    void setSubject(const QString& subject) {m_detail.setValue(QOrganizerItemEmailReminder::FieldSubject, subject);}

    QString body() const {return m_detail.value(QOrganizerItemEmailReminder::FieldBody);}
    void setBody(const QString& body) {m_detail.setValue(QOrganizerItemEmailReminder::FieldBody, body);}

    QVariantList attachments() {return m_detail.value<QVariantList>(QOrganizerItemEmailReminder::FieldAttachments);}
    void setAttachments(const QVariantList& attachments) {m_detail.setValue(QOrganizerItemEmailReminder::FieldAttachments, attachments);}

    void setRecipients(const QStringList& recipients) {m_detail.setValue(QOrganizerItemEmailReminder::FieldRecipients, recipients);}
    QStringList recipients() const {return m_detail.value<QStringList>(QOrganizerItemEmailReminder::FieldRecipients);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemEmailReminder)

//timestamp detail
class QDeclarativeOrganizerItemTimestamp : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime lastModified READ lastModified WRITE setLastModified NOTIFY valueChanged)
    Q_PROPERTY(QDateTime created READ created WRITE setCreated NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "timestamp");
    QDeclarativeOrganizerItemTimestamp(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setLastModified(const QDateTime& timestamp) {m_detail.setValue(QOrganizerItemTimestamp::FieldModificationTimestamp, timestamp);}
    QDateTime lastModified() const {return m_detail.value<QDateTime>(QOrganizerItemTimestamp::FieldModificationTimestamp);}
    void setCreated(const QDateTime& timestamp) {m_detail.setValue(QOrganizerItemTimestamp::FieldCreationTimestamp, timestamp);}
    QDateTime created() const {return m_detail.value<QDateTime>(QOrganizerItemTimestamp::FieldCreationTimestamp);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemTimestamp)


//type detail
class QDeclarativeOrganizerItemType : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "type");
    QDeclarativeOrganizerItemType(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setType(const QString& type) {m_detail.setValue(QOrganizerItemType::FieldType, type);}
    QString type() const {return m_detail.value(QOrganizerItemType::FieldType);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemType)

//journal time range detail
class QDeclarativeOrganizerJournalTimeRange : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime entryDateTime READ entryDateTime WRITE setEntryDateTime NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "journalTimeRange");
    QDeclarativeOrganizerJournalTimeRange(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setEntryDateTime(const QDateTime& entryDateTime) {m_detail.setValue(QOrganizerJournalTimeRange::FieldEntryDateTime, entryDateTime);}
    QDateTime entryDateTime() const {return m_detail.value<QDateTime>(QOrganizerJournalTimeRange::FieldEntryDateTime);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerJournalTimeRange)

//todo progress detail
class QDeclarativeOrganizerTodoProgress : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(int percentage READ percentageComplete WRITE setPercentageComplete NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)
    Q_ENUMS(Status)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName,"todoProgress");

    QDeclarativeOrganizerTodoProgress(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setFinishedDateTime(const QDateTime& finishedDateTime) {m_detail.setValue(QOrganizerTodoProgress::FieldFinishedDateTime, finishedDateTime);}
    QDateTime finishedDateTime() const {return m_detail.value<QDateTime>(QOrganizerTodoProgress::FieldFinishedDateTime);}

    void setPercentageComplete(int percentage)
    {
        if (percentage >=0 && percentage <= 100)
            m_detail.setValue(QOrganizerTodoProgress::FieldPercentageComplete, percentage);
    }
    int percentageComplete() const {return m_detail.value<int>(QOrganizerTodoProgress::FieldPercentageComplete);}

    enum Status {
        StatusNotStarted = QOrganizerTodoProgress::StatusNotStarted,
        StatusInProgress = QOrganizerTodoProgress::StatusInProgress,
        StatusComplete = QOrganizerTodoProgress::StatusComplete
    };

    void setStatus(Status status) {m_detail.setValue(QOrganizerTodoProgress::FieldStatus, (int) status);}
    Status status() const {return (Status) m_detail.value<int>(QOrganizerTodoProgress::FieldStatus);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoProgress)

//todo time range detail
class QDeclarativeOrganizerTodoTimeRange : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(bool isTimeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dueDateTime READ dueDateTime WRITE setDueDateTime NOTIFY valueChanged)
public:
    Q_DECLARE_LATIN1_CONSTANT(DetailName, "todoTimeRange");
    QDeclarativeOrganizerTodoTimeRange(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    void setStartDateTime(const QDateTime& startDateTime) {m_detail.setValue(QOrganizerTodoTimeRange::FieldStartDateTime, startDateTime);}
    QDateTime startDateTime() const {return m_detail.value<QDateTime>(QOrganizerTodoTimeRange::FieldStartDateTime);}
    void setDueDateTime(const QDateTime& dueDateTime) {m_detail.setValue(QOrganizerTodoTimeRange::FieldDueDateTime, dueDateTime);}
    QDateTime dueDateTime() const {return m_detail.value<QDateTime>(QOrganizerTodoTimeRange::FieldDueDateTime);}
    void setTimeSpecified(bool isTimeSpecified) {m_detail.setValue(QOrganizerTodoTimeRange::FieldTimeSpecified, isTimeSpecified);}
    bool isTimeSpecified() {return m_detail.value<bool>(QOrganizerTodoTimeRange::FieldTimeSpecified);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoTimeRange)

#endif // QDECLARATIVEORGANIZERITEMDETAIL_H



