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

#include "qcontactactionfilter.h"
#include "qcontactactionfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactActionFilter
  \brief The QContactActionFilter class provides a filter based around an
  action availability criterion.
 
  \ingroup contacts-filters
    
  It may be used to select contacts for which a particular action is
  available, or contacts which contain a detail of a particular value for
  which the action is available.
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactActionFilter);

/*!
 * \fn QContactActionFilter::QContactActionFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new action filter
 */

/*!
 * Constructs a new action filter
 */
QContactActionFilter::QContactActionFilter()
    : QContactFilter(new QContactActionFilterPrivate)
{
}

/*!
 * Sets the name of the action whose availability is required to \a action
 * \sa actionName()
 */
void QContactActionFilter::setActionName(const QString& action)
{
    Q_D(QContactActionFilter);
    d->m_action = action;
}

/*!
 * Sets the value criterion of the detail for which the action must be available to \a value
 * \sa value()
 */
void QContactActionFilter::setValue(const QVariant& value)
{
    Q_D(QContactActionFilter);
    d->m_value = value;
}

/*!
 * Sets the vendor details criterion of the action whose availability is required to the given \a vendorName and \a implementationVersion
 * \sa vendorName(), implementationVersion()
 */
void QContactActionFilter::setVendor(const QString& vendorName, int implementationVersion)
{
    Q_D(QContactActionFilter);
    d->m_vendorName = vendorName;
    if (!vendorName.isEmpty()) {
        d->m_implementationVersion = implementationVersion;
        return;
    }

    d->m_implementationVersion = -1;
}

/*!
 * Returns the action name criterion of the filter
 * \sa setActionName()
 */
QString QContactActionFilter::actionName() const
{
    Q_D(const QContactActionFilter);
    return d->m_action;
}

/*!
 * Returns the value criterion of the filter
 * \sa setValue()
 */
QVariant QContactActionFilter::value() const
{
    Q_D(const QContactActionFilter);
    return d->m_value;
}

/*!
 * Returns the vendor name criterion of the filter
 * \sa setVendor()
 */
QString QContactActionFilter::vendorName() const
{
    Q_D(const QContactActionFilter);
    return d->m_vendorName;
}

/*!
 * Returns the vendor-specified action implementation version criterion of the filter
 * \sa setVendor()
 */
int QContactActionFilter::implementationVersion() const
{
    Q_D(const QContactActionFilter);
    return d->m_implementationVersion;
}

QTM_END_NAMESPACE
