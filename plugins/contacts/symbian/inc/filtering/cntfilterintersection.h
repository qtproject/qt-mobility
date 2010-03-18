/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef CNTFILTERINTERSECTION_H_
#define CNTFILTERINTERSECTION_H_

#include "cntabstractcontactfilter.h"
#include "cntsymbiansrvconnection.h"
#include "cntdbinfo.h"
#include "qcontactdetailfilter.h"

class CntFilterIntersection : public CntAbstractContactFilter
{

public:
    CntFilterIntersection(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo);
    virtual ~CntFilterIntersection();
    QList<QContactLocalId> contacts(
            const QContactFilter &filter,
            const QList<QContactSortOrder> &sortOrders,
            bool &filterSupported,
            QContactManager::Error &error) ;
    bool filterSupported(const QContactFilter& filter);
    
    void getSqlQuery( const QContactIntersectionFilter& filter,
                            QString& tableName,
                            QString& sqlWhereClause ,
                            QContactManager::Error* error) const;

    void createSelectQuery(const QContactFilter& filter,
                                 QString& selectquery,
                                 QContactManager::Error* error);

private:
    void getSelectQueryforFilter(const QContactFilter& filter,QString& sqlSelectQuery,QContactManager::Error* error);
    
protected:
    CContactDatabase& m_contactdatabase;
    CntSymbianSrvConnection &m_srvConnection;
    CntDbInfo& m_dbInfo;
};

#endif /* CNTFILTERINTERSECTION_H_ */
