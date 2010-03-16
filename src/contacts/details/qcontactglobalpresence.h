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


#ifndef QCONTACTGLOBALPRESENCE_H
#define QCONTACTGLOBALPRESENCE_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactGlobalPresence : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldLastUpdateTimestamp;
    const char* FieldNickname;
    const char* FieldPresence;
    const char* PresenceAvailable;
    const char* PresenceHidden;
    const char* PresenceBusy;
    const char* PresenceAway;
    const char* PresenceExtendedAway;
    const char* PresenceUnknown;
    const char* PresenceOffline;
    const char* FieldStatusMessage;
    const char* FieldStatusImageUri;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactGlobalPresence, "GlobalPresence")
    Q_DECLARE_LATIN1_LITERAL(FieldLastUpdateTimestamp, "LastUpdateTimestamp");
    Q_DECLARE_LATIN1_LITERAL(FieldNickname, "Nickname");
    Q_DECLARE_LATIN1_LITERAL(FieldPresence, "Presence");
    Q_DECLARE_LATIN1_LITERAL(PresenceAvailable, "Available");
    Q_DECLARE_LATIN1_LITERAL(PresenceHidden, "Hidden");
    Q_DECLARE_LATIN1_LITERAL(PresenceBusy, "Busy");
    Q_DECLARE_LATIN1_LITERAL(PresenceAway, "Away");
    Q_DECLARE_LATIN1_LITERAL(PresenceExtendedAway, "ExtendedAway");
    Q_DECLARE_LATIN1_LITERAL(PresenceUnknown, "Unknown");
    Q_DECLARE_LATIN1_LITERAL(PresenceOffline, "Offline");
    Q_DECLARE_LATIN1_LITERAL(FieldStatusMessage, "StatusMessage");
    Q_DECLARE_LATIN1_LITERAL(FieldStatusImageUri, "StatusImageUri");
#endif

    void setLastUpdateTimestamp(const QDateTime& updateTimestamp) {setValue(FieldLastUpdateTimestamp, updateTimestamp);}
    QDateTime lastUpdateTimestamp() const {return value<QDateTime>(FieldLastUpdateTimestamp);}
    void setNickname(const QString& nickname) {setValue(FieldNickname, nickname);}
    QString nickname() const {return value(FieldNickname);}
    void setPresence(const QString& presence) {setValue(FieldPresence, presence);}
    QString presence() const {return value(FieldPresence);}
    void setStatusMessage(const QString& statusMessage) {setValue(FieldStatusMessage, statusMessage);}
    QString statusMessage() const {return value(FieldStatusMessage);}
    void setStatusImageUri(const QString& statusImageUri) {setValue(FieldStatusImageUri, statusImageUri);}
    QString statusImageUri() const {return value(FieldStatusImageUri);}
};

QTM_END_NAMESPACE

#endif

