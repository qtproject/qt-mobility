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

#ifndef QDECLARATIVECONTACTANNIVERSARY_H
#define QDECLARATIVECONTACTANNIVERSARY_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactanniversary.h"

class QDeclarativeContactAnniversary : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString calendarId READ calendarId WRITE setCalendarId NOTIFY fieldsChanged)
    Q_PROPERTY(QDate originalDate READ originalDate WRITE setOriginalDate NOTIFY fieldsChanged)
    Q_PROPERTY(QDateTime originalDateTime READ originalDateTime WRITE setOriginalDateTime NOTIFY fieldsChanged)
    Q_PROPERTY(QString event READ event WRITE setEvent NOTIFY fieldsChanged)
    Q_PROPERTY(SubType subType READ subType WRITE setSubType NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_ENUMS(SubType)
public:
    enum FieldType {
        FieldCalendarId = 0,
        FieldOriginalDate,
        FieldEvent,
        FieldSubType
    };

    enum SubType {
        SubTypeWedding = 0,
        SubTypeEngagement,
        SubTypeHouse,
        SubTypeEmployment,
        SubTypeMemorial,
        SubTypeInvalid
    };

    QDeclarativeContactAnniversary(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactAnniversary());
        connect(this, SIGNAL(fieldsChanged), SIGNAL(valueChanged()));
    }

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactAnniversary;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case FieldCalendarId:
            return QContactAnniversary::FieldCalendarId;
        case FieldOriginalDate:
            return QContactAnniversary::FieldOriginalDate;
        case FieldEvent:
            return QContactAnniversary::FieldEvent;
        case FieldSubType:
            return QContactAnniversary::FieldSubType;
        default:
            break;
        }
        return "";
    }
    void setOriginalDate(const QDate& date) {if (!readOnly()) detail().setValue(QContactAnniversary::FieldOriginalDate, date);}
    QDate originalDate() const {return detail().value<QDate>(QContactAnniversary::FieldOriginalDate);}
    void setOriginalDateTime(const QDateTime& dateTime) {if (!readOnly()) detail().setValue(QContactAnniversary::FieldOriginalDate, dateTime);}
    QDateTime originalDateTime() const {return detail().value<QDateTime>(QContactAnniversary::FieldOriginalDate);}

    void setCalendarId(const QString& calendarId) {if (!readOnly()) detail().setValue(QContactAnniversary::FieldCalendarId, calendarId);}
    QString calendarId() const {return detail().value(QContactAnniversary::FieldCalendarId);}
    void setEvent(const QString& event) {if (!readOnly()) detail().setValue(QContactAnniversary::FieldEvent, event);}
    QString event() const {return detail().value(QContactAnniversary::FieldEvent);}

    void setSubType(SubType subType)
    {
        if (!readOnly()) {
            QString subTypeString;
            switch (subType) {
            case SubTypeWedding:
                subTypeString = QContactAnniversary::SubTypeWedding.latin1();
                break;
            case SubTypeEngagement:
                subTypeString = QContactAnniversary::SubTypeEngagement.latin1();
                break;
            case SubTypeHouse:
                subTypeString = QContactAnniversary::SubTypeHouse.latin1();
                break;
            case SubTypeEmployment:
                subTypeString = QContactAnniversary::SubTypeEmployment.latin1();
                break;
            case SubTypeMemorial:
                subTypeString = QContactAnniversary::SubTypeMemorial.latin1();
                break;
           default:
                break;
            }

           detail().setValue(QContactAnniversary::FieldSubType, subTypeString);
        }
    }

    SubType subType() const
    {
        QString subType = detail().value(QContactAnniversary::FieldSubType);
        if (subType == QContactAnniversary::SubTypeWedding)
            return SubTypeWedding;
        else if (subType == QContactAnniversary::SubTypeEngagement)
            return SubTypeEngagement;
        else if (subType == QContactAnniversary::SubTypeHouse)
            return SubTypeHouse;
        else if (subType == QContactAnniversary::SubTypeEmployment)
            return SubTypeEmployment;
        else if (subType == QContactAnniversary::SubTypeMemorial)
            return SubTypeMemorial;
        return SubTypeInvalid;
    }
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactAnniversary)
#endif

