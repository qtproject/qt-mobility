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

#ifndef QDECLARATIVECONTACTGENDER_H
#define QDECLARATIVECONTACTGENDER_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactgender.h"

class QDeclarativeContactGender : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(GenderType gender READ gender WRITE setGender NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_ENUMS(GenderType)
    Q_CLASSINFO("DefaultProperty", "gender")
public:
    enum FieldType {
        Gender = 0
    };

    enum GenderType {
        Male = 0,
        Female
    };

    QDeclarativeContactGender(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactGender());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Gender;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case Gender:
            return QContactGender::FieldGender;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setGender(const GenderType v)
    {
        if (!readOnly() && v != gender()) {
            if (v == Male) {
                detail().setValue(QContactGender::FieldGender, QContactGender::GenderMale);
            } else if (v == Female) {
                detail().setValue(QContactGender::FieldGender, QContactGender::GenderFemale);
            }
            emit fieldsChanged();
        }
    }
    GenderType gender() const
    {
        if (detail().value(QContactGender::FieldGender) == QContactGender::GenderMale) {
            return Male;
        }
        return Female;
    }
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactGender)
#endif

