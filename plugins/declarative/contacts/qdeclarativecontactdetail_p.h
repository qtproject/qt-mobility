/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#ifndef QDECLARATIVECONTACTDETAIL_P_H
#define QDECLARATIVECONTACTDETAIL_P_H

#include <qdeclarative.h>
#include "qcontact.h"
#include "qcontactdetail.h"

QTM_USE_NAMESPACE;

class QDeclarativeContactDetail : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ContactDetailType type READ detailType NOTIFY valueChanged)
    Q_PROPERTY(QString definitionName READ definitionName NOTIFY valueChanged)
    Q_PROPERTY(QStringList contexts READ contexts WRITE setContexts NOTIFY valueChanged)
    Q_PROPERTY(QString detailUri READ detailUri WRITE setDetailUri NOTIFY valueChanged)
    Q_PROPERTY(QStringList linkedDetailUris READ linkedDetailUris WRITE setLinkedDetailUris NOTIFY valueChanged)
    Q_PROPERTY(QStringList fieldNames READ fieldNames NOTIFY valueChanged)
    Q_PROPERTY(bool readOnly READ readOnly NOTIFY valueChanged)
    Q_PROPERTY(bool removable READ removable NOTIFY valueChanged)
    Q_ENUMS(ContactDetailType)
public:
    QDeclarativeContactDetail(QObject* parent = 0);


    enum ContactDetailType {
        Address = 0,
        Anniversary,
        Avatar,
        Birthday,
        DisplayLabel,
        Email,
        Family,
        Favorite,
        Gender,
        Geolocation,
        GlobalPresence,
        Guid,
        Name,
        NickName,
        Note,
        OnlineAccount,
        Organization,
        PhoneNumber,
        Presence,
        Ringtone,
        SyncTarget,
        Tag,
        Timestamp,
        Url,
        Hobby,
        Customized = 100
    };

    QContactDetail& detail();
    const QContactDetail& detail() const;
    void setDetail(const QContactDetail& detail);

    bool readOnly() const;
    bool removable() const;

    QString definitionName() const;
    QStringList contexts() const;
    void setContexts(const QStringList& context);

    QString detailUri() const;
    void setDetailUri(const QString& detailUri);

    QStringList linkedDetailUris() const;
    void setLinkedDetailUris(const QStringList& linkedDetailUris);
    virtual ContactDetailType detailType() const;

    QStringList fieldNames() const;
    Q_INVOKABLE QVariant value(const QString& key) const;
    Q_INVOKABLE bool setValue(const QString& key, const QVariant& value);

    static QString definitionName(ContactDetailType type) ;
    static ContactDetailType detailType(const QString& definitionName) ;
    static QString fieldName(ContactDetailType detailType, int fieldType);
signals:
    void valueChanged();

protected:
    QContactDetail m_detail;
};

QML_DECLARE_TYPE(QDeclarativeContactDetail)

#endif // QDECLARATIVECONTACTDETAIL_P_H
