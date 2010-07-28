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
#ifndef CNTSRVCONNECTION_H
#define CNTSRVCONNECTION_H

// System includes
#include <e32std.h>
#include <cntdb.h>
#include <qcontactmanager.h>
#include <qcontactmanagerengine.h>

// User includes

// Forward declarations

// External data types

// Constants

QTM_USE_NAMESPACE
class CntSymbianSrvConnection : public RSessionBase
{
public:

    enum QueryType {
            CntNotdefined = 0,
            CntPredictiveSearchList = 98,
            CntSearchResultList = 99
        };
public:
    /*Constructor and destructor*/
    CntSymbianSrvConnection(QContactManagerEngine* engine);
    ~CntSymbianSrvConnection();
    
public:
    /* QT like functions */
    QList<QContactLocalId> searchContacts(const QString& searchQuery, 
                                          QContactManager::Error* error);
    QContact searchContactName(QContactLocalId contactId,
                               QContactManager::Error* error);
    QList<QContactLocalId> searchOnServer(const QString& searchQuery,
                            QueryType aQueryType,
                            QContactManager::Error* error);
    QList<QContact> searchAllContactNames(QContactManager::Error* error);

private:
    /* Symbian Leaving functions */
    QList<QContactLocalId> searchContactIdsL(const TDesC& aSqlQuery, QueryType aQueryType);
    QList<QContact> searchContactNamesL(const TDesC& aSqlQuery);
    void readContactsToBufferL(const TDesC& aSqlQuery, QueryType aQueryType);
    void ConnectSrvL();
    void OpenDatabaseL();
    TVersion Version() const;
    TDes8& GetReceivingBufferL(TInt aSize=0);

private:
    /* member varibles */ 
    QContactManagerEngine* m_manager;
    CBufFlat* m_buffer;
    TInt m_maxBufferSize;
    TPtr8 m_bufPtr;
    bool m_isInitialized;
    
    friend class ut_cntsymbianfiltersqlhelper;
};

#endif //CNTSRVCONNECTION_H
