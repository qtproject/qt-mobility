/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
