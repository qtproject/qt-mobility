/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtcontacts.h"
#include "qversitproperty.h"
#include "cntversitlandlineplugin.h"


/*!
    \class CntVersitLandlinePlugin
    \brief Implements the QVersitContactHandler interface for custom handling of 
    phone numbers with subtype QContactPhoneNumber::SubTypeLandline during import\export 
    of versit documents like vCards. 
  
    This plugin maps QContactPhoneNumber::SubTypeLandline to vCard type "VOICE". 

    \sa CntVersitLandlinePluginFactory
    
    
    IMPORT:
           Simply replace all subtypes QContactPhoneNumber::SubTypeVoice 
           with QContactPhoneNumber::SubTypeLandline in imported contact details
    
    EXPORT: 
           Append versit type parameter "VOICE" for subtype
           QContactPhoneNumber::SubTypeLandline to exported versit properties
*/





/*!
   Called for each processed versit property from QVersitContactImporter
   during contact import. 
 */
void CntVersitLandlinePlugin::propertyProcessed(
    const QVersitDocument& document,
    const QVersitProperty& property,
    const QContact& contact,
    bool* alreadyProcessed,
    QList<QContactDetail>* updatedDetails)
{
    Q_UNUSED(document);
    Q_UNUSED(contact);
    Q_UNUSED(property);
    Q_UNUSED(alreadyProcessed);
    Q_UNUSED(updatedDetails);
}

/*!
  Called from QVersitContactImporter as a final step during 
  contact import once all the document properties have been processed.
 */
void CntVersitLandlinePlugin::documentProcessed(
        const QVersitDocument& document, 
        QContact* contact)
{
    Q_UNUSED(document);
    
    // QtPhonebook backend doesn't support QContactPhoneNumber::SubTypeVoice.
    // Simply replace all occurrences with QContactPhoneNumber::SubTypeLandline
    // for every imported phone number. 
    
    // go through the list of imported details 
    QList<QContactDetail> details = contact->details();
    for (int i = 0;i < details.count();i++) {
        QContactDetail detail = details.at(i);
        if (detail.definitionName() == QContactPhoneNumber::DefinitionName) {
            QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
            // for every phone number, try convert SubTypeVoice to SubTypeLandline
            QStringList newsubtypes = phoneNumber.subTypes().replaceInStrings(
                    QContactPhoneNumber::SubTypeVoice,
                    QContactPhoneNumber::SubTypeLandline,
                    Qt::CaseInsensitive);    
            
            // save the changes if something was actually replaced
            if (newsubtypes != phoneNumber.subTypes()) {
                phoneNumber.setSubTypes(newsubtypes);
                contact->saveDetail(&phoneNumber);
            }
        }
    }
}



/*!
  Called for each processed detail from QVersitContactExporter during 
  contact export. 
  
  If the current detail being exported is a landline phone number, this
  plugin exports it to vCard type "VOICE".
 */
void CntVersitLandlinePlugin::detailProcessed(
    const QContact& contact,
    const QContactDetail& detail,
    const QVersitDocument& document,
    QSet<QString>* processedFields,
    QList<QVersitProperty>* toBeRemoved,
    QList<QVersitProperty>* toBeAdded)
{
    Q_UNUSED(contact);
    Q_UNUSED(processedFields);
    Q_UNUSED(document);
    Q_UNUSED(toBeRemoved);
   
    
    if ( detail.definitionName() == QContactPhoneNumber::DefinitionName ) { 
        QContactPhoneNumber number = static_cast<QContactPhoneNumber> (detail);  
        
        if (number.subTypes().contains(QContactPhoneNumber::SubTypeLandline, 
                Qt::CaseInsensitive)) {       
            exportLandLineToVoice(toBeAdded, number.number());
        }  
    }  
}  

/*!
  Called from QVersitContactExporter as a final step during 
  contact export once all the contact details have been processed 
 */
void CntVersitLandlinePlugin::contactProcessed(const QContact& contact, QVersitDocument* document)
{
    Q_UNUSED(contact);
    Q_UNUSED(document);  
    // no action
}



/*!
   In the list of exported properties, add the type parameter "VOICE" for 
   a property matching number. Called from detailProcessed during contact export.
   
   /sa detailProcessed
 */
void CntVersitLandlinePlugin::exportLandLineToVoice(
        QList<QVersitProperty> *toBeAdded,
        QString number)
{ 
    bool foundmatch = false;   
    
    // find the property matching to given number from 
    // a list of versit properties to be exported
    QMutableListIterator<QVersitProperty> i(*toBeAdded);         
    while (i.hasNext() && !foundmatch) {
        QVersitProperty property = i.next();
        
        // when the matching number is found, append the type parameter "VOICE"
        if (property.value() == number 
                && property.name() == QLatin1String("TEL") ) { 
                 
            // append the type parameter "VOICE" to the list 
            QStringList typeParameters = property.parameters().values(QLatin1String("TYPE"));       
            typeParameters << QLatin1String("VOICE");
            typeParameters.removeDuplicates();
            
            // insertParameter() always inserts to the beginning so to preserve the 
            // ordering, remove first all current "TYPE" parameters and then iterate 
            // the list backwards and insert parameters back again.
            property.removeParameters(QLatin1String("TYPE"));            
            while (!typeParameters.isEmpty()) {
                QString value = typeParameters.takeLast();        
                property.insertParameter(QLatin1String("TYPE"), value);
            }
            // save the changes
            i.setValue(property);      
            foundmatch = true;
        }
    }
}

// End of File
