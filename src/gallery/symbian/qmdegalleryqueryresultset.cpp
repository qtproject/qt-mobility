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
#include "qmdegalleryqueryresultset.h"
#include "qgallerymdsutility.h"
#include "qmdesession.h"
//API
#include "qgalleryqueryrequest.h"

QTM_BEGIN_NAMESPACE

QMDEGalleryQueryResultSet::QMDEGalleryQueryResultSet(QMdeSession *session, QObject *parent)
:QMDEGalleryResultSet(parent)
{
    m_request = static_cast<QGalleryQueryRequest *>(parent);
    m_live = false; // when live queries are supported, read this value from request
    m_session = session;

    createQuery();
}

QMDEGalleryQueryResultSet::~QMDEGalleryQueryResultSet()
{
    if( m_query ){
        m_query->RemoveObserver( *this );
        m_query->Cancel();
    }
    delete m_query;
    m_query = NULL;
}

void QMDEGalleryQueryResultSet::HandleQueryNewResults( CMdEQuery &aQuery,
                                                       TInt firstNewItemIndex,
                                                       TInt newItemCount )
{
    if( aQuery.ResultMode() == EQueryResultModeItem ) {
        int max = aQuery.Count();
        for ( TInt i = firstNewItemIndex; i < max; i++ ) {
            appendItem( static_cast<CMdEObject *>(aQuery.TakeOwnershipOfResult( i )) );
        }
    }

    //Signals that items have been inserted into a result set at
    emit itemsInserted(firstNewItemIndex, newItemCount);

    emit progressChanged(aQuery.Count(), KMdEQueryDefaultMaxCount);
}

void QMDEGalleryQueryResultSet::HandleQueryCompleted( CMdEQuery &aQuery, TInt aError )
{
    emit progressChanged(aQuery.Count(), aQuery.Count());

    if( aError == KErrNone ){
        finish(QGalleryAbstractRequest::Succeeded, m_live);
    }
    else{
        finish(QGalleryAbstractRequest::RequestError, false);
    }

}

void QMDEGalleryQueryResultSet::createQuery()
{
    if( !m_session ){
        finish(QGalleryAbstractRequest::ConnectionError, false);
        return;
    }

    int error = 0;
    TRAPD( err, m_query = m_session->NewObjectQueryL( this, m_request, error ) );
    if( err ){
        m_query = NULL;
        finish(QGalleryAbstractRequest::RequestError, false);
        return;
    }
    else if( error != QGalleryAbstractRequest::NoResult ){
       m_query->RemoveObserver( *this );
       delete m_query;
       m_query = NULL;
       finish(error, false);
       return;
    }

    // NewObjectQuery will return NULL if object type is not supported
    if( m_query ){
        TRAP( err, m_query->FindL() );
        if( err ){
            m_query->RemoveObserver( *this );
            delete m_query;
            m_query = NULL;
            finish(QGalleryAbstractRequest::RequestError, false);
        }
    }
    else{
        finish(QGalleryAbstractRequest::NotSupported, false);
    }
}

#include "moc_qmdegalleryqueryresultset.cpp"
QTM_END_NAMESPACE
