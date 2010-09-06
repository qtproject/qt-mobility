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


#ifndef QCONTACTSIMULATORBACKEND_P_H
#define QCONTACTSIMULATORBACKEND_P_H

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

#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"
#include "qcontactdetaildefinition.h"
#include "qcontactabstractrequest.h"
#include "qcontactchangeset.h"

#include "qcontactmemorybackend_p.h"

#include "contactconnection_simulator_p.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QContactSimulatorEngine : public QContactMemoryEngine
{
    Q_OBJECT

public:
    static QContactSimulatorEngine *createSimulatorEngine(const QMap<QString, QString>& parameters);

    void setNotifySimulator(bool on);

protected:
    QContactSimulatorEngine(QContactMemoryEngineData *data);

protected:
    bool mNotifySimulator;

    using QContactMemoryEngine::saveContact;
    virtual bool saveContact(QContact* theContact, QContactChangeSet& changeSet, QContactManager::Error* error);
    using QContactMemoryEngine::removeContact;
    virtual bool removeContact(const QContactLocalId& contactId, QContactChangeSet& changeSet, QContactManager::Error* error);
    using QContactMemoryEngine::saveRelationship;
    virtual bool saveRelationship(QContactRelationship* relationship, QContactChangeSet& changeSet, QContactManager::Error* error);
    using QContactMemoryEngine::removeRelationship;
    virtual bool removeRelationship(const QContactRelationship& relationship, QContactChangeSet& changeSet, QContactManager::Error* error);

    static QContactSimulatorEngine* engine;

    friend class Simulator::ContactConnection;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif
