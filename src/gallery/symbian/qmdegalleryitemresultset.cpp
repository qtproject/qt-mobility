/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

//Backend
#include "qmdegalleryitemresultset_p.h"
#include "qgallerymdsutility_p.h"
#include "qmdesession_p.h"
//API
#include "qdocumentgallery.h"
#include "qgalleryitemrequest.h"

QTM_BEGIN_NAMESPACE

QMDEGalleryItemResultSet::QMDEGalleryItemResultSet(QMdeSession *session, QObject *parent)
:QMDEGalleryResultSet(parent)
{
    m_request = static_cast<QGalleryItemRequest *>(parent);
    m_live = m_request->autoUpdate();
    m_session = session;
    m_resultObject = NULL;

    createQuery();
}

QMDEGalleryItemResultSet::~QMDEGalleryItemResultSet()
{
    m_session->RemoveObjectObserver( *this );
}
#ifdef MDS_25_COMPILATION_ENABLED
void QMDEGalleryItemResultSet::HandleObjectNotification( CMdESession& aSession,
    TObserverNotificationType aType,
    const RArray<TItemId>& aObjectIdArray )
{
    int err = KErrNone;
    if (aType == ENotifyModify) {
        TRAP(err, doHandleObjectNotificationL(aSession,QMDEGalleryItemResultSet::ENotifyModify, aObjectIdArray);)
    } else if (aType == ENotifyRemove) {
        TRAP(err, doHandleObjectNotificationL(aSession,QMDEGalleryItemResultSet::ENotifyRemove, aObjectIdArray);)
    }
    if (err != KErrNone)
        emit error(err);
}
#else

void QMDEGalleryItemResultSet::HandleObjectAdded(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray)
{
    //No impl needed.
}

void QMDEGalleryItemResultSet::HandleObjectModified(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray)
{
    TRAPD(err, doHandleObjectNotificationL(aSession,QMDEGalleryItemResultSet::ENotifyModify, aObjectIdArray);)

    if (err != KErrNone)
        emit error(err);
}

void QMDEGalleryItemResultSet::HandleObjectRemoved(CMdESession& aSession, const RArray<TItemId>& aObjectIdArray)
{
    TRAPD(err, doHandleObjectNotificationL(aSession,QMDEGalleryItemResultSet::ENotifyRemove, aObjectIdArray);)

    if (err != KErrNone)
        emit error(err);
}
#endif //MDS_25_COMPILATION_ENABLED

void QMDEGalleryItemResultSet::doHandleObjectNotificationL( CMdESession& /*aSession*/,
    QMdeSessionObserverNotificationType aType,
    const RArray<TItemId>& aObjectIdArray )
{
    if (aType == QMDEGalleryItemResultSet::ENotifyModify) {
        m_itemArray.Remove(0);
        delete m_resultObject;
        TRAP_IGNORE( m_resultObject = m_session->GetFullObjectL( aObjectIdArray[0] ) );
        if (m_resultObject) {
            m_itemArray.Append(m_resultObject);
            QString type = QDocumentGalleryMDSUtility::GetItemTypeFromMDEObject( m_resultObject );
            QStringList propertyList;
            QDocumentGalleryMDSUtility::GetDataFieldsForItemType( propertyList, type );
            QList<int> keys;
            const int propertyCount = propertyList.count();
            for (int i = 0; i < propertyCount; i++) {
                keys.append( QDocumentGalleryMDSUtility::GetPropertyKey( propertyList[i] ));
            }
            if (currentIndex() == 0) {
                emit currentItemChanged();
            }
            emit metaDataChanged( 0, 1, keys );
        }
    } else if (aType == QMDEGalleryItemResultSet::ENotifyRemove) {
        m_itemArray.Remove(0);
        delete m_resultObject;
        m_resultObject = NULL;
        m_isValid = false;
        emit itemsRemoved(0, 1);
        if (currentIndex() == 0) {
            emit currentItemChanged();
        }
    }
}

void QMDEGalleryItemResultSet::createQuery()
{
    TRAP_IGNORE( m_resultObject = m_session->GetFullObjectL( m_request->itemId().toUInt() ) );
    // After that resultObject contains NULL or the needed item
    if (m_resultObject) {
        m_currentObjectIDs.Append( m_resultObject->Id() );
        m_itemArray.Append(m_resultObject);
        if (m_live) {
            TRAPD( err,
                m_session->AddItemChangedObserverL( *this, m_currentObjectIDs );
            );
            if (err) {
                m_live = false;
            }
        }
        finish(m_live);
    } else {
        error(QDocumentGallery::ConnectionError);
    }
}

#include "moc_qmdegalleryitemresultset_p.cpp"
QTM_END_NAMESPACE
