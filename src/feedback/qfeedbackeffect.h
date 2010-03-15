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

#ifndef QFEEDBACKEFFECT_H
#define QFEEDBACKEFFECT_H

#include <qmobilityglobal.h>
#include <QtCore/QAbstractAnimation>

QTM_BEGIN_NAMESPACE

//continous
enum ContinuousEffect {
    ContinuousNone, ContinuousSmooth, ContinuousSlider, ContinuousPopup,
    ContinuousInput, ContinuousPinch, NumberOfContinuousFeedbacks, NoContinuousOverride,
    ContinuousUser = 1000, ContinuousMaxUser = 65535
};


//instant
enum InstantEffect {
  InstantNone, InstantBasic, InstantSensitive, InstantBasicButton,
  InstantSensitiveButton, InstantBasicKeypad, InstantSensitiveKeypad, InstantBasicSlider,
  InstantSensitiveSlider, InstantBasicItem, InstantSensitiveItem, InstantItemScroll,
  InstantItemPick, InstantItemDrop, InstantItemMoveOver, InstantBounceEffect,
  InstantCheckbox, InstantMultipleCheckbox, InstantEditor, InstantTextSelection,
  InstantBlankSelection, InstantLineSelection, InstantEmptyLineSelection, InstantPopUp,
  InstantPopupOpen, InstantPopupClose, InstantFlick, InstantStopFlick,
  InstantMultiInstantActivate, InstantRotateStep, InstantNumberOfInstantFeedbacks,
  InstantNoOverride, InstantUser = 65535, InstantMaxUser = 262140
};

QT_FORWARD_DECLARE_CLASS(QWidget)
class QFeedbackEffectPrivate;
class QVibraEffectPrivate;
class QTouchEffectPrivate;
class QFeedbackEffect;

class Q_FEEDBACK_EXPORT QFeedbackDevice
{
public:
    enum Type {
        //should we have different type for actuators: vibra, motor...
        Vibra,
        Motor
    };

    enum Capability {
        Envelope
      //TBD
    };

    enum State {
        //TBD
        Busy,
        Ready
    };

    State state() const;
    QFeedbackEffect *currentPlayingEffect() const;
    int simultaneousEffect() const; // I guess usually 1

    QFeedbackDevive defaultDevice(Type t = Vibra);

    static QList<QFeedbackDevice> devices();
};

class Q_FEEDBACK_EXPORT QFeedbackEffect : public QAbstractAnimation
{
public:
    enum Duration {
        INFINITE = -1
    };


    QFeedbackEffect();
    virtual ~QFeedbackEffect();

    void setDuration(int msecs);
    int duration() const;

    void setIntensity(qreal intensity);
    qreal intensity() const;

    //the envelope
    void setAttackTime(int msecs);
    int attackTime() const;

    void setAttackIntensity(qreal intensity);
    qreal attackIntensity() const;

    void setPriority(int); //TBD
    int priority() const;

    void setFadeTime(int msecs);
    int fadeTime() const;

    void setDevice(const QFeedBackDevice &device);
    QFeedbackDevice device() const;

    //What do we do with
    //- the period (and more generally the difference between magSweep and periodic
    //- the wave type
    //- the effect style (there is an equivalent to that in S60)

    //Is it enough to be able to repeat an animation with the animation framework?

    void start();
    void pause();
    void stop();

private:
    Q_DECLARE_PRIVATE(QFeedbackEffect)
};


//
// feedback used when the program need to decide dynamically the duration
// and intensity this one is using the Continuous motor
//
class Q_FEEDBACK_EXPORT QVibraEffect : public QFeedbackEffect
{
public:
    QVibraEffect();
    ~QVibraEffect();

    void start();

private:
    Q_DISABLE_COPY(QVibraEffect)
    Q_DECLARE_PRIVATE(QVibraEffect)

};



//
// feedback used when the program need to decide dynamically the duration
// and intensity this one is using the Continuous touch feedback
//
class Q_FEEDBACK_EXPORT QTouchEffect : public QFeedbackEffect
{
public:
    QTouchEffect();
    ~QTouchEffect();

    void start();

    void setContinuousEffect(ContinuousEffect);
    ContinuousEffect continuousEffect() const;

    static void play(InstantEffect effect);

    void setWidget(QWidget *);
    QWidget *widget() const;

private:
    Q_DISABLE_COPY(QTouchEffect)
    Q_DECLARE_PRIVATE(QTouchEffect)

};

QTM_END_NAMESPACE

#endif // QFEEDBACKEFFECT_H
