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

#include "qcontactsortorder.h"
#include "qcontactsortorder_p.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactSortOrder
  \brief The QContactSortOrder class defines how a list of contacts should be ordered according to some criteria
 */

/*!
 * \enum QContactSortOrder::BlankPolicy
 * Enumerates the ways in which the sort order interprets blanks when sorting contacts
 * \value BlanksFirst Considers blank values to evaluate to less than all other values in comparisons
 * \value BlanksLast Considers blank values to evaluate to greater than all other values in comparisons
 */

/*!
 * \fn QContactSortOrder::operator QList<QContactSortOrder>() const
 * Constructs a new list of sort orders containing only the current sort order
 */

/*!
 * \fn QContactSortOrder::operator!=(const QContactSortOrder& other) const
 * Returns true if this sort order is not identical to the \a other sort order
 * \sa operator==()
 */

/*!
 * Constructs a new sort order
 */
QContactSortOrder::QContactSortOrder()
    : d(new QContactSortOrderPrivate())
{
}

/*!
 * Frees any memory in use by this sort order
 */
QContactSortOrder::~QContactSortOrder()
{
}

/*!
 * Constructs a copy of the \a other sort order
 */
QContactSortOrder::QContactSortOrder(const QContactSortOrder& other)
    : d(other.d)
{

}

/*!
 * Assigns this sort order to be equal to \a other
 */
QContactSortOrder& QContactSortOrder::operator=(const QContactSortOrder& other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}

/*!
 * Returns true if the sort order is able to be used to sort a list of contacts; otherwise, returns false
 */
bool QContactSortOrder::isValid() const
{
    /* We clear both when one is empty, so we only need to check one */
    if (d->m_definitionName.isEmpty())
        return false;
    return true;
}

/*!
 * Returns true if this sort order is identical to the \a other sort order
 * \sa operator!=()
 */
bool QContactSortOrder::operator ==(const QContactSortOrder& other) const
{
    if (d->m_blankPolicy == other.d->m_blankPolicy &&
        d->m_direction == other.d->m_direction &&
        d->m_sensitivity == other.d->m_sensitivity &&
        d->m_definitionName == other.d->m_definitionName &&
        d->m_fieldName == other.d->m_fieldName)
        return true;
    return false;
}


#ifndef QT_NO_DATASTREAM
QDataStream& operator<<(QDataStream& out, const QContactSortOrder& sortOrder)
{
    return out << sortOrder.detailDefinitionName()
        << sortOrder.detailFieldName()
        << (quint32)sortOrder.blankPolicy()
        << (quint32)sortOrder.direction()
        << (quint32)sortOrder.caseSensitivity();
}

QDataStream& operator>>(QDataStream& in, QContactSortOrder& sortOrder)
{
    QString definitionName;
    QString fieldName;
    quint32 blankPolicy;
    quint32 direction;
    quint32 caseSensitivity;
    in >> definitionName >> fieldName >> blankPolicy >> direction >> caseSensitivity;
    sortOrder.setDetailDefinitionName(definitionName, fieldName);
    sortOrder.setBlankPolicy(QContactSortOrder::BlankPolicy(blankPolicy));
    sortOrder.setDirection(Qt::SortOrder(direction));
    sortOrder.setCaseSensitivity(Qt::CaseSensitivity(caseSensitivity));
    return in;
}
#endif

/*!
 * Sets the definition name of the details which will be inspected to perform sorting to \a definitionName,
 * and the name of those details' fields which contains the value which contacts will be sorted by to \a fieldName
 * \sa detailDefinitionName(), detailFieldName()
 */
void QContactSortOrder::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    if (definitionName.isEmpty() || fieldName.isEmpty()) {
        d->m_definitionName.clear();
        d->m_fieldName.clear();
    } else {
        d->m_definitionName = definitionName;
        d->m_fieldName = fieldName;
    }
}

/*!
 * Sets the sort order's policy on blank values with respect to sorting to \a blankPolicy
 * \sa blankPolicy()
 */
void QContactSortOrder::setBlankPolicy(BlankPolicy blankPolicy)
{
    d->m_blankPolicy = blankPolicy;
}

/*!
 * Sets the sort order direction to \a direction
 * \sa direction()
 */
void QContactSortOrder::setDirection(Qt::SortOrder direction)
{
    d->m_direction = direction;
}

/*!
 * Returns the definition name of the details which will be inspected to perform sorting.
 * Note that if a contact has multiple details of the definition, the result of the sorting
 * is undefined.
 * \sa setDetailDefinitionName()
 */
QString QContactSortOrder::detailDefinitionName() const
{
    return d->m_definitionName;
}

/*!
 * Returns the name of the field in the definition which will be inspected to perform sorting
 * \sa setDetailDefinitionName()
 */
QString QContactSortOrder::detailFieldName() const
{
    return d->m_fieldName;
}

/*!
 * Returns the blank policy of the sort order
 * \sa setBlankPolicy()
 */
QContactSortOrder::BlankPolicy QContactSortOrder::blankPolicy() const
{
    return d->m_blankPolicy;
}

/*!
 * Returns the direction of the sort order
 * \sa setDirection()
 */
Qt::SortOrder QContactSortOrder::direction() const
{
    return d->m_direction;
}

/*!
 * Returns the case sensitivity of the sort order
 * \sa setCaseSensitivity()
 */
Qt::CaseSensitivity QContactSortOrder::caseSensitivity() const
{
    return d->m_sensitivity;
}

/*!
 * Sets the case sensitivity of the sort order to \a sensitivity
 * \sa caseSensitivity()
 */
void QContactSortOrder::setCaseSensitivity(Qt::CaseSensitivity sensitivity)
{
    d->m_sensitivity = sensitivity;
}

QTM_END_NAMESPACE
