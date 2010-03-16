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


#ifndef QCONTACTONLINEACCOUNT_H
#define QCONTACTONLINEACCOUNT_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactOnlineAccount : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldAccountUri;
    const char* FieldServiceProvider;
    const char* FieldCapabilities;
    const char* FieldSubTypes;
    const char* SubTypeSip;
    const char* SubTypeSipVoip;
    const char* SubTypeImpp;
    const char* SubTypeVideoShare;

    // deprecated keys:
    const char* FieldNickname;
    const char* FieldStatusMessage;
    const char* FieldPresence;
    const char* PresenceAvailable;
    const char* PresenceHidden;
    const char* PresenceBusy;
    const char* PresenceAway;
    const char* PresenceExtendedAway;
    const char* PresenceUnknown;
    const char* PresenceOffline;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactOnlineAccount, "OnlineAccount")
    Q_DECLARE_LATIN1_LITERAL(FieldAccountUri, "AccountUri");
    Q_DECLARE_LATIN1_LITERAL(FieldServiceProvider, "ServiceProvider");
    Q_DECLARE_LATIN1_LITERAL(FieldCapabilities, "Capabilities");
    Q_DECLARE_LATIN1_LITERAL(FieldSubTypes, "SubTypes");
    Q_DECLARE_LATIN1_LITERAL(SubTypeSip, "Sip");
    Q_DECLARE_LATIN1_LITERAL(SubTypeSipVoip, "SipVoip");
    Q_DECLARE_LATIN1_LITERAL(SubTypeImpp, "Impp");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVideoShare, "VideoShare");

    // deprecated keys:
    Q_DECLARE_LATIN1_LITERAL(FieldNickname, "Nickname");
    Q_DECLARE_LATIN1_LITERAL(FieldPresence, "Presence");
    Q_DECLARE_LATIN1_LITERAL(FieldStatusMessage, "StatusMessage");
    Q_DECLARE_LATIN1_LITERAL(PresenceAvailable, "Available");
    Q_DECLARE_LATIN1_LITERAL(PresenceHidden, "Hidden");
    Q_DECLARE_LATIN1_LITERAL(PresenceBusy, "Busy");
    Q_DECLARE_LATIN1_LITERAL(PresenceAway, "Away");
    Q_DECLARE_LATIN1_LITERAL(PresenceExtendedAway, "ExtendedAway");
    Q_DECLARE_LATIN1_LITERAL(PresenceUnknown, "Unknown");
    Q_DECLARE_LATIN1_LITERAL(PresenceOffline, "Offline");
#endif

    void setAccountUri(const QString& accountUri) {setValue(FieldAccountUri, accountUri);}
    QString accountUri() const {return value(FieldAccountUri);}

    void setServiceProvider(const QString& serviceProvider) {setValue(FieldServiceProvider, serviceProvider);}
    QString serviceProvider() const {return value(FieldServiceProvider);}

    void setCapabilities(const QStringList& capabilities) {setValue(FieldCapabilities, capabilities);}
    QStringList capabilities() const {return value<QStringList>(FieldCapabilities);}

    void setSubTypes(const QStringList& subTypes) {setValue(FieldSubTypes, subTypes);}
    void setSubTypes(const QString& subType) {setValue(FieldSubTypes, QStringList(subType));}
    QStringList subTypes() const {return value<QStringList>(FieldSubTypes);}

    // deprecated functions:
    void Q_DECL_DEPRECATED setNickname(const QString& nickname);
    QString Q_DECL_DEPRECATED nickname() const;
    void Q_DECL_DEPRECATED setPresence(const QString& presence);
    QString Q_DECL_DEPRECATED presence() const;
    void Q_DECL_DEPRECATED setStatusMessage(const QString& statusMessage);
    QString Q_DECL_DEPRECATED statusMessage() const;
};

QTM_END_NAMESPACE

#endif

