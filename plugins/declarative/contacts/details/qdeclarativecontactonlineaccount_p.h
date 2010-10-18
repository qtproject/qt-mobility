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


#ifndef QDECLARATIVECONTACTONLINEACCOUNT_H
#define QDECLARATIVECONTACTONLINEACCOUNT_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactonlineaccount.h"
#include <QSet>
class QDeclarativeContactOnlineAccount : public QDeclarativeContactDetail
{
    Q_OBJECT
    Q_PROPERTY(QString accountUri READ accountUri WRITE setAccountUri NOTIFY fieldsChanged)
    Q_PROPERTY(QString serviceProvider READ serviceProvider WRITE  setServiceProvider NOTIFY fieldsChanged)
    Q_PROPERTY(QStringList capabilities READ capabilities WRITE  setCapabilities NOTIFY fieldsChanged)
    Q_PROPERTY(QVariantList subTypes READ subTypes WRITE  setSubTypes NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_ENUMS(OnlineAccountSubType)

    Q_CLASSINFO("DefaultProperty", "accountUri")
public:
    enum FieldType {
        AccountUri = 0,
        ServiceProvider,
        Capabilities,
        SubTypes
    };

    enum OnlineAccountSubType {
        Unknown = 0,
        Sip,
        SipVoip,
        Impp,
        VideoShare
    };

    QDeclarativeContactOnlineAccount(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactOnlineAccount());
        connect(this, SIGNAL(fieldsChanged()), SIGNAL(valueChanged()));
    }

    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactOnlineAccount;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case AccountUri:
            return QContactOnlineAccount::FieldAccountUri;
        case ServiceProvider:
            return QContactOnlineAccount::FieldServiceProvider;
        case Capabilities:
            return QContactOnlineAccount::FieldCapabilities;
        case SubTypes:
            return QContactOnlineAccount::FieldSubTypes;
        default:
            break;
        }
        //qWarning
        return QString();
    }
    void setAccountUri(const QString& v)
    {
        if (!readOnly() && v != accountUri()) {
            detail().setValue(QContactOnlineAccount::FieldAccountUri, v);
            emit fieldsChanged();
        }
    }
    QString accountUri() const {return detail().value(QContactOnlineAccount::FieldAccountUri);}

    void setServiceProvider(const QString& v)
    {
        if (!readOnly() && v != serviceProvider()) {
            detail().setValue(QContactOnlineAccount::FieldServiceProvider, v);
            emit fieldsChanged();
        }
    }
    QString serviceProvider() const {return detail().value(QContactOnlineAccount::FieldServiceProvider);}

    void setCapabilities(const QStringList& v)
    {
        if (!readOnly() && v.toSet() != capabilities().toSet()) {
            detail().setValue(QContactOnlineAccount::FieldCapabilities, v);
            emit fieldsChanged();
        }
    }
    QStringList capabilities() const {return detail().value<QStringList>(QContactOnlineAccount::FieldCapabilities);}

    void setSubTypes(const QVariantList& v)
    {
        QStringList savedList;
        foreach (const QVariant subType, v) {
            switch (static_cast<OnlineAccountSubType>(subType.value<int>()))
            {
            case Sip:
                savedList << QContactOnlineAccount::SubTypeSip;
                break;
            case SipVoip:
                savedList << QContactOnlineAccount::SubTypeSipVoip;
                break;
            case Impp:
                savedList << QContactOnlineAccount::SubTypeImpp;
                break;
            case VideoShare:
                savedList << QContactOnlineAccount::SubTypeVideoShare;
                break;
            default:
                //qWarning unkown subtype
                break;

            }
        }
        QStringList oldList = detail().value<QStringList>(QContactOnlineAccount::FieldSubTypes);
        if (!readOnly() && oldList.toSet() != savedList.toSet()) {
            detail().setValue(QContactOnlineAccount::FieldSubTypes, savedList);
            emit fieldsChanged();
        }
    }

    QVariantList subTypes() const
    {
        QVariantList returnList;
        QStringList savedList = detail().value<QStringList>(QContactOnlineAccount::FieldSubTypes);
        foreach (const QString& subType, savedList) {
            if (subType == QContactOnlineAccount::SubTypeSip)
                returnList << static_cast<int>(Sip);
            else if (subType == QContactOnlineAccount::SubTypeSipVoip)
                returnList << static_cast<int>(SipVoip);
            else if (subType == QContactOnlineAccount::SubTypeImpp)
                returnList << static_cast<int>(Impp);
            else if (subType == QContactOnlineAccount::SubTypeVideoShare)
                returnList << static_cast<int>(VideoShare);
            else {
                //qWarning unknown sub type
            }
        }
        return returnList;
    }

signals:
    void fieldsChanged();
};
QML_DECLARE_TYPE(QDeclarativeContactOnlineAccount)

#endif

