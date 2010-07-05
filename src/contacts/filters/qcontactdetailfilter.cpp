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

#include "qcontactdetailfilter.h"
#include "qcontactdetailfilter_p.h"
#include "qcontactfilter_p.h"
#include "qcontactmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QContactDetailFilter
  \brief The QContactDetailFilter class provides a filter based around a detail value criterion
 
  \ingroup contacts-filters
  
  It may be used to select contacts which contain a detail of a particular definition with a particular value
 */

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactDetailFilter);

/*!
 * \fn QContactDetailFilter::QContactDetailFilter(const QContactFilter& other)
 * Constructs a copy of \a other if possible, otherwise constructs a new detail filter
 */

/*!
 * Constructs a new detail filter
 */
QContactDetailFilter::QContactDetailFilter()
    : QContactFilter(new QContactDetailFilterPrivate)
{
}

/*!
 * Sets the name of the detail definition of which details will be matched to \a definitionName, and the name of the field in
 * details of that definition which will contain the value criterion to \a fieldName.
 * If \a definitionName is empty, the detail filter will match no contacts.  If \a fieldName
 * is empty, the detail filter acts like a "detail exists" filter; if any detail of the specified definition
 * is present in a contact, that contact will match the filter, regardless of what values might be
 * stored in that detail.
 * \sa detailDefinitionName()
 */
void QContactDetailFilter::setDetailDefinitionName(const QString& definitionName, const QString& fieldName)
{
    Q_D(QContactDetailFilter);
    d->m_defId = definitionName;
    d->m_fieldId = fieldName;
}

/*!
 * Sets the value criterion of the filter to \a value.
 * Note that if the field name criterion (set via setDetailDefinitionName())
 * of the filter is an empty string, this value will be ignored.
 * \sa value(), setDetailDefinitionName()
 */
void QContactDetailFilter::setValue(const QVariant& value)
{
    Q_D(QContactDetailFilter);
    d->m_exactValue = value;
}

/*!
 * Sets the semantics of the value matching criterion to those defined in \a flags
 * \sa matchFlags()
 */
void QContactDetailFilter::setMatchFlags(QContactFilter::MatchFlags flags)
{
    Q_D(QContactDetailFilter);
    d->m_flags = flags;
}

/*!
 * Returns the semantics of the value matching criterion
 * \sa setMatchFlags()
 */
QContactFilter::MatchFlags QContactDetailFilter::matchFlags() const
{
    Q_D(const QContactDetailFilter);
    return d->m_flags;
}

/*!
 * Returns the definition name of the details which will be inspected for matching values
 * \sa setDetailDefinitionName()
 */
QString QContactDetailFilter::detailDefinitionName() const
{
    Q_D(const QContactDetailFilter);
    return d->m_defId;
}

/*!
 * Returns the name of the field which contains the value which will be matched against the value criterion
 * \sa setDetailDefinitionName()
 */
QString QContactDetailFilter::detailFieldName() const
{
    Q_D(const QContactDetailFilter);
    return d->m_fieldId;
}

/*!
 * Returns the value criterion of the detail filter
 * \sa setValue()
 */
QVariant QContactDetailFilter::value() const
{
    Q_D(const QContactDetailFilter);
    return d->m_exactValue;
}

QTM_END_NAMESPACE
