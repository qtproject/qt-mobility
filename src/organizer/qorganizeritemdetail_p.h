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


#ifndef QORGANIZERITEMDETAIL_P_H
#define QORGANIZERITEMDETAIL_P_H

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

#include "qorganizeritemdetail.h"

#include <QSharedData>
#include <QString>
#include <QHash>

QTM_BEGIN_NAMESPACE

/*
  Yet another string class

  Mostly a wrapper around char *, where we do pointer equality checks before
  a strcmp, given our common use case of equivalent pointers.

  Also handles when things get passed in as a QString, by converting to Latin1
  and caching the result, and caching conversions to QString (hit by definitionName a lot)
*/
class QOrganizerItemStringHolder
{
public:
    QOrganizerItemStringHolder()
        : m_str(0)
    {
    }

    ~QOrganizerItemStringHolder()
    {
    }

    QOrganizerItemStringHolder(const QOrganizerItemStringHolder& other)
        : m_str(other.m_str)
    {
    }

    QOrganizerItemStringHolder& operator=(const QOrganizerItemStringHolder& other)
    {
        m_str = other.m_str; // checking for ==this is not worth the effort
        return *this;
    }

    QOrganizerItemStringHolder(const char *str)
        : m_str(str)
    {
    }

    QOrganizerItemStringHolder& operator=(const char *str)
    {
        m_str = str;
        return *this;
    }

    explicit QOrganizerItemStringHolder(const QString& str)
    {
        *this = str;
    }

    QOrganizerItemStringHolder& operator=(const QString& str)
    {
        m_str = s_allocated.value(str, 0);
        if (!m_str) {
            m_str = qstrdup(str.toLatin1().constData());
            s_allocated.insert(str, const_cast<char*>(m_str)); // it's my pointer
        }
        return *this;
    }

    bool operator==(const char* other) const
    {
        return other == m_str || (qstrcmp(other, m_str) == 0);
    }

    bool operator==(const QString& other) const
    {
        return (s_allocated.value(other, 0) == m_str) || (other == QLatin1String(m_str));
    }

    bool operator==(const QOrganizerItemStringHolder& other) const
    {
        return (other.m_str == m_str) || (qstrcmp(other.m_str, m_str) == 0);
    }

    QString toQString() const
    {
        QString s = s_qstrings.value(m_str);
        if (!s.isEmpty())
            return s;
        s = QString::fromLatin1(m_str);
        s_qstrings.insert(m_str, s);
        return s;
    }

public:
    // The only data we have
    const char* m_str;

    static QHash<QString, char*> s_allocated;
    static QHash<const char *, QString> s_qstrings;
};

uint qHash(const QOrganizerItemStringHolder& key);

class QOrganizerItemDetailPrivate : public QSharedData
{
public:
    QOrganizerItemDetailPrivate()
        : QSharedData(),
        m_id(lastDetailKey.fetchAndAddOrdered(1)),
        m_access(QOrganizerItemDetail::NoConstraint)
    {
    }

    QOrganizerItemDetailPrivate(const QOrganizerItemDetailPrivate& other)
        : QSharedData(other),
        m_id(other.m_id),
        m_definitionName(other.m_definitionName),
        m_values(other.m_values),
        m_access(other.m_access)
    {
    }

    ~QOrganizerItemDetailPrivate()
    {
    }

    int m_id; // internal, unique id.
    QOrganizerItemStringHolder m_definitionName;
    QHash<QOrganizerItemStringHolder, QVariant> m_values;
    QOrganizerItemDetail::AccessConstraints m_access;

    static QAtomicInt lastDetailKey;

    static void setAccessConstraints(QOrganizerItemDetail *d, QOrganizerItemDetail::AccessConstraints constraint)
    {
        d->d->m_access = constraint;
    }

    static const QOrganizerItemDetailPrivate* detailPrivate(const QOrganizerItemDetail& detail)
    {
        return detail.d.constData();
    }
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QOrganizerItemStringHolder), Q_MOVABLE_TYPE);

#endif
