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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    Q_PROPERTY(AnniversarySubType subType READ subType WRITE setSubType NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_ENUMS(AnniversarySubType)
public:
    enum FieldType {
        CalendarId = 0,
        OriginalDate,
        Event,
        SubType
    };

    enum AnniversarySubType {
        Unknown = 0,
        Wedding,
        Engagement,
        House,
        Employment,
        Memorial
    };

    QDeclarativeContactAnniversary(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactAnniversary());
        connect(this, SIGNAL(fieldsChanged), SIGNAL(valueChanged()));
    }

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Anniversary;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case CalendarId:
            return QContactAnniversary::FieldCalendarId;
        case OriginalDate:
            return QContactAnniversary::FieldOriginalDate;
        case Event:
            return QContactAnniversary::FieldEvent;
        case SubType:
            return QContactAnniversary::FieldSubType;
        default:
            break;
        }
        //qWarning()
        return QString();
    }
    void setOriginalDate(const QDate& v)
    {
        if (!readOnly() && v != originalDate()) {
            detail().setValue(QContactAnniversary::FieldOriginalDate, v);
            emit fieldsChanged();
        }
    }
    QDate originalDate() const {return detail().value<QDate>(QContactAnniversary::FieldOriginalDate);}
    void setOriginalDateTime(const QDateTime& v)
    {
        if (!readOnly() && v != originalDateTime()) {
            detail().setValue(QContactAnniversary::FieldOriginalDate, v);
            emit fieldsChanged();
        }
    }
    QDateTime originalDateTime() const {return detail().value<QDateTime>(QContactAnniversary::FieldOriginalDate);}

    void setCalendarId(const QString& v)
    {
        if (!readOnly() && v != calendarId()) {
            detail().setValue(QContactAnniversary::FieldCalendarId, v);
            emit fieldsChanged();
        }
    }
    QString calendarId() const {return detail().value(QContactAnniversary::FieldCalendarId);}
    void setEvent(const QString& v)
    {
        if (!readOnly() && v != event()) {
            detail().setValue(QContactAnniversary::FieldEvent, v);
            emit fieldsChanged();
        }
    }
    QString event() const {return detail().value(QContactAnniversary::FieldEvent);}

    void setSubType(AnniversarySubType v)
    {
        if (!readOnly() && v != subType()) {
            QString subTypeString;
            switch (v) {
            case Wedding:
                subTypeString = QContactAnniversary::SubTypeWedding.latin1();
                break;
            case Engagement:
                subTypeString = QContactAnniversary::SubTypeEngagement.latin1();
                break;
            case House:
                subTypeString = QContactAnniversary::SubTypeHouse.latin1();
                break;
            case Employment:
                subTypeString = QContactAnniversary::SubTypeEmployment.latin1();
                break;
            case Memorial:
                subTypeString = QContactAnniversary::SubTypeMemorial.latin1();
                break;
           default:
                break;
            }

           detail().setValue(QContactAnniversary::FieldSubType, subTypeString);
        }
    }

    AnniversarySubType subType() const
    {
        QString subType = detail().value(QContactAnniversary::FieldSubType);
        if (subType == QContactAnniversary::SubTypeWedding)
            return Wedding;
        else if (subType == QContactAnniversary::SubTypeEngagement)
            return Engagement;
        else if (subType == QContactAnniversary::SubTypeHouse)
            return House;
        else if (subType == QContactAnniversary::SubTypeEmployment)
            return Employment;
        else if (subType == QContactAnniversary::SubTypeMemorial)
            return Memorial;
        return Unknown;
    }
signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactAnniversary)
#endif

