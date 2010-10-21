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

#ifndef QDECLARATIVECONTACTAVATAR_H
#define QDECLARATIVECONTACTAVATAR_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactavatar.h"

class  QDeclarativeContactAvatar : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QUrl imageUrl READ imageUrl WRITE setImageUrl NOTIFY fieldsChanged)
    Q_PROPERTY(QUrl videoUrl READ videoUrl WRITE setVideoUrl NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
public:
    enum FieldType {
        ImageUrl = 0,
        VideoUrl
    };
    QDeclarativeContactAvatar(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactAvatar());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }
    void setImageUrl(const QUrl& v)
    {
        if (!readOnly() && v != imageUrl()) {
            detail().setValue(QContactAvatar::FieldImageUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl imageUrl() const {return detail().value<QUrl>(QContactAvatar::FieldImageUrl);}

    void setVideoUrl(const QUrl& v)
    {
        if (!readOnly() && v != videoUrl()) {
            detail().setValue(QContactAvatar::FieldVideoUrl, v);
            emit fieldsChanged();
        }
    }
    QUrl videoUrl() const {return detail().value<QUrl>(QContactAvatar::FieldVideoUrl);}

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::Avatar;
    }

    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case ImageUrl:
            return QContactAvatar::FieldImageUrl;
        case VideoUrl:
            return QContactAvatar::FieldVideoUrl;
        default:
            break;
        }
        //QWarning()
        return QString();
    }
signals:
    void fieldsChanged();

};
QML_DECLARE_TYPE(QDeclarativeContactAvatar)
#endif
