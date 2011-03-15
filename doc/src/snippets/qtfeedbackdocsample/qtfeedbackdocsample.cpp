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

#include "qmobilityglobal.h"
#include <qfeedbackactuator.h>
#include <qfeedbackeffect.h>

#include <QDebug>

QTM_USE_NAMESPACE

void completeExample();

void completeExample()
{
//! [Play the system theme button click effect]
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBasicButton);
//! [Play the system theme button click effect]

//! [Play the system theme bounce effect]
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeBounceEffect);
//! [Play the system theme bounce effect]

//! [Define a custom haptic effect]
    QFeedbackHapticsEffect rumble;
    rumble.setAttackIntensity(0.0);
    rumble.setAttackTime(250);
    rumble.setIntensity(1.0);
    rumble.setDuration(1000);
    rumble.setFadeTime(250);
    rumble.setFadeIntensity(0.0);
//! [Define a custom haptic effect]

//! [Start playing a custom haptic effect]
    rumble.start();
//! [Start playing a custom haptic effect]

//! [Pause a custom haptic effect]
    rumble.pause();
//! [Pause a custom haptic effect]

//! [Stop playing a custom haptic effect]
    rumble.stop();
//! [Stop playing a custom haptic effect]

//! [Query the state of a custom haptic effect]
    if (rumble.state() == QFeedbackEffect::Stopped)
        qDebug() << "The device has stopped rumbling!";
//! [Query the state of a custom haptic effect]

//! [Set the actuator which should play the custom effect]
    QFeedbackActuator *actuator = 0; // default system actuator
    QList<QFeedbackActuator*> actuators = QFeedbackActuator::actuators();
    foreach (QFeedbackActuator* temp, actuators) {
        if (temp->name() == "ExampleActuatorName") {
            actuator = temp;
        }
    }
    rumble.setActuator(actuator);
//! [Set the actuator which should play the custom effect]

//! [Play a haptic effect from a file]
    QFeedbackFileEffect hapticTune;
    hapticTune.setSource(QUrl::fromLocalFile("mySavedRumble.ifr"));
    hapticTune.load();
    hapticTune.start();
//! [Play a haptic effect from a file]
}
