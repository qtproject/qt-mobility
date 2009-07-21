/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactfilter.h"
#include "qcontactfilter_p.h"

#include "qcontactintersectionfilter.h"
#include "qcontactunionfilter.h"

#include "qcontactmanager.h"

template<> QContactFilterPrivate *QSharedDataPointer<QContactFilterPrivate>::clone()
{
    return d->clone();
}

//Q_IMPLEMENT_BASE_CONTACTFILTER_PRIVATE(QContactFilter)
QContactFilter::QContactFilter()
    : d_ptr(0)
{
}

QContactFilter::QContactFilter(const QContactFilter& other)
    : d_ptr(other.d_ptr)
{
}

QContactFilter& QContactFilter::operator=(const QContactFilter& other)
{
    if (this != &other) {
        d_ptr = other.d_ptr;
    }
    return *this;
}

QContactFilter::~QContactFilter()
{
}

QContactFilter::FilterType QContactFilter::type() const
{
    return d_ptr ? d_ptr->m_type : QContactFilter::Invalid;
}

bool QContactFilter::operator==(const QContactFilter& other) const
{
    /* An invalid filter is not equal to anything, even other invalid filters */
    if (!d_ptr)
        return false;

    /* Different types can't be equal */
    if (other.type() != type())
        return false;

    /* Otherwise, use the virtual op == */
    return d_ptr->compare(other.d_ptr);
}

QContactFilter::QContactFilter(QContactFilterPrivate *d)
    : d_ptr(d)
{

}

const QContactFilter operator&&(const QContactFilter& left, const QContactFilter& right)
{
    if (left.type() == QContactFilter::Intersection) {
        QContactIntersectionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QContactFilter::Intersection) {
        QContactIntersectionFilter bf(right);
        /* we can prepend the left to this one */
        bf.prepend(left);
        return bf;
    }

    /* usual fallback case */
    QContactIntersectionFilter nif;
    nif << left << right;
    return nif;
}

const QContactFilter operator||(const QContactFilter& left, const QContactFilter& right)
{
    if (left.type() == QContactFilter::Union) {
        QContactUnionFilter bf(left);
        /* we can just add the right to this one */
        bf.append(right);
        return bf;
    }

    if (right.type() == QContactFilter::Union) {
        QContactUnionFilter bf(right);
        /* we can prepend the left to this one */
        bf.prepend(left);
        return bf;
    }

    /* usual fallback case */
    QContactUnionFilter nif;
    nif << left << right;
    return nif;
}

