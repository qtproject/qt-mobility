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

#ifndef QORGANIZERITEMRECURRENCE_H
#define QORGANIZERITEMRECURRENCE_H

#include <QString>

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritem.h"
#include "qorganizeritemrecurrencerule.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerItemRecurrence : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldRecurrenceRules;
    const char* FieldExceptionRules;
    const char* FieldRecurrenceDates;
    const char* FieldExceptionDates;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerItemRecurrence, "Recurrence")
    Q_DECLARE_LATIN1_CONSTANT(FieldRecurrenceRules, "RecurrenceRules");
    Q_DECLARE_LATIN1_CONSTANT(FieldExceptionRules, "ExceptionRules");
    Q_DECLARE_LATIN1_CONSTANT(FieldRecurrenceDates, "RecurrenceDates");
    Q_DECLARE_LATIN1_CONSTANT(FieldExceptionDates, "ExceptionDates");
#endif


    void setRecurrenceRules(const QList<QOrganizerItemRecurrenceRule>& rrules)
    {
        QVariantList saveList;
        foreach (const QOrganizerItemRecurrenceRule& rule, rrules) {
            saveList << rule.variantValues();
        }
        setValue(FieldRecurrenceRules, saveList);
    }
    QList<QOrganizerItemRecurrenceRule> recurrenceRules() const
    {
        QList<QOrganizerItemRecurrenceRule> retn;
        QVariantList loadList = variantValue(FieldRecurrenceRules).toList();
        foreach (const QVariant& currRule, loadList) {
            QVariantMap ruleMap = currRule.toMap();
            retn << QOrganizerItemRecurrenceRule::fromVariantValues(ruleMap);
        }
        return retn;
    }

    void setRecurrenceDates(const QList<QDateTime>& rdates)
    {
        QVariantList saveList;
        foreach (const QDateTime& currDate, rdates) {
            saveList << currDate;
        }
        setValue(FieldRecurrenceDates, saveList);
    }
    QList<QDateTime> recurrenceDates() const
    {
        QList<QDateTime> retn;
        QVariantList loadList = variantValue(FieldRecurrenceDates).toList();
        foreach (const QVariant& currDate, loadList) {
            retn << currDate.toDateTime();
        }
        return retn;
    }

    void setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& xrules)
    {
        QVariantList saveList;
        foreach (const QOrganizerItemRecurrenceRule& rule, xrules) {
            saveList << rule.variantValues();
        }
        setValue(FieldExceptionRules, saveList);
    }
    QList<QOrganizerItemRecurrenceRule> exceptionRules() const
    {
        QList<QOrganizerItemRecurrenceRule> retn;
        QVariantList loadList = variantValue(FieldExceptionRules).toList();
        foreach (const QVariant& currRule, loadList) {
            QVariantMap ruleMap = currRule.toMap();
            retn << QOrganizerItemRecurrenceRule::fromVariantValues(ruleMap);
        }
        return retn;
    }

    void setExceptionDates(const QList<QDateTime>& xdates)
    {
        QVariantList saveList;
        foreach (const QDateTime& currDate, xdates) {
            saveList << currDate;
        }
        setValue(FieldExceptionDates, saveList);
    }
    QList<QDateTime> exceptionDates() const
    {
        QList<QDateTime> retn;
        QVariantList loadList = variantValue(FieldExceptionDates).toList();
        foreach (const QVariant& currDate, loadList) {
            retn << currDate.toDateTime();
        }
        return retn;
    }
};

QTM_END_NAMESPACE

#endif

