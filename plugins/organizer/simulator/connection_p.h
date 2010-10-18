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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

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

#include <QtCore/QObject>
#include <QtCore/QList>

#include "qorganizermanager.h"
#include "qorganizerdata_simulator_p.h"

QT_BEGIN_HEADER

QT_FORWARD_DECLARE_CLASS(QLocalSocket)

class QOrganizerSimulatorEngine;

QTM_BEGIN_NAMESPACE

namespace Simulator {

class MobilityConnection;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(MobilityConnection *mobilityCon);
    static Connection *instance();

    void setEngine(QOrganizerSimulatorEngine *engine);

    QLocalSocket *sendSocket();

    // should live in the engine?
    bool mNotifySimulator;

    class IdTranslation
    {
    public:
        QHash<QOrganizerItemId, QOrganizerItemId> items;
        QHash<QOrganizerCollectionId, QOrganizerCollectionId> collections;
    };

    void translateItemIds(QOrganizerItem *item, const IdTranslation &idTranslation);
    void translateCollectionIds(QOrganizerCollection *collection, const IdTranslation &idTranslation);

    IdTranslation mRemoteToLocal;
    IdTranslation mLocalToRemote;

    QString mManagerUri;

private slots:
    // called remotely
    void setOrganizerManagerUri(QString uri);
    void initialOrganizerDataSent();
    void clearOrganizerData(QtMobility::Simulator::OrganizerCollectionId defaultCollectionId);

    void saveOrganizerItem(QtMobility::QOrganizerItem item);
    void removeOrganizerItem(QtMobility::Simulator::OrganizerItemId id);

    void saveOrganizerCollection(QtMobility::QOrganizerCollection collection);
    void removeOrganizerCollection(QtMobility::Simulator::OrganizerCollectionId id);

    void saveOrganizerDetailDefinition(QtMobility::QOrganizerItemDetailDefinition definition,
                                       QString itemType);
    void removeOrganizerDetailDefinition(QString definitionName, QString itemType);

private:
    void getInitialData();

    MobilityConnection *mConnection;
    bool mRegisteredWithSimulator;
    bool mInitialDataReceived;
    QOrganizerSimulatorEngine *mEngine;
    QOrganizerManager mManager;
};

} // namespace Simulator

QTM_END_NAMESPACE
QT_END_HEADER

#endif // CONNECTION_H
