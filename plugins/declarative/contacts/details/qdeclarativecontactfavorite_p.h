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


#ifndef QDECLARATIVECONTACTFAVORITE_H
#define QDECLARATIVECONTACTFAVORITE_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactfavorite.h"

class QDeclarativeContactFavorite : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(bool favorite READ isFavorite WRITE setFavorite NOTIFY fieldsChanged)
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_CLASSINFO("DefaultProperty", "index")
public:
    enum FieldType {
        Favorite = 0,
        Index
    };
    QDeclarativeContactFavorite(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactFavorite());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactFavorite;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case Favorite:
            return QContactFavorite::FieldFavorite;
        case Index:
            return QContactFavorite::FieldIndex;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setFavorite(bool v)
    {
        if (!readOnly() && v != isFavorite()) {
            detail().setValue(QContactFavorite::FieldFavorite, v);
            emit fieldsChanged();
        }
    }
    bool isFavorite() const {return detail().variantValue(QContactFavorite::FieldFavorite).toBool();}
    void setIndex(int v)
    {
        if (!readOnly() && v != index()) {
            detail().setValue(QContactFavorite::FieldIndex, v);
            emit fieldsChanged();
        }
    }
    int index() const {return detail().variantValue(QContactFavorite::FieldIndex).toInt();}
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactFavorite)
#endif
