/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTPHONENUMBER_H
#define QCONTACTPHONENUMBER_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontactfilter.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
//! [0]
class Q_CONTACTS_EXPORT QContactPhoneNumber : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldNumber;
    const char* FieldSubTypes;
    const char* SubTypeLandline;
    const char* SubTypeMobile;
    const char* SubTypeFacsimile;
    const char* SubTypePager;
    const char* SubTypeVoice;
    const char* SubTypeModem;
    const char* SubTypeVideo;
    const char* SubTypeCar;
    const char* SubTypeBulletinBoardSystem;
    const char* SubTypeMessagingCapable;
    const char* SubTypeAssistant;
    const char* SubTypeDtmfMenu;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactPhoneNumber, "PhoneNumber")
    Q_DECLARE_LATIN1_LITERAL(FieldNumber, "PhoneNumber");
    Q_DECLARE_LATIN1_LITERAL(FieldSubTypes, "SubTypes");
    Q_DECLARE_LATIN1_LITERAL(SubTypeLandline, "Landline");
    Q_DECLARE_LATIN1_LITERAL(SubTypeMobile, "Mobile");
    Q_DECLARE_LATIN1_LITERAL(SubTypeFacsimile, "Facsimile");
    Q_DECLARE_LATIN1_LITERAL(SubTypePager, "Pager");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVoice, "Voice");
    Q_DECLARE_LATIN1_LITERAL(SubTypeModem, "Modem");
    Q_DECLARE_LATIN1_LITERAL(SubTypeVideo, "Video");
    Q_DECLARE_LATIN1_LITERAL(SubTypeCar, "Car");
    Q_DECLARE_LATIN1_LITERAL(SubTypeBulletinBoardSystem, "BulletinBoardSystem");
    Q_DECLARE_LATIN1_LITERAL(SubTypeMessagingCapable, "MessagingCapable");
    Q_DECLARE_LATIN1_LITERAL(SubTypeAssistant, "Assistant");
    Q_DECLARE_LATIN1_LITERAL(SubTypeDtmfMenu, "DtmfMenu");
#endif

    void setNumber(const QString& number) {setValue(FieldNumber, number);}
    QString number() const {return value(FieldNumber);}

    void setSubTypes(const QStringList& subTypes) {setValue(FieldSubTypes, subTypes);}
    void setSubTypes(const QString& subType) {setValue(FieldSubTypes, QStringList(subType));}
    QStringList subTypes() const {return value<QStringList>(FieldSubTypes);}

    // Convenience filter
    static QContactFilter match(const QString& number);
};
//! [0]

QTM_END_NAMESPACE

#endif

