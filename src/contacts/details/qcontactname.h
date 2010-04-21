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


#ifndef QCONTACTNAME_H
#define QCONTACTNAME_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontactfilter.h"

QTM_BEGIN_NAMESPACE

class Q_CONTACTS_EXPORT QContactName : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldPrefix;
    static const QLatin1Constant FieldFirstName;
    static const QLatin1Constant FieldMiddleName;
    static const QLatin1Constant FieldLastName;
    static const QLatin1Constant FieldSuffix;
    static const QLatin1Constant FieldCustomLabel;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactName, "Name")
    Q_DECLARE_LATIN1_CONSTANT(FieldPrefix, "Prefix");
    Q_DECLARE_LATIN1_CONSTANT(FieldFirstName, "FirstName");
    Q_DECLARE_LATIN1_CONSTANT(FieldMiddleName, "MiddleName");
    Q_DECLARE_LATIN1_CONSTANT(FieldLastName, "LastName");
    Q_DECLARE_LATIN1_CONSTANT(FieldSuffix, "Suffix");
    Q_DECLARE_LATIN1_CONSTANT(FieldCustomLabel, "CustomLabel");
#endif

    QString prefix() const {return value(FieldPrefix);}
    QString firstName() const {return value(FieldFirstName);}
    QString middleName() const {return value(FieldMiddleName);}
    QString lastName() const {return value(FieldLastName);}
    QString suffix() const {return value(FieldSuffix);}
    QString customLabel() const{return value(FieldCustomLabel);}

    void setPrefix(const QString& prefix) {setValue(FieldPrefix, prefix);}
    void setFirstName(const QString& firstName) {setValue(FieldFirstName, firstName);}
    void setMiddleName(const QString& middleName) {setValue(FieldMiddleName, middleName);}
    void setLastName(const QString& lastName) {setValue(FieldLastName, lastName);}
    void setSuffix(const QString& suffix) {setValue(FieldSuffix, suffix);}
    void setCustomLabel(const QString& customLabel) {setValue(FieldCustomLabel, customLabel);}

    // Convenience filter
    static QContactFilter match(const QString& name);
    static QContactFilter match(const QString& firstName, const QString& lastName);
};

QTM_END_NAMESPACE

#endif
