/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTPHONENUMBER_H
#define QCONTACTPHONENUMBER_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

/* Leaf class */
//! [0]
class QTCONTACTS_EXPORT QContactPhoneNumber : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldNumber;
    const char* AttributeSubTypeLandline;
    const char* AttributeSubTypeMobile;
    const char* AttributeSubTypeFacsimile;
    const char* AttributeSubTypePager;
    const char* AttributeSubTypeVoice;
    const char* AttributeSubTypeModem;
    const char* AttributeSubTypeVideo;
    const char* AttributeSubTypeCar;
    const char* AttributeSubTypeBulletinBoardSystem;
    const char* AttributeSubTypeMessagingCapable;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactPhoneNumber, "PhoneNumber")
    Q_DECLARE_LATIN1_LITERAL(FieldNumber, "PhoneNumber");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeLandline, "Landline");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeMobile, "Mobile");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeFacsimile, "Facsimile");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypePager, "Pager");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeVoice, "Voice");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeModem, "Modem");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeVideo, "Video");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeCar, "Car");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeBulletinBoardSystem, "BulletinBoardSystem");
    Q_DECLARE_LATIN1_LITERAL(AttributeSubTypeMessagingCapable, "MessagingCapable");
#endif

    void setNumber(const QString& number) {setValue(FieldNumber, number);}
    QString number() const {return value(FieldNumber);}
};
//! [0]

#endif

