/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef QDECLARATIVECONTACTURL_H
#define QDECLARATIVECONTACTURL_H
#include <QUrl>

#include "qdeclarativecontactdetail_p.h"
#include "qcontacturl.h"

class QDeclarativeContactUrl : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY fieldsChanged)
    Q_PROPERTY(UrlSubType subType READ subType WRITE setSubType NOTIFY fieldsChanged)

    Q_ENUMS(FieldType)
    Q_ENUMS(UrlSubType)
    Q_CLASSINFO("DefaultProperty", "url")
public:
    enum FieldType {
        Url = 0,
        SubType
    };

    enum UrlSubType {
        Unknown = 0,
        HomePage,
        Favourite
    };

    QDeclarativeContactUrl(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactUrl());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Url;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case Url:
            return QContactUrl::FieldUrl;
        case SubType:
            return QContactUrl::FieldSubType;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setUrl(const QString& v)
    {
        if (!readOnly() && v != url()) {
            detail().setValue(QContactUrl::FieldUrl, v);
        }
    }
    QString url() const {return detail().value(QContactUrl::FieldUrl);}

    void setSubType(UrlSubType v)
    {
        if (!readOnly() && v != subType()) {
            if (v == HomePage)
                detail().setValue(QContactUrl::FieldSubType, QContactUrl::SubTypeHomePage);
            else if (v == Favourite)
                detail().setValue(QContactUrl::FieldSubType, QContactUrl::SubTypeFavourite);
            emit fieldsChanged();
        }
    }
    UrlSubType subType() const
    {
        QString typeString = detail().value(QContactUrl::FieldSubType);
        if (typeString == QContactUrl::SubTypeFavourite)
            return Favourite;
        else if (typeString == QContactUrl::SubTypeHomePage)
            return HomePage;
        //qWarning: unknown type
        return Unknown;
    }
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactUrl)

#endif

