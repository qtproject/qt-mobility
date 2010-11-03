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

#include <qmobilityglobal.h>
#include "qfeedbackdata_simulator_p.h"
#include <QtCore/QObject>
#include <QtCore/QList>

QT_BEGIN_HEADER

QT_FORWARD_DECLARE_CLASS(QLocalSocket)
class QFeedbackSimulator;

QTM_BEGIN_NAMESPACE


namespace Simulator {

class MobilityConnection;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(MobilityConnection *mobilityCon);
    static Connection *instance();

    void connect(QFeedbackSimulator *plugin);
    int startEffect(int actuatorId, const QString &info, int duration);
    bool resumeEffect(int effectId);
    bool pauseEffect(int effectId);
    bool stopEffect(int effectId);
    void setEffectDuration(int effectId, int duration);
    void setActuatorEnabled(int actuatorId, bool enabled);

private slots:
    // called remotely
    void initialFeedbackDataSent();
    void setActuator(QtMobility::ActuatorData data);
    void removeActuator(int id);
    void setDefaultActuator(int id);
    void setFeedbackEffectState(int effectId, int state);

private:
    void getInitialData();

    MobilityConnection *mConnection;
    bool mRegisteredWithSimulator;
    bool mInitialDataReceived;
    QFeedbackSimulator *mPlugin;
};

} // namespace Simulator

QTM_END_NAMESPACE
QT_END_HEADER

#endif // CONNECTION_H
