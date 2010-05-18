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

#ifndef CNTFILTERDETAIL_H
#define CNTFILTERDETAIL_H

#include "cntabstractcontactfilter.h"
#include "cntsymbiansrvconnection.h"
#include "cntdbinfo.h"

#include <qmobilityglobal.h>

QTM_USE_NAMESPACE

class CntFilterDetail : public CntAbstractContactFilter
{
private:
    class TMatch {
        public:
            TMatch();
        
            static TInt32 createHash(const TDesC& phoneNumberString,
                                     TInt matchLength,
                                     TInt& numPhoneDigits);
            static void stripOutNonDigitChars(TDes& text);
            static TInt32 padOutPhoneMatchNumber(TInt32& phoneNumber,
                                                 TInt padOutLength);
        public:
            TInt32 iLowerSevenDigits;
            TInt32 iUpperDigits;
            TInt iNumLowerDigits;
            TInt iNumUpperDigits;
    };
public:
    CntFilterDetail(CContactDatabase& contactDatabase, CntSymbianSrvConnection &cntServer,CntDbInfo& dbInfo);
    ~CntFilterDetail();
    QList<QContactLocalId> contacts(
            const QContactFilter &filter,
            const QList<QContactSortOrder> &sortOrders,
            bool &filterSupported,
            QContactManager::Error* error);
    bool filterSupported(const QContactFilter& filter) ;
    
    void getTableNameWhereClause( const QContactDetailFilter& filter,
                                  QString& tableName,
                                  QString& sqlWhereClause ,
                                  QContactManager::Error* error) const;
    void createSelectQuery(const QContactFilter& filter,
                                 QString& sqlQuery,
                                 QContactManager::Error* error);
    void createMatchPhoneNumberQuery(const QContactFilter& filter,
                                     QString& sqlQuery,
                                     QContactManager::Error* error);
private:
    void updateForMatchFlag( const QContactDetailFilter& filter,
                             QString& fieldToUpdate ,
                             QContactManager::Error* error) const;
    QList<QContactLocalId>  HandlePredictiveSearchFilter(const QContactFilter& filter,
                                                         QContactManager::Error* error);

    CntFilterDetail::TMatch createPaddedPhoneDigits(const TDesC& number, 
                                                    const TInt numLowerDigits,
                                                    const TInt numUpperDigits,
                                                    QContactManager::Error* error);
    CntFilterDetail::TMatch createPhoneMatchNumber(const TDesC& text,
                                                   TInt lowerMatchLength,
                                                   TInt upperMatchLength,
                                                   QContactManager::Error* error);
    bool getMatchLengthL(TInt& matchLength);
    
protected:
    CContactDatabase& m_contactdatabase;
    CntSymbianSrvConnection &m_srvConnection;
    CntDbInfo& m_dbInfo;
};

#endif
