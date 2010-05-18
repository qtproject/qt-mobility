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

#ifdef SYMBIAN_BACKEND_USE_SQLITE

#include "cntsymbianfiltersql.h"
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"
#include "cntsymbiansrvconnection.h"

#include "cntfilterdetail.h"
#include "cntfilterdefault.h"
#include "cntfilterintersection.h"
#include "cntfilterunion.h"
#include "cntfilterrelationship.h"
#include <QLatin1String>
#include <e32cmn.h>
#include <cntdb.h>


CntSymbianFilter::CntSymbianFilter(QContactManagerEngine& /*manager*/, CContactDatabase& contactDatabase, CntSymbianSrvConnection &srvConnection, const CntTransformContact &transformContact):
    m_contactDatabase(contactDatabase),
    m_transformContact(transformContact),
    m_srvConnection(srvConnection)
{
    m_dbInfo = new CntDbInfo();
    initializeFilters();
}

void CntSymbianFilter::initializeFilters()
{
    m_filterMap.insert(QContactFilter::ContactDetailFilter, new CntFilterDetail(m_contactDatabase,m_srvConnection,*m_dbInfo));
    m_filterMap.insert(QContactFilter::DefaultFilter, new CntFilterDefault(m_contactDatabase,m_srvConnection,*m_dbInfo));
    m_filterMap.insert(QContactFilter::IntersectionFilter, new CntFilterIntersection(m_contactDatabase,m_srvConnection,*m_dbInfo));
    m_filterMap.insert(QContactFilter::UnionFilter, new CntFilterUnion(m_contactDatabase,m_srvConnection,*m_dbInfo));
    m_filterMap.insert(QContactFilter::RelationshipFilter, new CntFilterRelationship(m_contactDatabase,m_srvConnection,*m_dbInfo));
}

CntSymbianFilter::~CntSymbianFilter()
{
    //delete the all filters from the map
    QMap<QContactFilter::FilterType, CntAbstractContactFilter*>::iterator itr;

    for (itr = m_filterMap.begin(); itr != m_filterMap.end(); ++itr) {
        CntAbstractContactFilter* value = itr.value();
        delete value;
        value = 0;
    }
    
    delete m_dbInfo;  
}

QList<QContactLocalId> CntSymbianFilter::contacts(const QContactFilter& filter,
                                                  const QList<QContactSortOrder>& sortOrders,
                                                  bool &filterSupported,
                                                  QContactManager::Error* error)
{
    QList<QContactLocalId> ids;
    if (m_filterMap.contains(filter.type())) {
        ids = ( m_filterMap.value(filter.type()))->contacts(filter,sortOrders,filterSupported,error);
        return ids;   
    }
    *error = QContactManager::NotSupportedError;
    return ids;
}

bool CntSymbianFilter::filterSupported(const QContactFilter& filter)
{
    bool result = false;
    if (m_filterMap.contains(filter.type())) {
        result = m_filterMap.value(filter.type())->filterSupported(filter);
    }
    return result;
}

#endif
