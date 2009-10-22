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

#include "qcontactgroupfetchrequest.h"
#include "qcontactrequests_p.h"

/*!
  \class QContactGroupFetchRequest
  \brief The QContactGroupFetchRequest class allows a client to asynchronously
    request groups from a contacts store manager.
   \ingroup contacts-requests
 */

/*!
 * \fn QContactGroupFetchRequest::progress(QContactGroupFetchRequest* self, bool appendOnly)
 * This signal is emitted when some progress has been made on the request, causing either a change of
 * status or an update of results, or both.  It identifies which request the signal originated from
 * by including a pointer to \a self, and contains an \a appendOnly flag which signifies whether or not the total
 * ordering of the results have been maintained since the last progress signal was emitted.
 */

/*! Constructs a new group fetch request */
QContactGroupFetchRequest::QContactGroupFetchRequest()
    : QContactAbstractRequest(new QContactGroupFetchRequestPrivate)
{
}

/*! Cleans up the memory used by this group fetch request */
QContactGroupFetchRequest::~QContactGroupFetchRequest()
{
}

/*! Sets the list of ids of the groups which will be retrieved to \a ids */
void QContactGroupFetchRequest::setIds(const QList<QUniqueId>& ids)
{
    Q_D(QContactGroupFetchRequest);
    d->m_ids = ids;
}

/*! Returns the list of ids of the groups which will be retrieved */
QList<QUniqueId> QContactGroupFetchRequest::ids() const
{
    Q_D(const QContactGroupFetchRequest);
    return d->m_ids;
}

/*! Returns the list of groups which were retrieved by this request */
QList<QContactGroup> QContactGroupFetchRequest::groups() const
{
    Q_D(const QContactGroupFetchRequest);
    return d->m_groups;
}
