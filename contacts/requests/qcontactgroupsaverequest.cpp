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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactgroupsaverequest.h"
#include "qcontactrequests_p.h"

/*!
  \class QContactGroupSaveRequest
  \brief The QContactGroupSaveRequest class allows a client to asynchronously
    request that certain groups be saved to a contacts store.
   \ingroup contacts-requests
 */

/*!
 * \fn QContactGroupSaveRequest::progress(QContactGroupSaveRequest* self)
 * This signal is emitted when some progress has been made on the request, causing either a change of
 * status or an update of results, or both.  It identifies which request the signal originated from
 * by including a pointer to \a self.
 */

/*! Constructs a new group save request */
QContactGroupSaveRequest::QContactGroupSaveRequest()
    : QContactAbstractRequest(new QContactGroupSaveRequestPrivate)
{
}

/*! Cleans up the memory in use by this group save request */
QContactGroupSaveRequest::~QContactGroupSaveRequest()
{
}

/*! Sets the groups which will be saved by this request to \a groups */
void QContactGroupSaveRequest::setGroups(const QList<QContactGroup>& groups)
{
    Q_D(QContactGroupSaveRequest);
    d->m_groups = groups;
}

/*! Returns the list of groups which will be saved by this request if called prior to calling \c start(),
    otherwise returns the list of groups as they were saved in the contacts store */
QList<QContactGroup> QContactGroupSaveRequest::groups() const
{
    Q_D(const QContactGroupSaveRequest);
    return d->m_groups;
}
