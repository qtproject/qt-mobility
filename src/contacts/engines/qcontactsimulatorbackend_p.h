/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

    static QContactMemoryEngineData* engineData;

    friend class Simulator::ContactConnection;
};

QTM_END_NAMESPACE
QT_END_HEADER

#endif
