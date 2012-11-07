/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QORGANIZERITEMEMAILREMINDER_H
#define QORGANIZERITEMEMAILREMINDER_H

#include "qorganizeritemreminder.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerItemEmailReminder : public QOrganizerItemReminder
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldSubject;
    const char* FieldBody;
    const char* FieldAttachments;
    const char* FieldRecipients;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_REMINDER_DETAIL(QOrganizerItemEmailReminder, "EmailReminder")
    Q_DECLARE_LATIN1_CONSTANT(FieldSubject, "Subject");
    Q_DECLARE_LATIN1_CONSTANT(FieldBody, "Body");
    Q_DECLARE_LATIN1_CONSTANT(FieldAttachments, "Attachments");
    Q_DECLARE_LATIN1_CONSTANT(FieldRecipients, "Recipients");
#endif

    // email data if email notification.
    void setContents(const QString& subject, const QString& body, const QVariantList& attachments) {setValue(FieldSubject, subject); setValue(FieldBody, body); setValue(FieldAttachments, attachments);}
    QString subject() const {return value(FieldSubject);}
    QString body() const {return value(FieldBody);}
    QVariantList attachments() const {return value<QVariantList>(FieldAttachments);} // mime data type?
    void setRecipients(const QStringList& recipients) {setValue(FieldRecipients, recipients);}
    QStringList recipients() const {return value<QStringList>(FieldRecipients);}
};

QTM_END_NAMESPACE

#endif

