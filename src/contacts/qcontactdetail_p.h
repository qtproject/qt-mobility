/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTDETAIL_P_H
#define QCONTACTDETAIL_P_H

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

#include "qcontactdetail.h"

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
class QContactStringHolder
{
public:
    QContactStringHolder()
        : m_str(0)
    {
    }

    ~QContactStringHolder()
    {
    }

    QContactStringHolder(const QContactStringHolder& other)
        : m_str(other.m_str)
    {
    }

    QContactStringHolder& operator=(const QContactStringHolder& other)
    {
        m_str = other.m_str; // checking for ==this is not worth the effort
        return *this;
    }

    QContactStringHolder(const char *str)
        : m_str(str)
    {
    }

    QContactStringHolder& operator=(const char *str)
    {
        m_str = str;
        return *this;
    }

    explicit QContactStringHolder(const QString& str)
    {
        *this = str;
    }

    QContactStringHolder& operator=(const QString& str)
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

    bool operator==(const QContactStringHolder& other) const
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

uint qHash(const QContactStringHolder& key);

class QContactDetailPrivate : public QSharedData
{
public:
    QContactDetailPrivate()
        : QSharedData(),
        m_id(lastDetailKey.fetchAndAddOrdered(1)),
        m_access(QContactDetail::NoConstraint)
    {
    }

    QContactDetailPrivate(const QContactDetailPrivate& other)
        : QSharedData(other),
        m_id(other.m_id),
        m_definitionName(other.m_definitionName),
        m_values(other.m_values),
        m_access(other.m_access)
    {
    }

    ~QContactDetailPrivate()
    {
    }

    int m_id; // internal, unique id.
    QContactStringHolder m_definitionName;
    QHash<QContactStringHolder, QVariant> m_values;
    QContactDetail::AccessConstraints m_access;

    static QAtomicInt lastDetailKey;

    static void setAccessConstraints(QContactDetail *d, QContactDetail::AccessConstraints constraint)
    {
        d->d->m_access = constraint;
    }

    static const QContactDetailPrivate* detailPrivate(const QContactDetail& detail)
    {
        return detail.d.constData();
    }
};

QTM_END_NAMESPACE

Q_DECLARE_TYPEINFO(QTM_PREPEND_NAMESPACE(QContactStringHolder), Q_MOVABLE_TYPE);

#endif
