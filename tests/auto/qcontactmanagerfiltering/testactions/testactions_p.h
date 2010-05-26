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

#ifndef QCONTACTTESTACTIONS_P_H
#define QCONTACTTESTACTIONS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtcontacts.h"

#include <QSharedData>
#include <QString>
#include <QVariantMap>

QTM_USE_NAMESPACE

class DummyAction : public QContactAction
{
public:
    QVariantMap metaData() const {return QVariantMap();}

    bool invokeAction(const QContact&, const QContactDetail&, const QVariantMap&)
    {
        // Well, do something
        emit stateChanged(QContactAction::FinishedState);
        return true;
    }

    QVariantMap results() const
    {
        return QVariantMap();
    }

    State state() const {return QContactAction::FinishedState;}

};

class QIntegerAction : public DummyAction
{
    Q_OBJECT

public:
    QIntegerAction() {}
    ~QIntegerAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Number", "IntegerCo", 5); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Integer");
        df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df.setValue(value);
        return df;
    }
    bool isDetailSupported(const QContactDetail &detail, const QContact &) const
    {
        return detail.definitionName() == defAndFieldNamesForTypeForActions.value("Integer").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Integer").second).isNull();
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        return contact.details(defAndFieldNamesForTypeForActions.value("Integer").first);
    }
};

/* Static actions for testing matching */
class QPhoneNumberAction : public DummyAction
{
    Q_OBJECT

public:
    QPhoneNumberAction() {}
    ~QPhoneNumberAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("PhoneNumber", "PhoneNumberCo", 4); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        df.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
        df.setValue(value);
        return df;
    }
    bool isDetailSupported(const QContactDetail& detail, const QContact&) const
    {
        return detail.definitionName() == QContactPhoneNumber::DefinitionName
                && !detail.variantValue(QContactPhoneNumber::FieldNumber).isNull();
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        return contact.details(QContactPhoneNumber::DefinitionName);
    }
};

/* Static actions for testing matching */
class QDateAction : public DummyAction
{
    Q_OBJECT

public:
    QDateAction() {}
    ~QDateAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Date", "DateCo", 9); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Date");
        df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df.setValue(value);
        return df;
    }

    bool isDetailSupported(const QContactDetail &detail, const QContact &) const
    {
        return detail.definitionName() == defAndFieldNamesForTypeForActions.value("Date").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Date").second).isNull();
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        return contact.details(defAndFieldNamesForTypeForActions.value("Date").first);
    }
};

class QNumberAction : public DummyAction
{
    Q_OBJECT

public:
    QNumberAction() {}
    ~QNumberAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Number", "NumberCo", 42); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        QContactDetailFilter df;
        QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Double");
        df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df.setValue(value);

        QContactDetailFilter df2;
        defAndFieldName = defAndFieldNamesForTypeForActions.value("Integer");
        df2.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
        df2.setValue(value);

        /* We like either doubles or integers */
        return df | df2;
    }

    bool isDetailSupported(const QContactDetail &detail, const QContact &) const
    {
        if (detail.definitionName() == defAndFieldNamesForTypeForActions.value("Double").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Double").second).isNull()) {
            return true;
        }

        if (detail.definitionName() == defAndFieldNamesForTypeForActions.value("Integer").first
                && !detail.variantValue(defAndFieldNamesForTypeForActions.value("Integer").second).isNull()) {
            return true;
        }

        return false;
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        QList<QContactDetail> retn = contact.details(defAndFieldNamesForTypeForActions.value("Integer").first);
        retn.append(contact.details(defAndFieldNamesForTypeForActions.value("Double").first));
        return retn;
    }
};

class QBooleanAction : public DummyAction
{
    Q_OBJECT

public:
    QBooleanAction() {}
    ~QBooleanAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Boolean", "BooleanCo", 3); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        if (value.isNull() || (value.type() == QVariant::Bool && value.toBool() == true)) {
            /* This only likes bools that are true */
            QContactDetailFilter df;
            QPair<QString, QString> defAndFieldName = defAndFieldNamesForTypeForActions.value("Bool");
            df.setDetailDefinitionName(defAndFieldName.first, defAndFieldName.second);
            df.setValue(true);
            return df;
        } else {
            return QContactInvalidFilter();
        }
    }
    bool isDetailSupported(const QContactDetail &detail, const QContact &) const
    {
        return detail.definitionName() == defAndFieldNamesForTypeForActions.value("Bool").first
                && (detail.value<bool>(defAndFieldNamesForTypeForActions.value("Bool").second) == true);
    }
    QList<QContactDetail> supportedDetails(const QContact& contact) const
    {
        return contact.details(defAndFieldNamesForTypeForActions.value("Bool").first);
    }
};

class RecursiveAction : public DummyAction
{
    Q_OBJECT

public:
    RecursiveAction() {}
    ~RecursiveAction() {}

    QContactActionDescriptor actionDescriptor() const { return QContactActionDescriptor("Recursive", "RecursiveCo", 3); }

    QContactFilter contactFilter(const QVariant& value) const
    {
        /* Return a filter that selects us again.. */
        QContactActionFilter af;
        af.setActionName("Recursive");
        af.setValue(value);
        return af;
    }
    bool isDetailSupported(const QContactDetail&, const QContact&) const
    {
        return false;
    }
    QList<QContactDetail> supportedDetails(const QContact&) const
    {
        return QList<QContactDetail>();
    }
};

class AnotherRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    int implementationVersion() const {return 4;}
    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("Recursive");
        return af;
    }
};

/* A pair that reference each other */
class PairRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("PairRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("AnotherPairRecursive");
        return af;
    }
};

class AnotherPairRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("AnotherPairRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af;
    }
};

class IntersectionRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("IntersectionRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af & QContactFilter() & af;
    }
};

class UnionRecursiveAction : public RecursiveAction {
    Q_OBJECT

public:
    QContactActionDescriptor actionDescriptor() const
    {
        QContactActionDescriptor ret = RecursiveAction::actionDescriptor();
        ret.setActionName("UnionRecursive");
        return ret;
    }

    QContactFilter contactFilter(const QVariant& value) const
    {
        Q_UNUSED(value);
        /* Slightly looser filter */
        QContactActionFilter af;
        af.setActionName("PairRecursive");
        return af | QContactInvalidFilter() | af;
    }
};

#endif
