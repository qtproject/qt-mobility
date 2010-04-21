/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cntfilterinvalid.h"
#include "cnttransformcontact.h"
#include "cntfilterdetaildisplaylabel.h" //todo rename class to follow naming pattern CntFilterDetailDisplayLabel

CntFilterInvalid::CntFilterInvalid(CContactDatabase& contactDatabase,CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo) 
                                        :m_contactdatabase(contactDatabase),
                                         m_srvConnection(cntServer),
                                         m_dbInfo(dbInfo)
{
   // m_detailFilters.append(new CntFilterDetailDisplayLabel(m_srvConnection));
}

CntFilterInvalid::~CntFilterInvalid()
{
    
}

QList<QContactLocalId> CntFilterInvalid::contacts(
        const QContactFilter &filter,
        const QList<QContactSortOrder> &sortOrders,
        bool &filterSupportedflag,
        QContactManager::Error* error)
{
    Q_UNUSED(filter);
    Q_UNUSED(sortOrders);
    Q_UNUSED(filterSupportedflag);
    Q_UNUSED(error);
    //In case of invalid filter, we return empty list
    return QList<QContactLocalId>();
       
}


bool CntFilterInvalid::filterSupported(const QContactFilter& filter) 
{
    bool result = false;
    if(QContactFilter::InvalidFilter == filter.type())
        {
        result = true;
        }
    
    return result;
}

void CntFilterInvalid::createSelectQuery(const QContactFilter& filter,
                              QString& sqlQuery,
                              QContactManager::Error* error)

{
    Q_UNUSED(sqlQuery);
    if(!filterSupported(filter))
        {
        *error =  QContactManager::NotSupportedError;
        }
}
