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

#ifndef QMDEQUERY_H
#define QMDEQUERY_H

#include <qmobilityglobal.h>

#include <QtCore/qobject.h>
#include <e32std.h>
#include <mdequery.h>

QTM_BEGIN_NAMESPACE

class QGalleryAbstractResponse;

class QMdeQuery: public QObject, public MMdEQueryObserver
{
    Q_OBJECT

public:

    enum QueryType
    {
        Item = 0, // The query is for actual metadata objects
        Id, // The query is for the ids of the metadata objects
        Count, // The query if for the count of metadata objects
        Distinct,
    // The query is for distinct items
    };

    QMdeQuery(QGalleryAbstractResponse *aResponse = 0, QObject *parent = 0);
    virtual ~QMdeQuery();

    /**
     * Set query
     */
    void SetQuery(CMdEObjectQuery* aQuery);

    /**
     * Launch query
     */
    void FindL();

    /**
     * Set query type
     */
    void SetQueryType(QueryType aType);

    /**
     * Set limit to the query, for example only one object wanted
     */
    void SetLimit(TInt aLimit);

private:
    // from MMdEQueryObserver
    /**
     * See @ref MMdEQueryObserver::HandleQueryNewResults
     */
    void HandleQueryNewResults(CMdEQuery& aQuery, TInt aFirstNewItemIndex, TInt aNewItemCount);

    /**
     * See @ref MMdEQueryObserver::HandleQueryCompleted
     */
    void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);

private:

    CMdEObjectQuery* iQuery;
    TBool iResultModeSet;
    TInt iLimit;
    QGalleryAbstractResponse *m_response;

};

QTM_END_NAMESPACE

#endif // QMDEQUERY_H
