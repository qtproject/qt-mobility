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

#include "qcontactsortorder.h"
#include "qcontactsortorder_p.h"

QContactSortOrder::QContactSortOrder()
    : d(new QContactSortOrderPrivate())
{
}

QContactSortOrder::~QContactSortOrder()
{
}

QContactSortOrder::QContactSortOrder(const QContactSortOrder& other)
    : d(other.d)
{

}

QContactSortOrder& QContactSortOrder::operator=(const QContactSortOrder& other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}

bool QContactSortOrder::isValid() const
{
    /* We clear both when one is empty, so we only need to check one */
    if (d->m_definitionId.isEmpty())
        return false;
    return true;
}

void QContactSortOrder::setDetailDefinitionName(const QString& definitionId, const QString& fieldId)
{
    if (definitionId.isEmpty() || fieldId.isEmpty()) {
        d->m_definitionId.clear();
        d->m_fieldId.clear();
    } else {
        d->m_definitionId = definitionId;
        d->m_fieldId = fieldId;
    }
}

void QContactSortOrder::setBlankPolicy(BlankPolicy blankPolicy)
{
    d->m_blankPolicy = blankPolicy;
}

void QContactSortOrder::setDirection(Qt::SortOrder direction)
{
    d->m_direction = direction;
}

QString QContactSortOrder::detailDefinitionName() const
{
    return d->m_definitionId;
}

QString QContactSortOrder::detailFieldName() const
{
    return d->m_fieldId;
}

QContactSortOrder::BlankPolicy QContactSortOrder::blankPolicy() const
{
    return d->m_blankPolicy;
}

Qt::SortOrder QContactSortOrder::direction() const
{
    return d->m_direction;
}
