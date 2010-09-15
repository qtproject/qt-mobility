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
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldRecurrenceRules;
    static const QLatin1Constant FieldExceptionRules;
    static const QLatin1Constant FieldRecurrenceDates;
    static const QLatin1Constant FieldExceptionDates;
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
        for (int i = 0; i < rrules.count(); i++) {
            saveList << rrules.at(i).variantValues();
        }
        setValue(FieldRecurrenceRules, saveList);
    }

    QList<QOrganizerItemRecurrenceRule> recurrenceRules() const
    {
        QList<QOrganizerItemRecurrenceRule> retn;
        QVariantList loadList = variantValue(FieldRecurrenceRules).toList();
        for (int i = 0; i < loadList.count(); i++) {
            QVariantMap ruleMap = loadList.at(i).toMap();
            retn << QOrganizerItemRecurrenceRule::fromVariantValues(ruleMap);
        }
        return retn;
    }

    void setRecurrenceDates(const QList<QDate>& rdates)
    {
        QVariantList saveList;
        for (int i = 0; i < rdates.count(); i++) {
            saveList << rdates.at(i);
        }
        setValue(FieldRecurrenceDates, saveList);
    }

    QList<QDate> recurrenceDates() const
    {
        QList<QDate> retn;
        QVariantList loadList = variantValue(FieldRecurrenceDates).toList();
        for (int i = 0; i < loadList.count(); i++) {
            retn << loadList.at(i).toDate();
        }
        return retn;
    }

    void setExceptionRules(const QList<QOrganizerItemRecurrenceRule>& xrules)
    {
        QVariantList saveList;
        for (int i=0; i < xrules.count(); i++) {
            saveList << xrules.at(i).variantValues();
        }
        setValue(FieldExceptionRules, saveList);
    }

    QList<QOrganizerItemRecurrenceRule> exceptionRules() const
    {
        QList<QOrganizerItemRecurrenceRule> retn;
        QVariantList loadList = variantValue(FieldExceptionRules).toList();
        for(int i = 0; i < loadList.count(); i++) {
            QVariantMap ruleMap = loadList.at(i).toMap();
            retn << QOrganizerItemRecurrenceRule::fromVariantValues(ruleMap);
        }
        return retn;
    }

    void setExceptionDates(const QList<QDate>& xdates)
    {
        QVariantList saveList;
        for(int i = 0; i < xdates.count(); i++) {
            saveList << xdates.at(i);
        }
        setValue(FieldExceptionDates, saveList);
    }

    QList<QDate> exceptionDates() const
    {
        QList<QDate> retn;
        QVariantList loadList = variantValue(FieldExceptionDates).toList();
        for( int i = 0; i < loadList.count(); i++) {
            retn << loadList.at(i).toDate();
        }
        return retn;
    }
};

QTM_END_NAMESPACE

#endif

