/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QCONTACTLISTMODEL_P_H
#define QCONTACTLISTMODEL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtcontactsglobal.h"
#include "qcontactphonenumber.h"
#include "qcontactrequests.h"

#include "qcontactlistmodel.h"

#include <QSharedData>
#include <QMap>

class QContactListModelPrivate : public QSharedData
{
public:
    QContactListModelPrivate()
            : QSharedData(),
            m_manager(0),
            m_requestPolicy(QContactListModel::CancelOnCacheMissPolicy),
            m_halfCacheSize(10),
            m_quarterCacheSize(5),
            m_lastCacheCentreRow(-1),
            m_currentRow(-1),
            m_relevantDefinitionName(QString(QLatin1String(QContactPhoneNumber::DefinitionName))),
            m_relevantFieldName(QString(QLatin1String(QContactPhoneNumber::FieldNumber))),
            m_idRequest(0)
    {
    }

    QContactListModelPrivate(const QContactListModelPrivate& other)
            : QSharedData(other),
            m_idsToRows(other.m_idsToRows),
            m_rowsToIds(other.m_rowsToIds),
            m_manager(other.m_manager),
            m_requestPolicy(other.m_requestPolicy),
            m_halfCacheSize(other.m_halfCacheSize),
            m_quarterCacheSize(other.m_quarterCacheSize),
            m_cache(other.m_cache),
            m_lastCacheCentreRow(other.m_lastCacheCentreRow),
            m_currentRow(other.m_currentRow),
            m_requestCentreRows(other.m_requestCentreRows),
            m_relevantDefinitionName(other.m_relevantDefinitionName),
            m_relevantFieldName(other.m_relevantFieldName),
            m_idRequest(other.m_idRequest)
    {
    }

    ~QContactListModelPrivate()
    {
        if (m_idRequest) {
            m_idRequest->cancel();
            delete m_idRequest;
        }

        QList<QContactAbstractRequest*> requests = m_requestCentreRows.keys();
        for (int i = 0; i < requests.size(); i++) {
            QContactAbstractRequest* current = requests.at(i);
            current->cancel();
            m_requestCentreRows.remove(current);
            delete current;
        }
    }

    QMap<QContactLocalId, int> m_idsToRows;
    QMap<int, QContactLocalId> m_rowsToIds;

    QContactManager* m_manager;
    QContactListModel::AsynchronousRequestPolicy m_requestPolicy;
    int m_halfCacheSize;
    int m_quarterCacheSize;
    mutable QMap<int, QContact> m_cache;
    mutable int m_lastCacheCentreRow;
    mutable int m_currentRow;
    mutable QMap<QContactAbstractRequest*, int> m_requestCentreRows;

    QString m_relevantDefinitionName;
    QString m_relevantFieldName;

    QContactLocalIdFetchRequest* m_idRequest;
};

#endif
