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


#ifndef QDECLARATIVECONTACTEMAILADDRESS_H
#define QDECLARATIVECONTACTEMAILADDRESS_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactemailaddress.h"

class QDeclarativeContactEmailAddress : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QString emailAddress READ emailAddress WRITE setEmailAddress NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        EmailAddress = 0
    };
    QDeclarativeContactEmailAddress(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactEmailAddress());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Email;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case EmailAddress:
            return QContactEmailAddress::FieldEmailAddress;
        default:
            break;
        }
        //qWarning
        return QString();
    }

    void setEmailAddress(const QString& v)
    {
        if (!readOnly() && v != emailAddress()) {
            detail().setValue(QContactEmailAddress::FieldEmailAddress, v);
            emit fieldsChanged();
        }
    }
    QString emailAddress() const {return detail().value(QContactEmailAddress::FieldEmailAddress);}

signals:
    void fieldsChanged();
};
QML_DECLARE_TYPE(QDeclarativeContactEmailAddress)
#endif

