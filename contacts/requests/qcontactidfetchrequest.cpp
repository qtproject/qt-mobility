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

#include "qcontactidfetchrequest.h"
#include "qcontactrequests_p.h"

QContactIdFetchRequest::QContactIdFetchRequest()
    : QContactAbstractRequest(new QContactIdFetchRequestPrivate)
{
}

QContactIdFetchRequest::~QContactIdFetchRequest()
{
}

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactIdFetchRequest)

void QContactIdFetchRequest::setFilter(const QContactFilter& filter)
{
    Q_D(QContactIdFetchRequest);
    d->m_filter = filter;
}

void QContactIdFetchRequest::setSorting(const QList<QContactSortOrder>& sorting)
{
    Q_D(QContactIdFetchRequest);
    d->m_sorting = sorting;
}

QContactFilter QContactIdFetchRequest::filter() const
{
    Q_D(const QContactIdFetchRequest);
    return d->m_filter;
}

QList<QContactSortOrder> QContactIdFetchRequest::sorting() const
{
    Q_D(const QContactIdFetchRequest);
    return d->m_sorting;
}

QList<QUniqueId> QContactIdFetchRequest::ids() const
{
    Q_D(const QContactIdFetchRequest);
    return d->m_ids;
}


bool QContactIdFetchRequest::start()
{
    return false;
}

bool QContactIdFetchRequest::cancel()
{
    return false;
}

bool QContactIdFetchRequest::waitForFinished(int msecs)
{
    return false;
}

bool QContactIdFetchRequest::waitForProgress(int msecs)
{
    return false;
}
