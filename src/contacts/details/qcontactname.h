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


#ifndef QCONTACTNAME_H
#define QCONTACTNAME_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

class Q_CONTACTS_EXPORT QContactName : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldPrefix;
    const char* FieldFirst;
    const char* FieldMiddle;
    const char* FieldLast;
    const char* FieldSuffix;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactName, "Name")
    Q_DECLARE_LATIN1_LITERAL(FieldPrefix, "Prefix");
    Q_DECLARE_LATIN1_LITERAL(FieldFirst, "First");
    Q_DECLARE_LATIN1_LITERAL(FieldMiddle, "Middle");
    Q_DECLARE_LATIN1_LITERAL(FieldLast, "Last");
    Q_DECLARE_LATIN1_LITERAL(FieldSuffix, "Suffix");
#endif

    QString prefix() const {return value(FieldPrefix);}
    QString first() const {return value(FieldFirst);}
    QString middle() const {return value(FieldMiddle);}
    QString last() const {return value(FieldLast);}
    QString suffix() const {return value(FieldSuffix);}

    void setPrefix(const QString& prefix) {setValue(FieldPrefix, prefix);}
    void setFirst(const QString& first) {setValue(FieldFirst, first);}
    void setMiddle(const QString& middle) {setValue(FieldMiddle, middle);}
    void setLast(const QString& last) {setValue(FieldLast, last);}
    void setSuffix(const QString& suffix) {setValue(FieldSuffix, suffix);}
};

#endif
