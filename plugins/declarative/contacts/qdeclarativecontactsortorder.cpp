/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "qdeclarativecontactsortorder_p.h"


QDeclarativeContactSortOrder::QDeclarativeContactSortOrder(QObject* parent)
    :QObject(parent),
      m_detailType(QDeclarativeContactDetail::ContactCustomizedDetail),
      m_fieldType(-1)
{
}
void QDeclarativeContactSortOrder::setDetail(QDeclarativeContactDetail::ContactDetailType detailType)
{
    m_detailType = detailType;
}

QDeclarativeContactDetail::ContactDetailType QDeclarativeContactSortOrder::detail() const
{
    return m_detailType;
}

void QDeclarativeContactSortOrder::setField(int fieldType)
{
    m_fieldType = fieldType;
}

int QDeclarativeContactSortOrder::field() const
{
    return m_fieldType;
}


QDeclarativeContactSortOrder::BlankPolicy QDeclarativeContactSortOrder::blankPolicy() const
{
    if (m_sortOrder.blankPolicy() == QContactSortOrder::BlanksFirst)
        return QDeclarativeContactSortOrder::BlanksFirst;
    return QDeclarativeContactSortOrder::BlanksLast;
}

void QDeclarativeContactSortOrder::setBlankPolicy(QDeclarativeContactSortOrder::BlankPolicy blankPolicy)
{
    if (blankPolicy == QDeclarativeContactSortOrder::BlanksFirst)
        m_sortOrder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    else
        m_sortOrder.setBlankPolicy(QContactSortOrder::BlanksLast);
}

QDeclarativeContactSortOrder::SortOrder QDeclarativeContactSortOrder::direction() const
{
    if (m_sortOrder.direction() == Qt::DescendingOrder)
        return QDeclarativeContactSortOrder::DescendingOrder;
    return QDeclarativeContactSortOrder::AscendingOrder;
}
void QDeclarativeContactSortOrder::setDirection(QDeclarativeContactSortOrder::SortOrder direction)
{
    if (direction == QDeclarativeContactSortOrder::DescendingOrder)
        m_sortOrder.setDirection(Qt::DescendingOrder);
    else
        m_sortOrder.setDirection(Qt::AscendingOrder);
}

QDeclarativeContactSortOrder::CaseSensitivity QDeclarativeContactSortOrder::caseSensitivity() const
{
    if (m_sortOrder.caseSensitivity() == Qt::CaseInsensitive)
        return QDeclarativeContactSortOrder::CaseInsensitive;
    return QDeclarativeContactSortOrder::CaseSensitive;
}
void QDeclarativeContactSortOrder::setCaseSensitivity(QDeclarativeContactSortOrder::CaseSensitivity sensitivity)
{
    if (sensitivity == QDeclarativeContactSortOrder::CaseInsensitive)
        m_sortOrder.setCaseSensitivity(Qt::CaseInsensitive);
    else
        m_sortOrder.setCaseSensitivity(Qt::CaseSensitive);
}

QContactSortOrder QDeclarativeContactSortOrder::sortOrder()
{
    QString detailName = QDeclarativeContactDetail::definitionName(m_detailType);
    QString fieldName = QDeclarativeContactDetail::fieldName(m_detailType, m_fieldType);
    m_sortOrder.setDetailDefinitionName(detailName, fieldName);
    return m_sortOrder;
}

void QDeclarativeContactSortOrder::setSortOrder(const QContactSortOrder& sortOrder)
{
    m_sortOrder = sortOrder;
    emit sortOrderChanged();
}
