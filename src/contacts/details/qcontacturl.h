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

#ifndef QCONTACTURL_H
#define QCONTACTURL_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactUrl : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldUrl;
    static const QLatin1Constant FieldSubType;
    static const QLatin1Constant SubTypeHomePage;
    static const QLatin1Constant SubTypeFavorite;
    static const QLatin1Constant SubTypeSocialNetworking;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactUrl, "Url")
    Q_DECLARE_LATIN1_CONSTANT(FieldUrl, "Url");
    Q_DECLARE_LATIN1_CONSTANT(FieldSubType, "SubType");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeHomePage, "HomePage");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeFavorite, "Favorite");
#endif

    void setUrl(const QString& url) {setValue(FieldUrl, url);}
    QString url() const {return value(FieldUrl);}

    void setSubType(const QString& subType) {setValue(FieldSubType, subType);}
    QString subType() const {return value(FieldSubType);}
};

QTM_END_NAMESPACE

#endif

