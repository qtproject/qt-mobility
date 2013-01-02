/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVCARDRESTOREHANDLER_P_H
#define QVCARDRESTOREHANDLER_P_H

#include "qmobilityglobal.h"
#include "qlatin1constant.h"
#include "qcontactdetail.h"

QTM_BEGIN_NAMESPACE
class QVersitProperty;

// This is a map from Versit group names to the details that were generated from properties with the
// said groups.  Multiple details can be associated with a single group.
class DetailGroupMap
{
public:
    QList<QContactDetail> detailsInGroup(const QString& groupName) const;
    void insert(const QString& groupName, const QContactDetail& detail);
    void update(const QContactDetail& detail);
    void clear();

private:
    QHash<int, QString> mDetailGroupName; // detailid -> group name
    QHash<int, QContactDetail> mDetailById; // detailid -> detail
};

class QVCardRestoreHandler
{
public:
    bool propertyProcessed(const QVersitProperty& property,
                           QList<QContactDetail>* updatedDetails);
    void documentProcessed();

    Q_DECLARE_LATIN1_CONSTANT(PropertyName, "X-NOKIA-QCONTACTFIELD");
    Q_DECLARE_LATIN1_CONSTANT(DetailDefinitionParameter, "DETAIL");
    Q_DECLARE_LATIN1_CONSTANT(FieldParameter, "FIELD");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameter, "DATATYPE");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterVariant, "VARIANT");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterDate, "DATE");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterDateTime, "DATETIME");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterTime, "TIME");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterBool, "BOOL");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterInt, "INT");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterUInt, "UINT");
    Q_DECLARE_LATIN1_CONSTANT(DatatypeParameterUrl, "URL");
    Q_DECLARE_LATIN1_CONSTANT(GroupPrefix, "G");

private:
    static QVariant deserializeValue(const QVersitProperty& property);
    DetailGroupMap mDetailGroupMap; // remembers which details came from which groups
};

QTM_END_NAMESPACE

#endif
