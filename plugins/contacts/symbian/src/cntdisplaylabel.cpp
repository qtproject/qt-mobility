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

#include "cntdisplaylabel.h"
#include <qtcontacts.h>

/*! 
 * Constructor
 */

CntDisplayLabel::CntDisplayLabel()
{
    setDisplayLabelDetails();
}

/*! 
 * Destructor
 */
CntDisplayLabel::~CntDisplayLabel()
{
}

/*! 
 * Sets the fields that should be used for displaylabel
 * 
 * The order is defined in the QList of preferred options
 * Whereas the qmap includes the fields that should be used for 
 * the specific option
 */
void CntDisplayLabel::setDisplayLabelDetails()
{
    //this will be variated in the future
    //add the contact details definition names + fields to be used.
    //preferred details to be used (note qmap sets the values in reverse order)
    QMap<QString, QString> preferredContactDetails;
    preferredContactDetails.insertMulti(QContactName::DefinitionName, QContactName::FieldLast);
    preferredContactDetails.insertMulti(QContactName::DefinitionName, QContactName::FieldFirst);
    m_contactDetails.append(preferredContactDetails);        
    
    //if preferred details not exist use these
    QMap<QString, QString> preferredContactDetails2;
    preferredContactDetails2.insertMulti(QContactOrganization::DefinitionName, QContactOrganization::FieldName);
    m_contactDetails.append(preferredContactDetails2);    
    
    //preferred group details to be used
    QMap<QString, QString> preferredGroupDetails;
    preferredGroupDetails.insert(QContactName::DefinitionName, QContactName::FieldCustomLabel);
    m_groupDetails.append(preferredGroupDetails);
}

/*! 
 * Returns a synthesised display label of a contact
 * \a contact to read the data from .
 * \a error On return, contains the possible error.
 * \return synthesised display label 
 */
QString CntDisplayLabel::synthesizeDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    QString displayLabel;
    
    //contact
    if(contact.type() == QContactType::TypeContact) {
        displayLabel = generateDisplayLabel(contact, contactDisplayLabelDetails());    
    }
    
    //group
    else if (contact.type() == QContactType::TypeGroup) {
        displayLabel = generateDisplayLabel(contact, groupDisplayLabelDetails());
    } 
    
    //invalid type
    else {
        error = QContactManager::InvalidContactTypeError;
    }
    
    return displayLabel;
}

/*! 
 * Generate the display label
 * \a contact to read the data from .
 * \a detailList contains the details to be read from the contact
 * \return synthesised display label 
 */
QString CntDisplayLabel::generateDisplayLabel( const QContact &contact, const QList<QMap<QString, QString> > detailList) const
{
    QString displayLabel("");
    
    for(int i = 0; i < detailList.count() && displayLabel.isEmpty(); i++ )
    {
        QMap<QString, QString> detailMap = detailList.at(i);
        
        //iterate through the details
        QMapIterator<QString, QString> iterator(detailMap);
        QContactDetail contactDetail;
        
        while (iterator.hasNext()) {
            iterator.next();
            
            //read the detail
            contactDetail = contact.detail(iterator.key());
            
            if(displayLabel.isEmpty()){ //read the value and set it as display label
                displayLabel =  contactDetail.value(iterator.value());
            }
            else{ //read the value and append it to the display label
                QString label = contactDetail.value(iterator.value());
                
                if(!label.isEmpty())
                {
                    displayLabel.append(delimiter());
                    displayLabel.append(label);
                }
            }
        }
    }
    
    //no display label, set default value
    if(displayLabel.isEmpty())
    {
        //should the unnamned be read from somewhere?
        displayLabel = QString(tr("Unnamed"));
    }
    
    return displayLabel;
}

/*! 
 * delemiter to be used in display label
 * \return delemiter 
 */
QString CntDisplayLabel::delimiter() const
{
    //this will be variated in the future.
    return " ";
}

/*!
 * Returns the display name detail definition names used for a contact
 */
QList<QMap<QString, QString> > CntDisplayLabel::contactDisplayLabelDetails() const
{
    return m_contactDetails;
}

/*!
 * Returns the display name detail definition names used by groups
 */
QList<QMap<QString, QString> > CntDisplayLabel::groupDisplayLabelDetails() const
{
    return m_groupDetails;
}
