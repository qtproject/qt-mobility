/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Qt Software Information (qt-info@nokia.com)
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
#include <e32cmn.h>
#include <cntdb.h>

// User includes
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"
#include "qcontactmanager.h"

// Forward declarations

// External data types

// Constants

class CntSymbianFilterSqlHelper : public RSessionBase
{
public:
    CntSymbianFilterSqlHelper();
    ~CntSymbianFilterSqlHelper();

public:
    /*Generic functions for all filters*/
    QList<QContactLocalId> SearchContactsL(const QContactFilter& filter, 
                                           QContactManager::Error& error);
    
private:
    void InitializeDbL();

    void createSqlQuery(const QContactFilter& filter,
                          QString& sqlQuery,
                          QContactManager::Error& error);
    /* Return true if this filter is leaf filter*/ 
    bool isSingleFilter(const QContactFilter& filter, 
                             QContactManager::Error& error) const;
    /*Local helper functions used for creating the sql query */
    void updateSqlQueryForSingleFilter(const QContactFilter& filter,
                                       QString& sqlQuery,
                                       QContactManager::Error& error);
    void updateSqlQueryForDetailFilter(const QContactFilter& filter,
                                       QString& sqlQuery,
                                       QContactManager::Error& error);
    void initializeTransformContactData();
    void createDatabaseColumnMap();
    void convertFieldIdToSqlDbColumnName(const quint32 fieldId,
                                         QString& sqlDbTableColumnName );
    
    /* Create a session to the cntsrv */
    void ConnectSrvL();
    void OpenDatabaseL();
    TVersion Version() const;
    TDes8& GetReceivingBufferL(TInt aSize=0) const;
    QList<QContactLocalId> UnpackCntIdArrayL() const;

private: //class varibles   
    CBufFlat* m_buffer;
    mutable TInt m_maxBufferSize;
    mutable TPtr8 m_bufPtr;
    bool m_isDbInitialized;
};

#endif//CNTSYMBIANFILTERSQLHELPER_H
