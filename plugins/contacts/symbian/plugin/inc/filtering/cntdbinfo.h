/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef CNTDBINFO_H_
#define CNTDBINFO_H_

#include <qtcontacts.h>
#include <qcontactfilter.h>
#include <qcontactmanager.h>
#include <qcontactsortorder.h>
#include <QPair>

QTM_USE_NAMESPACE

class CntSymbianSrvConnection;
class CntSymbianFilter;
class CntDbInfo : public QObject
{
    Q_OBJECT
    
public:
    enum TCommAddrType
            {
            EPhoneNumber,
            EEmailAddress,
            ESipAddress
            };
			
	// comm address table's extra type info values. specify if the phone number is mobile or non mobile 		
    enum TCommAddrExtraInfoType
            {
            ENonMobileNumber = 0,
            EMobileNumber
            };
public:
    CntDbInfo(QContactManagerEngine* engine);
    virtual ~CntDbInfo();
    
    void getDbTableAndColumnName( const QString definitionName,
                                  const QString fieldName,
                                  QString& tableName,
                                  QString& columnName,
                                  bool& isSubType) const;
    bool SupportsDetail(QString definitionName, QString fieldName);

    QString getSortQuery(const QList<QContactSortOrder> &sortOrders,
                         const QString& selectQuery,
                         QContactManager::Error* error);
    bool isStringFieldType(const QString definitionName);
    void setDirAndCaseSensitivity(QContactSortOrder s, QString& str);

private:
    QHash<QString,QString> contactsTableIdColumNameMapping;
    QHash<QString,QPair<int,bool> > commAddrTableIdColumNameMapping;
    
    QHash<QString,int> commAddrExtraInfoMap;
    QContactManagerEngine* m_engine; // not owned
};

#endif /* CNTDBINFO_H_ */
