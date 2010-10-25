/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEORGANIZERITEMDETAIL_H
#define QDECLARATIVEORGANIZERITEMDETAIL_H

#include <QtDeclarative>

#include <QVariant>
#include "qorganizeritemdetail.h"
#include "qorganizeritemdetails.h"
#include "qdeclarativeorganizerrecurrencerule_p.h"

QTM_USE_NAMESPACE;

class QDeclarativeOrganizerItemDetail : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString definitionName READ definitionName)
    Q_PROPERTY(QStringList fieldNames READ fieldNames)
    Q_PROPERTY(ItemDetailType type READ type)
    Q_PROPERTY(bool readOnly READ readOnly)
    Q_PROPERTY(bool removable READ removable)
    Q_ENUMS(ItemDetailType)
public:
    enum ItemDetailType {
        EventTime = 0,
        JournalTime,
        TodoTime,
        TodoProgress,
        Reminder,
        AudibleReminder,
        VisualReminder,
        EmailReminder,
        Comment,
        Description,
        DisplayLabel,
        Guid,
        Location,
        Parent,
        Priority,
        Recurrence,
        Timestamp,
        Type,
        Tag,
        Customized = 100
    };

    explicit QDeclarativeOrganizerItemDetail(QObject* parent = 0)
        :QObject(parent)
    {

    }

    bool readOnly() const
    {
        return m_detail.accessConstraints().testFlag(QOrganizerItemDetail::ReadOnly);
    }

    bool removable() const
    {
        return !m_detail.accessConstraints().testFlag(QOrganizerItemDetail::Irremovable);
    }

    virtual ItemDetailType type() const
    {
        return Customized;
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

    QStringList fieldNames() const
    {
        return m_detail.variantValues().keys();
    }
    Q_INVOKABLE QVariant value(const QString& key) const
    {
        return m_detail.variantValue(key);
    }

    Q_INVOKABLE bool setValue(const QString& key, const QVariant& value)
    {
        bool ok = m_detail.setValue(key, value);
        emit detailChanged();
        return ok;
    }

    Q_INVOKABLE bool removeValue(const QString& key)
    {
        bool ok = m_detail.removeValue(key);
        emit detailChanged();
        return ok;
    }

    static QString definitionName(ItemDetailType type) ;
    static ItemDetailType detailType(const QString& definitionName) ;
    static QString fieldName(ItemDetailType detailType, int fieldType);

signals:
    void detailChanged();

protected:
    QOrganizerItemDetail m_detail;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemDetail)


//event time range detail
class QDeclarativeOrganizerEventTime : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime endDateTime READ endDateTime WRITE setEndDateTime NOTIFY valueChanged)
    Q_PROPERTY(bool isAllDay READ isAllDay WRITE setAllDay NOTIFY valueChanged)
public:
    enum FieldType {
        StartDateTime = 0,
        EndDateTime,
        AllDay
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "eventTime");

    QDeclarativeOrganizerEventTime(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }

    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::EventTime;
    }

    void setStartDateTime(const QDateTime& datetime)
    {
         if (datetime != startDateTime() && !readOnly()) {
            m_detail.setValue(QOrganizerEventTime::FieldStartDateTime, datetime);
            emit valueChanged();
         }
    }
    QDateTime startDateTime() const {return m_detail.value<QDateTime>(QOrganizerEventTime::FieldStartDateTime);}
    void setEndDateTime(const QDateTime& datetime)
    {
        if (datetime != endDateTime() && !readOnly()) {
            m_detail.setValue(QOrganizerEventTime::FieldEndDateTime, datetime);
            emit valueChanged();
        }
    }
    QDateTime endDateTime() const {return m_detail.value<QDateTime>(QOrganizerEventTime::FieldEndDateTime);}
    void setAllDay(bool allDay)
    {
        if (allDay != isAllDay() && !readOnly()) {
            m_detail.setValue(QOrganizerEventTime::FieldAllDay, allDay);
            emit valueChanged();
        }
    }
    bool isAllDay() {return m_detail.value<bool>(QOrganizerEventTime::FieldAllDay);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerEventTime)

//comment detail
class QDeclarativeOrganizerItemComment : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY valueChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Comment = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "comment");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "comments");

    QDeclarativeOrganizerItemComment(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Comment;
    }

    void setComment(const QString& newComment)
    {
        if (newComment != comment() && !readOnly()) {
            m_detail.setValue(QOrganizerItemComment::FieldComment, newComment);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Description = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "description");

    QDeclarativeOrganizerItemDescription(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Description;
    }

    void setDescription(const QString& desc)
    {
        if (desc != description() && !readOnly()) {
            m_detail.setValue(QOrganizerItemDescription::FieldDescription, desc);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Label = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "label");
    QDeclarativeOrganizerItemDisplayLabel(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::DisplayLabel;
    }

    void setLabel(const QString& newLabel)
    {
        if (newLabel != label() && !readOnly()) {
            m_detail.setValue(QOrganizerItemDisplayLabel::FieldLabel, newLabel);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Guid = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "guid");
    QDeclarativeOrganizerItemGuid(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Guid;
    }

    void setGuid(const QString& newGuid)
    {
        if (newGuid != guid() && !readOnly()) {
            m_detail.setValue(QOrganizerItemGuid::FieldGuid, newGuid);
            emit valueChanged();
        }
    }
    QString guid() const {return m_detail.value(QOrganizerItemGuid::FieldGuid);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemGuid)

//parent detail
class QDeclarativeOrganizerItemParent : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(uint parentId READ parentId WRITE setParentId NOTIFY valueChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)

    Q_ENUMS(FieldType)
public:
    enum FieldType {
        ParentId = 0,
        OriginalDate
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "parent");
    QDeclarativeOrganizerItemParent(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }

    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Parent;
    }

    void setParentId(uint newParentId)
    {
        if (newParentId != parentId() && !readOnly()) {
            m_detail.setValue(QOrganizerItemParent::FieldParentId, newParentId);
            emit valueChanged();
        }
    }
    uint parentId() const
    {
        return m_detail.variantValue(QOrganizerItemParent::FieldParentId).toInt();
    }


    void setOriginalDate(const QDate& date)
    {
        if (date != originalDate() && !readOnly()) {
            m_detail.setValue(QOrganizerItemParent::FieldOriginalDate, date);
            emit valueChanged();
        }
    }
    QDate originalDate() const {return m_detail.variantValue(QOrganizerItemParent::FieldOriginalDate).toDate();}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemParent)

//location detail
class QDeclarativeOrganizerItemLocation : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude NOTIFY valueChanged)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude NOTIFY valueChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY valueChanged)

    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Latitude = 0,
        Longitude,
        Label
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "location");
    QDeclarativeOrganizerItemLocation(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Location;
    }
    void setLabel(const QString& newLabel)
    {
        if (newLabel != label() && !readOnly()) {
            m_detail.setValue(QOrganizerItemLocation::FieldLabel, newLabel);
            emit valueChanged();
        }
    }
    QString label() const {return m_detail.value(QOrganizerItemLocation::FieldLabel);}

    void setLatitude(double newLatitude)
    {
        if (qFuzzyCompare(newLatitude, latitude()) && !readOnly()) {
            m_detail.setValue(QOrganizerItemLocation::FieldLatitude, newLatitude);
            emit valueChanged();
        }
    }
    double latitude() const {return m_detail.value<double>(QOrganizerItemLocation::FieldLatitude);}
    void setLongitude(double newLongitude)
    {
        if (!qFuzzyCompare(newLongitude, longitude()) && !readOnly()) {
            m_detail.setValue(QOrganizerItemLocation::FieldLongitude, newLongitude);
            emit valueChanged();
        }
    }
    double longitude() const {return m_detail.value<double>(QOrganizerItemLocation::FieldLongitude);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemLocation)

//priority detail
class QDeclarativeOrganizerItemPriority : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(PriorityType priority READ priority WRITE setPriority NOTIFY valueChanged)
    Q_ENUMS(PriorityType)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Priority = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "priority");
    enum PriorityType {
        Unknown = QOrganizerItemPriority::UnknownPriority,
        Highest = QOrganizerItemPriority::HighestPriority,
        ExtremelyHigh = QOrganizerItemPriority::ExtremelyHighPriority,
        VeryHigh = QOrganizerItemPriority::VeryHighPriority,
        High = QOrganizerItemPriority::HighPriority,
        Medium = QOrganizerItemPriority::MediumPriority,
        Low = QOrganizerItemPriority::LowPriority,
        VeryLow = QOrganizerItemPriority::VeryLowPriority,
        ExtremelyLow = QOrganizerItemPriority::ExtremelyLowPriority,
        Lowest = QOrganizerItemPriority::LowestPriority
    };

    QDeclarativeOrganizerItemPriority(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Priority;
    }

    void setPriority(PriorityType newPriority)
    {
        if (newPriority != priority() && !readOnly()) {
            m_detail.setValue(QOrganizerItemPriority::FieldPriority, static_cast<int>(newPriority));
            emit valueChanged();
        }
    }
    PriorityType priority() const {return static_cast<PriorityType>(m_detail.value<int>(QOrganizerItemPriority::FieldPriority));}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemPriority)

//recurrence detail
class QDeclarativeOrganizerItemRecurrence : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerRecurrenceRule> recurrenceRules READ recurrenceRules NOTIFY recurrenceRulesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerRecurrenceRule> exceptionRules READ exceptionRules NOTIFY exceptionRulesChanged)
    Q_PROPERTY(QVariantList recurrenceDates READ recurrenceDates WRITE setRecurrenceDates NOTIFY valueChanged)
    Q_PROPERTY(QVariantList exceptionDates  READ exceptionDates WRITE setExceptionDates NOTIFY valueChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        RecurrenceRules = 0,
        ExceptionRules,
        RecurrenceDates,
        ExceptionDates
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "recurrence");
    QDeclarativeOrganizerItemRecurrence(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
        connect(this, SIGNAL(recurrenceRulesChanged()), SLOT(_saveRecurrenceRules()));
        connect(this, SIGNAL(exceptionRulesChanged()), SLOT(_saveExceptionRules()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Recurrence;
    }

    QDeclarativeListProperty<QDeclarativeOrganizerRecurrenceRule> recurrenceRules()
    {
        return QDeclarativeListProperty<QDeclarativeOrganizerRecurrenceRule>(this, m_recurrenceRules);
    }

    QDeclarativeListProperty<QDeclarativeOrganizerRecurrenceRule> exceptionRules()
    {
        return QDeclarativeListProperty<QDeclarativeOrganizerRecurrenceRule>(this, m_exceptionRules);
    }

    QVariantList recurrenceDates() const
    {
        return m_detail.variantValue(QOrganizerItemRecurrence::FieldRecurrenceRules).toList();
    }

    void setRecurrenceDates(const QVariantList& dates)
    {
        if (dates != recurrenceDates() && !readOnly()) {
            m_detail.setValue(QOrganizerItemRecurrence::FieldRecurrenceRules, dates);
            emit valueChanged();
        }
    }

    void setExceptionDates(const QVariantList& dates)
    {
        if (dates != exceptionDates() && !readOnly()) {
            m_detail.setValue(QOrganizerItemRecurrence::FieldExceptionRules, dates);
            emit valueChanged();
        }
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
        QSet<QOrganizerRecurrenceRule> rules;
        foreach (const QDeclarativeOrganizerRecurrenceRule* r, m_recurrenceRules) {
            rules << r->rule();
        }
        m_detail.setValue(QOrganizerItemRecurrence::FieldRecurrenceRules, QVariant::fromValue(rules));
        emit valueChanged();
    }

    void _saveExceptionRules()
    {
        QSet<QOrganizerRecurrenceRule> rules;
        foreach (const QDeclarativeOrganizerRecurrenceRule* r, m_exceptionRules) {
            rules << r->rule();
        }
        m_detail.setValue(QOrganizerItemRecurrence::FieldExceptionRules, QVariant::fromValue(rules));

        emit valueChanged();
    }

private:
    QList<QDeclarativeOrganizerRecurrenceRule*>   m_recurrenceRules;
    QList<QDeclarativeOrganizerRecurrenceRule*>   m_exceptionRules;
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemRecurrence)

//reminder detail
class QDeclarativeOrganizerItemReminder : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(ReminderType reminderType READ reminderType NOTIFY valueChanged)
    Q_PROPERTY(int secondsBeforeStart READ secondsBeforeStart WRITE setSecondsBeforeStart NOTIFY valueChanged)
    Q_PROPERTY(int repetitionCount READ repetitionCount WRITE setRepetitionCount NOTIFY valueChanged)
    Q_PROPERTY(int repetitionDelay READ repetitionDelay WRITE setRepetitionDelay NOTIFY valueChanged)
    Q_ENUMS(ReminderType)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Type = 0,
        SecondsBeforeStart,
        RepetitionCount,
        RepetitionDelay
    };
    static QString fieldNameFromFieldType(int type);

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
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }

    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Reminder;
    }

    ReminderType reminderType() const
    {
        if (m_detail.definitionName() == QOrganizerItemAudibleReminder::DefinitionName) {
            return QDeclarativeOrganizerItemReminder::AudibleReminder;
        } else if (m_detail.definitionName() == QOrganizerItemEmailReminder::DefinitionName) {
            return QDeclarativeOrganizerItemReminder::EmailReminder;
        } else if (m_detail.definitionName() == QOrganizerItemVisualReminder::DefinitionName) {
            return QDeclarativeOrganizerItemReminder::VisualReminder;
        }

        return QDeclarativeOrganizerItemReminder::NoReminder;
    }

    void setSecondsBeforeStart(int seconds)
    {
        if (seconds != secondsBeforeStart() && !readOnly()) {
            m_detail.setValue(QOrganizerItemReminder::FieldSecondsBeforeStart, seconds);
            emit valueChanged();
        }
    }
    int secondsBeforeStart() const {return m_detail.value<int>(QOrganizerItemReminder::FieldSecondsBeforeStart);}

    void setRepetitionDelay(int delaySeconds)
    {
        if (delaySeconds != repetitionDelay() && !readOnly()) {
            m_detail.setValue(QOrganizerItemReminder::FieldRepetitionDelay, delaySeconds);
            emit valueChanged();
        }
    }
    void setRepetitionCount(int count)
    {
        if (count != repetitionCount() && !readOnly()) {
            m_detail.setValue(QOrganizerItemReminder::FieldRepetitionCount, count);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        DataUrl = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "audibleReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "audibleReminders");
    QDeclarativeOrganizerItemAudibleReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemReminder(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::AudibleReminder;
    }
    void setDataUrl(const QUrl& url)
    {
        if (url != dataUrl() && !readOnly()) {
            m_detail.setValue(QOrganizerItemAudibleReminder::FieldDataUrl, url);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        DataUrl = 0,
        Message
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "visualReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "visualReminders");
    QDeclarativeOrganizerItemVisualReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemReminder(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }

    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::VisualReminder;
    }
    // message and visual data to be displayed if visual notification.
    void setMessage(const QString& msg)
    {
        if (msg != message() && !readOnly()) {
            m_detail.setValue(QOrganizerItemVisualReminder::FieldMessage, msg);
            emit valueChanged();
        }
    }
    QString message() const {return m_detail.value<QString>(QOrganizerItemVisualReminder::FieldMessage);}
    void setDataUrl(const QUrl& url)
    {
        if (url != dataUrl() && !readOnly()) {
            m_detail.setValue(QOrganizerItemVisualReminder::FieldDataUrl, url);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Subject = 0,
        Body,
        Recipients,
        Attachments
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "emailReminder");
    Q_DECLARE_LATIN1_CONSTANT(DetailGroupName, "emailReminders");
    QDeclarativeOrganizerItemEmailReminder(QObject* parent = 0)
        :QDeclarativeOrganizerItemReminder(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::EmailReminder;
    }
    QString subject() const {return m_detail.value(QOrganizerItemEmailReminder::FieldSubject);}
    void setSubject(const QString& newSubject)
    {
        if (newSubject != subject() && !readOnly()) {
            m_detail.setValue(QOrganizerItemEmailReminder::FieldSubject, newSubject);
            emit valueChanged();
        }
    }

    QString body() const {return m_detail.value(QOrganizerItemEmailReminder::FieldBody);}
    void setBody(const QString& newBody)
    {
        if (newBody != body() && !readOnly()) {
            m_detail.setValue(QOrganizerItemEmailReminder::FieldBody, newBody);
            emit valueChanged();
        }
    }

    QVariantList attachments() {return m_detail.value<QVariantList>(QOrganizerItemEmailReminder::FieldAttachments);}
    void setAttachments(const QVariantList& newAttachments)
    {
        if (newAttachments != attachments() && !readOnly()) {
            m_detail.setValue(QOrganizerItemEmailReminder::FieldAttachments, newAttachments);
            emit valueChanged();
        }
    }

    void setRecipients(const QStringList& newRecipients)
    {
        if (newRecipients != recipients() && !readOnly()) {
            m_detail.setValue(QOrganizerItemEmailReminder::FieldRecipients, newRecipients);
            emit valueChanged();
        }
    }
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
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        LastModified = 0,
        Created
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "timestamp");
    QDeclarativeOrganizerItemTimestamp(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Timestamp;
    }
    void setLastModified(const QDateTime& timestamp)
    {
        if (timestamp != lastModified() && !readOnly()) {
            m_detail.setValue(QOrganizerItemTimestamp::FieldModificationTimestamp, timestamp);
            emit valueChanged();
        }
    }
    QDateTime lastModified() const {return m_detail.value<QDateTime>(QOrganizerItemTimestamp::FieldModificationTimestamp);}
    void setCreated(const QDateTime& timestamp)
    {
        if (timestamp != created() && !readOnly()) {
            m_detail.setValue(QOrganizerItemTimestamp::FieldCreationTimestamp, timestamp);
            emit valueChanged();
        }
    }
    QDateTime created() const {return m_detail.value<QDateTime>(QOrganizerItemTimestamp::FieldCreationTimestamp);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemTimestamp)


//type detail
class QDeclarativeOrganizerItemType : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(OrganizerItemType itemType READ itemType WRITE setItemType NOTIFY valueChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        ItemType = 0
    };
    static QString fieldNameFromFieldType(int type);

    enum OrganizerItemType {
        Event = 0,
        EventOccurrence,
        Todo,
        TodoOccurrence,
        Note,
        Journal,
        Customized
    };

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "type");
    QDeclarativeOrganizerItemType(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::Type;
    }
    void setItemType(OrganizerItemType newType)
    {
        if (newType != itemType() && !readOnly()) {
            switch (newType) {
            case Event:
                m_detail.setValue(QOrganizerItemType::FieldType, QOrganizerItemType::TypeEvent);
                break;
            case EventOccurrence:
                m_detail.setValue(QOrganizerItemType::FieldType, QOrganizerItemType::TypeEventOccurrence);
                break;
            case Todo:
                m_detail.setValue(QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodo);
                break;
            case TodoOccurrence:
                m_detail.setValue(QOrganizerItemType::FieldType, QOrganizerItemType::TypeTodoOccurrence);
                break;
            case Note:
                m_detail.setValue(QOrganizerItemType::FieldType, QOrganizerItemType::TypeNote);
                break;
            case Journal:
                m_detail.setValue(QOrganizerItemType::FieldType, QOrganizerItemType::TypeJournal);
                break;
            default:
                qWarning() << "I don't known how to set the customized item type name here!";
                break;
            }
            emit valueChanged();
        }
    }
    OrganizerItemType itemType() const
    {
        QString typeString = m_detail.value(QOrganizerItemType::FieldType);
        if (typeString == QOrganizerItemType::TypeEvent)
            return Event;
        else if (typeString == QOrganizerItemType::TypeEventOccurrence)
            return EventOccurrence;
        else if (typeString == QOrganizerItemType::TypeTodo)
            return Todo;
        else if (typeString == QOrganizerItemType::TypeTodoOccurrence)
            return TodoOccurrence;
        else if (typeString == QOrganizerItemType::TypeNote)
            return Note;
        else if (typeString == QOrganizerItemType::TypeJournal)
            return Journal;
        qWarning() << "Unknown organizer item type: " << typeString;
        return Customized;
    }

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemType)

//journal time range detail
class QDeclarativeOrganizerJournalTime : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDateTime entryDateTime READ entryDateTime WRITE setEntryDateTime NOTIFY valueChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        EntryDateTime = 0
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "journalTime");
    QDeclarativeOrganizerJournalTime(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::JournalTime;
    }
    void setEntryDateTime(const QDateTime& datetime)
    {
        if (datetime != entryDateTime() && !readOnly()) {
            m_detail.setValue(QOrganizerJournalTime::FieldEntryDateTime, datetime);
            emit valueChanged();
        }
    }
    QDateTime entryDateTime() const {return m_detail.value<QDateTime>(QOrganizerJournalTime::FieldEntryDateTime);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerJournalTime)

//todo progress detail
class QDeclarativeOrganizerTodoProgress : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(StatusType status READ status WRITE setStatus NOTIFY valueChanged)
    Q_PROPERTY(int percentage READ percentageComplete WRITE setPercentageComplete NOTIFY valueChanged)
    Q_PROPERTY(QDateTime finishedDateTime READ finishedDateTime WRITE setFinishedDateTime NOTIFY valueChanged)
    Q_ENUMS(StatusType)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Status = 0,
        Percentage,
        FinishedDateTime
    };

    enum StatusType {
        NotStarted = QOrganizerTodoProgress::StatusNotStarted,
        InProgress = QOrganizerTodoProgress::StatusInProgress,
        Complete = QOrganizerTodoProgress::StatusComplete
    };
    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName,"todoProgress");

    QDeclarativeOrganizerTodoProgress(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::TodoProgress;
    }

    void setFinishedDateTime(const QDateTime& datetime)
    {
        if (datetime != finishedDateTime() && !readOnly()) {
            m_detail.setValue(QOrganizerTodoProgress::FieldFinishedDateTime, datetime);
            emit valueChanged();
        }
    }
    QDateTime finishedDateTime() const {return m_detail.value<QDateTime>(QOrganizerTodoProgress::FieldFinishedDateTime);}

    void setPercentageComplete(int percentage)
    {
        if (percentage != percentageComplete() && !readOnly()) {
            if (percentage >=0 && percentage <= 100) {
                m_detail.setValue(QOrganizerTodoProgress::FieldPercentageComplete, percentage);
                emit valueChanged();
            } else {
                qWarning() << "Trying to set an invalid percentage value:" << percentage;
            }
        }
    }
    int percentageComplete() const {return m_detail.value<int>(QOrganizerTodoProgress::FieldPercentageComplete);}



    void setStatus(StatusType newStatus)
    {
        if (newStatus != status() && !readOnly()) {
            m_detail.setValue(QOrganizerTodoProgress::FieldStatus, (int) newStatus);
            emit valueChanged();
        }
    }
    StatusType status() const {return (StatusType) m_detail.value<int>(QOrganizerTodoProgress::FieldStatus);}
signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoProgress)

//todo time range detail
class QDeclarativeOrganizerTodoTime : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(bool allDay READ isAllDay WRITE setAllDay NOTIFY valueChanged)
    Q_PROPERTY(QDateTime startDateTime READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime dueDateTime READ dueDateTime WRITE setDueDateTime NOTIFY valueChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        AllDay = 0,
        StartDateTime,
        DueDateTime
    };

    static QString fieldNameFromFieldType(int type);

    Q_DECLARE_LATIN1_CONSTANT(DetailName, "todoTime");
    QDeclarativeOrganizerTodoTime(QObject* parent = 0)
        :QDeclarativeOrganizerItemDetail(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(detailChanged()));
    }
    virtual ItemDetailType type() const
    {
        return QDeclarativeOrganizerItemDetail::TodoTime;
    }
    void setStartDateTime(const QDateTime& datetime)
    {
        if (datetime != startDateTime() && !readOnly()) {
            m_detail.setValue(QOrganizerTodoTime::FieldStartDateTime, datetime);
            emit valueChanged();
        }
    }
    QDateTime startDateTime() const {return m_detail.value<QDateTime>(QOrganizerTodoTime::FieldStartDateTime);}
    void setDueDateTime(const QDateTime& dateTime)
    {
        if (dateTime != dueDateTime() && !readOnly()) {
            m_detail.setValue(QOrganizerTodoTime::FieldDueDateTime, dateTime);
            emit valueChanged();
        }
    }
    QDateTime dueDateTime() const {return m_detail.value<QDateTime>(QOrganizerTodoTime::FieldDueDateTime);}

    void setAllDay(bool allDay)
    {
        if (allDay != isAllDay() && !readOnly()) {
            m_detail.setValue(QOrganizerTodoTime::FieldAllDay, allDay);
            emit valueChanged();
        }
    }
    bool isAllDay() {return m_detail.value<bool>(QOrganizerTodoTime::FieldAllDay);}

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerTodoTime)

#endif // QDECLARATIVEORGANIZERITEMDETAIL_H



