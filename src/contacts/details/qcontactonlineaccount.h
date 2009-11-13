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

/* Leaf class */
class Q_CONTACTS_EXPORT QContactOnlineAccount : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldAccountUri;
    const char* FieldSubTypes;
    const char* SubTypeSip;
    const char* SubTypeH323;
    const char* SubTypeXmpp;
    const char* SubTypeInternet;
    const char* SubTypeShareVideo;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactOnlineAccount, "OnlineAccount")
    Q_DECLARE_LATIN1_LITERAL(FieldAccountUri, "AccountUri");
    Q_DECLARE_LATIN1_LITERAL(FieldSubTypes, "SubTypes");
    Q_DECLARE_LATIN1_LITERAL(SubTypeSip, "Sip");
    Q_DECLARE_LATIN1_LITERAL(SubTypeH323, "H323");
    Q_DECLARE_LATIN1_LITERAL(SubTypeXmpp, "Xmpp");
    Q_DECLARE_LATIN1_LITERAL(SubTypeInternet, "Internet");
    Q_DECLARE_LATIN1_LITERAL(SubTypeShareVideo, "ShareVideo");
#endif

    void setAccountUri(const QString& accountUri) {setValue(FieldAccountUri, accountUri);}
    QString accountUri() const {return value(FieldAccountUri);}

    void setSubTypes(const QStringList& subTypes) {setValue(FieldSubTypes, subTypes);}
    void setSubTypes(const QString& subType) {setValue(FieldSubTypes, QStringList(subType));}
    QStringList subTypes() const {return value<QStringList>(FieldSubTypes);}
};

#endif

