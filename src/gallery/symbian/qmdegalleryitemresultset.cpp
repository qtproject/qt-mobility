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


//Backend
#include "qmdegalleryitemresultset.h"
#include "qgallerymdsutility.h"
#include "qmdesession.h"
//API
#include "qgalleryitemrequest.h"

QTM_BEGIN_NAMESPACE

QMDEGalleryItemResultSet::QMDEGalleryItemResultSet(QMdeSession *session, QObject *parent)
:QMDEGalleryResultSet(parent)
{
    m_request = static_cast<QGalleryItemRequest *>(parent);
    m_live = false; // when live queries are supported, read this value from request
    m_session = session;

    createQuery();
}

QMDEGalleryItemResultSet::~QMDEGalleryItemResultSet()
{

}
void QMDEGalleryItemResultSet::createQuery()
{
    TRAP_IGNORE( m_resultObject = m_session->GetFullObjectL( m_request->itemId().toUInt() ) );
    // After that resultObject contains NULL or the needed item
    if( m_resultObject ){
        finish(QGalleryAbstractRequest::Succeeded, m_live);
    }
    else {
        finish(QGalleryAbstractRequest::RequestError, false);
    }
}

#include "moc_qmdegalleryitemresultset.cpp"
QTM_END_NAMESPACE
