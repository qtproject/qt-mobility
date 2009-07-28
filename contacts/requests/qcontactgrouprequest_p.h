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


#ifndef QCONTACTGROUPREQUEST_P_H
#define QCONTACTGROUPREQUEST_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QPointer>

#include "qcontactabstractrequest.h"
#include "qcontactmanager.h"
#include "qcontactgrouprequestresult.h"
#include "qcontactgroup.h"

class QContactGroupRequestData
{
public:
    QContactGroupRequestData()
        :   m_isRestrictedIds(false),
            m_status(QContactAbstractRequest::Inactive),
            m_error(QContactManager::NoError),
            m_manager(0),
            m_result(0)
    {
    }

    ~QContactGroupRequestData()
    {
    }

    void _q_statusUpdate(const QContactAbstractRequest::Status& status, const QContactManager::Error& error)
    {
        m_status = status;
        m_error = error;
        // XXX emit status update?
    }

    bool m_isRestrictedIds;

    QList<QUniqueId> m_requestIds;
    QList<QContactGroup> m_requestObjects;

    QContactAbstractRequest::Status m_status;
    QContactManager::Error m_error;

    QPointer<QContactManager> m_manager;
    QContactGroupRequestResult* m_result;
};

#endif
