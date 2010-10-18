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

#ifndef QDECLARATIVECONTACTTHUMBNAIL_H
#define QDECLARATIVECONTACTTHUMBNAIL_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactthumbnail.h"

class QDeclarativeContactThumbnail : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QString thumbnail READ thumbnail WRITE setThumbnail NOTIFY fieldsChanged)
    Q_ENUMS(FieldType);
    Q_CLASSINFO("DefaultProperty", "thumbnail")
public:
    enum FieldType {
        Thumbnail = 0
    };

    QDeclarativeContactThumbnail(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactThumbnail());
    }

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Thumbnail;
    }

    void setThumbnail(const QString& thumbnail)
    {
        //TODO
        Q_UNUSED(thumbnail)

    }
    QString thumbnail() const
    {
        //TODO
        return QString();
    }
signals:
    void fieldsChanged();
};
QML_DECLARE_TYPE(QDeclarativeContactThumbnail)
#endif
