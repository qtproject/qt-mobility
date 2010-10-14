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


#ifndef QDECLARATIVECONTACTPHONENUMBER_H
#define QDECLARATIVECONTACTPHONENUMBER_H

#include "qdeclarativecontactdetail_p.h"
#include "qcontactphonenumber.h"

class  QDeclarativeContactPhoneNumber : public QDeclarativeContactDetail
{
    Q_OBJECT

    Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY fieldsChanged)
    Q_PROPERTY(QVariantList subTypes READ subTypes WRITE setSubTypes NOTIFY fieldsChanged)
    Q_ENUMS(FieldType)
    Q_ENUMS(SubType)

    Q_CLASSINFO("DefaultProperty", "number")
public:
    enum FieldType {
        FieldNumber = 0,
        FieldSubTypes
    };

    enum SubType {
        SubTypeLandline = 0,
        SubTypeMobile,
        SubTypeFax,
        SubTypePager,
        SubTypeVoice,
        SubTypeModem,
        SubTypeVideo,
        SubTypeCar,
        SubTypeBulletinBoardSystem,
        SubTypeMessagingCapable,
        SubTypeAssistant,
        SubTypeDtmfMenu
    };

    QDeclarativeContactPhoneNumber(QObject* parent = 0)
        :QDeclarativeContactDetail(parent)
    {
        setDetail(QContactPhoneNumber());
        connect(this, SIGNAL((fieldsChanged)), SIGNAL(valueChanged()));
    }
    ContactDetailType detailType() const
    {
        return QDeclarativeContactDetail::ContactPhoneNumber;
    }
    static QString fieldNameFromFieldType(int fieldType)
    {
        switch (fieldType) {
        case FieldNumber:
            return QContactPhoneNumber::FieldNumber;
        case FieldSubTypes:
            return QContactPhoneNumber::FieldSubTypes;
        default:
            break;
        }
        return "";
    }
    void setNumber(const QString& number) {if (!readOnly()) detail().setValue(QContactPhoneNumber::FieldNumber, number);}
    QString number() const {return detail().value(QContactPhoneNumber::FieldNumber);}


    void setSubTypes(const QVariantList& subTypes)
    {
        QStringList savedList;
        foreach (const QVariant subType, subTypes) {
            switch (static_cast<SubType>(subType.value<int>()))
            {
            case SubTypeLandline:
                savedList << QContactPhoneNumber::SubTypeLandline;
                break;
            case SubTypeMobile:
                savedList << QContactPhoneNumber::SubTypeMobile;
                break;
            case SubTypeFax:
                savedList << QContactPhoneNumber::SubTypeFax;
                break;
            case SubTypePager:
                savedList << QContactPhoneNumber::SubTypePager;
                break;
            case SubTypeVoice:
                savedList << QContactPhoneNumber::SubTypeVoice;
                break;
            case SubTypeModem:
                savedList << QContactPhoneNumber::SubTypeModem;
                break;
            case SubTypeVideo:
                savedList << QContactPhoneNumber::SubTypeVideo;
                break;
            case SubTypeCar:
                savedList << QContactPhoneNumber::SubTypeCar;
                break;
            case SubTypeBulletinBoardSystem:
                savedList << QContactPhoneNumber::SubTypeBulletinBoardSystem;
                break;
            case SubTypeMessagingCapable:
                savedList << QContactPhoneNumber::SubTypeMessagingCapable;
                break;
            case SubTypeAssistant:
                savedList << QContactPhoneNumber::SubTypeAssistant;
                break;
            case SubTypeDtmfMenu:
                savedList << QContactPhoneNumber::SubTypeDtmfMenu;
                break;
            default:
                break;

            }
        }
        if (!readOnly()) detail().setValue(QContactPhoneNumber::FieldSubTypes, savedList);
    }

    QVariantList subTypes() const
    {
        QVariantList returnList;
        QStringList savedList = detail().value<QStringList>(QContactPhoneNumber::FieldSubTypes);
        foreach (const QString& subType, savedList) {
            if (subType == QContactPhoneNumber::SubTypeLandline)
                returnList << static_cast<int>(SubTypeLandline);
            else if (subType == QContactPhoneNumber::SubTypeMobile)
                returnList << static_cast<int>(SubTypeMobile);
            else if (subType == QContactPhoneNumber::SubTypeFax)
                returnList << static_cast<int>(SubTypeFax);
            else if (subType == QContactPhoneNumber::SubTypePager)
                returnList << static_cast<int>(SubTypePager);
            else if (subType == QContactPhoneNumber::SubTypeVoice)
                returnList << static_cast<int>(SubTypeVoice);
            else if (subType == QContactPhoneNumber::SubTypeModem)
                returnList << static_cast<int>(SubTypeModem);
            else if (subType == QContactPhoneNumber::SubTypeVideo)
                returnList << static_cast<int>(SubTypeVideo);
            else if (subType == QContactPhoneNumber::SubTypeCar)
                returnList << static_cast<int>(SubTypeCar);
            else if (subType == QContactPhoneNumber::SubTypeBulletinBoardSystem)
                returnList << static_cast<int>(SubTypeBulletinBoardSystem);
            else if (subType == QContactPhoneNumber::SubTypeMessagingCapable)
                returnList << static_cast<int>(SubTypeMessagingCapable);
            else if (subType == QContactPhoneNumber::SubTypeAssistant)
                returnList << static_cast<int>(SubTypeAssistant);
            else if (subType == QContactPhoneNumber::SubTypeDtmfMenu)
                returnList << static_cast<int>(SubTypeDtmfMenu);
        }
        return returnList;
    }

signals:
    void fieldsChanged();

};
QML_DECLARE_TYPE(QDeclarativeContactPhoneNumber)
#endif

