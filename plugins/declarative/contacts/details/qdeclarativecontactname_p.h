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


#ifndef QDECLARATIVECONTACTNAME_H
#define QDECLARATIVECONTACTNAME_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactname.h"

class QDeclarativeContactName : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY fieldsChanged)
    Q_PROPERTY(QString firstName READ firstName WRITE setFirstName NOTIFY fieldsChanged)
    Q_PROPERTY(QString middleName READ middleName WRITE setMiddleName NOTIFY fieldsChanged)
    Q_PROPERTY(QString lastName READ lastName WRITE setLastName NOTIFY fieldsChanged)
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY fieldsChanged)
    Q_PROPERTY(QString customLabel READ customLabel WRITE setCustomLabel NOTIFY fieldsChanged)

    Q_ENUMS(FieldType)
    Q_CLASSINFO("DefaultProperty", "firstName")
public:
    enum FieldType {
        Prefix = 0,
        FirstName,
        MiddleName,
        LastName,
        Suffix,
        CustomLabel
    };

    QDeclarativeContactName(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactName());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Name;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case Prefix:
            return QContactName::FieldPrefix;
        case FirstName:
            return QContactName::FieldFirstName;
        case MiddleName:
            return QContactName::FieldMiddleName;
        case LastName:
            return QContactName::FieldLastName;
        case Suffix:
            return QContactName::FieldSuffix;
        case CustomLabel:
            return QContactName::FieldCustomLabel;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    QString prefix() const {return detail().value(QContactName::FieldPrefix);}
    QString firstName() const {return detail().value(QContactName::FieldFirstName);}
    QString middleName() const {return detail().value(QContactName::FieldMiddleName);}
    QString lastName() const {return detail().value(QContactName::FieldLastName);}
    QString suffix() const {return detail().value(QContactName::FieldSuffix);}
    QString customLabel() const{return detail().value(QContactName::FieldCustomLabel);}

    void setPrefix(const QString& v)
    {
        if (!readOnly() && v != prefix()) {
            detail().setValue(QContactName::FieldPrefix, v);
            emit fieldsChanged();
        }
    }

    void setFirstName(const QString& v)
    {
        if (!readOnly() && v != firstName()) {
            detail().setValue(QContactName::FieldFirstName, v);
            emit fieldsChanged();
        }
    }
    void setMiddleName(const QString& v)
    {
        if (!readOnly() && v != middleName()) {
            detail().setValue(QContactName::FieldMiddleName, v);
            emit fieldsChanged();
        }
    }
    void setLastName(const QString& v)
    {
        if (!readOnly() && v != lastName()) {
            detail().setValue(QContactName::FieldLastName, v);
            emit fieldsChanged();
        }
    }
    void setSuffix(const QString& v)
    {
        if (!readOnly() && v != suffix()) {
            detail().setValue(QContactName::FieldSuffix, v);
            emit fieldsChanged();
        }
    }
    void setCustomLabel(const QString& v)
    {
        if (!readOnly() && v != customLabel()) {
            detail().setValue(QContactName::FieldCustomLabel, v);
            emit fieldsChanged();
        }
    }

signals:
    void fieldsChanged();
};

QML_DECLARE_TYPE(QDeclarativeContactName)
#endif
