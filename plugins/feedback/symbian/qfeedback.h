/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
#ifndef QFEEDBACK_SYMBIAN_H
#define QFEEDBACK_SYMBIAN_H

#include <QtGui/QWidget>

#include <qmobilityglobal.h>

#include <qfeedbackplugininterfaces.h>

#include <hwrmvibra.h>


QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QTouchFeedback;

class QFeedbackSymbian : public QObject, public QFeedbackHapticsInterface
#ifndef NO_TACTILE_SUPPORT
, public QFeedbackThemeInterface
#endif //NO_TACTILE_SUPPORT
{
    Q_OBJECT
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackHapticsInterface)
#ifndef NO_TACTILE_SUPPORT
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackThemeInterface)
#endif //NO_TACTILE_SUPPORT
public:
    QFeedbackSymbian();
    virtual ~QFeedbackSymbian();

    virtual QList<QFeedbackActuator> actuators();

    //for device handling
    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);
    virtual bool isActuatorCapabilitySupported(QFeedbackActuator::Capability);

    virtual QFeedbackEffect::ErrorType updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty);
    virtual QFeedbackEffect::ErrorType updateEffectState(const QFeedbackHapticsEffect *);
    virtual QAbstractAnimation::State actualEffectState(const QFeedbackHapticsEffect *);

#ifndef NO_TACTILE_SUPPORT
    virtual bool play(QFeedbackEffect::ThemeEffect);
#endif // NO_TACTILE_SUPPORT

private:
    CHWRMVibra *vibra();


    CCoeControl *defaultWidget();

    CHWRMVibra *m_vibra;
    bool m_vibraActive;
};


QT_END_HEADER

#endif
