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

#include "cntfilterdetaildisplaylabel.h"
#include "cntdisplaylabel.h"
#include <qcontactname.h>
#include <qcontactorganization.h>

#include <qtcontacts.h>

CntFilterDetailDisplayLabel::CntFilterDetailDisplayLabel(/*CntSymbianSrvConnection &cntServer*/) //: m_srvConnection(cntServer)
{
// TODO Auto-generated constructor stub

}

CntFilterDetailDisplayLabel::~CntFilterDetailDisplayLabel()
{
// TODO Auto-generated destructor stub
}

#ifdef afdsfds0
bool CntFilterDetailDisplayLabel::filterSupported(const QContactFilter& filter) 
{
    bool result = false;

    if( filter.type() == QContactFilter::ContactDetailFilter)
    {
        const QContactDetailFilter detailFilter(filter);
        
        if (detailFilter.detailDefinitionName() == QContactDisplayLabel::DefinitionName)
        {
            result = true;
        }
    }
    
    return false;
}

#endif

void CntFilterDetailDisplayLabel::createSelectQuery(const QContactFilter& filter,
                              QString& sqlQuery,
                              QContactManager::Error* error)

{
    //Commented currently since this will be checked in contacts call intially
    //if(filterSupported(filter))
    {
        const QContactDetailFilter detailFilter(filter);
        
        *error = QContactManager::NoError;
        
        //get the contact fields that should be checked
        CntDisplayLabel displayLabel;
        QList<QPair<QLatin1String, QLatin1String> > contactFields = displayLabel.contactFilterDetails();
        
        //search values
        QStringList searchStrings = detailFilter.value().toStringList();

        //default sql query
        sqlQuery = "SELECT contact_id FROM contact WHERE (type_flags>>24)=0";
        
        //everything ok
        if(!searchStrings.isEmpty() && searchStrings.count() <= contactFields.count() )
        {
            QString subQuery;
            QStringList columns;
            
            //get the column names
            for(int i = 0; i < contactFields.count(); i++)
            {
                columns << columnName(contactFields.at(i));
            }
            
            //single search value
            if(searchStrings.count() == 1)
            {
                createQuerySingleSearchValue(subQuery, searchStrings.at(0), columns);
            }
            
            //multiple search values
            else
            {
                createQueryMultipleSearchValues(subQuery, searchStrings, columns);    
            }
            
            if(!subQuery.isEmpty()){
            sqlQuery += " AND (" + subQuery + ')';
            }
       
            *error = QContactManager::NoError;
        }
        
        //if specified more filter criterias than contact fields return error
        else if(searchStrings.count() > contactFields.count()){
            *error = QContactManager::BadArgumentError;
        }
    }
        
}

QString CntFilterDetailDisplayLabel::createSelectQuery(const QContactFilter& filter,
                                  const QList<QContactSortOrder>& sortOrders,
                                  QContactManager::Error* error) const
{
    Q_UNUSED(sortOrders);
    QString result;
    //Commented currently since this will be checked in contacts call intially
    //if(filterSupported(filter))
    {
        const QContactDetailFilter detailFilter(filter);
        
        *error = QContactManager::NoError;
        
        //get the contact fields that should be checked
        CntDisplayLabel displayLabel;
        QList<QPair<QLatin1String, QLatin1String> > contactFields = displayLabel.contactFilterDetails();
        
        //search values
        QStringList searchStrings = detailFilter.value().toStringList();

        //default sql query
        result = "SELECT contact_id FROM contact WHERE (type_flags>>24)=0";
        
        //everything ok
        if(!searchStrings.isEmpty() && searchStrings.count() <= contactFields.count() )
        {
            QString subQuery;
            QStringList columns;
            
            //get the column names
            for(int i = 0; i < contactFields.count(); i++)
            {
                columns << columnName(contactFields.at(i));
            }
            
            //single search value
            if(searchStrings.count() == 1)
            {
                createQuerySingleSearchValue(subQuery, searchStrings.at(0), columns);
            }
            
            //multiple search values
            else
            {
                createQueryMultipleSearchValues(subQuery, searchStrings, columns);    
            }
            
            if(!subQuery.isEmpty()){
                result += " AND (" + subQuery + ')';
            }
       
            *error = QContactManager::NoError;
        }
        
        //if specified more filter criterias than contact fields return error
        else if(searchStrings.count() > contactFields.count()){
            *error = QContactManager::BadArgumentError;
        }
    }
    
    return result;
}

/* Creates a sql query for a single search value  
 * 
 * \a sqlQuery where the query is written
 * \a searchValue the value that should be used to select contacts
 * \a columns the columns to be looked from
 * 
 */
void CntFilterDetailDisplayLabel::createQuerySingleSearchValue(QString& sqlQuery, const QString &searchValue, const QStringList &columns) const
{
    for (int i = 0; i < columns.count(); i++)
    {
        sqlQuery += createSubQuery(searchValue, columns.at(i));
         
        if( i < (columns.count() - 1))
        {
            sqlQuery += " OR ";
        }
    }
}

/* Creates a sql query for a multiple search values, Note supports only 2 search and columns values currently   
 * 
 * \a sqlQuery where the query is written
 * \a searchValues the value that should be used to select contacts
 * \a columns the columns to be looked from
 */
void CntFilterDetailDisplayLabel::createQueryMultipleSearchValues(QString& sqlQuery, const QStringList &searchValues, const QStringList &columns) const
{
    if( searchValues.count() == 2 && columns.count() == 2 )
    {
        sqlQuery += createSubQuery(searchValues.at(0), columns.at(1)) + " AND ";
        sqlQuery += createSubQuery(searchValues.at(1), columns.at(0)) + " OR ";
        sqlQuery += createSubQuery(searchValues.at(0), columns.at(0)) + " AND ";
        sqlQuery += createSubQuery(searchValues.at(1), columns.at(1));
    }
}
        
/* Creates a sql LIKE Statement for the search valuea and column  
 * 
 * \a searchValue to be added to the query
 * \a column to be added to the query
 * \return the sql LIKE query
 */
QString CntFilterDetailDisplayLabel::createSubQuery(const QString &searchValue, const QString &column) const
{
    return ('(' + column + " LIKE \'" + searchValue + "%\' OR " + column + " LIKE \'% " + searchValue + "%\')");
}

/*
 * Get the sql column name based on the detail 
 * 
 * \a detail to search the column name for
 * \a columnName contains the column name or empty string if none found
 */
QString CntFilterDetailDisplayLabel::columnName(const QPair<QLatin1String, QLatin1String> &detail) const
{
    QString columnName = "";
    
    //Name detail
    if(detail.first == QContactName::DefinitionName)
    {
        if(detail.second == QContactName::FieldFirstName)
        {
            columnName = "first_name";
        }
        
        else if(detail.second == QContactName::FieldLastName)
        {
            columnName = "last_name";
        }
    }
    
    //Organization
    else if(detail.first == QContactOrganization::DefinitionName)
    {
        if(detail.second == QContactOrganization::FieldName)
        {
            columnName = "company_name";
        }
    }
    
    return columnName;
}
