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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QCONTACTNAME_H
#define QCONTACTNAME_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

class QTCONTACTS_EXPORT QContactName : public QContactDetail
{
public:
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactName, "Name")

    static const char DefinitionId[]; // == staticType() == "Name"

    static const char FieldPrefix[];
    static const char FieldFirst[];
    static const char FieldMiddle[];
    static const char FieldLast[];
    static const char FieldSuffix[];

    QString prefix() const {return value(QLatin1String(FieldPrefix));}
    QString first() const {return value(QLatin1String(FieldFirst));}
    QString middle() const {return value(QLatin1String(FieldMiddle));}
    QString last() const {return value(QLatin1String(FieldLast));}
    QString suffix() const {return value(QLatin1String(FieldSuffix));}

    void setPrefix(const QString& prefix) {setValue(QLatin1String(FieldPrefix), prefix);}
    void setFirst(const QString& first) {setValue(QLatin1String(FieldFirst), first);}
    void setMiddle(const QString& middle) {setValue(QLatin1String(FieldMiddle), middle);}
    void setLast(const QString& last) {setValue(QLatin1String(FieldLast), last);}
    void setSuffix(const QString& suffix) {setValue(QLatin1String(FieldSuffix), suffix);}
};

#endif
