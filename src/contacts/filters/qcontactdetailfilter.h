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

#ifndef QCONTACTDETAILFILTER_H
#define QCONTACTDETAILFILTER_H

#include "qcontactfilter.h"

QTM_BEGIN_NAMESPACE

class QContactDetailFilterPrivate;
class Q_CONTACTS_EXPORT QContactDetailFilter : public QContactFilter
{
public:
    QContactDetailFilter();
    QContactDetailFilter(const QContactFilter& other);

    /* Mutators */
    void setDetailDefinitionName(const QString& definition, const QString& fieldName = QString());
    void setMatchFlags(QContactFilter::MatchFlags flags);

    /* Filter Criterion */
    void setValue(const QVariant& value);

    /* Accessors */
    QString detailDefinitionName() const;
    QString detailFieldName() const;
    QContactFilter::MatchFlags matchFlags() const;

    QVariant value() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactDetailFilter)
};

QTM_END_NAMESPACE

#endif
