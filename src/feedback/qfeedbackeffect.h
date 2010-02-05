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

#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <qmobilityglobal.h>

class QWidget;
class QGraphicsView;
class QGraphicsItem;


QTM_BEGIN_NAMESPACE

enum ContinuousEffect {
    ContinuousNone, ContinuousSmooth, ContinuousSlider, ContinuousPopup,
    ContinuousInput, ContinuousPinch, NumberOfContinuousFeedbacks, NoContinuousOverride,
    ContinuousUser = 1000, ContinuousMaxUser = 65535
};

enum InstantEffect {
  InstantNone, InstantBasic, InstantSensitive, InstantBasicButton,
  InstantSensitiveButton, InstantBasicKeypad, InstantSensitiveKeypad, InstantBasicSlider,
  InstantSensitiveSlider, InstantBasicItem, InstantSensitiveItem, InstantItemScroll,
  InstantItemPick, InstantItemDrop, InstantItemMoveOver, InstantBounceEffect,
  InstantCheckbox, InstantMultipleCheckbox, InstantEditor, InstantTextSelection,
  InstantBlankSelection, InstantLineSelection, InstantEmptyLineSelection, InstantPopUp,
  InstantPopupOpen, InstantPopupClose, InstantFlick, InstantStopFlick,
  InstantMultitouchActivate, InstantRotateStep, InstantNumberOfInstantFeedbacks,
  InstantNoOverride, InstantUser = 65535, InstantMaxUser = 262140
};

enum HitAreaType {
    HitAreaMouseButtonPress,
    HitAreaMouseButtonRelease
};

enum TacticonEffect {
  TacticonNone, TacticonPositive, TacticonNeutral, TacticonNegative
};

enum EffectType {
    ContinuousFeedback, InstantFeedback, TacticonFeedback
};

class Q_FEEDBACK_EXPORT QFeedbackEffect
{
public:
    QFeedbackEffect();
    virtual ~QFeedbackEffect();


    virtual EffectType effectType() const = 0;

    void setWindow(const QWidget *w);
    const QWidget *window() const;

private:
    const QWidget *m_win;
};

//
// feedback used when the program need to decide dynamically the duration
// and intensity
//
class Q_FEEDBACK_EXPORT QContinuousEffect : public QFeedbackEffect
{
public:
    QContinuousEffect();
    ~QContinuousEffect();

    EffectType effectType() const;

    void setTimeout(int msecs);
    int timeout() const;

    void setIntensity(qreal intensity);
    qreal intensity() const;

    void setContinuousEffect(ContinuousEffect effect);
    ContinuousEffect continuousEffect() const;

private:
    int m_timeout;
    qreal m_intensity;
    ContinuousEffect m_effect;
};

//
// instantaneous feedback. You can also register it for a rectangle on a widget/item
// TODO: find a better name
//
class Q_FEEDBACK_EXPORT QInstantEffect : public QFeedbackEffect
{
public:
    QInstantEffect(InstantEffect effect = InstantNone);
    ~QInstantEffect();

    EffectType effectType() const;

    void setInstantEffect(InstantEffect effect);
    InstantEffect instantEffect() const;
private:
    InstantEffect m_effect;
};


//that is the one you can register
class Q_FEEDBACK_EXPORT QHitAreaEffect : public QInstantEffect
{
public:
    QHitAreaEffect();
    ~QHitAreaEffect();

    void setHitAreaType(HitAreaType hitAreaType);
    HitAreaType hitAreaType() const;

    void setRect(const QRect &rect);
    void setRect(const QWidget* widget);
    void setRect(const QGraphicsItem* graphicsItem, const QGraphicsView* graphicsView);
    QRect rect() const;
private:
    QRect m_rect;
    HitAreaType m_hitAreaType;
};



//
// tacticon effects...
//
class Q_FEEDBACK_EXPORT QTacticonEffect : public QFeedbackEffect
{
public:
    QTacticonEffect();
    ~QTacticonEffect();

    EffectType effectType() const;

    void setTacticonEffect(TacticonEffect effect);
    TacticonEffect tacticonEffect() const;
private:
    TacticonEffect m_effect;
};



QTM_END_NAMESPACE

#endif // QFEEDBACKEFFECT_H
