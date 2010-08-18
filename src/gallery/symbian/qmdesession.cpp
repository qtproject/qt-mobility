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

#include "qmdesession.h"
#include "qgalleryabstractrequest.h"

#include "qmdequery.h"
#include <e32base.h>

QTM_BEGIN_NAMESPACE

QMdeSession::QMdeSession(QObject *parent)
:QObject(parent)
,m_cmdeSession(NULL)
{
    TRAP_IGNORE( m_cmdeSession = CMdESession::NewL( *this ) );
}

QMdeSession::~QMdeSession()
{
    if (m_cmdeSession) {
        delete m_cmdeSession;
    }
}

void QMdeSession::HandleSessionOpened(CMdESession& /*aSession*/, TInt /*aError*/)
{

}

void QMdeSession::HandleSessionError(CMdESession& /*aSession*/, TInt /*aError*/)
{

}

CMdENamespaceDef& QMdeSession::GetDefaultNamespaceDefL()
{
    return m_cmdeSession->GetDefaultNamespaceDefL();
}

QMdeQuery* QMdeSession::NewObjectQueryL(CMdENamespaceDef &aNamespaceDef,
    CMdEObjectDef& aObjectDef, QGalleryAbstractResponse *aResponse)
{
    QMdeQuery* ret = new QMdeQuery(aResponse, this);
    CMdEObjectQuery* mdeQuery = m_cmdeSession->NewObjectQueryL(aNamespaceDef, aObjectDef, ret);
    ret->SetQuery(mdeQuery);

    return ret;
}

int QMdeSession::RemoveObject(TInt aItemId)
{
    TItemId result = 0;
    int ret = QGalleryAbstractRequest::NoResult;
    CMdENamespaceDef& defaultNamespaceDef = GetDefaultNamespaceDefL();
    TRAPD( err, result = m_cmdeSession->RemoveObjectL( aItemId, &defaultNamespaceDef ) );

    if (err == KErrNone) {
        if (result != KNoId) {
            ret = QGalleryAbstractRequest::Succeeded;
        }
        else {
            ret = QGalleryAbstractRequest::RequestError;
        }
    }
    else {
        ret = QGalleryAbstractRequest::RequestError;
    }

    return ret;
}
#include "moc_qmdesession.cpp"
QTM_END_NAMESPACE
