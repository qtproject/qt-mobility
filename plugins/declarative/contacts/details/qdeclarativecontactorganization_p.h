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

#ifndef QDECLARATIVECONTACTORGANIZATION_H
#define QDECLARATIVECONTACTORGANIZATION_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactorganization.h"
#include <QSet>
class QDeclarativeContactOrganization : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl logoUrl READ logoUrl WRITE setLogoUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QStringList department READ department WRITE setDepartment NOTIFY fieldsChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY fieldsChanged)
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY fieldsChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY fieldsChanged)
    Q_PROPERTY(QString assistantName READ assistantName WRITE setAssistantName NOTIFY fieldsChanged)
    Q_CLASSINFO("DefaultProperty", "name")
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        Name = 0,
        LogoUrl,
        Department,
        Location,
        Role,
        Title,
        AssistantName
    };

    QDeclarativeContactOrganization(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactOrganization());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactOrganization;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case Name:
            return QContactOrganization::FieldName;
        case LogoUrl:
            return QContactOrganization::FieldLogoUrl;
        case Department:
            return QContactOrganization::FieldDepartment;
        case Location:
            return QContactOrganization::FieldLocation;
        case Role:
            return QContactOrganization::FieldRole;
        case Title:
            return QContactOrganization::FieldTitle;
        case AssistantName:
            return QContactOrganization::FieldAssistantName;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setName(const QString& v)
    {
        if (!readOnly() && v != name()) {
            detail().setValue(QContactOrganization::FieldName, v);
            emit fieldsChanged();
        }
    }
    QString name() const {return detail().value(QContactOrganization::FieldName);}
    void setLogoUrl(const QUrl& v)
    {
        if (!readOnly() && v != logoUrl()) {
            detail().setValue(QContactOrganization::FieldLogoUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl logoUrl() const {return detail().value(QContactOrganization::FieldLogoUrl);}
    void setDepartment(const QStringList& v)
    {
        if (!readOnly() && v.toSet() != department().toSet()) {
            detail().setValue(QContactOrganization::FieldDepartment, v);
            emit fieldsChanged();
        }
    }
    QStringList department() const {return detail().value<QStringList>(QContactOrganization::FieldDepartment);}
    void setLocation(const QString& v)
    {
        if (!readOnly() && v != location()) {
            detail().setValue(QContactOrganization::FieldLocation, v);
            emit fieldsChanged();
        }
    }
    QString location() const {return detail().value(QContactOrganization::FieldLocation);}
    void setRole(const QString& v)
    {
        if (!readOnly() && v != role()) {
            detail().setValue(QContactOrganization::FieldRole, v);
            emit fieldsChanged();
        }
    }
    QString role() const {return detail().value(QContactOrganization::FieldRole);}
    void setTitle(const QString& v)
    {
        if (!readOnly() && v != title()) {
            detail().setValue(QContactOrganization::FieldTitle, v);
            emit fieldsChanged();
        }
    }
    QString title() const {return detail().value(QContactOrganization::FieldTitle);}
    void setAssistantName(const QString& v)
    {
        if (!readOnly() && v != assistantName()) {
            detail().setValue(QContactOrganization::FieldAssistantName, v);
            emit fieldsChanged();
        }
    }
    QString assistantName() const {return detail().value(QContactOrganization::FieldAssistantName);}
signals:
    void fieldsChanged();

};
QML_DECLARE_TYPE(QDeclarativeContactOrganization)

#endif
