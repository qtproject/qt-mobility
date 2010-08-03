/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
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


#ifndef QORGANIZERITEMREMINDER_H
#define QORGANIZERITEMREMINDER_H

#include <QString>
#include <QUrl>
#include <QList>
#include <QVariantList>

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritem.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerItemReminder : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldReminderTypes;
    const char* FieldDateTime;
    const char* FieldTimeDelta;
    const char* FieldRepetitionCount;
    const char* FieldRepetitionDelay;
    const char* FieldVisualMessage;
    const char* FieldVisualDataUrl;
    const char* FieldVisualData;
    const char* FieldAudibleDataUrl;
    const char* FieldAudibleData;
    const char* FieldTactileDataUrl;
    const char* FieldTactileData;
    const char* FieldEmailSubject;
    const char* FieldEmailBody;
    const char* FieldEmailAttachments;
    const char* FieldEmailRecipients;
    const char* FieldProcedureUrl;
    const char* FieldProcedureArguments;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerItemReminder, "Reminder")
    Q_DECLARE_LATIN1_CONSTANT(FieldReminderTypes, "ReminderTypes");
    Q_DECLARE_LATIN1_CONSTANT(FieldDateTime, "DateTime");
    Q_DECLARE_LATIN1_CONSTANT(FieldTimeDelta, "TimeDelta");
    Q_DECLARE_LATIN1_CONSTANT(FieldRepetitionCount, "RepetitionCount");
    Q_DECLARE_LATIN1_CONSTANT(FieldRepetitionDelay, "RepetitionDelay");
    Q_DECLARE_LATIN1_CONSTANT(FieldVisualMessage, "VisualMessage");
    Q_DECLARE_LATIN1_CONSTANT(FieldVisualDataUrl, "VisualDataUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldVisualData, "VisualData");
    Q_DECLARE_LATIN1_CONSTANT(FieldAudibleDataUrl, "AudibleDataUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldAudibleData, "AudibleData");
    Q_DECLARE_LATIN1_CONSTANT(FieldTactileDataUrl, "TactileDataUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldTactileData, "TactileData");
    Q_DECLARE_LATIN1_CONSTANT(FieldEmailSubject, "EmailSubject");
    Q_DECLARE_LATIN1_CONSTANT(FieldEmailBody, "EmailBody");
    Q_DECLARE_LATIN1_CONSTANT(FieldEmailAttachments, "EmailAttachments");
    Q_DECLARE_LATIN1_CONSTANT(FieldEmailRecipients, "EmailRecipients");
    Q_DECLARE_LATIN1_CONSTANT(FieldProcedureUrl, "ProcedureUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldProcedureArguments, "ProcedureArguments");
#endif

    // XXX TODO: do we need a reminder priority?
    // highest priority = will cancel phone calls to remind you, lowest = only if the device is idle?

    enum ReminderType {
        NoReminder = 0,
        VisualReminder = 0x01,
        AudibleReminder = 0x02,
        TactileReminder = 0x04,
        EmailReminder = 0x08,
        ProcedureReminder = 0x10
    };
    Q_DECLARE_FLAGS(ReminderTypes, ReminderType)

    void setReminderTypes(ReminderTypes reminderTypes) {setValue(FieldReminderTypes, static_cast<int>(reminderTypes));}
    ReminderTypes reminderTypes() const {return static_cast<ReminderTypes>(value<int>(FieldReminderTypes));}

    // XXX TODO: convert date time to timezone date time
    void setDateTime(const QDateTime& dateTime) {setValue(FieldDateTime, dateTime);}
    QDateTime dateTime() const {return value<QDateTime>(FieldDateTime);}
    void setTimeDelta(int secondsBefore) {setValue(FieldTimeDelta, secondsBefore);}
    int timeDelta() const {return value<int>(FieldTimeDelta);}

    // duration/repetition
    void setRepetition(int count, int delaySeconds) {setValue(FieldRepetitionCount, count); setValue(FieldRepetitionDelay, delaySeconds);}
    int repetitionDelay() const {return value<int>(FieldRepetitionDelay);}
    int repetitionCount() const {return value<int>(FieldRepetitionCount);}

    // message and visual data to be displayed if visual notification.
    void setVisualMessage(const QString& message) {setValue(FieldVisualMessage, message);}
    QString visualMessage() const {return value<QString>(FieldVisualMessage);}
    void setVisualDataUrl(const QUrl& dataUrl) {setValue(FieldVisualDataUrl, dataUrl);}
    QUrl visualDataUrl() const {return value<QUrl>(FieldVisualDataUrl);}
    void setVisualData(const QByteArray& data) {setValue(FieldVisualData, data);}
    QByteArray visualData() const {return value<QByteArray>(FieldVisualData);}

    // audio data to be played if audible notification.
    void setAudibleDataUrl(const QUrl& dataUrl) {setValue(FieldAudibleDataUrl, dataUrl);}
    QUrl audibleDataUrl() const {return value<QUrl>(FieldAudibleDataUrl);}
    void setAudibleData(const QByteArray& data) {setValue(FieldAudibleData, data);}
    QByteArray audibleData() const {return value<QByteArray>(FieldAudibleData);}

    // tactile data to be played if tactile notification.
    void setTactileDataUrl(const QUrl& dataUrl) {setValue(FieldTactileDataUrl, dataUrl);}
    QUrl tactileDataUrl() const {return value<QUrl>(FieldTactileDataUrl);}
    void setTactileData(const QByteArray& data) {setValue(FieldTactileData, data);}
    QByteArray tactileData() const {return value<QByteArray>(FieldTactileData);}

    // email data if email notification.
    void setEmailContents(const QString& subject, const QString& body, const QVariantList& attachments) {setValue(FieldEmailSubject, subject); setValue(FieldEmailBody, body); setValue(FieldEmailAttachments, attachments);}
    QString emailSubject() const {return value(FieldEmailSubject);}
    QString emailBody() const {return value(FieldEmailBody);}
    QVariantList emailAttachments() {return value<QVariantList>(FieldEmailAttachments);}
    void setEmailRecipients(const QStringList& recipients) {setValue(FieldEmailRecipients, recipients);}
    QStringList emailRecipients() const {return value<QStringList>(FieldEmailRecipients);}

    // procedure if procedure notification
    void setProcedure(const QUrl& url, const QString& arguments) {setValue(FieldProcedureUrl, url); setValue(FieldProcedureArguments, arguments);}
    QUrl procedureUrl() const {return value<QUrl>(FieldProcedureUrl);}
    QString procedureArguments() const {return value(FieldProcedureArguments);}
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QOrganizerItemReminder::ReminderTypes);

QTM_END_NAMESPACE

#endif

