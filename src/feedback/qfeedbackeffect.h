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

#ifndef QFEEDBACKEFFECT_H
#define QFEEDBACKEFFECT_H

#include <qmobilityglobal.h>
#include <QtCore/QAbstractAnimation>
#include <QtCore/QStringList>


QT_BEGIN_HEADER
    
QTM_BEGIN_NAMESPACE

class QFeedbackActuator;
class QFeedbackHapticsEffectPrivate;
class QFeedbackFileEffectPrivate;

class Q_FEEDBACK_EXPORT QFeedbackEffect : public QAbstractAnimation
{
    Q_OBJECT
    Q_ENUMS(ThemeEffect)
    Q_ENUMS(ErrorType)
public:
    enum ThemeEffect {
        ThemeBasic, ThemeSensitive, ThemeBasicButton, ThemeSensitiveButton,
        ThemeBasicKeypad, ThemeSensitiveKeypad, ThemeBasicSlider, ThemeSensitiveSlider,
        ThemeBasicItem, ThemeSensitiveItem, ThemeItemScroll, ThemeItemPick, ThemeItemDrop,
        ThemeItemMoveOver, ThemeBounceEffect, ThemeCheckbox, ThemeMultipleCheckbox, ThemeEditor,
        ThemeTextSelection, ThemeBlankSelection, ThemeLineSelection, ThemeEmptyLineSelection,
        ThemePopUp, ThemePopupOpen, ThemePopupClose, ThemeFlick, ThemeStopFlick,
        ThemeMultitouchActivate, ThemeRotateStep, ThemeLongPress, ThemePositiveTacticon,
        ThemeNeutralTacticon, ThemeNegativeTacticon,
        NumberOfThemeEffects,
        ThemeUser = 65535
    };

    enum ErrorType {
        //to be completed
        NoError,
        UnknownError,
        DeviceBusy
    };

    explicit QFeedbackEffect(QObject *parent = 0);

    static bool supportsThemeEffect();
    static bool playThemeEffect(ThemeEffect effect);

Q_SIGNALS:
    void error(ErrorType); //the feedback could not be played

};

class Q_FEEDBACK_EXPORT QFeedbackHapticsEffect : public QFeedbackEffect
{
    Q_OBJECT
public:
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity)
    Q_PROPERTY(int attackTime READ attackTime WRITE setAttackTime)
    Q_PROPERTY(qreal attackIntensity READ attackIntensity WRITE setAttackIntensity)
    Q_PROPERTY(int fadeTime READ fadeTime WRITE setFadeTime)
    Q_PROPERTY(int fadeIntensity READ fadeIntensity WRITE setFadeIntensity)
    Q_PROPERTY(int period READ period WRITE setPeriod)
    Q_PROPERTY(QFeedbackActuator actuator READ actuator WRITE setActuator)

    enum Duration {
        INFINITE = -1
    };

    explicit QFeedbackHapticsEffect(QObject *parent = 0);
    ~QFeedbackHapticsEffect();

    void setDuration(int msecs);
    int duration() const;

    void setIntensity(qreal intensity);
    qreal intensity() const;

    //the envelope
    void setAttackTime(int msecs);
    int attackTime() const;

    void setAttackIntensity(qreal intensity);
    qreal attackIntensity() const;

    void setFadeTime(int msecs);
    int fadeTime() const;

    void setFadeIntensity(qreal intensity);
    qreal fadeIntensity() const;

    void setPeriod(int msecs);
    int period() const;

    void setActuator(const QFeedbackActuator &actuator);
    QFeedbackActuator actuator() const;

protected:
    //virtual methods from QAbstractAnimation
    void updateCurrentTime(int currentTime);
    void updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState);

private:
    friend class QFeedbackHapticsEffectPrivate;
    QScopedPointer<QFeedbackHapticsEffectPrivate> priv;
};

class Q_FEEDBACK_EXPORT QFeedbackFileEffect : public QFeedbackEffect
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ isLoaded WRITE setLoaded)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)

public:
    explicit QFeedbackFileEffect(QObject *parent = 0);
    ~QFeedbackFileEffect();

    int duration() const;

    bool isLoading() const;
    bool isLoaded() const;

    void load();
    void unload();
    void setLoaded(bool);

    QString fileName() const;
    void setFileName(const QString &);

    static QStringList supportedMimeTypes();

signals:
    void loadFinished(bool);

protected:
    //virtual methods from QAbstractAnimation
    void updateCurrentTime(int currentTime);
    void updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState);

private:
    friend class QFeedbackFileEffectPrivate;
    QScopedPointer<QFeedbackFileEffectPrivate> priv;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QFEEDBACKEFFECT_H
