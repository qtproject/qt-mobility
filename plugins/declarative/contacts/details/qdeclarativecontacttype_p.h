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

#ifndef QDECLARATIVECONTACTTYPE_H
#define QDECLARATIVECONTACTTYPE_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontacttype.h"

class QDeclarativeContactType : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY fieldsChanged)
    Q_ENUMS(FieldType);
    Q_CLASSINFO("DefaultProperty", "type")
public:
    enum FieldType {
        Type = 0
    };
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Type;
    }

    QDeclarativeContactType(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactType());
    }

    void setType(const QString& type) {detail().setValue(QContactType::FieldType, type);}
    QString type() const {return detail().value(QContactType::FieldType);}
signals:
    void fieldsChanged();
};
QML_DECLARE_TYPE(QDeclarativeContactType)
#endif

