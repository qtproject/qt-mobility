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

#ifndef QMDESESSION_P_H
#define QMDESESSION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmobilityglobal.h>

#include "qgalleryqueryrequest.h"

#include <QObject>
#include <QEventloop>
#include <mdesession.h>
#include <mdequery.h>
#include <e32std.h>

QTM_BEGIN_NAMESPACE

class QGalleryAbstractResponse;

class QMdeSession : public QObject, public MMdESessionObserver
{
    Q_OBJECT
public:

    QMdeSession(QObject *parent = 0);
    virtual ~QMdeSession();

public: // From MMdESessionObserver
    /**
     * For checking MdE initialization status
     * @param aSession  MdE Session which was opened
     * @param aError  Error code from the opening
     */
    void HandleSessionOpened( CMdESession &aSession, TInt aError );

    /**
     * For checking MdE session errors
     * @param aSession  MdE Session which was opened
     * @param aError  Error which has occurred
     */
    void HandleSessionError( CMdESession &aSession, TInt aError );

    CMdENamespaceDef& GetDefaultNamespaceDefL();

    CMdEObject* GetFullObjectL( const unsigned int id );

    void CommitObjectL( CMdEObject& object );

    CMdEObjectQuery* NewObjectQueryL(MMdEQueryObserver *observer,
        QGalleryQueryRequest *request,
        int &error);

    int RemoveObject( const unsigned int itemId );

    void AddItemAddedObserverL( MMdEObjectObserver &observer, CMdELogicCondition &condition );
    void AddItemChangedObserverL( MMdEObjectObserver &observer, RArray<TItemId> &idArray );
    void RemoveObjectObserver( MMdEObjectObserver &observer );

private:

    QEventLoop m_eventLoop;
    CMdESession *m_cmdeSession;
};
QTM_END_NAMESPACE

#endif // QMDESESSION_H
