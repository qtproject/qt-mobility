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

#ifdef SYMBIAN_BACKEND_USE_SQLITE

#include "cntsymbianfiltersql.h"
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"

#include <e32cmn.h>
#include <cntdb.h>

CntSymbianFilter::CntSymbianFilter(CContactDatabase& contactDatabase):
    m_contactDatabase(contactDatabase)
{
    m_sqlhelper = new CntSymbianFilterSqlHelper(contactDatabase);
}

CntSymbianFilter::~CntSymbianFilter()
{
    delete m_sqlhelper;
}

QList<QContactLocalId> CntSymbianFilter::contacts(
            const QContactFilter& filter,
            const QList<QContactSortOrder>& sortOrders,
            bool &filterSupportedFlag,
            QContactManager::Error& error)
{
    QList<QContactLocalId> matches;
    
    //temp solution to retrieve all contacts
    if(filter.type() == QContactFilter::DefaultFilter)
    {
        TTime epoch(0);
        CContactIdArray* idArray = m_contactDatabase.ContactsChangedSinceL(epoch); // return all contacts
        
        // copy the matching contact ids
        for(int i(0); i < idArray->Count(); i++) {
            matches.append(QContactLocalId((*idArray)[i]));
        }
    }
    
    else
    {
        //sort order not supported yet
        matches = m_sqlhelper->searchContacts(filter,error);
        // Tell the caller do slow filtering if the filter is not supported
        filterSupportedFlag = filterSupported(filter);
    }
    
    return matches;
}

bool CntSymbianFilter::filterSupported(const QContactFilter& filter)
{
    TBool result;

    // Map filter support into a boolean value
    FilterSupport support = filterSupportLevel(filter);
    if (support == Supported || support == SupportedPreFilterOnly) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

CntAbstractContactFilter::FilterSupport CntSymbianFilter::filterSupportLevel(const QContactFilter& filter)
{
    return m_sqlhelper->filterSupportLevel(filter);
}

#endif
