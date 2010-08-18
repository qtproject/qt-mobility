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

#include "qmdequery.h"
//API
#include "qgalleryqueryrequest.h"
#include "qgalleryabstractresponse.h"
//Symbian
#include <e32base.h>
#include <mdeobject.h>

QTM_BEGIN_NAMESPACE

QMdeQuery::QMdeQuery(QGalleryAbstractResponse *response, QObject *parent)
    :QObject(parent)
    ,iLimit(KMdEQueryDefaultMaxCount)
    ,m_response(response)
{
    iQuery = NULL;
}

QMdeQuery::~QMdeQuery()
{
    if (iQuery) {
        delete iQuery;
    }

    iQuery = NULL;
}

void QMdeQuery::SetQuery(CMdEObjectQuery* aQuery)
{
    if (iQuery) {
        delete iQuery;
    }

    iQuery = aQuery;
}

void QMdeQuery::FindL()
{
    if (iQuery) {
        iQuery->FindL(iLimit);
    }
}

void QMdeQuery::SetQueryType(QueryType aType)
{
    if (iQuery) {
        TQueryResultMode resultMode = EQueryResultModeItem;
        switch (aType) {
        case Id:
        {
            resultMode = EQueryResultModeId;
            break;
        }
        case Count:
        {
            resultMode = EQueryResultModeCount;
            break;
        }
        case Distinct:
        {
            resultMode = EQueryResultModeDistinctValues;
            break;
        }
        case Item:
        default:
        {
            resultMode = EQueryResultModeItem;
            break;
        }
        }

        iQuery->SetResultMode(resultMode);
        iResultModeSet = ETrue;
    }
}

void QMdeQuery::SetLimit(TInt aLimit)
{
    iLimit = aLimit;
}

void QMdeQuery::HandleQueryNewResults(CMdEQuery& aQuery, TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/)
{
    //m_response->queryProgressed(aQuery.Count(), KMdEQueryDefaultMaxCount);
}

void QMdeQuery::HandleQueryCompleted(CMdEQuery& aQuery, TInt aError)
{
   //m_response->queryProgressed(aQuery.Count(), aQuery.Count());

    // All results appended when query is ready
    if (aQuery.ResultMode() == EQueryResultModeItem) {
        //RArray<TItemId> idArray = aQuery.ResultIds();
        //int max = idArray.Count();
        int max = aQuery.Count();
        for (TInt i = 0; ++i < max;) {
            //TODO:: changes parent() usage
//            ((QGalleryResultSet*) parent())->appendItem(
//                static_cast<CMdEObject*> (aQuery.TakeOwnershipOfResult(i)));
        }
    }

    if (aError == KErrNone) {
        //m_response->queryFinished(QGalleryAbstractRequest::Succeeded,false);
    }
    else {
        //m_response->queryFinished(aError, false); // Symbian error code
    }
}
#include "moc_qmdequery.cpp"
QTM_END_NAMESPACE
