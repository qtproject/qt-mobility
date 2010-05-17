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

#ifndef QORGANIZERMAEMO5_P_H
#define QORGANIZERMAEMO5_P_H

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

#include <QSharedData>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QQueue>
#include <QPair>
#include <QSet>
#include <QDateTime>
#include <QString>
#include <QObject>

#include "qorganizeritem.h"
#include "qorganizeritemmanager.h"
#include "qorganizeritemmanagerengine.h"
#include "qorganizeritemmanagerenginefactory.h"
#include "qorganizeritemdetaildefinition.h"
#include "qorganizeritemabstractrequest.h"
#include "qorganizeritemchangeset.h"

QTM_USE_NAMESPACE

class Q_DECL_EXPORT QOrganizerItemMaemo5Factory : public QObject, public QOrganizerItemManagerEngineFactory
{
  Q_OBJECT
  Q_INTERFACES(QtMobility::QOrganizerItemManagerEngineFactory)
  public:
    QOrganizerItemManagerEngine* engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error*);
    QString managerName() const;
};

class QOrganizerItemMaemo5EngineData : public QSharedData
{
public:
    QOrganizerItemMaemo5EngineData()
        : QSharedData()
    {
    }

    QOrganizerItemMaemo5EngineData(const QOrganizerItemMaemo5EngineData& other)
        : QSharedData(other),
        m_managerName(other.m_managerName),
        m_managerParameters(other.m_managerParameters),
        m_managerVersion(other.m_managerVersion)
    {
    }

    ~QOrganizerItemMaemo5EngineData()
    {
    }

    QString m_managerName;
    QString m_managerParameters;
    int m_managerVersion;

};

class QOrganizerItemMaemo5Engine : public QOrganizerItemManagerEngine
{
    Q_OBJECT

public:
    static QOrganizerItemMaemo5Engine *createMaemo5Engine(const QMap<QString, QString>& parameters);

    ~QOrganizerItemMaemo5Engine();

    /* URI reporting */
    QString managerName() const;
    QMap<QString, QString> managerParameters() const;
    int managerVersion() const;

    QList<QOrganizerItem> itemInstances(const QOrganizerItem& generator, const QDateTime& periodStart, const QDateTime& periodEnd, int maxCount, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItemLocalId> itemIds(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, QOrganizerItemManager::Error* error) const;
    QList<QOrganizerItem> items(const QOrganizerItemFilter& filter, const QList<QOrganizerItemSortOrder>& sortOrders, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;
    QOrganizerItem item(const QOrganizerItemLocalId& itemId, const QOrganizerItemFetchHint& fetchHint, QOrganizerItemManager::Error* error) const;

    bool saveItems(QList<QOrganizerItem>* items, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);
    bool removeItems(const QList<QOrganizerItemLocalId>& itemIds, QMap<int, QOrganizerItemManager::Error>* errorMap, QOrganizerItemManager::Error* error);

    /* Definitions - Accessors and Mutators */
    QMap<QString, QOrganizerItemDetailDefinition> detailDefinitions(const QString& itemType, QOrganizerItemManager::Error* error) const;
    QOrganizerItemDetailDefinition detailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error) const;
    bool saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& itemType, QOrganizerItemManager::Error* error);
    bool removeDetailDefinition(const QString& definitionId, const QString& itemType, QOrganizerItemManager::Error* error);

    /* Capabilities reporting */
    bool hasFeature(QOrganizerItemManager::ManagerFeature feature, const QString& itemType) const;
    bool isFilterSupported(const QOrganizerItemFilter& filter) const;
    QList<QVariant::Type> supportedDataTypes() const;
    QStringList supportedItemTypes() const;

private:
    // conversion functions
    static QOrganizerEvent convertCEvent(const CEvent& cevent);
    static QOrganizerTodo convertCTodo(const CTodo& ctodo);
    static QOrganizerJournal convertCJournal(const CJournal& cjournal);

private:
    QOrganizerItemMaemo5EngineData* d;

    friend class QOrganizerItemMaemo5EngineFactory;
};

#endif

