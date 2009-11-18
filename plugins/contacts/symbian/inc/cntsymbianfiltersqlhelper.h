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
#ifndef CNTSYMBIANFILTERSQLHELPER_H
#define CNTSYMBIANFILTERSQLHELPER_H

// System includes
#include <qtcontactsglobal.h>
// User includes
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"
#include "cntsymbiansrvconnection.h"
#include "cntabstractcontactfilter.h"

// Forward declarations

// External data types

// Constants

class CntSymbianFilterSqlHelper
{
public: 
    Q_DECLARE_LATIN1_LITERAL(SingleQuote,"'")  ;
    Q_DECLARE_LATIN1_LITERAL(PercentSign,"%") ;
    Q_DECLARE_LATIN1_LITERAL(Space," ") ;
    Q_DECLARE_LATIN1_LITERAL(EqualTo,"=") ;
    Q_DECLARE_LATIN1_LITERAL(SqlLike,"LIKE") ;
    Q_DECLARE_LATIN1_LITERAL(SqlNotNull,"NOT NULL") ;
    Q_DECLARE_LATIN1_LITERAL(contactsTable,"contact") ;
    Q_DECLARE_LATIN1_LITERAL(commAddrTable,"comm_addr") ;
    //This is copied from pltables.h from cntmodel.
    // This definition needs to be exported and the file included
    // so that duplicity is avoided
    enum TCommAddrType
            {
            EPhoneNumber,
            EEmailAddress,
            ESipAddress
            };
public:
    CntSymbianFilterSqlHelper(CContactDatabase &contactDatabase);
    virtual ~CntSymbianFilterSqlHelper();

public:
    /*Generic functions for all filters*/
    QList<QContactLocalId> searchContacts(const QContactFilter& filter, 
                                           QContactManager::Error& error);
    CntAbstractContactFilter::FilterSupport filterSupported(const QContactFilter& filter);

private:
    void createSqlQuery(const QContactFilter& filter,
                          QString& sqlQuery,
                          QContactManager::Error& error);
    /* Return true if this filter is leaf filter*/ 
    bool isSingleFilter(const QContactFilter& filter) const;
    /*Local helper functions used for creating the sql query */
    void updateSqlQueryForSingleFilter(const QContactFilter& filter,
                                       QString& sqlQuery,
                                       QContactManager::Error& error);
    void updateSqlQueryForDetailFilter(const QContactFilter& filter,
                                       QString& sqlQuery,
                                       QContactManager::Error& error);
    void getSqlDbTableAndColumnNameforDetailFilter(
                                const QContactDetailFilter& filter ,
                                bool& isSubType,
                                QString& tableName,
                                QString& columnName );
    void updateFieldForDeatilFilterMatchFlag( const QContactDetailFilter& filter,
                                              QString& fieldToUpdate ,
                                              QContactManager::Error& error) const;
    void CntSymbianFilterSqlHelper::HandlePhonenumberDetailFilter(const QContactDetailFilter detailFilter);
    void getMatchLengthL(TInt& matchLength);
    TInt CntSymbianFilterSqlHelper::searchPhoneNumbers(
            CContactIdArray*& idArray,
            const TDesC& phoneNumber,
            const TInt matchLength);
    CntAbstractContactFilter::FilterSupport checkIfDetailFilterSupported(const QContactDetailFilter& detailFilter) const;
    
private:
    CntSymbianSrvConnection* m_srvConnection;
    CContactDatabase &m_contactDatabase;
    bool isSearchingDone;
    QHash<int,QString> contactsTableIdColumNameMapping;
    QHash<int,int> commAddrTableIdColumNameMapping;
};

#endif//CNTSYMBIANFILTERSQLHELPER_H
