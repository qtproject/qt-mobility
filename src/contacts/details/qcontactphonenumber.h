/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldNumber;
    static const QLatin1Constant FieldSubTypes;
    static const QLatin1Constant SubTypeLandline;
    static const QLatin1Constant SubTypeMobile;
    static const QLatin1Constant SubTypeFax;
    static const QLatin1Constant SubTypePager;
    static const QLatin1Constant SubTypeVoice;
    static const QLatin1Constant SubTypeModem;
    static const QLatin1Constant SubTypeVideo;
    static const QLatin1Constant SubTypeCar;
    static const QLatin1Constant SubTypeBulletinBoardSystem;
    static const QLatin1Constant SubTypeMessagingCapable;
    static const QLatin1Constant SubTypeAssistant;
    static const QLatin1Constant SubTypeDtmfMenu;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactPhoneNumber, "PhoneNumber")
    Q_DECLARE_LATIN1_CONSTANT(FieldNumber, "PhoneNumber");
    Q_DECLARE_LATIN1_CONSTANT(FieldSubTypes, "SubTypes");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeLandline, "Landline");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeMobile, "Mobile");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeFax, "Fax");
    Q_DECLARE_LATIN1_CONSTANT(SubTypePager, "Pager");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeVoice, "Voice");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeModem, "Modem");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeVideo, "Video");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeCar, "Car");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeBulletinBoardSystem, "BulletinBoardSystem");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeMessagingCapable, "MessagingCapable");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeAssistant, "Assistant");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeDtmfMenu, "DtmfMenu");
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

