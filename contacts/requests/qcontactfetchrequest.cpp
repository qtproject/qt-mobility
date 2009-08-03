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

#include "qcontactfetchrequest.h"
#include "qcontactrequests_p.h"

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactFetchRequest)

QContactFetchRequest::QContactFetchRequest()
    : QContactAbstractRequest(new QContactFetchRequestPrivate)
{
}

QContactFetchRequest::~QContactFetchRequest()
{
}

void QContactFetchRequest::setFilter(const QContactFilter& filter)
{
    Q_D(QContactFetchRequest);
    d->m_filter = filter;
}

void QContactFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    Q_D(QContactFetchRequest);
    d->m_sorting = sorting;
}

void QContactFetchRequest::setDefinitionRestrictions(const QStringList& definitionNames)
{
    Q_D(QContactFetchRequest);
    d->m_definitionRestrictions = definitionNames;
}

QContactFilter QContactFetchRequest::filter() const
{
    Q_D(const QContactFetchRequest);
    return d->m_filter;
}

QList<QContactSortOrder> QContactFetchRequest::sorting() const
{
    Q_D(const QContactFetchRequest);
    return d->m_sorting;
}

QStringList QContactFetchRequest::definitionRestrictions() const
{
    Q_D(const QContactFetchRequest);
    return d->m_definitionRestrictions;
}

QList<QContact> QContactFetchRequest::contacts() const
{
    Q_D(const QContactFetchRequest);
    return d->m_contacts;
}
