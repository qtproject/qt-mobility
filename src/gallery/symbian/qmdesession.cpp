/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

// Backend
#include "qmdesession_p.h"
#include "qgallerymdsutility_p.h"
// API
#include "qgalleryabstractrequest.h"
// Symbian
#include <mdeconstants.h>
#include <e32base.h>

QTM_BEGIN_NAMESPACE

QMdeSession::QMdeSession(QObject *parent)
    :QObject(parent)
    ,m_cmdeSession(NULL)
{
    TRAP_IGNORE( m_cmdeSession = CMdESession::NewL( *this ) );
    m_eventLoop.exec();
}

QMdeSession::~QMdeSession()
{
    if (m_cmdeSession) {
        delete m_cmdeSession;
    }
}

void QMdeSession::HandleSessionOpened(CMdESession& /*aSession*/, TInt /*aError*/)
{
    m_eventLoop.quit();
}

void QMdeSession::HandleSessionError(CMdESession& /*aSession*/, TInt /*aError*/)
{
    m_eventLoop.quit();
}

CMdENamespaceDef& QMdeSession::GetDefaultNamespaceDefL()
{
#ifdef MDS_25_COMPILATION_ENABLED
    return m_cmdeSession->GetDefaultNamespaceDefL();
#else
    CMdENamespaceDef *nameSpaceDef = NULL;
        nameSpaceDef = m_cmdeSession->GetDefaultNamespaceDefL();
    if (!nameSpaceDef) 
        User::Leave(KErrBadHandle);
   
    return *nameSpaceDef;    
#endif //MDS_25_COMPILATION_ENABLED
}

CMdEObject* QMdeSession::GetFullObjectL(const unsigned int id)
{
    if (m_cmdeSession)
        return m_cmdeSession->GetFullObjectL(id);
    else
        return NULL;
}

void QMdeSession::CommitObjectL( CMdEObject& object )
{
    if (m_cmdeSession)
        m_cmdeSession->CommitObjectL( object );
}

CMdEObjectQuery* QMdeSession::NewObjectQueryL(MMdEQueryObserver *observer,
    QGalleryQueryRequest *request,
    int &error)
{
    CMdENamespaceDef& defaultNamespace = GetDefaultNamespaceDefL();
    CMdEObjectQuery* query = NULL;

#ifdef MDS_25_COMPILATION_ENABLED
    if (request->rootType() == QDocumentGallery::File.name()) {
        CMdEObjectDef& mediaObjDef = defaultNamespace.GetObjectDefL(
            MdeConstants::MediaObject::KMediaObject );
        CMdEObjectDef& imageObjDef = defaultNamespace.GetObjectDefL(
            MdeConstants::Image::KImageObject );
        CMdEObjectDef& videoObjDef = defaultNamespace.GetObjectDefL(
            MdeConstants::Video::KVideoObject );
        CMdEObjectDef& audioObjDef = defaultNamespace.GetObjectDefL(
            MdeConstants::Audio::KAudioObject );

        // query media object properties from image and video objects
        RPointerArray<CMdEObjectDef>* objectDefs = new (ELeave) RPointerArray<CMdEObjectDef>( 3 );
        objectDefs->Append( &imageObjDef );
        objectDefs->Append( &videoObjDef );
        objectDefs->Append( &audioObjDef );

        TRAPD( err, query = m_cmdeSession->NewObjectQueryL( mediaObjDef, objectDefs, observer ) );
        if (err) {
            delete objectDefs;
            return NULL;
        }
    } else {
        CMdEObjectDef& objdef = QDocumentGalleryMDSUtility::ObjDefFromItemTypeL(defaultNamespace, request->rootType() );
        if( objdef.Name() == MdeConstants::Object::KBaseObject ) {
            // Base object definition is only returned if the root type does not match supported types
            return NULL;
        }
        query = m_cmdeSession->NewObjectQueryL( defaultNamespace, objdef, observer );
    }

#else
    CMdEObjectDef& objdef = QDocumentGalleryMDSUtility::ObjDefFromItemTypeL(defaultNamespace, request->rootType());
    query = m_cmdeSession->NewObjectQueryL( defaultNamespace, objdef, observer );
#endif //MDS_25_COMPILATION_ENABLED

    q_check_ptr(query); // check if we have a valid pointer as the next function is not leaving
    error = QDocumentGalleryMDSUtility::SetupQueryConditions(query, request, defaultNamespace);

    return query;
}

int QMdeSession::RemoveObject( const unsigned int itemId )
{
    TItemId result = 0;
    int ret = QDocumentGallery::NoError;
    TRAPD( err,
        CMdENamespaceDef& defaultNamespaceDef = GetDefaultNamespaceDefL();
        result = m_cmdeSession->RemoveObjectL( itemId, &defaultNamespaceDef )
    );

    if (err == KErrNone) {
        if (result != KNoId) {
            ret = QDocumentGallery::NoError;
        } else {
            ret = QDocumentGallery::ItemIdError;
        }
    } else {
        ret = QDocumentGallery::ConnectionError;
    }

    return ret;
}

void QMdeSession::AddItemAddedObserverL( MMdEObjectObserver& observer, CMdELogicCondition &aCondition )
{
#ifdef MDS_25_COMPILATION_ENABLED
    TLogicConditionOperator logicOperator = aCondition.Operator();
    CMdELogicCondition *condition = CMdELogicCondition::NewL( logicOperator );
    // ownership is passed to the observer.
    m_cmdeSession->AddObjectObserverL( observer, condition, ENotifyAdd );
#endif //MDS_25_COMPILATION_ENABLED
    // adding an itemaddedobserver is not needed for mds 2.0 as mds 2.0 doesn't separate observers
    // by condition operator
}

void QMdeSession::AddItemChangedObserverL( MMdEObjectObserver& observer, RArray<TItemId> &idArray )
{
    CMdELogicCondition *condition = CMdELogicCondition::NewL( ELogicConditionOperatorOr );
    CleanupStack::PushL(condition);
    condition->AddObjectConditionL( idArray );
    CleanupStack::Pop(condition); // condition
#ifdef MDS_25_COMPILATION_ENABLED
    m_cmdeSession->AddObjectObserverL( observer, condition, (ENotifyModify | ENotifyRemove) );
#else
    // TODO: check whether default namespace is ok (NULL -> default used)
    m_cmdeSession->AddObjectObserverL( observer, condition, NULL );
#endif //MDS_25_COMPILATION_ENABLED
}

void QMdeSession::RemoveObjectObserver( MMdEObjectObserver& observer )
{
    if (m_cmdeSession) {
#ifdef MDS_25_COMPILATION_ENABLED    
        TRAP_IGNORE( m_cmdeSession->RemoveObjectObserverL(observer));
#else
        m_cmdeSession->RemoveObjectObserver(observer);
#endif //MDS_25_COMPILATION_ENABLED
    }
}

#include "moc_qmdesession_p.cpp"
QTM_END_NAMESPACE
