/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cntfilterintersection.h"
#include "cntfilterdetail.h"
#include "cntfilterunion.h"
#include "cntfilterdefault.h"
#include "cntfilterrelationship.h"
#include "cnttransformcontact.h"

CntFilterIntersection::CntFilterIntersection(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo)
                         : m_contactdatabase(contactDatabase),
                           m_srvConnection(cntServer),
                           m_dbInfo(dbInfo),
                           m_emulateBestMatching(false)
{
}

CntFilterIntersection::~CntFilterIntersection()
{
}


QList<QContactLocalId> CntFilterIntersection::contacts(const QContactFilter &filter,
                                                       const QList<QContactSortOrder> &sortOrders,
                                                       bool &filterSupportedflag,
                                                       QContactManager::Error* error)
{
    Q_UNUSED(sortOrders);
    Q_UNUSED(filterSupportedflag);
    //Check if any invalid filter is passed 
    if (filterSupported(filter) == false) {
        *error =  QContactManager::NotSupportedError;
        return QList<QContactLocalId>();
    }
    QList<QContactLocalId> idList;
    QString sqlQuery;
    createSelectQuery(filter,sqlQuery,error);
    QString sortQuery = m_dbInfo.getSortQuery(sortOrders, sqlQuery, error);
    //fetch the contacts
    if (*error == QContactManager::NoError ) {
        idList =  m_srvConnection.searchContacts(sortQuery, error);
    }
    return idList;
}


bool CntFilterIntersection::filterSupported(const QContactFilter& filter) 
{
    bool result = false;
    if (QContactFilter::IntersectionFilter == filter.type()) {
       result = true;
    }
    return result;
}

void CntFilterIntersection::createSelectQuery(const QContactFilter& filter,
                                              QString& selectquery,
                                              QContactManager::Error* error)

{
    const QContactIntersectionFilter& intersectionfilter = static_cast<const QContactIntersectionFilter&>(filter);
    if (!filterSupported(filter)) {
        *error = QContactManager::NotSupportedError;
        return;
    }
    QList<QContactFilter> individualFilters = intersectionfilter.filters();
    //QString selectquery;
    int fltrcnt =  individualFilters.count();          
    if (fltrcnt)
        getSelectQueryforFilter(individualFilters[0],selectquery,error);
    
    for (int i=1; i< fltrcnt ; i++) {
    
        QString query;
        getSelectQueryforFilter(individualFilters[i],query,error);
        if (*error == QContactManager::NoError ) {
            selectquery.append(" INTERSECT ");
            selectquery += query;
        }
    }
}

void CntFilterIntersection::getSelectQueryforFilter(const QContactFilter& filter,
                                                    QString& sqlSelectQuery,
                                                    QContactManager::Error* error)
{
    switch(filter.type()) {
        case QContactFilter::DefaultFilter: {
            CntFilterDefault defaultfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
            defaultfltr.createSelectQuery(filter,sqlSelectQuery,error);
            break;
        }
        case QContactFilter::ContactDetailFilter: {
            QContactDetailFilter detailfilter(filter);
            if (detailfilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName ) {
                CntFilterDetail dtlfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
#ifdef PBK_UNIT_TEST
            if (m_emulateBestMatching) {
                dtlfltr.emulateBestMatching();
            }
#endif
            dtlfltr.createMatchPhoneNumberQuery(filter,sqlSelectQuery,error);
            }
            else {
                CntFilterDetail dtlfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
                dtlfltr.createSelectQuery(filter,sqlSelectQuery,error);
            }
            break;
        }
        case QContactFilter::RelationshipFilter: {
            CntFilterRelationship relationfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
            relationfltr.createSelectQuery(filter,sqlSelectQuery,error);
            break;
        }
        case QContactFilter::IntersectionFilter: {
            sqlSelectQuery += "SELECT DISTINCT contact_id FROM (";
            CntFilterIntersection intersectionfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
            intersectionfltr.createSelectQuery(filter,sqlSelectQuery,error);
            sqlSelectQuery += ')';
            break;
        }
        case QContactFilter::UnionFilter: {
            sqlSelectQuery += "SELECT DISTINCT contact_id FROM (";
            CntFilterUnion unionfltr(m_contactdatabase,m_srvConnection,m_dbInfo);
            unionfltr.createSelectQuery(filter,sqlSelectQuery,error);
            sqlSelectQuery += ')';
            break;
        }
        default: {
            *error = QContactManager::NotSupportedError;
            break;
        }
    }
}

#ifdef PBK_UNIT_TEST
void CntFilterIntersection::emulateBestMatching()
{
    m_emulateBestMatching = true;
}
#endif

    
