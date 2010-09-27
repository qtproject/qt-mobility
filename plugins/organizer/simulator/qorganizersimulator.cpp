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

#include "qorganizersimulator_p.h"
#include "qtorganizer.h"
#include "connection_p.h"

//QTM_USE_NAMESPACE

QOrganizerItemMemoryEngineData *QOrganizerItemSimulatorEngine::engineData = 0;

QOrganizerItemManagerEngine* QOrganizerItemSimulatorFactory::engine(const QMap<QString, QString>& parameters, QOrganizerItemManager::Error* error)
{
    Q_UNUSED(parameters);
    Q_UNUSED(error);

    // all simulator engines always share the same engineData
    QOrganizerItemMemoryEngineData* data = QOrganizerItemSimulatorEngine::engineData;
    if (data) {
        data->m_refCount.ref();
    } else {
        data = new QOrganizerItemMemoryEngineData();
        data->m_id = QLatin1String("simulatorengine");
        data->m_anonymous = false;

        // we never want the data to be deleted, so push the refcount to two!
        data->m_refCount.ref();

        QOrganizerItemSimulatorEngine::engineData = data;
    }
    QOrganizerItemSimulatorEngine *engine = new QOrganizerItemSimulatorEngine(data);
    Simulator::Connection::instance()->setEngine(engine);
    return engine;
}

QOrganizerItemEngineLocalId* QOrganizerItemSimulatorFactory::createItemEngineLocalId() const
{
    return new QOrganizerItemMemoryEngineLocalId;
}

QOrganizerCollectionEngineLocalId* QOrganizerItemSimulatorFactory::createCollectionEngineLocalId() const
{
    /* TODO - return the localid specific to the engine */
    return new QOrganizerCollectionMemoryEngineLocalId;
}

QString QOrganizerItemSimulatorFactory::managerName() const
{
    /* TODO - put your engine name here */
    return QString("simulator");
}
Q_EXPORT_PLUGIN2(qtorganizer_simulator, QOrganizerItemSimulatorFactory);


QOrganizerItemSimulatorEngine::~QOrganizerItemSimulatorEngine()
{
    // have to be extremely careful with the QOrganizerItemMemoryEngine destructor
    // it decrements the refcount and deletes engineData if the count hits 0
    // luckily, we set up the refcount to always be >= 2
    Q_ASSERT(engineData->m_refCount >= 2);
}

QString QOrganizerItemSimulatorEngine::managerName() const
{
    /* TODO - put your engine name here */
    return QLatin1String("simulator");
}

int QOrganizerItemSimulatorEngine::managerVersion() const
{
    /* TODO - implement this appropriately.  This is strictly defined by the engine, so return whatever you like */
    return 1;
}

bool QOrganizerItemSimulatorEngine::saveItem(QOrganizerItem* theOrganizerItem, const QOrganizerCollectionLocalId& collectionId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    return QOrganizerItemMemoryEngine::saveItem(theOrganizerItem, collectionId, changeSet, error);
}

bool QOrganizerItemSimulatorEngine::removeItem(const QOrganizerItemLocalId& organizeritemId, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    return QOrganizerItemMemoryEngine::removeItem(organizeritemId, changeSet, error);
}

bool QOrganizerItemSimulatorEngine::saveDetailDefinition(const QOrganizerItemDetailDefinition& def, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    return QOrganizerItemMemoryEngine::saveDetailDefinition(def, organizeritemType, changeSet, error);
}

bool QOrganizerItemSimulatorEngine::removeDetailDefinition(const QString& definitionId, const QString& organizeritemType, QOrganizerItemChangeSet& changeSet, QOrganizerItemManager::Error* error)
{
    return QOrganizerItemMemoryEngine::removeDetailDefinition(definitionId, organizeritemType, changeSet, error);
}

QOrganizerItemSimulatorEngine::QOrganizerItemSimulatorEngine(QOrganizerItemMemoryEngineData* data)
    : QOrganizerItemMemoryEngine(data)
{
}
