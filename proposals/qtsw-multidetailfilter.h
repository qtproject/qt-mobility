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

#ifndef QTSWMULTIDETAILFILTER_H
#define QTSWMULTIDETAILFILTER_H

// It's possible that QCDF would be replaced by this.

// usual tree of expressions etc

#include "qcontactdetail.h"

class QContactDetailFilterExpression
{
public:
    enum ExpressionType {InvalidExpression, LogicalExpression, NegateExpression, ValueExpression};

    ExpressionType type();

    QContactDetailFilterExpression operator~();
    QContactDetailFilterExpression operator&(const QContactDetailFilterExpression& other);
    QContactDetailFilterExpression operator|(const QContactDetailFilterExpression& other);
};

class QContactDetailFilterLogicalExpression : public QContactDetailFilterExpression
{
public:
    enum LogicalOperation {AndOperation, OrOperation};

    LogicalOperation operation() const;
    void setOperation(LogicalOperation operation);

    QList<QContactDetailFilterExpression> expressions() const;
    void setExpressions(const QList<QContactDetailFilterExpression>& list);
};

class QContactDetailFilterNegateExpression : public QContactDetailFilterExpression
{
public:
    QContactDetailFilterExpression expression() const;
    void setExpression(const QContactDetailFilterExpression& expression);
};

class QContactDetailFilterValueExpression : public QContactDetailFilterExpression
{
public:
    QString fieldName() const;
    void setFieldName(const QString& fieldName);

    QVariant value();
    void setValue(const QVariant& value);

    Qt::MatchFlags matchFlags() const;
    void setMatchFlags(Qt::MatchFlags flags);

    // This should be rolled into match flags or setvalue
    bool valueExcluded() const;
    void setValueExcluded(bool excludeFlag);
};

class QContactMultiDetailFilter : public QContactFilter
{
public:
    QContactMultiDetailFilter();
    QContactMultiDetailFilter(const QContactMultiDetailFilter& other);

    /* Mutators */
    void setDetailDefinitionName(const QString& definition);

    /* Accessors */
    QString detailDefinitionName() const;

    /* and the actual expression */
    QContactDetailFilterExpression expression() const;
    void setExpression(const QContactDetailFilterExpression& expression);

    // Helper function for replacing QCDF
    void setExpression(const QString& fieldName, const QVariant& value = QVariant(), Qt::MatchFlags = Qt::MatchExactly);

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactMultiDetailFilter)
};



/* Sample code */

void sampleCode()
{
    /* Find all the contacts with "home mobile" numbers */
    QContactMultiDetailFilter qcmdf;
    qcmdf.setDetailDefinitionName(QContactPhoneNumber::DefinitionName);

    QContactDetailFilterValueExpression homebit;
    homebit.setFieldName(QContactDetail::FieldContext);
    homebit.setValue(QContactDetail::ContextHome);

    QContactDetailFilterValueExpression mobilebit;
    mobilebit.setFieldName(QContactPhoneNumber::FieldSubType);
    mobilebit.setValue(QContactPhoneNumber::SubTypeMobile);

    qcmdf.setExpression(homebit & mobilebit);

    QList<QUniqueId> ids = manager->contacts(qcmdf);
}

void sampleCode()
{
    /* Find all the contacts that have phonenumber that aren't only faxes */
    QContactMultiDetailFilter qcmdf;
    qcmdf.setDetailDefinitionName(QContactPhoneNumber::DefinitionName);

    QContactDetailFilterValueExpression faxbit;
    faxbit.setFieldName(QContactPhoneNumber::FieldSubType);
    faxbit.setValue(QContactPhoneNumber::SubTypeFax);
    faxbit.setValueExcluded(true);

    qcmdf.setExpression(faxbit);

    QList<QUniqueId> ids = manager->contacts(qcmdf);
}

#endif // QTSWMULTIDETAILFILTER_H
