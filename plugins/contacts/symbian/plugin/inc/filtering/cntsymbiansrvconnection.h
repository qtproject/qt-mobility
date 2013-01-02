/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
    QList<QPair<QContactLocalId, QString> > searchPhoneNumbers(const QString& searchQuery, 
                                                  QContactManager::Error* error);

private:
    /* Symbian Leaving functions */
    QList<QContactLocalId> searchContactIdsL(const TDesC& aSqlQuery, QueryType aQueryType);
    QList<QContact> searchContactNamesL(const TDesC& aSqlQuery);
    QList<QPair<QContactLocalId, QString> > searchPhoneNumbersL(const TDesC& aSqlQuery, QueryType aQueryType);
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
