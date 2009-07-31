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

#include "qcontactgroupsaverequest.h"
#include "qcontactrequests_p.h"

QContactGroupSaveRequest::QContactGroupSaveRequest()
    : QContactAbstractRequest(new QContactGroupSaveRequestPrivate)
{
}

QContactGroupSaveRequest::~QContactGroupSaveRequest()
{
}

Q_IMPLEMENT_CONTACTFILTER_PRIVATE(QContactGroupSaveRequest)

void QContactGroupSaveRequest::setGroups(const QList<QContactGroup>& groups)
{
    Q_D(QContactGroupSaveRequest);
    d->m_groups = groups;
}

QList<QContactGroup> QContactGroupSaveRequest::groups() const
{
    Q_D(const QContactGroupSaveRequest);
    return d->m_groups;
}

bool QContactGroupSaveRequest::start()
{
    return false;
}

bool QContactGroupSaveRequest::cancel()
{
    return false;
}

bool QContactGroupSaveRequest::waitForFinished(int msecs)
{
    return false;
}

bool QContactGroupSaveRequest::waitForProgress(int msecs)
{
    return false;
}
