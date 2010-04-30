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

#include <cntitem.h>

#include "cntrelationship.h"
#include "cntsymbiantransformerror.h"

/* ... The macros changed names */
#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

/*!
 * Constructor
 *
 * \a contactDatabase CContactDatabase with established connection to the database
 * \a managerUri current manager uri
 */
CntRelationship::CntRelationship(CContactDatabase* contactDatabase, const QString &managerUri)
    :m_contactDatabase(contactDatabase),
    m_managerUri(managerUri)
{
    CntAbstractRelationship *relationshipGroup = new CntRelationshipGroup(contactDatabase, managerUri);
    m_relationshipMap.insert(relationshipGroup->relationshipType(), relationshipGroup);
}

/*!
 * Destructor
 */
CntRelationship::~CntRelationship()
{
    QMap<QString, CntAbstractRelationship *>::iterator itr;

    /* XXX maybe use qDeleteAll? */
    for (itr = m_relationshipMap.begin(); itr != m_relationshipMap.end(); ++itr)
    {
        CntAbstractRelationship* value = itr.value();
        delete value;
        value = 0;
    }
}

/*!
 * \return whether relationships of type \a relationshipType is supported by contacts of \a contactType
 */
bool CntRelationship::isRelationshipTypeSupported(const QString &relationshipType, const QString &contactType) const
{
    Q_UNUSED(contactType);
    return m_relationshipMap.contains(relationshipType);

    /* XXX Old code:
    QStringList supportedTypes;

    foreach(const QString& type, m_relationshipMap.keys()) {
        supportedTypes.append(type);
    }
    return supportedTypes;
    */
}

/* !
 * Retrive the contacts relationships
 *
 * \a relationshipType The type of the relationship
 * \a participantId The contact id
 * \a role The contact role
 * \a error Error returned
 */
QList<QContactRelationship> CntRelationship::relationships(const QString& relationshipType, const QContactId& participantId, QContactRelationship::Role role, QContactManager::Error* error) const
{
    QList<QContactRelationship> returnValue;
    *error = QContactManager::NoError;

    // if relationshipType is empty, relationships of any type are returned.
    if (relationshipType.isEmpty())
    {
        foreach (const QString& type, m_relationshipMap.keys())
        {
            // get the relationship
            CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(type);

            // retrieve the relationships
            TRAPD(symbianError, returnValue.append(abstractRelationship->relationshipsL(participantId, role, error)));

            // if error translate it into a qt error
            if (symbianError != KErrNone){
                CntSymbianTransformError::transformError(symbianError, error);
            }
            
            // return empty list if there was an error
            if (*error != QContactManager::NoError && *error != QContactManager::DoesNotExistError) {
                return QList<QContactRelationship>();
            }
        }
        // if relationships found, update error
        if (!returnValue.isEmpty() && *error == QContactManager::DoesNotExistError) {
            // this can be the case if nothing is found for last relationship type
            *error = QContactManager::NoError;
        }
    }
    //check if we support the relationship
    else if (m_relationshipMap.contains(relationshipType))
    {
        //get the relationship
        CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationshipType);

        //retrieve the relationships
        TRAPD(symbianError, returnValue = abstractRelationship->relationshipsL(participantId, role, error));

        //if error translate it into a qt error
        if (symbianError != KErrNone){
            CntSymbianTransformError::transformError(symbianError, error);
        }
    }
    else{
        *error = QContactManager::NotSupportedError;
    }
    
    // No relationships found?
    if (*error == QContactManager::NoError && returnValue.count() == 0 ) {
        *error = QContactManager::DoesNotExistError;
    }

    return returnValue;
}

/* !
 * Save the relationship
 *
 * \a affectedContactIds will include the affected contact ids
 * \a relationship to be saved
 * \a error Error returned
 */
bool CntRelationship::saveRelationship(QSet<QContactLocalId> *affectedContactIds, QContactRelationship* relationship, QContactManager::Error* error)
{
    bool returnValue(false);
    *error = QContactManager::NoError;
    if (validateRelationship(*relationship, error))
    {
        // Update manager uri to this manager if it is empty
        if (relationship->second().managerUri().isEmpty()) {
            QContactId second = relationship->second();
            second.setManagerUri(m_managerUri);
            relationship->setSecond(second);
        }
        
        //get the relationship
        CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationship->relationshipType());
    
        //save the relationship
        TRAPD(symbianError, returnValue = abstractRelationship->saveRelationshipL(affectedContactIds, relationship, error));
    
        //if symbian error translate it into a qt error
        if (symbianError != KErrNone){
            returnValue = false;
            CntSymbianTransformError::transformError(symbianError, error);
        }
    }
    return returnValue;
}

/* !
 * Save many relationships
 *
 * \a affectedContactIds will include the affected contact ids
 * \a relationships to be saved
 * \a errorMap storage place for errors
 * \return true if there were no errors saving
 */
bool CntRelationship::saveRelationships(QSet<QContactLocalId> *affectedContactIds, QList<QContactRelationship>* relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    QContactManager::Error singleError;    
    bool returnValue(true);

    *error = QContactManager::NoError;

    // loop through the relationships
    for (int i = 0; i < relationships->count(); i++)
    {
        // save the relationship
        saveRelationship(affectedContactIds, &(relationships->operator[](i)), &singleError);
        if (errorMap && singleError != QContactManager::NoError) {
            errorMap->insert(i, singleError);
        }
        
        // update the total error
        if (singleError != QContactManager::NoError) {
            *error = singleError;
            returnValue = false;
        }

    }

    return returnValue;
}

/* !
 * Remove the relationship
 *
 * \a affectedContactIds will include the affected contact ids
 * \a relationship to be removed
 * \a error Error returned
 * \return true if no error otherwise false
 */
bool CntRelationship::removeRelationship(QSet<QContactLocalId> *affectedContactIds, const QContactRelationship &relationship, QContactManager::Error* error)
{
    bool returnValue(false);
    *error = QContactManager::NoError;
    if (validateRelationship(relationship, error))
    {
        //get the relationship
        CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationship.relationshipType());

        TRAPD(symbianError, returnValue = abstractRelationship->removeRelationshipL(affectedContactIds, relationship, error));

        //if symbian error translate it into a qt error
        if (symbianError != KErrNone){
            returnValue = false;
            CntSymbianTransformError::transformError(symbianError, error);
        }
    }
    return returnValue;
}

/* !
 * Remove many relationships
 *
 * \a affectedContactIds will include the affected contact ids
 * \a relationships to be removed
 * \a errorMap storage place for errors
 * \return true if there were no errors removing, false otherwise
 */
bool CntRelationship::removeRelationships(QSet<QContactLocalId> *affectedContactIds, const QList<QContactRelationship>& relationships, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    bool returnValue(true);
    *error = QContactManager::NoError;
    QContactManager::Error qtError(QContactManager::NoError);

    //loop through the relationships
    for(int i = 0; i < relationships.count(); i++)
    {
        //remove the relationships
        removeRelationship(affectedContactIds, relationships.at(i), &qtError);
        if (errorMap && qtError != QContactManager::NoError)
            errorMap->insert(i, qtError);
        
        // update the total error
        if (qtError != QContactManager::NoError) {
            returnValue = false;
            *error = qtError;
        }
    }
    return returnValue;
}

bool CntRelationship::validateRelationship(const QContactRelationship &relationship, QContactManager::Error* error)
{
    *error = QContactManager::NoError;
    
    // check if supported in this manager
    if (!m_relationshipMap.contains(relationship.relationshipType())) {
        *error = QContactManager::NotSupportedError;
        return false;
    }
    
    QContactId first = relationship.first();
    QContactId second = relationship.second();    
    
    // zero id contacts not accepted
    if (!(first.localId() && second.localId())) {
        *error = QContactManager::InvalidRelationshipError;
        return false;
    }
    
    // "first" must be a contact in this manager
    if (!first.managerUri().isEmpty() && first.managerUri() != m_managerUri) {
        *error = QContactManager::InvalidRelationshipError;
        return false;
    }

    // "first" must be found in the database
    CContactItem* contact = 0;
    TRAP_IGNORE(contact = m_contactDatabase->ReadContactL(first.localId()));
    if (!contact) {
        *error = QContactManager::InvalidRelationshipError;
        return false;
    }
    delete contact;
    
    // check if "second" is in this manager 
    if (second.managerUri().isEmpty() || second.managerUri() == m_managerUri)
    {
        // circular relationships not allowed
        if (first.localId() == second.localId()) {
            *error = QContactManager::InvalidRelationshipError;
            return false;
        }
    
        // "second" must be found in the database
        contact = 0;
        TRAP_IGNORE(contact = m_contactDatabase->ReadContactL(second.localId()));
        if (!contact) {
            *error = QContactManager::InvalidRelationshipError;
            return false;
        }
        delete contact;        
    }
    
    // do additional checks in the actual implementation
    CntAbstractRelationship *abstractRelationship = m_relationshipMap.value(relationship.relationshipType());
    return abstractRelationship->validateRelationship(relationship, error);
}
