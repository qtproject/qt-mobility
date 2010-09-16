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
    Q_PROPERTY(QDateTime start READ startDateTime WRITE setStartDateTime NOTIFY valueChanged)
    Q_PROPERTY(QDateTime end READ endDateTime WRITE setEndDateTime NOTIFY valueChanged)
    Q_PROPERTY(bool timeSpecified READ isTimeSpecified WRITE setTimeSpecified NOTIFY valueChanged)
public:
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

//audible reminder detail
class QDeclarativeOrganizerItemAudibleReminder : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QUrl dataUrl READ dataUrl WRITE setDataUrl NOTIFY valueChanged)
public:
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

//comment detail
class QDeclarativeOrganizerItemComment : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY valueChanged)
public:
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

//description detail
class QDeclarativeOrganizerItemDescription : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY valueChanged)
public:
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

//display label detail
class QDeclarativeOrganizerItemDisplayLabel : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY valueChanged)
public:
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

//email reminder detail
class QDeclarativeOrganizerItemEmailReminder : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString subject READ subject WRITE setSubject NOTIFY valueChanged)
    Q_PROPERTY(QString body READ body WRITE setBody NOTIFY valueChanged)
    Q_PROPERTY(QStringList recipients READ recipients WRITE setRecipients NOTIFY valueChanged)
    Q_PROPERTY(QStringList attachments READ attachments WRITE setAttachments NOTIFY valueChanged)
public:
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

//guid detail
class QDeclarativeOrganizerItemGuid : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString guid READ guid WRITE setGuid NOTIFY valueChanged)
public:
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

//instance origin detail
class QDeclarativeQrganizerItemInstanceOrigin : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QOrganizerItemLocalId parentLocalId READ parentLocalId WRITE setParentLocalId NOTIFY valueChanged)
    Q_PROPERTY(QString originalDate READ originalDate WRITE setOriginalDate NOTIFY valueChanged)

public:
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

//location detail
class QDeclarativeOrganizerItemLocation : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString geoLocation READ geoLocation WRITE setGeoLocation NOTIFY valueChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY valueChanged)
    Q_PROPERTY(QString locationName READ locationName WRITE setLocationName NOTIFY valueChanged)

public:
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

//priority detail
class QDeclarativeOrganizerItemPriority : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QString geoLocation READ geoLocation WRITE setGeoLocation NOTIFY valueChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY valueChanged)
    Q_PROPERTY(QString locationName READ locationName WRITE setLocationName NOTIFY valueChanged)

public:
    QDeclarativeOrganizerItemPriority(QObject* parent)
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
QML_DECLARE_TYPE(QDeclarativeOrganizerItemPriority)

//recurrence detail
class QDeclarativeOrganizerItemRecurrence : public QDeclarativeOrganizerItemDetail
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> recurrenceRules READ recurrenceRules NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeOrganizerItemRecurrenceRule> exceptionRules READ exceptionRules NOTIFY valueChanged)
public:
    QDeclarativeOrganizerItemRecurrence(QObject* parent)
        :QDeclarativeOrganizerItemDetail(parent)
    {
    }

signals:
    void valueChanged();
};
QML_DECLARE_TYPE(QDeclarativeOrganizerItemRecurrence)

//reminder detail
//timestamp detail
//type detail
//visual reminder detail
//journal time range detail
//todo progress detail
//todo time range detail
#endif // QDECLARATIVEORGANIZERITEMDETAIL_H



