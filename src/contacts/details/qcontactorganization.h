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

#ifndef QCONTACTORGANIZATION_H
#define QCONTACTORGANIZATION_H

#include <QString>
#include <QUrl>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactOrganization : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldName;
    static const QLatin1Constant FieldLogoUrl;
    static const QLatin1Constant FieldDepartment;
    static const QLatin1Constant FieldLocation;
    static const QLatin1Constant FieldRole;
    static const QLatin1Constant FieldTitle;
    static const QLatin1Constant FieldAssistantName;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactOrganization, "Organization")
    Q_DECLARE_LATIN1_CONSTANT(FieldName, "Name");
    Q_DECLARE_LATIN1_CONSTANT(FieldLogoUrl, "LogoUrl");
    Q_DECLARE_LATIN1_CONSTANT(FieldDepartment, "Department");
    Q_DECLARE_LATIN1_CONSTANT(FieldLocation, "Location");
    Q_DECLARE_LATIN1_CONSTANT(FieldRole, "Role");
    Q_DECLARE_LATIN1_CONSTANT(FieldTitle, "Title");
    Q_DECLARE_LATIN1_CONSTANT(FieldAssistantName, "AssistantName");
#endif

    void setName(const QString& name) {setValue(FieldName, name);}
    QString name() const {return value(FieldName);}
    void setLogoUrl(const QUrl& logo) {setValue(FieldLogoUrl, logo);}
    QUrl logoUrl() const {return value(FieldLogoUrl);}
    void setDepartment(const QStringList& department) {setValue(FieldDepartment, department);}
    QStringList department() const {return value<QStringList>(FieldDepartment);}
    void setLocation(const QString& location) {setValue(FieldLocation, location);}
    QString location() const {return value(FieldLocation);}
    void setRole(const QString& role) {setValue(FieldRole, role);}
    QString role() const {return value(FieldRole);}
    void setTitle(const QString& title) {setValue(FieldTitle, title);}
    QString title() const {return value(FieldTitle);}
    void setAssistantName(const QString& assistantName) {setValue(FieldAssistantName, assistantName);}
    QString assistantName() const {return value(FieldAssistantName);}
};

QTM_END_NAMESPACE

#endif
