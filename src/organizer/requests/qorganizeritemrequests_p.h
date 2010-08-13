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

#ifndef QORGANIZERITEMREQUESTS_P_H
#define QORGANIZERITEMREQUESTS_P_H

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

#include "qorganizeritemabstractrequest_p.h"
#include "qorganizeritemfilter.h"
#include "qorganizeritemsortorder.h"
#include "qorganizeritem.h"
#include "qtorganizerglobal.h"

#include <QStringList>
#include <QList>

QTM_BEGIN_NAMESPACE

class QOrganizerItemSaveRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemSaveRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemSaveRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::ItemSaveRequest;
    }

    QList<QOrganizerItem> m_organizeritems;
    QMap<int, QOrganizerItemManager::Error> m_errors;
};

class QOrganizerItemFetchRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemFetchRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemFetchRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::ItemFetchRequest;
    }

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;
};

class QOrganizerItemInstanceFetchRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemInstanceFetchRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemInstanceFetchRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::ItemInstanceFetchRequest;
    }

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;
    QOrganizerItemFetchHint m_fetchHint;

    QList<QOrganizerItem> m_organizeritems;
};

class QOrganizerItemRemoveRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemRemoveRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemRemoveRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::ItemRemoveRequest;
    }

    QList<QOrganizerItemLocalId> m_organizeritemIds;
    QMap<int, QOrganizerItemManager::Error> m_errors;
};

class QOrganizerItemLocalIdFetchRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemLocalIdFetchRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate()
    {
    }

    ~QOrganizerItemLocalIdFetchRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::ItemLocalIdFetchRequest;
    }

    QOrganizerItemFilter m_filter;
    QList<QOrganizerItemSortOrder> m_sorting;

    QList<QOrganizerItemLocalId> m_ids;
};

class QOrganizerItemDetailDefinitionFetchRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemDetailDefinitionFetchRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate(),
        m_organizeritemType(QString(QLatin1String(QOrganizerItemType::TypeNote)))
    {
    }

    ~QOrganizerItemDetailDefinitionFetchRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::DetailDefinitionFetchRequest;
    }

    QString m_organizeritemType;
    QStringList m_names;
    QMap<QString, QOrganizerItemDetailDefinition> m_definitions;
    QMap<int, QOrganizerItemManager::Error> m_errors;
};

class QOrganizerItemDetailDefinitionSaveRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemDetailDefinitionSaveRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate(),
        m_organizeritemType(QString(QLatin1String(QOrganizerItemType::TypeNote)))
    {
    }

    ~QOrganizerItemDetailDefinitionSaveRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::DetailDefinitionSaveRequest;
    }

    QString m_organizeritemType;
    QList<QOrganizerItemDetailDefinition> m_definitions;
    QMap<int, QOrganizerItemManager::Error> m_errors;
};

class QOrganizerItemDetailDefinitionRemoveRequestPrivate : public QOrganizerItemAbstractRequestPrivate
{
public:
    QOrganizerItemDetailDefinitionRemoveRequestPrivate()
        : QOrganizerItemAbstractRequestPrivate(),
        m_organizeritemType(QString(QLatin1String(QOrganizerItemType::TypeNote)))
    {
    }

    ~QOrganizerItemDetailDefinitionRemoveRequestPrivate()
    {
    }

    QOrganizerItemAbstractRequest::RequestType type() const
    {
        return QOrganizerItemAbstractRequest::DetailDefinitionRemoveRequest;
    }

    QString m_organizeritemType;
    QStringList m_names;
    QMap<int, QOrganizerItemManager::Error> m_errors;
};



QTM_END_NAMESPACE

#endif
