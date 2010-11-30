/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVECONTACTURL_H
#define QDECLARATIVECONTACTURL_H
#include <QUrl>

#include "qdeclarativecontactdetail_p.h"
#include "qcontacturl.h"

class QDeclarativeContactUrl : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QVariant subType READ subType WRITE setSubType NOTIFY fieldsChanged)

    Q_ENUMS(FieldType)
    Q_ENUMS(UrlSubType)
public:
    enum FieldType {
        Url = 0,
        SubType
    };

    enum UrlSubType {
        Unknown = 0,
        HomePage,
        Blog,
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

    void setSubType(const QVariant& v)
    {
        if (!readOnly()) {
            QString st;
            if (v.type() != QVariant::String) {
                switch (v.toInt()) {
                case HomePage:
                    st = QLatin1String(QContactUrl::SubTypeHomePage);
                    break;
                case Blog:
                    st = QLatin1String(QContactUrl::SubTypeBlog);
                    break;
                case Favourite:
                    st = QLatin1String(QContactUrl::SubTypeFavourite);
                    break;
                default:break;
                }
            } else {
                st = v.toString();
            }

            if (st != detail().value(QContactUrl::FieldSubType)) {
                detail().setValue(QContactUrl::FieldSubType, st);
                emit fieldsChanged();
            }
        }
    }
    QVariant subType() const
    {
        return detail().value(QContactUrl::FieldSubType);
    }
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactUrl)

#endif

