/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qfeedbackactuator.h>
#include "qfeedback_symbian.h"
#include <QtCore/QVariant>
#include <QtCore/QtPlugin>
#include <QtGui/QApplication>

#ifdef USE_CHWRMHAPTICS_PLZ
#include <e32def.h>
#include <hwrmhaptics.h>
#include <hwrmlogicalactuators.h>
#endif //USE_CHWRMHAPTICS_PLZ

#ifdef USE_CHWRMVIBRA_PLZ
#include <hwrmvibra.h>
#endif //USE_CHWRMVIBRA_PLZ

#ifdef HAS_THEME_EFFECTS
#include <touchfeedback.h>
#endif //HAS_THEME_EFFECTS

Q_EXPORT_PLUGIN2(feedback_symbian, QFeedbackSymbian)


//TODO: is activeWindow good enough
//or should we create a widget for that?
CCoeControl *QFeedbackSymbian::defaultWidget()
{
    QWidget *w = QApplication::activeWindow();
    return w ? w->winId() : 0;
}

QFeedbackSymbian::QFeedbackSymbian()
{
#ifdef USE_CHWRMVIBRA_PLZ
    m_vibra = 0;
    m_vibraActive = false;
#endif

    initActuators();
}

QFeedbackSymbian::~QFeedbackSymbian()
{
#ifdef USE_CHWRMHAPTICS_PLZ
    qDeleteAll(m_haptics);
#endif
}

// TODO does this need to be vibra vs haptics?
void QFeedbackSymbian::initActuators()
{
#ifdef USE_CHWRMHAPTICS_PLZ
    // Retrieve the supported actuators from the haptics backend
    // XXX TODO: is this correct?  What about user-installed actuators?
    TUint32 supportedActuators(0);
    TInt err(KErrNone);
    CHWRMHaptics* haptics(0);
    QT_TRAP_THROWING(haptics = CHWRMHaptics::NewL(NULL, NULL));
    if (!haptics)
        return; // unable to initialize haptics backend.
    haptics->SupportedActuators(supportedActuators);
    if (EHWRMLogicalActuatorDevice & supportedActuators) {
        m_actuators << createFeedbackActuator(this, EHWRMLogicalActuatorDevice);
    }
    if (EHWRMLogicalActuatorAny & supportedActuators) {
        m_actuators << createFeedbackActuator(this, EHWRMLogicalActuatorAny);
    }
    delete haptics;
    return;
#endif //USE_CHWRMHAPTICS_PLZ

#ifdef USE_CHWRMVIBRA_PLZ
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
    //if we don't have advanced tactile support then the MTouchFeedback doesn't really support custom effects
    if (touchInstance()->TouchFeedbackSupported()) {
        m_actuators << createFeedbackActuator(this, TOUCH_DEVICE);
    }
#endif //HAS_ADVANCED_TACTILE_SUPPORT
    m_actuators << createFeedbackActuator(this, VIBRA_DEVICE);
#endif //USE_CHWRMVIBRA_PLZ
}

QFeedbackInterface::PluginPriority QFeedbackSymbian::pluginPriority()
{
    return PluginLowPriority;
}

QList<QFeedbackActuator*> QFeedbackSymbian::actuators()
{
    return m_actuators;
}
