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
#ifndef QFEEDBACK_SYMBIAN_H
#define QFEEDBACK_SYMBIAN_H

#include <QtCore/QElapsedTimer>
#include <QtCore/QHash>
#include <QtGui/QWidget>

#include <qmobilityglobal.h>

#include <qfeedbackplugininterfaces.h>

#include <e32base.h>
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

    virtual PluginPriority pluginPriority();

    virtual QList<QFeedbackActuator> actuators();

    //for device handling
    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);
    virtual bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability);

    virtual void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty);
    virtual void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *);

#ifndef NO_TACTILE_SUPPORT
    virtual bool play(QFeedbackEffect::ThemeEffect);
#endif // NO_TACTILE_SUPPORT

private:
    CHWRMVibra *vibra();


    CCoeControl *defaultWidget();

    CHWRMVibra *m_vibra;
    bool m_vibraActive;

    class PausableElapsedTimer
    {
    public:
        PausableElapsedTimer() : pausedTime(0) { }

        void start()
        {
            m_elapsedTimer.start();
        }

        void pause()
        {
            if (m_elapsedTimer.isValid())
                pausedTime += m_elapsedTimer.elapsed();
            m_elapsedTimer.invalidate();
        }

        bool isPaused() const
        {
            return !m_elapsedTimer.isValid();
        }


        int elapsed() const
        {
            return pausedTime + m_elapsedTimer.isValid() ? m_elapsedTimer.elapsed() : 0;
        }


    private:
        QElapsedTimer m_elapsedTimer;
        int pausedTime;

    };

    QHash<const QFeedbackHapticsEffect*, PausableElapsedTimer> m_elapsed;
};


QT_END_HEADER

#endif
