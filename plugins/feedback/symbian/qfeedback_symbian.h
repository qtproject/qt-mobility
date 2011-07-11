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
#ifndef QFEEDBACK_SYMBIAN_H
#define QFEEDBACK_SYMBIAN_H

#include <QtCore/QHash>
#include <QtCore/QTimer>
#include <QtGui/QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
#include <QtCore/QElapsedTimer>
#else
#include <QtCore/QTime>
#endif

#include <qmobilityglobal.h>
#include <qfeedbackplugininterfaces.h>
#include <e32base.h>

#ifdef USE_CHWRMVIBRA_PLZ
#include <hwrmvibra.h>
#define VIBRA_DEVICE 0
#endif //USE_CHWRMVIBRA_PLZ

#ifdef HAS_THEME_EFFECTS
class MTouchFeedback;
#ifdef HAS_ADVANCED_TACTILE_SUPPORT
#define TOUCH_DEVICE 1
#endif //HAS_ADVANCED_TACTILE_SUPPORT
#endif //HAS_THEME_EFFECTS

#ifdef USE_CHWRMHAPTICS_PLZ
#include <hwrmhaptics.h>
#endif //USE_CHWRMHAPTICS_PLZ

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

// We have theme effects on 5.0 onwards
// and file effects if CHWRMHaptics is found (s^3 onwards?)

#ifdef HAS_CUSTOM_EFFECTS
class QFeedbackSymbian : public QObject, public QFeedbackHapticsInterface
#ifdef HAS_THEME_EFFECTS
, public QFeedbackThemeInterface
#endif
#ifdef HAS_FILE_EFFECTS
, public QFeedbackFileInterface
#endif
#ifdef USE_CHWRMHAPTICS_PLZ
, public MHWRMHapticsObserver
#endif
{
    Q_OBJECT
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackHapticsInterface)
#ifdef HAS_THEME_EFFECTS
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackThemeInterface)
#endif
#ifdef HAS_FILE_EFFECTS
    Q_INTERFACES(QTM_NAMESPACE::QFeedbackFileInterface)
#endif
public:
    QFeedbackSymbian();
    virtual ~QFeedbackSymbian();

    virtual PluginPriority pluginPriority();

    virtual QList<QFeedbackActuator*> actuators();

    //for device handling
    virtual void setActuatorProperty(const QFeedbackActuator &, ActuatorProperty, const QVariant &);
    virtual QVariant actuatorProperty(const QFeedbackActuator &, ActuatorProperty);
    virtual bool isActuatorCapabilitySupported(const QFeedbackActuator &, QFeedbackActuator::Capability);

    virtual void updateEffectProperty(const QFeedbackHapticsEffect *, EffectProperty);
    virtual void setEffectState(const QFeedbackHapticsEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackHapticsEffect *);

#ifdef HAS_THEME_EFFECTS
    virtual bool play(QFeedbackEffect::ThemeEffect);
#endif

#ifdef HAS_FILE_EFFECTS
    // File interface functions
    virtual void setLoaded(QFeedbackFileEffect*, bool);
    virtual void setEffectState(QFeedbackFileEffect *, QFeedbackEffect::State);
    virtual QFeedbackEffect::State effectState(const QFeedbackFileEffect *);
    virtual int effectDuration(const QFeedbackFileEffect*);
    virtual QStringList supportedMimeTypes();
#endif

#ifdef USE_CHWRMHAPTICS_PLZ
    // MHWRMHapticsObserver functions
    void EffectCompletedL(TInt error, TInt handle);
    void HapticsStatusChangedL(THWRMHapticsStatus aStatus);
#endif

private:
#ifdef USE_CHWRMVIBRA_PLZ
    CHWRMVibra *vibra();

    CHWRMVibra *m_vibra;
    bool m_vibraActive;

    class PausableElapsedTimer
    {
    public:
        PausableElapsedTimer() : pausedTime(0), mTimer(0)
        {
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
           m_elapsedTimer.invalidate();
#else
            m_elapsedTimer = QTime();
#endif //QT_VERSION
        }

        ~PausableElapsedTimer()
        {
            delete mTimer;
        }

        void start(const QFeedbackHapticsEffect* effect, int millis)
        {
            m_elapsedTimer.start();
            Q_ASSERT(mTimer == 0);
            mTimer = new QTimer;
            mTimer->setSingleShot(true);
            mTimer->setInterval(millis);
            connect(mTimer, SIGNAL(timeout()), const_cast<QFeedbackHapticsEffect*>(effect), SIGNAL(stateChanged()));
            mTimer->start();
        }

        bool isTimerActive() const {
            if (!mTimer)
                return false;
            return mTimer->isActive();
        }

        void pause()
        {
            if (m_elapsedTimer.isValid())
                pausedTime += m_elapsedTimer.elapsed();
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
            m_elapsedTimer.invalidate();
#else
            m_elapsedTimer = QTime();
#endif //QT_VERSION
            delete mTimer;
            mTimer = 0;
        }

        bool isPaused() const
        {
            return !m_elapsedTimer.isValid();
        }

        int elapsed() const
        {
            return pausedTime + (m_elapsedTimer.isValid() ? m_elapsedTimer.elapsed() : 0);
        }

    private:
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        QElapsedTimer m_elapsedTimer;
#else
        QTime m_elapsedTimer;
#endif //QT_VERSION
        int pausedTime;
        QTimer* mTimer;

    };

    QHash<const QFeedbackHapticsEffect*, PausableElapsedTimer> m_elapsed;

#endif //USE_CHWRMVIBRA_PLZ
#ifdef USE_CHWRMHAPTICS_PLZ
    CHWRMHaptics *haptics(int actuator);
    CHWRMHaptics *fileHaptics();

    QHash<THWRMLogicalActuators, CHWRMHaptics*> m_haptics;

    struct FileContent {
        FileContent() : fileHandle(0), refCount(0) { }
        FileContent(const QByteArray &arr) : fileHandle(0), ba(arr), refCount(1) { }
        TPtrC8 constData() const {return TPtrC8(reinterpret_cast<const unsigned char *>(ba.constData()), ba.length());}
        TInt fileHandle;

        QByteArray ba;
        int refCount;
    };
    QHash<QString, FileContent> fileData;
    QHash<const QFeedbackEffect*, TInt> effectHandles;
    QHash<TInt, const QFeedbackEffect*> handleEffects;
#endif //USE_CHWRMHAPTICS_PLZ

#ifdef HAS_THEME_EFFECTS
    static MTouchFeedback *touchInstance();
#endif //HAS_THEME_EFFECTS

    void initActuators();
    QList<QFeedbackActuator*> m_actuators;
    CCoeControl *defaultWidget();
};
#endif //HAS_CUSTOM_EFFECTS


QT_END_HEADER

#endif
